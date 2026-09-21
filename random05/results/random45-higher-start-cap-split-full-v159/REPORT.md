# Higher startup admission caps: RANDOM-04/05

Audited 2026-09-21T23:48:27.685553+00:00. All ten full1000/2000-step cases pass independent replay and strict limits; source307d5cc9/build159. Disabled2782/4242 controls match all six archived trace fields. All eight interventions lose throughput.

| Instance | Startup cap | Duration | Tasks | Difference from control |
|---|---:|---:|---:|---:|
| RANDOM-04 | 620 | 25 | 2761 | -21 |
| RANDOM-04 | 620 | 100 | 2753 | -29 |
| RANDOM-04 | 700 | 25 | 2726 | -56 |
| RANDOM-04 | 700 | 100 | 2756 | -26 |
| RANDOM-05 | 740 | 25 | 4185 | -57 |
| RANDOM-05 | 740 | 100 | 4050 | -192 |
| RANDOM-05 | 800 | 25 | 4151 | -91 |
| RANDOM-05 | 800 | 100 | 4117 | -125 |

Keep the previous admission profiles. Selected2782/4242 remain +5.02%/+33.73% versus matchedmax(NMS,KK)=2649/3172. Higher early admission does not solve the observed startup gap in these trials; this does not prove every startup intervention would fail. No fresh inputs used.
