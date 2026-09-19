#!/usr/bin/env python3
"""Extract matched full-run effects and calibrate screening horizons on a 25-step grid."""
import argparse
import bisect
import concurrent.futures
import hashlib
import json
from pathlib import Path
import statistics


def distribution(values):
    values = sorted(values)
    return {"n": len(values), "mean": statistics.mean(values) if values else None,
            "p90": values[int(.9 * (len(values) - 1))] if values else None}


def extract(item):
    case, path = item
    data = json.loads(path.read_text())
    horizon = data["makespan"]
    tasks = {t[0]: t for t in data["tasks"]}
    completions, pickups = {}, {}
    for step, robot, task, stop in data["events"]:
        if stop == 1:
            pickups[task] = step
        if stop == len(tasks[task][2]) // 2:
            completions[task] = step
    assert len(completions) == data["numTaskFinished"], path
    times = sorted(completions.values())
    grid = list(range(25, horizon + 1, 25))
    if grid[-1] != horizon:
        grid.append(horizon)
    assignments = {}
    initial_distance = []
    for robot, schedule in enumerate(data["actualSchedule"]):
        for entry in schedule.split(","):
            if not entry:
                continue
            step, task = map(int, entry.split(":"))
            if task < 0:
                continue
            assignments[task] = step
            if step == 1:
                row, col, _ = data["start"][robot]
                target = tasks[task][2]
                initial_distance.append(abs(row - target[0]) + abs(col - target[1]))
    fingerprint = hashlib.sha256()
    encoder = json.JSONEncoder(separators=(",", ":"))
    for key in ["actualPaths", "actualSchedule", "events", "tasks"]:
        fingerprint.update(key.encode())
        for chunk in encoder.iterencode(data[key]):
            fingerprint.update(chunk.encode())
    summaries = [line for line in path.with_suffix(".log").read_text().splitlines()
                 if line.startswith("[cgar-scheduler]")]
    counters = {k: int(v) for k, v in (field.split("=", 1) for field in summaries[-1].split()[1:])} if summaries else {}
    outer_planning = data["plannerTimes"]
    planning = data.get("entryComputeTimes", outer_planning)
    if "entryComputeTimes" in data:
        assert len(planning) == horizon and all(x >= 0 for x in planning), 'incomplete entry timing series'
        assert len(planning) == data["entryComputeSamples"]
        assert max(planning) == data["entryComputeMaxSeconds"]
    result = {"case": case, "instance": path.stem, "steps": horizon, "tasks": len(completions),
              "curve": {str(step): bisect.bisect_right(times, step) for step in grid},
              "completed_per_1000": [bisect.bisect_right(times, min(horizon, start + 1000)) -
                                     bisect.bisect_right(times, start) for start in range(0, horizon, 1000)],
              "trajectory_sha256": fingerprint.hexdigest(), "last_scheduler_counters": counters,
              "initial_pickup_manhattan": distribution(initial_distance),
              "observed_pickup_delay": distribution([pickups[t] - step + 1 for t, step in assignments.items() if t in pickups]),
              "assigned_without_pickup": sum(t not in pickups for t in assignments),
              "outstanding_task_age": distribution([horizon - task[1] for t, task in tasks.items() if t not in completions]),
              "max_decision_seconds": max(planning), "total_decision_seconds": sum(planning),
              "decision_timing_source": "entry_completion" if "entryComputeTimes" in data else "simulator_outer_including_logging",
              "max_outer_plan_seconds": max(outer_planning), "total_outer_plan_seconds": sum(outer_planning)}
    print(case, path.stem, result["tasks"], flush=True)
    return result


def effect(control, repair):
    return 100 * (repair / control - 1) if control else None


def category(value):
    return 1 if value >= 1 else (-1 if value <= -1 else 0)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    spec = json.loads((args.input / "spec.json").read_text())
    cases = spec["cases"]
    summaries = {case["name"]: json.loads((args.input / case["name"] / "summary.json").read_text()) for case in cases}
    assert all(len(rows) == 10 and all(r["valid"] for r in rows) for rows in summaries.values())
    items = [(case["name"], args.input / case["name"] / (row["instance"] + ".json"))
             for case in cases for row in summaries[case["name"]]]
    with concurrent.futures.ProcessPoolExecutor(max_workers=4) as pool:
        extracted = list(pool.map(extract, items))
    by_key = {(r["case"], r["instance"]): r for r in extracted}
    instances = [r["instance"] for r in summaries[cases[0]["name"]]]
    paired, horizons, repeatability, budgets = [], [], [], []
    for name in instances:
        seeds = []
        for seed in range(3):
            control = by_key[("control-s{}-r0".format(seed), name)]
            repair = by_key[("repair-s{}-r0".format(seed), name)]
            seeds.append((control, repair))
        paired.append({"instance": name, "seeds": [{"seed": seed, "control": c["tasks"], "repair": r["tasks"],
                       "effect_percent": effect(c["tasks"], r["tasks"])} for seed, (c, r) in enumerate(seeds)]})
        grid = list(map(int, seeds[0][0]["curve"]))
        full = [effect(c["tasks"], r["tasks"]) for c, r in seeds]
        directional, magnitude = [], []
        for step in grid:
            counts = [(c["curve"][str(step)], r["curve"][str(step)]) for c, r in seeds]
            enough = all(c >= 100 and r >= 100 for c, r in counts)
            effects = [effect(c, r) for c, r in counts]
            good = enough and all(category(e) == category(f) for e, f in zip(effects, full))
            directional.append(good)
            magnitude.append(good and all(abs(e - f) <= 5 for e, f in zip(effects, full)))
        def first_stable(flags):
            last_bad = max((i for i, good in enumerate(flags) if not good), default=-1)
            return grid[last_bad + 1] if last_bad + 1 < len(grid) else None
        horizons.append({"instance": name, "full_steps": seeds[0][0]["steps"],
                         "nonzero_effect_observed": any(abs(e) >= 1 for e in full),
                         "directional_screen_steps": first_stable(directional) if any(abs(e) >= 1 for e in full) else None,
                         "effect_within_5pp_steps": first_stable(magnitude) if any(abs(e) >= 1 for e in full) else None,
                         "full_effect_percent_by_seed": full})
        for policy in ["control", "repair"]:
            fingerprints = [by_key[("{}-s0-r{}".format(policy, repeat), name)]["trajectory_sha256"] for repeat in range(3)]
            repeatability.append({"instance": name, "policy": policy, "identical_trajectories": len(set(fingerprints)) == 1})
        budgets.append({"instance": name, "identical_trajectories":
                        by_key[("repair-s0-r0", name)]["trajectory_sha256"] ==
                        by_key[("repair-s0-budget5000", name)]["trajectory_sha256"]})
    args.output.mkdir(parents=True, exist_ok=True)
    for name, value in [("metrics.json", extracted), ("paired-results.json", paired), ("horizons.json", horizons),
                        ("repeatability.json", repeatability), ("budget-invariance.json", budgets), ("run-summaries.json", summaries)]:
        (args.output / name).write_text(json.dumps(value, indent=2) + "\n")
    print("ANALYSIS_COMPLETE", flush=True)


if __name__ == "__main__":
    main()
