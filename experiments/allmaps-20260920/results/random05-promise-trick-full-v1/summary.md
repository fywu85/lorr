# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_control / seed0 | 2574 | 97.0 | 167.54 | 0.193 | 2000 |
| RANDOM-05 / trick_after_turn / seed0 | 2548 | 97.36 | 165.03 | 0.193 | 2000 |
| RANDOM-05 / trick_short_preference / seed0 | 2539 | 96.95 | 164.58 | 0.193 | 2000 |
| RANDOM-05 / trick_after_turn_short / seed0 | 2546 | 97.35 | 162.12 | 0.193 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
