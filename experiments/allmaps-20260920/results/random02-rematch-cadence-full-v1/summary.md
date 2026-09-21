# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_budget4_cadence1_control / seed0 | 1188 | 129.49 | 190.47 | 0.066 | 405 |
| RANDOM-02 / trick_budget1_cadence1 / seed0 | 1183 | 127.94 | 192.36 | 0.065 | 413 |
| RANDOM-02 / trick_budget2_cadence1 / seed0 | 1188 | 128.9 | 184.96 | 0.065 | 405 |
| RANDOM-02 / trick_budget8_cadence1 / seed0 | 1188 | 128.62 | 191.04 | 0.066 | 405 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
