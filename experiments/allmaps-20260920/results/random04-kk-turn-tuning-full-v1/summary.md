# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_kk_turn6_control / seed0 | 1801 | 183.46 | 255.28 | 0.118 | 1000 |
| RANDOM-04 / trick_kk_turn2 / seed0 | 1947 | 187.04 | 245.83 | 0.117 | 1000 |
| RANDOM-04 / trick_kk_turn4 / seed0 | 1795 | 185.81 | 264.11 | 0.117 | 1000 |
| RANDOM-04 / trick_kk_turn8 / seed0 | 1741 | 181.91 | 240.06 | 0.117 | 1000 |
| RANDOM-04 / trick_kk_turn12 / seed0 | 1340 | 186.63 | 249.13 | 0.116 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
