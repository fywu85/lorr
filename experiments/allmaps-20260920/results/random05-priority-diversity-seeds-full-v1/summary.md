# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-05 / trick_workers8_control / seed2 | 2865 | 559.11 | 622.85 | 0.194 | 2000 |
| RANDOM-05 / trick_workers32_equal / seed2 | 2900 | 585.82 | 727.3 | 0.195 | 2000 |
| RANDOM-05 / trick_workers8_control / seed4 | 2836 | 559.67 | 619.77 | 0.193 | 2000 |
| RANDOM-05 / trick_workers32_equal / seed4 | 2889 | 641.43 | 821.48 | 0.196 | 2000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
