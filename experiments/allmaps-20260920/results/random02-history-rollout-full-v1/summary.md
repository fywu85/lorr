# Controlled CGAR factor matrix

Archived inputs and declared planner seeds, frozen factor profiles. Throughput comparisons require full horizons. Shared hosts and32decimalGB; exact enforced entry limit is recorded above. Simulator validates decisions; complete movement counters and waiting events are reconciled, not an independent full-action replay. No matched NMS or SoTA claim.

Enforced entry limit: 1000ms. Planner seeds: [2].

| Instance / variant / seed | Tasks | Mean ms | Max ms | RSS GB | Outstanding age p90 |
|---|---:|---:|---:|---:|---:|
| RANDOM-02 / trick_current_control / seed2 | 1215 | 273.09 | 347.81 | 0.141 | 440 |
| RANDOM-02 / trick_refresh_keep6 / seed2 | 1175 | 293.2 | 357.34 | 0.139 | 421 |
| RANDOM-02 / trick_refresh_keep10 / seed2 | 1210 | 292.59 | 360.58 | 0.140 | 417 |
| RANDOM-02 / trick_refresh_keep14 / seed2 | 1181 | 297.07 | 357.86 | 0.139 | 443 |

Throughput is primary; fairness is a secondary reported metric. The separate RANDOM-05 solver is untouched.

[Verification](verification.json), [paired effects](paired-results.json), [waiting](fairness.json), [regional work](regional-work.json).
