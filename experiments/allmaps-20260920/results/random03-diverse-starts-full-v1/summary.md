# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-03 / trick_one8m_control / seed0 | 1902 | 188.45 | 297.63 | 0.085 | 800 |
| RANDOM-03 / trick_two4m / seed0 | 1859 | 148.73 | 198.33 | 0.085 | 800 |
| RANDOM-03 / trick_four2m / seed0 | 1741 | 119.77 | 162.9 | 0.085 | 800 |
| RANDOM-03 / trick_four8m / seed0 | 1724 | 192.02 | 321.26 | 0.084 | 800 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
