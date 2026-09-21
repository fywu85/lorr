# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current2048_control / seed4 | 1144 | 297.04 | 391.88 | 0.140 | 496 |
| RANDOM-02 / trick_current2048_prefix / seed4 | 1202 | 295.27 | 361.25 | 0.143 | 412 |
| RANDOM-02 / trick_current4096_prefix / seed4 | 1199 | 404.28 | 499.74 | 0.144 | 438 |
| RANDOM-02 / trick_uniform2048_prefix / seed4 | 1063 | 312.16 | 381.46 | 0.142 | 536 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
