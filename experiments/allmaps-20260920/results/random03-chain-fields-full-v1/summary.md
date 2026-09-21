# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_generic_chain_control / seed0 | 1613 | 138.69 | 192.07 | 0.084 | 800 |
| RANDOM-03 / trick_kk_chain / seed0 | 1841 | 144.61 | 202.9 | 0.084 | 800 |
| RANDOM-03 / trick_nms_chain / seed0 | 1716 | 141.39 | 205.79 | 0.084 | 800 |
| RANDOM-03 / trick_kk_chain_pickup4 / seed0 | 1890 | 144.8 | 197.97 | 0.084 | 782 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
