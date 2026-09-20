# TRICK: empirical completion-margin percentile

An optional replacement for the five-bucket mean excess duration in the existing
known-horizon policy. Requires --trick WAREHOUSE, a positive configured horizon,
CGAR_TRICK_HORIZON_MARGIN=1 and CGAR_TRICK_HORIZON_MARGIN_PERCENTILE in1..100.
Missing/zero percentile preserves the rational-mean policy and does not allocate
histogram nodes. No general solver default changes.

Only already completed tasks with unchanged first accepted holders train the
estimator. It uses the same final-proposal physical bound and five bound buckets
as the current winner. Each bucket stores exact duration-minus-bound counts.
Once per scheduling entry, rank ceil(n*p/100) selects an observed excess; every
candidate pair uses that immutable five-value snapshot. Delays are not clipped.
No incomplete-task duration or future completion trains a bucket. Empty buckets
use zero margin. A percentile is a heuristic, not a calibrated success guarantee;
completed-only sampling, traffic changes and coarse buckets remain limitations.

Keep the three original tiers and their ordinary HRRN/cost/id ties. Forced oldest
admission bypasses horizon ordering. Held/started task protection, finite retarget
budget, CGAR primary/recovery, all-impossible assignment, and ordinary behavior
at/past the configured horizon remain unchanged. The explicit short-task trick
stays OFF. The meaning of a percentile is purely observed completion duration;
it does not change temporal-search work or silently return a partial plan.

Tests: exact nearest ranks on skewed duplicated samples in all five buckets,
inclusive boundaries, no unfinished or retargeted training, immutable snapshots,
activation and reconfiguration guards. Existing real scheduling/fair-admission
checks cover mean and50/75/90; real native matching calibrates the final accepted
holder in percentile mode. The complete CGAR regression suite passes in V96.

Experiment: active200-step configuredH200 screen, exact mean control againstV92;
independent generic800-step exact control; then full5000-step/10000-robot paired
seeds0/2 for mean,median,p75,p90 on32bound physical GRID cores (4each). All complete
fixed work, shared5s development,32decimalGB. Full mean controls must reproduce
154889/154556 and their complete trajectories. Report task-age tails as well as
throughput. No quality claim from startup screens; no promotion before full data.

## Full results and refinement

V96 full comparison is verified: mean154889/154556, median154767/154466,
p75154991/154691, p90155057/154727. p90 improves both seeds168/171, preserves
ordinary fairness and raises the best to155057. All8full cases pass; both mean
controls and pre-intervention prefixes are exact. Gains are entirely in the last
1000steps. [Full evidence](../../results/horizon-percentile-full-v96/summary.md).

The next fixed comparison uses p90 as its exact repeated control against p95/p99
on seeds0/2, with two additional p90 seeds1/3. Same frozen V96 binary and source,
4bound physical cores per process, shared5s/32decimalGB. The analyzer freezes
the verified p90 reference and checks its complete trajectories, avoiding
an unnecessary repeat of the already reproduced mean baseline. No value above
p90 is promoted before its full results and waiting tails are checked.
