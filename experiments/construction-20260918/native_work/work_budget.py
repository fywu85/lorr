#!/usr/bin/env python3
"""Summarize physical actions from already-verified full Warehouse runs."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path


def read(path):
    return json.loads(path.read_text())


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def summarize(verification, metrics, accounting):
    assert verification["all_valid_within_deadline_and_memory"] and not verification["failures"]
    assert accounting["all_valid"] and accounting["all_cases_completed"]
    indexed = {row["case"]: row for row in metrics}
    reports = accounting["reports"]
    result = []
    for verified in verification["rows"]:
        name = verified["case"]
        metric, work = indexed[name], reports[name]
        assert metric["trajectory_sha256"] == work["trajectory_sha256"] == verified["trajectory_sha256"]
        assert metric["tasks"] == work["completed"] == verified["tasks"]
        assert metric["steps"] == work["steps"] == 5000 and work["robots"] == 10000
        assert metric["movement_diagnostics"]["complete"]
        total = work["all_robot_steps"]
        assert total == work["steps"] * work["robots"]
        actions = dict(forward=0, turn=0, wait=0)
        phases = {}
        for number, label in [(0, "idle"), (1, "empty"), (2, "loaded")]:
            observed = metric["movement_phases"][str(number)]
            phase = dict(forward=observed["fw"], turn=observed["cr"] + observed["ccr"], wait=observed["wait"])
            assert sum(phase.values()) == work["phase_steps"][label]
            assert sum(observed[key] for key in ["closer", "farther", "equal", "unknown"]) == observed["fw"]
            for key, value in phase.items():
                assert value >= 0
                actions[key] += value
            phases[label] = dict(actions=phase, total=sum(phase.values()),
                                 fraction_of_all_actions=sum(phase.values()) / total)
        assert sum(actions.values()) == total
        assert sum(work["unfinished_task_steps"].values()) == work["unfinished_task_total"]
        assert work["completed_task_duration"]["sum"] + work["unfinished_task_total"] == total
        result.append(dict(case=name, seed=verified["seed"], variant=verified["variant"],
                           tasks=verified["tasks"], trajectory_sha256=verified["trajectory_sha256"],
                           total_actions=total, actions=actions,
                           action_fractions={key: value / total for key, value in actions.items()},
                           phases=phases, unfinished_task_steps=work["unfinished_task_steps"],
                           unfinished_task_total=work["unfinished_task_total"],
                           unfinished_task_fraction=work["unfinished_task_fraction"],
                           completed_task_robot_work=work["completed_task_duration"],
                           robots_with_no_completion=work["robots_with_no_completion"]))
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--results", type=Path, required=True)
    parser.add_argument("--outcomes", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    paths = {"verification": args.results / "verification.json", "metrics": args.results / "metrics.json",
             "accounting": args.outcomes / "accounting.json"}
    hashes = {key: digest(path) for key, path in paths.items()}
    data = {key: read(path) for key, path in paths.items()}
    assert data["accounting"]["verification_sha256"] == hashes["verification"]
    rows = summarize(**data)
    assert hashes == {key: digest(path) for key, path in paths.items()}
    output = dict(checked_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), all_valid=True,
                  source_commit=data["verification"]["exact_production_source_commit"],
                  binary_sha256=data["verification"]["binary_sha256"],
                  inputs={key: dict(path=str(path), sha256=hashes[key]) for key, path in paths.items()},
                  summarizer_sha256=digest(Path(__file__)), rows=rows,
                  limits=["Summarizes completed verified artifacts; does not replay raw actions again.",
                          "Empty/loaded/idle partitions and forward/turn/wait partitions each sum to all actions.",
                          "Unfinished work overlaps both partitions; never add it as a separate cost.",
                          "Empty travel, turns and waits include necessary work, not just avoidable overhead.",
                          "Temporal offered actions are overwritten with the final plan before movement logging; zero blocked/safety counts cannot establish uncongested motion.",
                          "End-censored work and action fractions are not causal savings or throughput upper bounds."])
    args.output.write_text(json.dumps(output, indent=2) + "\n")
    print("WORK_BUDGET_VERIFIED", len(rows), sum(row["total_actions"] for row in rows))


if __name__ == "__main__":
    main()
