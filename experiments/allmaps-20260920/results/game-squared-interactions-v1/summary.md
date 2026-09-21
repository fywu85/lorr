# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| GAME / trick_squared_adapted_control / seed0 | 14664 | 264.06 | 892.35 | 10.614 | 5000 |
| GAME / trick_squared_adapted_dispatch / seed0 | 15574 | 279.46 | 911.11 | 10.550 | 5000 |
| GAME / trick_squared_native / seed0 | 10429 | 298.52 | 968.98 | 10.494 | 5000 |
| GAME / trick_squared_native_dispatch / seed0 | 10471 | 306.84 | 966.75 | 10.488 | 5000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
