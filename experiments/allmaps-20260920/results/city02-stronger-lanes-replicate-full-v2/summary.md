# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_current_control / seed2 | 16199 | 250.97 | 836.87 | 8.551 | 2543 |
| CITY-02 / trick_lane24 / seed2 | 16754 | 293.01 | 810.43 | 8.525 | 2527 |
| CITY-02 / trick_current_control / seed4 | 16117 | 248.99 | 813.99 | 8.496 | 2556 |
| CITY-02 / trick_lane24 / seed4 | 16839 | 312.99 | 804.54 | 8.502 | 2494 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
