# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| CITY-02 / trick_current_control / seed0 | 16315 | 253.12 | 821.75 | 8.511 | 2540 |
| CITY-02 / trick_lane24 / seed0 | 16830 | 314.64 | 809.83 | 10.284 | 2502 |
| CITY-02 / trick_lane32 / seed0 | 16526 | 276.19 | 815.1 | 8.509 | 2524 |
| CITY-02 / trick_lane48 / seed0 | 16615 | 290.16 | 809.28 | 8.520 | 2540 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
