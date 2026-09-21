# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_selected_control / seed0 | 1188 | 127.47 | 181.41 | 0.065 | 405 |
| RANDOM-02 / trick_geometric_bound / seed0 | 1176 | 128.04 | 188.6 | 0.065 | 408 |
| RANDOM-02 / trick_geometric_mean / seed0 | 1193 | 128.38 | 189.96 | 0.066 | 399 |
| RANDOM-02 / trick_geometric_p90 / seed0 | 1169 | 128.7 | 191.99 | 0.066 | 405 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
