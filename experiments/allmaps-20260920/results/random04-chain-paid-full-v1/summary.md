# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_current_control / seed0 | 2059 | 224.52 | 277.2 | 0.174 | 1000 |
| RANDOM-04 / trick_chain_terminal / seed0 | 1908 | 221.48 | 264.34 | 0.174 | 1000 |
| RANDOM-04 / trick_chain_paid / seed0 | 1714 | 225.71 | 267.59 | 0.174 | 1000 |
| RANDOM-04 / trick_chain_paid_strict / seed0 | 1683 | 222.71 | 286.28 | 0.174 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
