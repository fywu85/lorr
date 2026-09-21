# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_parts4_round12_control / seed0 | 2915 | 867.39 | 969.12 | 0.194 | 2000 |
| RANDOM-05 / trick_parts8_round12 / seed0 | 2874 | 495.05 | 557.06 | 0.197 | 2000 |
| RANDOM-05 / trick_parts8_round16 / seed0 | 2946 | 709.01 | 799.65 | 0.198 | 2000 |
| RANDOM-05 / trick_parts8_round16_peak / seed0 | 2918 | 648.47 | 735.13 | 0.198 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
