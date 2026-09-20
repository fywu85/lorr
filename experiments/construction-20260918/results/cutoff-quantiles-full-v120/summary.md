# TRICK: guarded cutoff quantiles p90, p95 and p99

Retain p90. The p95 margin loses 96 / 54 tasks; p99 loses 362 / 303. All differences occur in the final 1,000 steps, with more empty and unfinished task work under both larger quantiles. All six full runs, exact controls and pre-intervention prefixes pass. These losses are not a reason to weaken fairness or the one-retarget limit.

Verified 2026-09-20T22:15:50.520742+00:00. Source [37f592a](https://github.com/fywu85/lorr/commit/37f592a48bdb24b936f8d0be8be22ddd405b4ddb); binary `3f66eefd2a54589e48965b5b4031cbbe4d67bd8e6423c8cdc1cb3108f24bf641`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct 4-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /4 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_guard_q90 /0 | 155,173 | +0 | 34,110 | 453.463 | 0.987985 | 11.947 | 1.489 | 38.77 |
| trick_guard_q95 /0 | 155,077 | -96 | 34,014 | 449.309 | 0.985806 | 11.921 | 1.503 | 38.44 |
| trick_guard_q99 /0 | 154,811 | -362 | 33,748 | 452.355 | 0.993981 | 11.955 | 1.501 | 38.69 |
| trick_guard_q90 /2 | 155,090 | +0 | 34,087 | 449.145 | 1.000461 | 11.955 | 1.506 | 38.41 |
| trick_guard_q95 /2 | 155,036 | -54 | 34,033 | 449.707 | 1.002968 | 11.967 | 1.507 | 38.48 |
| trick_guard_q99 /2 | 154,787 | -303 | 33,784 | 446.949 | 1.001312 | 11.957 | 1.507 | 38.22 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_guard_q90 /0 | 189 | 533 /1072 | 0 | 0 | 1 | 0 |
| trick_guard_q95 /0 | 189 | 536 /1072 | 0 | 0 | 1 | 0 |
| trick_guard_q99 /0 | 206 | 549 /1072 | 0 | 0 | 1 | 0 |
| trick_guard_q90 /2 | 187 | 536 /1071 | 0 | 0 | 1 | 0 |
| trick_guard_q95 /2 | 189 | 537 /1071 | 0 | 0 | 1 | 0 |
| trick_guard_q99 /2 | 197 | 548 /1076 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_guard_q95 /0 | +5,244 | +33,256 | -6,614 | +1,614 | +5,000 |
| trick_guard_q99 /0 | +23,342 | +133,593 | -12,644 | +3,866 | +8,778 |
| trick_guard_q95 /2 | +7,221 | +20,565 | +1,187 | -306 | -881 |
| trick_guard_q99 /2 | +27,818 | +123,503 | -5,516 | +1,911 | +3,605 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).

Independent replay verifies identical action, assignment and event prefixes before each percentile policy first changes a decision: 3,798 / 3,777 steps for p95 at seeds 0/2, and 3,776 / 3,777 for p99. The p95 outstanding-age p90 rises 3 / 1 steps; p99 rises 16 / 12. No initially revealed task remains never assigned, every first-half task completes, and the one-retarget and after-pickup protections remain intact.
