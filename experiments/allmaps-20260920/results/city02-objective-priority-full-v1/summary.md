# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_adapted_control / seed0 | 16315 | 254.01 | 818.33 | 8.510 | 2540 |
| CITY-02 / trick_remaining_potential / seed0 | 15233 | 336.45 | 810.66 | 8.524 | 2577 |
| CITY-02 / trick_chain_priority / seed0 | 16013 | 264.89 | 807.65 | 8.494 | 2645 |
| CITY-02 / trick_remaining_chain_priority / seed0 | 14241 | 275.45 | 811.2 | 8.484 | 2651 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
