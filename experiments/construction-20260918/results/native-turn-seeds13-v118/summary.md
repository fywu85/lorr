# TRICK: native turn price 4, additional full seeds 1 and 3

The additional turn-price pairs change throughput by +116 / -159. Across seeds 0–3, turn 4 changes totals by -25 / +116 / +90 / -159 (mean +5.5; two wins, two losses). This does not support replacing turn 1, and no case exceeds the 155,173 record. All fairness safeguards remain; physical-turn reductions do not establish a throughput benefit.

Verified 2026-09-20T22:04:30.480804+00:00. Source [dac3fc7](https://github.com/fywu85/lorr/commit/dac3fc75cb6a149b3e9a6b4770b64170ead87ed9); binary `4f194edc5a806ae31198ef5ff29b995372b2e92bf266f490c010dbf0574481a5`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct 4-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /4 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_turn1 /1 | 154,999 | +0 | 34,060 | 449.491 | 0.993706 | 11.946 | 1.501 | 38.49 |
| trick_turn4 /1 | 155,115 | +116 | 34,104 | 451.429 | 1.006834 | 11.952 | 1.504 | 38.58 |
| trick_turn1 /3 | 155,020 | +0 | 34,120 | 447.125 | 0.986468 | 11.925 | 1.505 | 38.26 |
| trick_turn4 /3 | 154,861 | -159 | 34,044 | 452.496 | 1.026952 | 11.952 | 1.505 | 38.71 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_turn1 /1 | 186 | 536 /1090 | 0 | 0 | 1 | 0 |
| trick_turn4 /1 | 188 | 533 /1076 | 0 | 0 | 1 | 0 |
| trick_turn1 /3 | 184 | 535 /1331 | 0 | 0 | 1 | 0 |
| trick_turn4 /3 | 181 | 537 /1331 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_turn4 /1 | -11,622 | -7,984 | +20,864 | -12,878 | -7,986 |
| trick_turn4 /3 | -6,955 | +9,415 | -46,374 | -10,638 | +57,012 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).
