# TRICK: upper cutoff-margin quantiles with the confirmed matching guard

This bounded follow-up compares prospective p90, p95 and p99 residual-duration
margins. The earlier p50/p75/p90 trial improved toward p90 on both seeds, and the
current policy still spends about2.1% of robot actions on tasks unfinished at the
cutoff. That is a measured work partition, not a recoverable-work or throughput
bound. Higher quantiles may favor tasks more likely to finish; they can also
worsen assignments or return to ordinary ordering when every candidate is in the
same unfavorable tier. No monotonic throughput benefit is assumed.

All three arms use the confirmed native turn1, pickup8, matching cutoff-guard
profile with configured horizon5000. Only the percentile changes. Calibration
uses already completed, unchanged-holder tasks and an immutable prospective
snapshot; no future outcomes or hidden input enter the policy. HRRN, forced-oldest
admission, held/started/primary/recovery protection, one unopened retarget and its
20-step cooldown remain intact. Short preference stays OFF. Both admission and
the whole-cycle guard use the selected quantile. This is an explicit Warehouse
trick requiring --trick WAREHOUSE and disclosed horizon knowledge.

Use frozen source37f592a48bdb24b936f8d0be8be22ddd405b4ddb and V110 binary
3f66eefd2a54589e48965b5b4031cbbe4d67bd8e6423c8cdc1cb3108f24bf641.
Run all six seed0/2 cases for the full5000 steps, concurrently on four distinct
physical cores each, shared-host5s entry deadline, RSS below32decimalGB. Exact
p90 controls must reproduce155173/155090 and their full V110 trajectory hashes.
Preserve full action/task/waiting/resource accounting; no prefix-based ranking.
No production code changes or new quality result are introduced by this profile.

V120 is complete. The exact p90 controls reproduce155173/155090 and their
full hashes. P95 scores155077/155036 (−96/−54), and p99 scores154811/154787
(−362/−303). All losses occur in the final1000steps; both empty work and
unfinished-task work increase. All six full cases, 300million actions, exact
pre-intervention prefixes, fixed work and resource/ownership checks pass.
Retain p90. These results agree with the earlier higher-quantile losses under
pickupweight5 (V98); the new trial includes pickup8 and the matching guard.
[Full report](../results/cutoff-quantiles-full-v120/summary.md).
