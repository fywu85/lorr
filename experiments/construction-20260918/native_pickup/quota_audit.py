#!/usr/bin/env python3
"""Check pickup-field quota exposure against saved completion events and counters."""
import argparse
from collections import Counter
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import sys

ROOT = next(p for p in Path(__file__).resolve().parents if (p / "tools/cpu_resources.py").is_file())


def read(path):
    return json.loads(path.read_text())


def sha(path):
    h = hashlib.sha256()
    with path.open("rb") as source:
        for block in iter(lambda: source.read(4 * 1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()


def trajectory(data):
    h = hashlib.sha256()
    encoder = json.JSONEncoder(separators=(",", ":"))
    for key in ["actualPaths", "actualSchedule", "events", "tasks"]:
        h.update(key.encode())
        for chunk in encoder.iterencode(data[key]):
            h.update(chunk.encode())
    return h.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--raw", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    request = read(args.raw / "request.json")
    assert sha(args.raw / "quota_audit.py") == request["analyzer_sha256"]
    assert sha(args.raw / "verification.json") == request["verification_sha256"]
    assert sha(args.raw / "metrics.json") == request["metrics_sha256"]
    sys.path.insert(0, str(ROOT / "tools"))
    from cpu_resources import cpu_resources
    cpu = cpu_resources()
    assert cpu["physical_cores_visible"] == 1 and cpu["effective_cpu_quota"] is None
    os.sched_setaffinity(0, cpu["representative_cpus"])
    verification = read(args.raw / "verification.json")
    assert verification["all_valid_within_deadline_and_memory"] and not verification["failures"]
    metrics = {m["case"]: m for m in read(args.raw / "metrics.json")}
    reports = []
    for row in verification["rows"]:
        if row["variant"] != request["variant"]:
            continue
        path = Path(row["raw_case"]) / "WAREHOUSE.json"
        original = sha(path)
        data = read(path)
        assert data["makespan"] == 5000 and data["teamSize"] == 10000
        assert data["numTaskFinished"] == row["tasks"]
        assert not any(data[k] for k in ["numPlannerErrors", "numScheduleErrors", "numEntryTimeouts"])
        assert trajectory(data) == row["trajectory_sha256"] == metrics[row["case"]]["trajectory_sha256"]
        assert all(metrics[row["case"]]["movement_phases"]["0"][k] == 0 for k in ["fw", "cr", "ccr", "wait"])
        tasks = {t[0]: t for t in data["tasks"]}
        completion = Counter(tick for tick, robot, task, stop in data["events"] if stop == len(tasks[task][2]) // 2)
        assert sum(completion.values()) == row["tasks"] and all(0 < tick <= 5000 for tick in completion)
        quota = int(row["environment"]["CGAR_PICKUP_FULL_ROBOTS"])
        cumulative = []
        fields = 0
        for tick in range(5000):
            fields += min(quota, completion[tick])
            cumulative.append(fields)
        observed = []
        for line in path.with_suffix(".log").read_text().splitlines():
            if not line.startswith("[cgar-pickup-full] "):
                continue
            values = dict(re.findall(r"(\w+)=([0-9]+)", line))
            tick, measured = int(values["t"]), int(values["fields"])
            assert int(values["robot_limit"]) == quota and int(values["threads"]) == 4
            assert measured == cumulative[tick], (row["case"], tick, measured, cumulative[tick])
            observed.append(dict(tick=tick, cumulative_fields=measured))
        assert len(observed) == 25 and [x["tick"] for x in observed] == list(range(0, 5000, 200))
        windows = {}
        for start in [1, 4000, 4800, 4900]:
            ticks = list(range(start, 5000))
            windows[str(start)] = dict(decisions=len(ticks), newly_free=sum(completion[t] for t in ticks),
                                      quota_exceeded_decisions=sum(completion[t] > quota for t in ticks),
                                      excess_robots=sum(max(0, completion[t] - quota) for t in ticks),
                                      max_newly_free=max(completion[t] for t in ticks))
        assert sha(path) == original
        reports.append(dict(case=row["case"], seed=row["seed"], tasks=row["tasks"], quota=quota,
                            input_sha256=original, trajectory_sha256=row["trajectory_sha256"],
                            counter_samples=observed, windows=windows,
                            exceeded_ticks=[dict(tick=t, newly_free=completion[t], excess=completion[t]-quota)
                                            for t in range(1,5000) if completion[t] > quota],
                            final_tick_completions_excluded=completion[5000],
                            newly_free_histogram=dict(sorted(Counter(completion[t] for t in range(1,5000)).items()))))
        print("QUOTA_AUDITED", row["case"], windows["1"], flush=True)
        del data
    assert len(reports) == 2
    args.output.mkdir(exist_ok=False, parents=True)
    result = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), all_valid=True,
                  reports=reports, source_commit=verification["exact_production_source_commit"],
                  binary_sha256=verification["binary_sha256"], request=request,
                  allocation=cpu, job_id=os.environ["JOB_ID"],
                  limits=["Read-only event replay; no new solver performance result.",
                          "Zero goal-less actions and exact cumulative field counters support completion-driven free-robot counts on these runs.",
                          "Tick 0 uses generic mass dispatch; completions at tick 5000 have no following scheduling entry.",
                          "Excess robots use the existing bounded pickup fallback; they are not left unassigned.",
                          "A quota witness is neither a bad assignment nor a predicted throughput gain."])
    (args.output / "accounting.json").write_text(json.dumps(result, indent=2) + "\n")
    print("QUOTA_AUDIT_COMPLETE", len(reports), flush=True)


if __name__ == "__main__":
    main()
