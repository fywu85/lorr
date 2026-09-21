# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / generic_selected_control / seed0 | 1484 | 135.92 | 204.95 | 0.083 | 800 |
| RANDOM-03 / generic_chain_ranks / seed0 | 1613 | 139.1 | 191.37 | 0.084 | 800 |
| RANDOM-03 / generic_portfolio8 / seed0 | 1439 | 104.92 | 136.38 | 0.084 | 800 |
| RANDOM-03 / generic_chain_portfolio8 / seed0 | 1543 | 107.97 | 143.55 | 0.084 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
