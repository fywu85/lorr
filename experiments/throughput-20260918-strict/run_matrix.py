#!/usr/bin/env python3
"""Run matched strict-deadline policies on reserved GRID cores (one core per instance)."""
import argparse
import concurrent.futures
import datetime
import hashlib
import json
import os
from pathlib import Path
import queue
import shlex
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools"))
from benchmark_gridengine import SOURCES, write
from cpu_resources import cpu_resources


def execute(out):
    spec = json.loads((out / "spec.json").read_text())
    resources = cpu_resources()
    cpus = resources["representative_cpus"][:30]
    assert len(cpus) == 30 and int(os.environ["NSLOTS"]) >= 30, resources
    assert resources["effective_cpu_quota"] is None or resources["effective_cpu_quota"] >= 30, resources
    for path, expected in spec["sources"].items():
        assert hashlib.sha256((ROOT / path).read_bytes()).hexdigest() == expected, path
    write(out / "allocation.json", {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
          "resources": resources, "selected_cpus": cpus, "job_id": os.environ.get("JOB_ID")})
    groups = queue.Queue()
    for start in range(0, 30, 5):
        groups.put(cpus[start:start + 5])

    def run(case):
        selected = groups.get()
        try:
            env = {k: v for k, v in os.environ.items() if not k.startswith("CGAR_")}
            env["CGAR_FALLBACK_REPAIR"] = str(case["repair"])
            command = [sys.executable, str(ROOT / "tools/benchmark_cgar.py"), "--output", str(out / case["name"]),
                       "--binary", str(out / "lifelong"), "--jobs", "5", "--cpu-list", ",".join(map(str, selected)),
                       "--seed", str(case["seed"]), "--plan-time-limit-ms", str(case["time_limit_ms"])]
            if spec.get("horizon_profile") is not None:
                command += ["--horizon-profile", str(out / "horizons.json")]
            with (out / (case["name"] + ".log")).open("w") as log:
                result = subprocess.run(command, cwd=str(ROOT), env=env, stdout=log, stderr=subprocess.STDOUT)
            print(json.dumps({"case": case["name"], "returncode": result.returncode}), flush=True)
            return result.returncode
        finally:
            groups.put(selected)

    with concurrent.futures.ThreadPoolExecutor(max_workers=6) as pool:
        codes = list(pool.map(run, spec["cases"]))
    env = {k: v for k, v in os.environ.items() if not k.startswith("CGAR_")}
    probe = subprocess.run([sys.executable, str(ROOT / "tools/benchmark_cgar.py"), "--output", str(out / "timeout-probe"),
                            "--binary", str(out / "lifelong"), "--jobs", "1", "--cpu-list", str(cpus[0]),
                            "--seed", "0", "--instances", "RANDOM-01", "--steps", "2", "--plan-time-limit-ms", "1"],
                           cwd=str(ROOT), env=env)
    row = json.loads((out / "timeout-probe/summary.json").read_text())[0]
    expected_timeout = probe.returncode == 1 and row["exit"] == 124 and row["outcome"] == "timeout" and not row["valid"]
    code = 0 if not any(codes) and expected_timeout else 1
    write(out / "completion.json", {"returncode": code, "case_returncodes": codes,
          "expected_timeout_verified": expected_timeout,
          "finished_utc": datetime.datetime.now(datetime.timezone.utc).isoformat()})
    return code


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--horizon-profile", type=Path, help="Run only the three-seed policy pairs at these horizons")
    parser.add_argument("--execute", action="store_true", help=argparse.SUPPRESS)
    args = parser.parse_args()
    out = args.output.resolve()
    if args.execute:
        return execute(out)
    profile = json.loads(args.horizon_profile.read_text()) if args.horizon_profile else None
    out.mkdir(parents=True, exist_ok=False)
    if profile is not None:
        write(out / "horizons.json", profile)
    shutil.copy2(ROOT / "cgar/build/lifelong", out / "lifelong")
    cases = []
    for seed, repeat in [(0, 0), (1, 0), (2, 0), (0, 1), (0, 2)]:
        for repair in [0, 1]:
            cases.append({"name": "{}-s{}-r{}".format("repair" if repair else "control", seed, repeat),
                          "seed": seed, "repeat": repeat, "repair": repair, "time_limit_ms": 1000})
    cases.append({"name": "repair-s0-budget5000", "seed": 0, "repeat": 0, "repair": 1, "time_limit_ms": 5000})
    if profile is not None:
        cases = cases[:6]
    write(out / "spec.json", {"cases": cases, "horizon_profile": profile,
          "source_commit": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=str(ROOT), text=True).strip(),
          "source_dirty": bool(subprocess.check_output(["git", "status", "--porcelain"], cwd=str(ROOT), text=True)),
          "sources": {p: hashlib.sha256((ROOT / p).read_bytes()).hexdigest() for p in SOURCES},
          "binary_sha256": hashlib.sha256((out / "lifelong").read_bytes()).hexdigest(),
          "allocation_policy": "30 reserved physical cores; six suites at once, five single-core instances per suite"})
    command = [sys.executable, str(Path(__file__).resolve()), "--execute", "--output", str(out)]
    (out / "job.sh").write_text("#!/bin/bash\nset -eu\nexec " + " ".join(shlex.quote(x) for x in command) + "\n")
    submit = ["/opt/n1ge/bin/lx24-amd64/qsub", "-terse", "-w", "e", "-cwd", "-q", "debian.q", "-pe", "threaded", "30",
              "-binding", "linear:30", "-l", "exclusive=true,h_rt=00:30:00,h_vmem=4G", "-m", "n", "-N", "lorr_matrix",
              "-j", "y", "-o", str(out / "scheduler.log"), "-S", "/bin/bash", str(out / "job.sh")]
    result = subprocess.run(submit, cwd=str(ROOT), stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
    write(out / "submission.json", {"command": submit, "returncode": result.returncode, "response": result.stdout})
    print(result.stdout, end="", flush=True)
    return result.returncode


if __name__ == "__main__":
    raise SystemExit(main())
