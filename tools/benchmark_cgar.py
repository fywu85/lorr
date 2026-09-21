#!/usr/bin/env python3
"""Run the MR24 suite into a fresh directory, preserving the previous CGAR runs."""
import argparse
import concurrent.futures
import csv
import datetime
import hashlib
import json
import os
import queue
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import time

from cpu_resources import cpu_resources

ROOT = Path(__file__).resolve().parents[1]
FIELDS = ("teamSize", "numTaskFinished", "makespan", "numPlannerErrors", "numScheduleErrors", "numEntryTimeouts")
MAX_PROCESS_MEMORY_BYTES = 32_000_000_000

def header(path):
    with path.open() as stream:
        text = stream.read(2048)
    result = {key: int(re.search(r'"' + key + r'"\s*:\s*(\d+)', text).group(1)) for key in FIELDS}
    for key in ['entryComputeSamples', 'entryComputeMaxSeconds']:
        match = re.search(r'"' + key + r'"\s*:\s*([-+0-9.eE]+)', text)
        if match:
            result[key] = float(match.group(1)) if key.endswith('Seconds') else int(match.group(1))
    for key in ['experimentTrack', 'trick']:
        match = re.search(r'"' + key + r'"\s*:\s*("[^"\n]*")', text)
        if match:
            result[key] = json.loads(match.group(1))
    return result


def trick_receipt_valid(log, instance, expected_hash, expected_components=None):
    receipts = [line for line in log.splitlines() if line.startswith('[CGAR_TRICK] ')]
    components = [line for line in log.splitlines() if line.startswith('[CGAR_TRICK_COMPONENTS] ')]
    if not instance:
        return not receipts and not components
    if len(receipts) != 1:
        return False
    fields = dict(field.split('=', 1) for field in receipts[0].split()[1:] if '=' in field)
    valid = fields.get('instance') == instance and fields.get('field_sha256') == expected_hash
    if expected_components is not None:
        if len(components) != 1:
            return False
        actual = dict(field.split('=', 1) for field in components[0].split()[1:] if '=' in field)
        # Legacy binaries predate these explicit components; absent means OFF.
        for key in ('matching', 'remaining_flow', 'native_metric', 'native_bands', 'rank_squared', 'random_reference', 'game_active_limit', 'game_tabu', 'horizon_manhattan'):
            if key in expected_components:
                actual.setdefault(key, '0')
        valid = valid and actual == dict(instance=instance, started_tasks='protected',
                                       **{k: str(v) for k, v in expected_components.items()})
    return valid



def run_logged(command, log_path, cwd, environment, local_spool=False):
    """Preserve logs and exit status; optional node-local writes avoid NFS in timed entries."""
    def run(path):
        with path.open("w") as log:
            return subprocess.run(command, cwd=cwd, env=environment, stdout=log, stderr=subprocess.STDOUT)
    if not local_spool:
        return run(log_path)
    with tempfile.TemporaryDirectory(prefix="cgar-log-", dir="/tmp") as temporary:
        local = Path(temporary) / log_path.name
        try:
            return run(local)
        finally:
            if local.exists():
                shutil.copy2(local, log_path)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--jobs", type=int, default=1)
    parser.add_argument("--instances", nargs="*")
    parser.add_argument("--binary", type=Path, default=ROOT / "cgar/build/lifelong")
    parser.add_argument("--source-manifest", type=Path, help="Build provenance for a frozen executable")
    parser.add_argument("--steps", type=int, help="Override the horizon for each selected instance")
    parser.add_argument("--horizon-profile", type=Path, help="JSON mapping of instance names to shorter screening horizons")
    parser.add_argument("--plan-time-limit-ms", type=int, default=1000, help="Decision deadline; 1000 is the competition setting")
    parser.add_argument("--trick", choices=["WAREHOUSE", "SORTATION", "CITY-01", "CITY-02", "GAME", "RANDOM-01", "RANDOM-02", "RANDOM-03", "RANDOM-04", "RANDOM-05"], help="Explicit map-specific policy; absent means generic")
    parser.add_argument("--seed", type=int, help="Set CGAR_SEED explicitly")
    parser.add_argument("--log-detail-level", type=int, choices=[1, 2, 3], default=1, help="Simulator verbosity; 2 retains warnings and failures")
    parser.add_argument("--local-log-spool", action="store_true", help="Write solver stdout/stderr on the allocated node, then archive after exit")
    parser.add_argument("--cpu-list", help="Distinct allowed logical CPUs, grouped per concurrent run")
    parser.add_argument("--cpus-per-instance", type=int, default=1, help="Reserved CPUs in each process affinity mask")
    args = parser.parse_args()
    if args.jobs < 1 or args.cpus_per_instance < 1 or args.plan_time_limit_ms < 1 or (args.steps is not None and args.steps < 1):
        parser.error("jobs, steps, and time limits must be positive")
    if args.steps is not None and args.horizon_profile is not None:
        parser.error("steps and horizon-profile are mutually exclusive")
    resources = cpu_resources()
    cpus = [int(value) for value in args.cpu_list.split(",")] if args.cpu_list else []
    if cpus and (len(cpus) < args.jobs * args.cpus_per_instance or len(cpus) % args.cpus_per_instance or len(set(cpus)) != len(cpus) or
                 not set(cpus).issubset(resources["logical_cpu_affinity"])):
        parser.error("cpu-list must contain complete, distinct allowed CPU groups for every concurrent job")
    if args.cpus_per_instance > 1 and not cpus:
        parser.error("multiple CPUs per instance require an explicit cpu-list")
    if cpus and not shutil.which("taskset"):
        parser.error("taskset is required for explicit CPU binding")
    available_cpus = queue.Queue()
    for start in range(0, len(cpus), args.cpus_per_instance):
        group = cpus[start:start + args.cpus_per_instance]
        available_cpus.put(group[0] if args.cpus_per_instance == 1 else group)
    environment = os.environ.copy()
    if args.seed is not None:
        environment["CGAR_SEED"] = str(args.seed)
    times = json.loads((ROOT / "mr24/simulation_time.json").read_text())
    names = args.instances or list(times)
    if len(set(names)) != len(names) or not set(names).issubset(times):
        parser.error("instances must be distinct MR24 names")
    if args.trick and names != [args.trick]:
        parser.error("--trick requires exactly the named instance")
    if args.horizon_profile:
        profile = json.loads(args.horizon_profile.read_text())
        if not isinstance(profile, dict) or not set(names).issubset(profile) or any(type(v) is not int or v < 1 for v in profile.values()):
            parser.error("horizon-profile must give a positive integer horizon for every selected instance")
        times.update(profile)
    if args.steps is not None:
        times = {name: args.steps for name in times}
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=False)
    binary = out / "lifelong"
    shutil.copy2(args.binary.resolve(), binary)
    instances = {p.stem: p.resolve() for p in (ROOT / "mr24").glob("*/*.json")}
    sources = ([ROOT / "cgar/src/driver.cpp", ROOT / "cgar/inc/SharedEnv.h", ROOT / "cgar/CMakeLists.txt", ROOT / "cgar/inc/Entry.h", ROOT / "cgar/inc/CompetitionSystem.h",
                ROOT / "cgar/src/CompetitionSystem.cpp", ROOT / "cgar/cgar_planner/cgar.cpp"] +
               sorted((ROOT / "cgar/cgar_planner").glob("*.hpp")) +
               sorted((ROOT / "cgar/tricks").glob("*.hpp")) +
               [ROOT / "cgar/src/MAPFPlanner.cpp", ROOT / "cgar/src/TaskScheduler.cpp", ROOT / "cgar/src/Entry.cpp"])
    provenance = json.loads(args.source_manifest.read_text()) if args.source_manifest else None
    binary_hash = hashlib.sha256(binary.read_bytes()).hexdigest()
    if provenance is not None and provenance["binary_sha256"] != binary_hash:
        parser.error("source-manifest does not describe this executable")
    component_keys = ['CGAR_TRICK_LANES', 'CGAR_TRICK_SHORT_TASKS', 'CGAR_TRICK_UNOPENED_MATCH', 'CGAR_TRICK_REMAINING_FLOW', 'CGAR_TRICK_NATIVE_METRIC', 'CGAR_TRICK_NATIVE_BANDS', 'CGAR_TRICK_RANDOM_UNIFORM', 'CGAR_TRICK_RANK_SQUARED', 'CGAR_TRICK_GAME_TABU', 'CGAR_TRICK_HORIZON_MANHATTAN']
    explicit_components = any(k in environment for k in component_keys) or 'CGAR_TRICK_RANDOM_REFERENCE' in environment or 'CGAR_TRICK_GAME_ACTIVE_LIMIT' in environment or 'CGAR_TRICK_LANE_COST' in environment
    if explicit_components and not args.trick:
        parser.error('CGAR_TRICK component settings require --trick <instance>')
    if any(environment.get(k, '0') not in ('0', '1') for k in component_keys):
        parser.error('CGAR_TRICK component settings must be 0 or 1')
    reference = environment.get('CGAR_TRICK_RANDOM_REFERENCE', '0')
    if reference not in ('0','1','2','3'):
        parser.error('CGAR_TRICK_RANDOM_REFERENCE must be 0, 1, 2 or 3')
    lane_cost = environment.get('CGAR_TRICK_LANE_COST', '16')
    if lane_cost not in ('4','8','12','16','24','32','48','64'):
        parser.error('CGAR_TRICK_LANE_COST must be 4, 8, 12, 16, 24, 32, 48 or 64')
    if 'CGAR_TRICK_LANE_COST' in environment and (args.trick not in ('CITY-01','CITY-02','GAME') or
            environment.get('CGAR_TRICK_LANES','1') != '1' or environment.get('CGAR_TRICK_NATIVE_METRIC','0') != '0'):
        parser.error('lane cost requires explicit CITY/GAME adapted lanes')
    game_active = environment.get('CGAR_TRICK_GAME_ACTIVE_LIMIT', '0')
    if not re.fullmatch(r'[0-9]+', game_active) or int(game_active) > 6500:
        parser.error('CGAR_TRICK_GAME_ACTIVE_LIMIT must be in [0,6500]')
    if any(k in environment for k in ('CGAR_TRICK_GAME_ACTIVE_LIMIT','CGAR_TRICK_GAME_TABU')) and args.trick != 'GAME':
        parser.error('GAME fleet settings require --trick GAME')
    if environment.get('CGAR_TRICK_GAME_TABU','0') == '1' and not int(game_active):
        parser.error('GAME tabu selection requires a positive active limit')
    expected_components = None
    if args.trick and explicit_components:
        short = int(environment.get('CGAR_TRICK_SHORT_TASKS', '0'))
        expected_components = dict(lanes=int(environment.get('CGAR_TRICK_LANES', '1')),
                                   short_tasks=short, hrrn=0 if short else int(environment.get('CGAR_HRRN', '1')) != 0,
                                   oldest_admission=1-short)
        expected_components['hrrn'] = int(expected_components['hrrn'])
        expected_components['matching'] = int(environment.get('CGAR_TRICK_UNOPENED_MATCH', '0'))
        expected_components['remaining_flow'] = int(environment.get('CGAR_TRICK_REMAINING_FLOW', '0'))
        expected_components['native_metric'] = int(environment.get('CGAR_TRICK_NATIVE_METRIC', '0'))
        expected_components['native_bands'] = int(environment.get('CGAR_TRICK_NATIVE_BANDS', '0'))
        expected_components['rank_squared'] = int(environment.get('CGAR_TRICK_RANK_SQUARED', '0'))
        expected_components['horizon_manhattan'] = int(environment.get('CGAR_TRICK_HORIZON_MANHATTAN', '0'))
    if args.trick in ('RANDOM-01', 'RANDOM-02', 'RANDOM-03', 'RANDOM-04', 'RANDOM-05') and expected_components is not None:
        expected_components['random_uniform'] = int(environment.get('CGAR_TRICK_RANDOM_UNIFORM', '0'))
        expected_components['random_reference'] = int(reference)
    if args.trick == 'GAME' and expected_components is not None:
        expected_components['game_active_limit'] = int(game_active)
        expected_components['game_tabu'] = int(environment.get('CGAR_TRICK_GAME_TABU','0'))
    if int(game_active) and environment.get('CGAR_TRICK_GAME_TABU','0') == '1' and provenance is not None:
        fleet_asset = 'cgar/tricks/game_fleet_tabu.hpp'
        if provenance['sources'].get(fleet_asset) != hashlib.sha256((ROOT/fleet_asset).read_bytes()).hexdigest():
            parser.error('GAME fleet asset does not match the frozen binary source manifest')
    expected_trick_field = 'none' if args.trick else None
    if args.trick and environment.get('CGAR_TRICK_LANES', '1') == '1':
        native = environment.get('CGAR_TRICK_NATIVE_METRIC', '0') == '1'
        bands = environment.get('CGAR_TRICK_NATIVE_BANDS', '0') == '1'
        random_field = args.trick in ('RANDOM-01', 'RANDOM-02', 'RANDOM-03', 'RANDOM-04', 'RANDOM-05')
        asset_prefix = 'random' if random_field else 'city' if args.trick in ('CITY-01','CITY-02') else args.trick.lower()
        if random_field and (not native or bands):
            parser.error('RANDOM field requires native metric without bands')
        if args.trick in ('CITY-01','CITY-02','GAME') and bands:
            parser.error('CITY/GAME native fields have no bands')
        asset_name = ('cgar/tricks/' + asset_prefix + '_native.hpp') if native or args.trick in ('SORTATION','CITY-01','CITY-02','GAME') else 'cgar/tricks/warehouse_lanes.hpp'
        if random_field and reference in ('1','2') and environment.get('CGAR_TRICK_RANDOM_UNIFORM','0') != '1':
            asset_name = 'cgar/tricks/random_reference.hpp'
            asset_prefix = 'random_nms' if reference == '1' else 'random_kk' + args.trick[-2:]
        asset = (ROOT / asset_name).read_bytes()
        if provenance is not None and provenance['sources'].get(asset_name) != hashlib.sha256(asset).hexdigest():
            parser.error('trick receipt asset does not match the frozen binary source manifest')
        declaration = (asset_prefix + '_native_' + ('bands' if bands else 'nobands') + '_field_sha256') if native else asset_prefix + '_field_sha256'
        if not native and lane_cost != '16':
            declaration = asset_prefix + '_lane' + lane_cost + '_field_sha256'
        if random_field and environment.get('CGAR_TRICK_RANDOM_UNIFORM', '0') == '1':
            declaration = 'random_uniform_field_sha256'
        match = re.search(declaration + r'\[\] = "([0-9a-f]{64})"', asset.decode())
        if not match:
            parser.error('trick asset does not declare its field hash')
        expected_trick_field = match.group(1)
    metadata = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
                "experiment_track": "TRICK" if args.trick else "GENERIC",
                "trick": args.trick, "trick_argv": ["--trick", args.trick] if args.trick else [],
                "expected_trick_field_sha256": expected_trick_field,
                "expected_trick_components": expected_components,
                "jobs": args.jobs, "plan_time_limit_ms": args.plan_time_limit_ms, "preprocess_time_limit_ms": 30000,
                "log_detail_level": args.log_detail_level,
                "log_spool": "node_local_tmp" if args.local_log_spool else "archive",
                "benchmark_runner_sha256": hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
                "max_process_memory_bytes": MAX_PROCESS_MEMORY_BYTES,
                "binary_sha256": binary_hash,
                "sources": provenance["sources"] if provenance else {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},
                "build_provenance": provenance,
                "environment": {k: v for k, v in environment.items() if k.startswith("CGAR_")},
                "cpu_resources": resources, "cpu_binding": cpus, "cpus_per_instance": args.cpus_per_instance,
                "source_binary": str(args.binary.resolve()),
                "instances": {name: {"input": str(instances[name]), "steps": times[name]} for name in names}}
    (out / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
    results = {}

    def save():
        ordered = [results[name] for name in names if name in results]
        (out / "summary.json").write_text(json.dumps(ordered, indent=2) + "\n")
        fields = ["instance", "before", "after", "delta_percent", "makespan", "planner_errors", "schedule_errors", "timeouts", "wall_seconds", "exit", "outcome", "internal_timeouts", "valid"]
        with (out / "summary.csv").open("w", newline="") as stream:
            writer = csv.DictWriter(stream, fieldnames=fields, lineterminator="\n")
            writer.writeheader()
            writer.writerows({k: row.get(k) for k in fields} for row in ordered)
        lines = ["# CGAR migration benchmark", "",
                 ("**Horizon override:** draft percentage changes are omitted where the reference horizon differs." if args.steps is not None or args.horizon_profile else ""), "One run per MR24 instance; {} ms per decision and 30000 ms preprocessing.".format(args.plan_time_limit_ms),
                 "Before values are the existing draft's output files, summarized in `benchmarks/cgar-draft.json`. Error counts validate the simulator interface, not a liveness proof.", "",
                 "| Instance | Outcome | Before | After | Change | Planner errors | Schedule errors | Simulator timeouts | Internal timeouts | Wall seconds |",
                 "|---|---|---:|---:|---:|---:|---:|---:|---:|---:|"]
        for row in ordered:
            change = "{:+.1f}%".format(row["delta_percent"]) if row["delta_percent"] is not None else "n/a"
            display = {k: ("n/a" if v is None else v) for k, v in row.items()}
            lines.append("| {instance} | {outcome} | {before} | {after} | {change} | {planner_errors} | {schedule_errors} | {timeouts} | {internal_timeouts} | {wall_seconds:.2f} |".format(change=change, **display))
        (out / "summary.md").write_text("\n".join(lines) + "\n")

    def run(name):
        steps = times[name]
        output = out / (name + ".json")
        command = [str(binary), "-i", str(instances[name]), "-o", str(output), "-s", str(steps), "-t", str(args.plan_time_limit_ms), "-p", "30000", "-d", str(args.log_detail_level)]
        if args.trick:
            command += ["--trick", args.trick]
        cpu = available_cpus.get() if cpus else None
        if cpu is not None:
            command = ["taskset", "-c", ",".join(map(str, cpu)) if isinstance(cpu, list) else str(cpu)] + command
        usage_file = out / (name + ".resources.json")
        command = [sys.executable, str(ROOT / "tools/run_with_usage.py"),
                   "--output", str(usage_file), "--"] + command
        started = time.monotonic()
        try:
            result = run_logged(command, out / (name + ".log"), str(ROOT / "cgar"), environment, args.local_log_spool)
        finally:
            if cpu is not None:
                available_cpus.put(cpu)
        elapsed = time.monotonic() - started
        baseline = ROOT / "runs" / ("cgar_" + name + "_s" + str(steps) + ".json")
        old = header(baseline) if baseline.exists() else json.loads((ROOT / "benchmarks/cgar-draft.json").read_text())[name]
        data = header(output) if output.exists() else dict.fromkeys(FIELDS, None)
        usage = json.loads(usage_file.read_text()) if usage_file.exists() else {}
        internal_timeout = result.returncode == 124
        valid = result.returncode == 0 and data["makespan"] == steps and all(data[key] == 0 for key in FIELDS[3:])
        entry_time = data.get("entryComputeMaxSeconds")
        entry_timing_valid = entry_time is None or (data.get("entryComputeSamples") == steps and
                                                    0 <= entry_time <= args.plan_time_limit_ms / 1000.0)
        peak_bytes = usage.get("peak_rss_kib", 0) * 1024
        memory_valid = 0 < peak_bytes <= MAX_PROCESS_MEMORY_BYTES
        receipt_valid = trick_receipt_valid((out / (name + ".log")).read_text(), args.trick, expected_trick_field, expected_components)
        # New binaries derive these labels from the CLI value received by BaseSystem.
        # Failed entries have no result; legacy generic binaries may omit the labels.
        track_valid = (not output.exists() or
                       (data.get('experimentTrack') == ('TRICK' if args.trick else 'GENERIC') and
                        data.get('trick') == (args.trick or '')) or
                       (not args.trick and 'experimentTrack' not in data and
                        provenance is not None and 'cgar/src/driver.cpp' not in provenance['sources']))
        valid = valid and entry_timing_valid and memory_valid and receipt_valid and track_valid
        outcome = "success" if valid else ("flag_mismatch" if not receipt_valid or not track_valid else
                                           "timeout" if internal_timeout or data["numEntryTimeouts"] or not entry_timing_valid
                                           else "memory_limit" if peak_bytes > MAX_PROCESS_MEMORY_BYTES else "failed")
        row = {"instance": name, "before": old["numTaskFinished"], "after": data["numTaskFinished"],
               "delta_percent": (100 * (data["numTaskFinished"] / old["numTaskFinished"] - 1)
                                 if valid and old["makespan"] == steps else None),
               "draft_comparison_same_horizon": old["makespan"] == steps,
               "makespan": data["makespan"], "planner_errors": data["numPlannerErrors"],
               "schedule_errors": data["numScheduleErrors"], "timeouts": data["numEntryTimeouts"],
               "wall_seconds": round(elapsed, 3), "exit": result.returncode, "cpu": cpu,
               "outcome": outcome, "internal_timeouts": int(internal_timeout), "valid": valid, "process_resources": usage,
               "entry_compute_max_seconds": entry_time, "entry_compute_samples": data.get("entryComputeSamples"),
               "entry_timing_valid": entry_timing_valid, "memory_valid": memory_valid,
               "trick_receipt_valid": receipt_valid, "experiment_track_valid": track_valid,
               "peak_process_rss_bytes": peak_bytes, "max_process_memory_bytes": MAX_PROCESS_MEMORY_BYTES}
        return row

    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        futures = {pool.submit(run, name): name for name in names}
        for future in concurrent.futures.as_completed(futures):
            name = futures[future]
            row = future.result()
            results[name] = row
            save()
            print(json.dumps(row), flush=True)
    metadata["finished_utc"] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    (out / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
    return 0 if all(row["valid"] for row in results.values()) else 1

if __name__ == "__main__":
    raise SystemExit(main())
