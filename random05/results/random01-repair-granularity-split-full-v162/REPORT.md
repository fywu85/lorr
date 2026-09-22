# RANDOM-01 repair granularity: no improvement

All 12 original full runs pass independent replay and strict limits. The seed1/0 eight-agent controls reproduce 739/736 in all six trace fields. All alternatives are worse in aggregate; mixed groups tie seed0 only.

| Groups | Iterations per island | Seed1 | Seed0 | Paired total |
|---|---:|---:|---:|---:|
| group8 | 512 | 739 | 736 | 1475 |
| group1 | 4096 | 691 | 692 | 1383 |
| group2 | 2048 | 731 | 726 | 1457 |
| group4 | 1024 | 727 | 730 | 1457 |
| group16 | 256 | 725 | 727 | 1452 |
| mixed8 | 512 | 735 | 736 | 1471 |

All cases request 4,096 agent repairs per island and step. Mixed groups cycle through 4/4/8/16 agents. This controls nominal requested work, not expansions or wall time. Every selected-profile setting except group size/iterations/mixing remains fixed, including annealing0.25. No new held-out stream was used.

Keep the selected739 result (+6.79% versus matched max(NMS, KK)=692). The result rejects these repair variants; it does not establish a throughput ceiling. Next focus: task assignment and task-to-task transitions, guided by current-trace diagnosis.

[Frozen protocol](../../experiments/random01-repair-granularity-v162-protocol.json), [independent audit](audit.json), [exact controls](control-equivalence.json).
