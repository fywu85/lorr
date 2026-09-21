# Selected full-run movement accounting

Updated 2026-09-21T13:53:26.075022+00:00. Every robot action is reconciled to robots times full horizon.
These shares describe the current selected seeds; cross-row or historical changes
are not controlled causal effects or estimates of avoidable work.

| Instance | Tasks | Forward % | Turn % | Wait % | Before pickup % | Loaded wait % | Idle % |
|---|---:|---:|---:|---:|---:|---:|---:|
| WAREHOUSE | 155173 | 93.98 | 3.03 | 2.99 | 18.64 | 2.28 | 0.00 |
| SORTATION | 150894 | 96.30 | 2.57 | 1.12 | 25.09 | 0.67 | 0.00 |
| CITY-01 | 8440 | 94.07 | 5.21 | 0.73 | 4.18 | 0.68 | 0.00 |
| CITY-02 | 16839 | 93.03 | 5.42 | 1.55 | 4.15 | 1.46 | 0.00 |
| GAME | 24447 | 45.79 | 51.37 | 2.84 | 1.67 | 1.85 | 50.00 |
| RANDOM-01 | 693 | 75.28 | 24.23 | 0.49 | 13.02 | 0.39 | 0.00 |
| RANDOM-02 | 1215 | 68.71 | 30.08 | 1.21 | 10.45 | 1.03 | 0.00 |
| RANDOM-03 | 1982 | 55.44 | 40.65 | 3.91 | 10.38 | 3.49 | 0.00 |
| RANDOM-04 | 2321 | 44.85 | 32.26 | 22.89 | 16.77 | 19.10 | 0.00 |
| RANDOM-05 | 3610 | 35.89 | 31.27 | 32.85 | 10.47 | 22.93 | 14.24 |

CITY01/02 and SORTATION already spend most actions moving forward. Route length
and task selection remain stronger hypotheses than removing stationary waits.
RANDOM02/03 spend a large share turning, while RANDOM04 still combines frequent
turns and waits. Evaluate coordinated route changes rather than assuming all of
those actions are avoidable. GAME limits its active fleet; RANDOM05 has capacity
parking and an admission cap. Those are labelled tricks, not fairness guarantees.

[Exact source commits and counter provenance](selected-movement-20260921-1353.json). Historical accounting
is preserved in [the earlier snapshot](selected-movement-20260921.json).
