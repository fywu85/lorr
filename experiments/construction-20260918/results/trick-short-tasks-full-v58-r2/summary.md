# Short-task preference loses in this CGAR comparison

| Policy | Seed 0 | Seed 2 | Mean |
|---|---:|---:|---:|
| Control | 144,510 | 144,107 | 144,308.5 |
| Short-task preference | 136,833 | 134,580 | 135,706.5 |
| Lane guidance | 146,659 | 146,566 | 146,612.5 |
| Both | 137,985 | 137,934 | 137,959.5 |

Removing the age preference and forced-oldest admission reduces throughput by 5.961% without lanes and 5.902% with lanes. The average interaction is -51 tasks; the individual interactions are -997 and +895, so these two seeds do not support a consistent large interaction.

| Short-task change | No lanes | With lanes |
|---|---:|---:|
| Empty robot steps | +34.31% | +32.83% |
| Completed chain mean | -3.13% | -3.13% |
| Unpicked after at least 4,000 steps | 1,358 / 1,356 | 1,390 / 1,392 |

Both controls have zero unpicked tasks aged at least4,000 steps. Unassigned mean age rises from about100 to about2,700 steps. The replay conserves all50million robot steps per run and revealed = completed + outstanding original chain distance at every checkpoint. Completed-chain means refer to different selected cohorts; the per-run inventory identity does not imply a cross-policy one-for-one causal decomposition.

All eight full5,000-step runs and40,000 complete decisions are valid under the authorized shared-host5s development deadline, with fixed search work and process RSS below11.945GB. Maximum observed entry time is1.498199s on the slow control. This is not strict1s certification. The generic-control and lane-only trajectory hashes exactly reproduce their prior full runs. Source[f939b3b](https://github.com/fywu85/lorr/commit/f939b3b1ab44368e83617a9f97300f26c4063210).

Keep the new component OFF. The earlier lane-only result remains separate from the generic frontier. This experiment measures the effect inside CGAR; it cannot assign a fraction of NMS/KK advantage to task preference because their rematching, metrics and motion planning also differ.

[Verification](factorial-verification.json), [effects](effects.json), [task pool and travel](task-pool-and-travel.json), [scheduler counters](scheduling-counters.json).
