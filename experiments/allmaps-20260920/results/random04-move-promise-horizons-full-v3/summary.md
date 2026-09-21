# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-04 / trick_promise2_h20_control / seed0 | 2237 | 261.18 | 308.84 | 0.174 | 1000 |
| RANDOM-04 / trick_promise2_h15 / seed0 | 2212 | 247.13 | 289.44 | 0.174 | 1000 |
| RANDOM-04 / trick_promise2_h25 / seed0 | 2234 | 264.63 | 313.7 | 0.174 | 1000 |
| RANDOM-04 / trick_promise2_without_futures / seed0 | 2239 | 219.96 | 278.68 | 0.118 | 1000 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
