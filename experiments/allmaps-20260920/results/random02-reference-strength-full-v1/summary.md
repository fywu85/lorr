# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_control / seed2 | 1215 | 279.41 | 362.2 | 0.143 | 440 |
| RANDOM-02 / trick_reference_strength25 / seed2 | 1172 | 302.75 | 380.06 | 0.140 | 444 |
| RANDOM-02 / trick_reference_strength50 / seed2 | 1189 | 289.43 | 369.24 | 0.142 | 447 |
| RANDOM-02 / trick_reference_strength75 / seed2 | 1212 | 289.2 | 378.82 | 0.145 | 495 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
