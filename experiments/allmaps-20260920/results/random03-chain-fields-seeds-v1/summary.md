# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_generic_chain_control / seed2 | 1794 | 151.65 | 200.46 | 0.084 | 800 |
| RANDOM-03 / trick_kk_chain_pickup4 / seed2 | 1763 | 143.95 | 198.0 | 0.084 | 800 |
| RANDOM-03 / trick_generic_chain_control / seed4 | 1614 | 140.37 | 187.93 | 0.084 | 800 |
| RANDOM-03 / trick_kk_chain_pickup4 / seed4 | 1684 | 137.01 | 195.16 | 0.084 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
