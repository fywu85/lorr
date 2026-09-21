# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| SORTATION / trick_selected_control / seed0 | 150333 | 449.4 | 991.13 | 12.457 | 660 |
| SORTATION / trick_age_pickup8_match64 / seed0 | 150894 | 446.92 | 993.9 | 12.423 | 656 |
| SORTATION / trick_direct_pickup4_match64 / seed0 | 150081 | 432.44 | 982.84 | 12.371 | 4031 |
| SORTATION / trick_direct_pickup8_match64 / seed0 | timeout | n/a | n/a | 1.493 | n/a |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
