#!/usr/bin/env python3
"""Run the MR24 suite into a fresh directory, preserving the previous CGAR runs."""
import argparse
import concurrent.futures
import csv
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import time

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
    args = parser.parse_args()
    out = args.output.resolve()
    out.mkdir(parents=True, exist_ok=False)
    binary = out / "lifelong"
    shutil.copy2(ROOT / "cgar/build/lifelong", binary)
    times = json.loads((ROOT / "mr24/simulation_time.json").read_text())
    names = args.instances or list(times)
    instances = {p.stem: p.resolve() for p in (ROOT / "mr24").glob("*/*.json")}
    sources = [ROOT / "cgar/cgar_planner/cgar.cpp", ROOT / "cgar/cgar_planner/cgar.hpp",
               ROOT / "cgar/src/MAPFPlanner.cpp", ROOT / "cgar/src/TaskScheduler.cpp"]
    metadata = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
                "jobs": args.jobs, "plan_time_limit_ms": 1000, "preprocess_time_limit_ms": 30000,
                "binary_sha256": hashlib.sha256(binary.read_bytes()).hexdigest(),
                "sources": {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest() for p in sources},
                "environment": {k: v for k, v in os.environ.items() if k.startswith("CGAR_")},
                "instances": {name: {"input": str(instances[name]), "steps": times[name]} for name in names}}
    (out / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
    results = {}

    def save():
        ordered = [results[name] for name in names if name in results]
        (out / "summary.json").write_text(json.dumps(ordered, indent=2) + "\n")
        fields = ["instance", "before", "after", "delta_percent", "makespan", "planner_errors", "schedule_errors", "timeouts", "wall_seconds", "exit", "valid"]
        with (out / "summary.csv").open("w", newline="") as stream:
            writer = csv.DictWriter(stream, fieldnames=fields, lineterminator="\n")
            writer.writeheader()
            writer.writerows({k: row.get(k) for k in fields} for row in ordered)
        lines = ["# CGAR migration benchmark", "", "One run per MR24 instance; 1000 ms per decision and 30000 ms preprocessing.",
                 "Before values are the existing draft's output files, summarized in `benchmarks/cgar-draft.json`. Error counts validate the simulator interface, not a liveness proof.", "",
                 "| Instance | Before | After | Change | Planner errors | Schedule errors | Timeouts | Wall seconds |",
                 "|---|---:|---:|---:|---:|---:|---:|---:|"]
        for row in ordered:
            lines.append("| {instance} | {before} | {after} | {delta_percent:+.1f}% | {planner_errors} | {schedule_errors} | {timeouts} | {wall_seconds:.2f} |".format(**row))
        (out / "summary.md").write_text("\n".join(lines) + "\n")

    def run(name):
        steps = times[name]
        output = out / (name + ".json")
        command = [str(binary), "-i", str(instances[name]), "-o", str(output), "-s", str(steps), "-t", "1000", "-p", "30000"]
        started = time.monotonic()
        with (out / (name + ".log")).open("w") as log:
            result = subprocess.run(command, cwd=str(ROOT / "cgar"), stdout=log, stderr=subprocess.STDOUT)
        elapsed = time.monotonic() - started
        baseline = ROOT / "runs" / ("cgar_" + name + "_s" + str(steps) + ".json")
        old = header(baseline) if baseline.exists() else json.loads((ROOT / "benchmarks/cgar-draft.json").read_text())[name]
        data = header(output) if output.exists() else dict.fromkeys(FIELDS, 0)
        row = {"instance": name, "before": old["numTaskFinished"], "after": data["numTaskFinished"],
               "delta_percent": 100 * (data["numTaskFinished"] / old["numTaskFinished"] - 1),
               "makespan": data["makespan"], "planner_errors": data["numPlannerErrors"],
               "schedule_errors": data["numScheduleErrors"], "timeouts": data["numEntryTimeouts"],
               "wall_seconds": round(elapsed, 3), "exit": result.returncode,
               "valid": result.returncode == 0 and data["makespan"] == steps and all(data[key] == 0 for key in FIELDS[3:])}
        return row

    with concurrent.futures.ThreadPoolExecutor(max_workers=max(1, args.jobs)) as pool:
        futures = {pool.submit(run, name): name for name in names}
        for future in concurrent.futures.as_completed(futures):
            name = futures[future]
            row = future.result()
            results[name] = row
            save()
            print(json.dumps(row), flush=True)
    return 0 if all(row["valid"] for row in results.values()) else 1

if __name__ == "__main__":
    raise SystemExit(main())
