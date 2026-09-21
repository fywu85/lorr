# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / generic_selected_control / seed0 | 1084 | 133.48 | 189.54 | 0.065 | 552 |
| RANDOM-02 / generic_chain_ranks / seed0 | 1080 | 134.81 | 189.89 | 0.066 | 570 |
| RANDOM-02 / generic_portfolio8 / seed0 | 1083 | 71.97 | 124.74 | 0.065 | 552 |
| RANDOM-02 / generic_chain_portfolio8 / seed0 | 1075 | 69.31 | 109.49 | 0.066 | 571 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
