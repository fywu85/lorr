# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_turn2_temperature1000 / seed0 | 1947 | 344.05 | 683.27 | 0.121 | 1000 |
| RANDOM-04 / trick_turn2_temperature0 / seed0 | 1793 | 181.77 | 236.1 | 0.117 | 1000 |
| RANDOM-04 / trick_turn2_temperature5000 / seed0 | 1924 | 182.53 | 240.58 | 0.117 | 1000 |
| RANDOM-04 / trick_turn2_temperature10000 / seed0 | 1828 | 182.04 | 228.7 | 0.117 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
