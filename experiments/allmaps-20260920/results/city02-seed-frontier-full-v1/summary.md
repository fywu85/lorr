# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [6, 8, 10, 12].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_current_profile / seed6 | 16719 | 293.87 | 814.97 | 8.511 | 2504 |
| CITY-02 / trick_current_profile / seed8 | 16840 | 309.19 | 802.56 | 8.564 | 2513 |
| CITY-02 / trick_current_profile / seed10 | 16704 | 294.48 | 810.92 | 8.526 | 2505 |
| CITY-02 / trick_current_profile / seed12 | 16741 | 301.97 | 790.04 | 8.531 | 2529 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
