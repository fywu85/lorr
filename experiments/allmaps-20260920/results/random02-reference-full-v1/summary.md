# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_generic_control / seed0 | 1084 | 133.58 | 192.18 | 0.066 | 552 |
| RANDOM-02 / trick_uniform_native / seed0 | 1081 | 130.44 | 188.33 | 0.065 | 550 |
| RANDOM-02 / trick_nms_arrows / seed0 | 1123 | 128.88 | 171.14 | 0.065 | 526 |
| RANDOM-02 / trick_kk_forward / seed0 | 1120 | 130.32 | 178.96 | 0.066 | 548 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
