# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_turn20_control / seed4 | 1197 | 131.58 | 179.87 | 0.065 | 411 |
| RANDOM-02 / trick_turn12 / seed4 | 1159 | 127.89 | 186.44 | 0.066 | 449 |
| RANDOM-02 / trick_turn16 / seed4 | 1181 | 130.6 | 193.58 | 0.065 | 445 |
| RANDOM-02 / trick_turn24 / seed4 | 1028 | 128.88 | 183.13 | 0.065 | 524 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
