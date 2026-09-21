# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2, 4].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_budget1_control / seed2 | 1113 | 128.8 | 181.03 | 0.066 | 480 |
| RANDOM-02 / trick_budget4_cadence1 / seed2 | 1189 | 130.95 | 184.53 | 0.065 | 443 |
| RANDOM-02 / trick_budget1_control / seed4 | 1135 | 128.04 | 183.6 | 0.066 | 435 |
| RANDOM-02 / trick_budget4_cadence1 / seed4 | 1197 | 128.68 | 183.38 | 0.066 | 411 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
