# TRICK: full static-lane remaining-potential comparison

**New overall best: 147,502 tasks, seed 2.** Source [0f4183f](https://github.com/fywu85/lorr/commit/0f4183fcdafcaab391ace632f70a0c513caba14b). Static lanes and remaining-potential scoring are ON; matching and short preference are OFF. Explicit `--trick WAREHOUSE` is required.

All four full cases pass source/binary, simulator, complete-entry, 32GB RSS, prescribed work and disjoint physical-binding checks. Both disabled controls exactly reproduce the earlier lane-only trajectories. Shared-host 5s development runs; no strict 1s certification.

| Seed | Lane-only | Remaining potential | Task change | Final 1,000 change | Empty-work change |
|---|---:|---:|---:|---:|---:|
| 0 | 146,659 | 147,422 | +763 | +201 | -154900 |
| 2 | 146,566 | 147,502 | +936 | +195 | -176219 |

Paired mean improves **0.57942%**, with both totals and late windows positive. Empty steps per completion fall from67.180/67.299 to65.781/65.678. Loaded turns rise by110,527/104,982 while loaded waits fall34,441/46,590. Completed-cohort and static-lane replay is queued to interpret these policy differences; raw action changes alone are not causal savings.

Best run completed 2026-09-20T09:07:01.739904+00:00. Mean entry 309.626ms, maximum 0.979071793s, peak RSS 11,839,000,576 bytes, 1.842 average CPU cores of four reserved, wall 26.92 minutes.

Target remains unmet: **7,293 tasks**, requiring **4.9443%** growth from the best run. A separate combination with bounded matching is the next interaction test; additivity is not assumed.

[Comparison](comparison.json) · [Verification](verification.json) · [Exact best configuration and trajectory](best-record.json).
