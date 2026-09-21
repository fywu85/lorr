# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_control / seed4 | 1197 | 128.7 | 180.16 | 0.066 | 411 |
| RANDOM-02 / trick_chain_score / seed4 | 1061 | 127.17 | 192.32 | 0.108 | 496 |
| RANDOM-02 / trick_order2_control / seed4 | 1109 | 127.78 | 177.83 | 0.065 | 495 |
| RANDOM-02 / trick_chain_both / seed4 | 1025 | 130.06 | 193.88 | 0.108 | 553 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
