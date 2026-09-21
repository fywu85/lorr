# Selected full-run movement accounting

Shares below use every robot action in each selected full run. Their sum is
independently reconciled to robots times horizon. They describe where actions go;
they are not causal throughput ceilings or measured gains from eliminating waits.

| Instance | Tasks | Forward % | Turn % | Wait % | Before pickup % | Loaded wait % | Idle % |
|---|---:|---:|---:|---:|---:|---:|---:|
| WAREHOUSE | 155173 | 93.98 | 3.03 | 2.99 | 18.64 | 2.28 | 0.00 |
| SORTATION | 150894 | 96.30 | 2.57 | 1.12 | 25.09 | 0.67 | 0.00 |
| CITY-01 | 8440 | 94.07 | 5.21 | 0.73 | 4.18 | 0.68 | 0.00 |
| CITY-02 | 16315 | 92.32 | 5.95 | 1.74 | 4.04 | 1.65 | 0.00 |
| GAME | 24447 | 45.79 | 51.37 | 2.84 | 1.67 | 1.85 | 50.00 |
| RANDOM-01 | 693 | 75.28 | 24.23 | 0.49 | 13.02 | 0.39 | 0.00 |
| RANDOM-02 | 1215 | 68.71 | 30.08 | 1.21 | 10.45 | 1.03 | 0.00 |
| RANDOM-03 | 1982 | 55.44 | 40.65 | 3.91 | 10.38 | 3.49 | 0.00 |
| RANDOM-04 | 2059 | 38.02 | 36.84 | 25.14 | 14.81 | 21.53 | 0.00 |
| RANDOM-05 | 3065 | 28.82 | 33.03 | 38.15 | 19.73 | 26.86 | 6.23 |

CITY01/02 spend most actions moving forward; wider matching does not address
that dominant work. Task selection and route length are stronger next hypotheses
than removing stationary waits alone. RANDOM01/02/03 spend more on rotations;
oriented route/chain choices can matter even where waiting is low. Crowded maps
require separate analysis of coordinated motion and occupancy. GAME intentionally
limits task receivers; its idle share is a declared trick, not a fairness claim.

[Exact source commits and counter provenance](selected-movement-20260921.json).
