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
import time

from cpu_resources import cpu_resources

ROOT = Path(__file__).resolve().parents[1]
FIELDS = ("teamSize", "numTaskFinished", "makespan", "numPlannerErrors", "numScheduleErrors", "numEntryTimeouts")

def header(path):
    with path.open() as stream:
        text = stream.read(2048)
    return {key: int(re.search(r'"' + key + r'"\s*:\s*(\d+)', text).group(1)) for key in FIELDS}

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
    parser.add_argument("--seed", type=int, help="Set CGAR_SEED explicitly")
    parser.add_argument("--cpu-list", help="Comma-separated logical CPUs, one reserved for each concurrent run")
    args = parser.parse_args()
    if args.jobs < 1 or args.plan_time_limit_ms < 1 or (args.steps is not None and args.steps < 1):
        parser.error("jobs, steps, and time limits must be positive")
    if args.steps is not None and args.horizon_profile is not None:
        parser.error("steps and horizon-profile are mutually exclusive")
    resources = cpu_resources()
    cpus = [int(value) for value in args.cpu_list.split(",")] if args.cpu_list else []
    if cpus and (len(cpus) < args.jobs or len(set(cpus)) != len(cpus) or
                 not set(cpus).issubset(resources["logical_cpu_affinity"])):
        parser.error("cpu-list must contain distinct allowed CPUs, at least one per concurrent job")
    if cpus and not shutil.which("taskset"):
        parser.error("taskset is required for explicit CPU binding")
    available_cpus = queue.Queue()
    for cpu in cpus:
        available_cpus.put(cpu)
    environment = os.environ.copy()
    if args.seed is not None:
        environment["CGAR_SEED"] = str(args.seed)
    times = json.loads((ROOT / "mr24/simulation_time.json").read_text())
    names = args.instances or list(times)
    if len(set(names)) != len(names) or not set(names).issubset(times):
        parser.error("instances must be distinct MR24 names")
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
    sources = [ROOT / "cgar/cgar_planner/cgar.cpp", ROOT / "cgar/cgar_planner/cgar.hpp",
               ROOT / "cgar/cgar_planner/pibt_kernel.hpp",
               ROOT / "cgar/src/MAPFPlanner.cpp", ROOT / "cgar/src/TaskScheduler.cpp", ROOT / "cgar/src/Entry.cpp"]
    provenance = json.loads(args.source_manifest.read_text()) if args.source_manifest else None
    binary_hash = hashlib.sha256(binary.read_bytes()).hexdigest()
    if provenance is not None and provenance["binary_sha256"] != binary_hash:
        parser.error("source-manifest does not describe this executable")
    metadata = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
                "jobs": args.jobs, "plan_time_limit_ms": args.plan_time_limit_ms, "preprocess_time_limit_ms": 30000,
                "binary_sha256": binary_hash,
                "sources": provenance["sources"] if provenance else {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},
                "build_provenance": provenance,
                "environment": {k: v for k, v in environment.items() if k.startswith("CGAR_")},
                "cpu_resources": resources, "cpu_binding": cpus,
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
        command = [str(binary), "-i", str(instances[name]), "-o", str(output), "-s", str(steps), "-t", str(args.plan_time_limit_ms), "-p", "30000"]
        cpu = available_cpus.get() if cpus else None
        if cpu is not None:
            command = ["taskset", "-c", str(cpu)] + command
        usage_file = out / (name + ".resources.json")
        command = [sys.executable, str(ROOT / "tools/run_with_usage.py"),
                   "--output", str(usage_file), "--"] + command
        started = time.monotonic()
        try:
            with (out / (name + ".log")).open("w") as log:
                result = subprocess.run(command, cwd=str(ROOT / "cgar"), env=environment,
                                        stdout=log, stderr=subprocess.STDOUT)
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
        outcome = "success" if valid else ("timeout" if internal_timeout or data["numEntryTimeouts"] else "failed")
        row = {"instance": name, "before": old["numTaskFinished"], "after": data["numTaskFinished"],
               "delta_percent": (100 * (data["numTaskFinished"] / old["numTaskFinished"] - 1)
                                 if valid and old["makespan"] == steps else None),
               "draft_comparison_same_horizon": old["makespan"] == steps,
               "makespan": data["makespan"], "planner_errors": data["numPlannerErrors"],
               "schedule_errors": data["numScheduleErrors"], "timeouts": data["numEntryTimeouts"],
               "wall_seconds": round(elapsed, 3), "exit": result.returncode, "cpu": cpu,
               "outcome": outcome, "internal_timeouts": int(internal_timeout), "valid": valid, "process_resources": usage}
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
