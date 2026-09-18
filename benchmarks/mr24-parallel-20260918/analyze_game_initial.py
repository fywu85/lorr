"""Compare GAME first-assignment cohorts from the saved sequential and parallel runs."""
from pathlib import Path
import json
import statistics

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent

def stats(values):
    values = sorted(values)
    return {"n": len(values), "mean": round(statistics.mean(values), 2) if values else None,
            "median": statistics.median(values) if values else None,
            "p90": values[int(.9 * (len(values) - 1))] if values else None}

results = []
for label, path in [("sequential", ROOT / "runs/cgar-fixed-20260917-v2/GAME.json"),
                    ("parallel", ROOT / "runs/cgar-parallel-20260918/GAME.json")]:
    data = json.loads(path.read_text())
    tasks = {task[0]: task for task in data["tasks"]}
    pickups = {task: time for time, robot, task, stop in data["events"] if stop == 1}
    distances, delays, times = [], [], []
    for robot, schedule in enumerate(data["actualSchedule"]):
        assigned, task = next((time, task) for time, task in
                              (map(int, entry.split(":")) for entry in schedule.split(",")) if task >= 0)
        row, column, facing = data["start"][robot]
        orientation = {"E": 0, "S": 1, "W": 2, "N": 3}[facing]
        for action in data["actualPaths"][robot][::2][:assigned - 1]:
            if action == "F":
                row += (0, 1, 0, -1)[orientation]
                column += (1, 0, -1, 0)[orientation]
            elif action == "R":
                orientation = (orientation + 1) % 4
            elif action == "C":
                orientation = (orientation + 3) % 4
        target = tasks[task][2]
        distances.append(abs(row - target[0]) + abs(column - target[1]))
        times.append(assigned)
        if task in pickups:
            delays.append(pickups[task] - assigned + 1)
    result = {"instance": "GAME", "execution": label,
              "cohort": "first task assigned to each robot, position at assignment", "n": len(distances),
              "assignment_step": stats(times), "pickup_manhattan": stats(distances),
              "observed_pickup_delay": stats(delays), "unobserved_first_pickups": len(distances) - len(delays),
              "horizon": data["makespan"], "completed_tasks": data["numTaskFinished"]}
    results.append(result)
    print(json.dumps(result), flush=True)
    del data
(HERE / "game-initial-cohorts.json").write_text(json.dumps(results, indent=2) + "\n")
