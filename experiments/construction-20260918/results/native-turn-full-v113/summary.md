# TRICK: native guidance turn prices 1, 2 and 4

Price 2 loses 95/37 tasks on paired full seeds 0/2 and is rejected. Price 4 changes totals by -25/+90 (mean +32.5), reaching 155095/155146 without beating the 155173 record. It merits additional seeds and a separate cutoff-guard combination test, not an immediate replacement. Physical turns still take one simulator tick; route and pickup guidance prices change together. The guard is OFF in this comparison. The first independent-accounting allocation failed its CPU-affinity assertion before reading trajectories; the same frozen analysis passed when retried on a binding-capable host.

Verified 2026-09-20T21:23:11.408657+00:00. Source [dac3fc7](https://github.com/fywu85/lorr/commit/dac3fc75cb6a149b3e9a6b4770b64170ead87ed9); binary `4f194edc5a806ae31198ef5ff29b995372b2e92bf266f490c010dbf0574481a5`.

Every case completes5000steps with10000robots, zero planner/scheduler errors and timeouts, and independently reconciled50million robot actions. All28source/test hashes, fixed work, distinct 4-core bindings,5s entry deadline and32decimalGB RSS checks pass. Exact prior controls reproduce their complete trajectory hashes. Shared-host timings do not certify the competition1s limit or isolate speedups.

| Variant / seed | Tasks | Paired change | Final1000 | Mean entry ms | Max entry s | Peak RSS GB | Mean CPU cores /4 | Full minutes |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_turn1 /0 | 155,120 | +0 | 34,057 | 453.882 | 0.972386 | 11.946 | 1.498 | 38.82 |
| trick_turn2 /0 | 155,025 | -95 | 34,080 | 448.738 | 0.995391 | 11.949 | 1.509 | 38.37 |
| trick_turn4 /0 | 155,095 | -25 | 34,105 | 453.043 | 0.988612 | 11.941 | 1.502 | 38.72 |
| trick_turn1 /2 | 155,056 | +0 | 34,053 | 450.599 | 0.987062 | 11.946 | 1.503 | 38.51 |
| trick_turn2 /2 | 155,019 | -37 | 34,064 | 448.940 | 0.997095 | 11.915 | 1.506 | 38.37 |
| trick_turn4 /2 | 155,146 | +90 | 34,120 | 452.331 | 0.995808 | 11.931 | 1.501 | 38.64 |

| Variant / seed | Oldest never assigned | Outstanding age p90 /max | First-half tasks incomplete | Initial never assigned | Max retargets | After-pickup assignments |
|---|---:|---:|---:|---:|---:|---:|
| trick_turn1 /0 | 188 | 533 /1078 | 0 | 0 | 1 | 0 |
| trick_turn2 /0 | 187 | 534 /1071 | 0 | 0 | 1 | 0 |
| trick_turn4 /0 | 188 | 535 /1092 | 0 | 0 | 1 | 0 |
| trick_turn1 /2 | 193 | 536 /1044 | 0 | 0 | 1 | 0 |
| trick_turn2 /2 | 185 | 534 /1014 | 0 | 0 | 1 | 0 |
| trick_turn4 /2 | 189 | 533 /1336 | 0 | 0 | 1 | 0 |

Ordinary HRRN and forced-oldest admission remain enabled, short preference remains OFF, and held/started/primary/recovery protections remain. Finite observations on solver seeds of one fixed input do not prove starvation freedom. Tail regressions are retained in the table.

| Candidate / seed | Empty robot-work change | Unfinished task-work change | Forward change | Turn change | Wait change |
|---|---:|---:|---:|---:|---:|
| trick_turn2 /0 | +4,579 | +2,229 | -32,133 | -349 | +32,482 |
| trick_turn4 /0 | +203 | +1,170 | -10,701 | -8,772 | +19,473 |
| trick_turn2 /2 | -2,051 | -9,576 | -23,738 | -810 | +24,548 |
| trick_turn4 /2 | -6,247 | -10,074 | +14,183 | -10,596 | -3,587 |

Unfinished-task work overlaps the empty/loaded and action partitions; never add these as separate costs. Changed task cohorts and trajectories prevent a causal savings interpretation. The assumed5000-step horizon and all Warehouse policies require `--trick WAREHOUSE`.

[Full verification and exact per-case configuration](verification.json), [fairness](fairness.json), [physical work](work-budget.json), [sampled policy work](work-samples.json).
