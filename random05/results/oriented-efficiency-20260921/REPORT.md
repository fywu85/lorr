# Completed loaded-route efficiency

Generated 2026-09-21T16:25:14.162406+00:00. Independent unit-action shortest paths include the pickup heading and all later task waypoints.

| Instance | PILOT tasks | Loaded steps | Relaxed minimum | Extra loaded steps | Extra / all robot steps |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 729 | 49,409 | 45,966 | 3,443 | 5.7% |
| RANDOM-02 | 1,408 | 99,522 | 85,754 | 13,768 | 11.5% |
| RANDOM-03 | 2,602 | 269,583 | 160,349 | 109,234 | 34.1% |
| RANDOM-04 | 2,777 | 480,590 | 152,779 | 327,811 | 46.8% |
| RANDOM-05 | 4,175 | 1,179,580 | 240,404 | 939,176 | 58.7% |

RANDOM-01 has relatively little extra loaded motion/delay compared with the crowded cases. This supports testing assignment and end-of-run choices there, while substantial motion inefficiency remains observable on RANDOM-03–05. The gap includes unavoidable multi-robot interactions; it is not a prediction of achievable throughput. The bound also permits zero-time repeated waypoints, making it a relaxation of even collision-free execution.

| Instance | Common completed tasks | PILOT loaded steps | NMS loaded steps | PILOT bound | NMS bound |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 644 | 42,593 | 46,809 | 39,555 | 39,631 |
| RANDOM-02 | 1,215 | 84,113 | 92,954 | 72,260 | 72,434 |
| RANDOM-03 | 2,327 | 232,453 | 241,705 | 136,604 | 136,779 |
| RANDOM-04 | 2,530 | 417,594 | 410,667 | 131,303 | 131,648 |
| RANDOM-05 | 3,141 | 846,762 | 1,021,067 | 168,834 | 169,301 |

Common task IDs control part of the task-content difference. Assignments, pickup headings, arrival times and congestion histories still differ; this is observational evidence. All ten source trajectories already passed full movement, assignment and event replay. The independent bound code asserts its lower bound is no greater than actual loaded time for every completed task.

Reproduce with the standalone C++ distance builder and Python analyzer in `random05/tools/`; only NumPy and the standard C++ library are required. A separate three-cell fixture checks forward, rotation and reversal distances. The diagnostic links to no planner code.

[Audit and per-case hashes](audit.json), [frozen input traces](manifest.json), [distance generation](distance-generation.json).
