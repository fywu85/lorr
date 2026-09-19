# Full travel accounting on the graded reference

The ordinary heading/traffic-aware pickup policy cuts total empty robot-steps
by1.63%/1.46% on seeds0/2. Independent replay matches every full empty-motion
counter, passes the hand-counted release/reassignment fixture, and verifies the
same map bytes as all four simulator inputs. Distances are exact unweighted
free-cell shortest paths, ignoring turns and traffic; this is diagnosis only.

| Policy | Seed | Tasks | Empty robot-steps | Completed chain mean | Loaded elapsed mean |
|---|---:|---:|---:|---:|---:|
| Graded pickup OFF |0|138402|10124626|242.33|278.16|
| Graded pickup ON |0|138963|9959933|242.55|278.18|
| Graded pickup OFF |2|139303|10220869|242.32|275.60|
| Graded pickup ON |2|139677|10071201|242.60|275.98|
| Local KittyKnight |reference|152981|10049537|233.84|252.52|

The pickup improvement accompanies shorter empty travel, with little change in
loaded elapsed time or completed chain length. Whole empty travel is already
near the local leader total. The leader completes shorter chains and spends
less elapsed time per loaded task. Different selected/completed cohorts prevent
a causal additive split, and unfinished tasks censor per-completion averages.
KittyKnight used38.858GB versus our32GB cap; this is not an official equal-resource
comparison. Its frequent reassignments make last-assignment pickup delay an
unreliable measure of total empty travel; the table uses whole empty robot-steps.

[All four runs and provenance](results/graded-pickup-travel-v1/),
[compact comparison](results/graded-pickup-travel-v1/comparison.json),
[full quality/time/memory checks](results/graded-pickup-flow-full-v41/comparison.json).
Further decomposition of complete-field profiles16/32/64 is running separately.
