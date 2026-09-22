# RANDOM-01 island cooperation: no improvement

Eight full600-step runs pass independent replay and strict allocation/deadline/memory checks. Both controls reproduce all six archived trajectory/event/schedule fields. Current739 profile, source0bb92cea, fixed512 repairs per island; pickup-heading price and old chain mode remain off. Extra communication/merging overhead is measured.

| Variant | Seed1 | Seed0 | Paired total |
|---|---:|---:|---:|
| Control | 739 | 736 | 1475 |
| Merge compatible components | 737 | 734 | 1471 |
| Two sharing rounds | 732 | 733 | 1465 |
| Merge and two rounds | 732 | 735 | 1467 |

Every alternative loses in paired total. Keep the739 record; this experiment provides no evidence of a throughput ceiling. No fresh inputs were used.

[Independent audit](audit.json), [exact controls](control-equivalence.json).
