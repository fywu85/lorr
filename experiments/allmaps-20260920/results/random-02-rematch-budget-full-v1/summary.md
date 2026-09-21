# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [0].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_budget1_control / seed0 | 1160 | 129.1 | 192.03 | 0.065 | 441 |
| RANDOM-02 / trick_budget2 / seed0 | 1160 | 128.82 | 183.04 | 0.065 | 441 |
| RANDOM-02 / trick_budget4 / seed0 | 1160 | 130.0 | 181.63 | 0.065 | 441 |
| RANDOM-02 / trick_budget4_cadence1 / seed0 | 1188 | 128.72 | 183.45 | 0.065 | 405 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
