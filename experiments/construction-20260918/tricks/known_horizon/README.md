# TRICK: configured-known-horizon admission

Requires --trick WAREHOUSE and CGAR_TRICK_KNOWN_HORIZON=5000. This explicitly assumes
the evaluation horizon is known; SharedEnvironment does not supply it. No claim is
made that competitors knew it. The extra receipt says assumption=configured.
Zero is OFF. Malformed/out-of-range settings and any setting without the CLI gate
are rejected. At/past the configured horizon, ordinary ordering resumes and logs once.

For NEW assignments to free robots only, rank pairs whose lower bound exceeds
H-now after other pairs. Keep the original HRRN/cost/tie order within each tier.
Keep the original endpoint-bucket ordering and forced-oldest admission comparator.
Do not drop tasks, alter held/started/primary/recovery assignments, change matching,
or park robots when every candidate is impossible. A rank is not a completion guarantee.

Bound=max(1,spatial pickup)+original spatial chain+exactly repeated consecutive
stops. The initial same-cell errand and each subsequent repeated errand need one
tick. Spatial pickup uses a const cache peek or Manhattan; no table build or LRU
mutation. Full core==free is required because ordinary CGAR tables can exclude
pockets. Refined/flow chain pricing is excluded. Native weighted costs never enter
this physical lower bound. Equality with remaining ticks stays feasible.

V86 complete regression suite passes:28 metric/table/horizon-boundary profiles,
five repeated-service cases, ordinary age ranking at/past the horizon, independent
oldest admission, held/started protection, no simulator metadata mutation, and ten
activation/basis guards. Source/binary are bound in build-provenance/v86.

Startup plan: three200-step seed0 profiles H=0/200/5000 under identical best native
bands+matching64+pickupweight5+ordinary fairness. H0/H5000 must exactly reproduce
V81; H200 deliberately exercises active ranking but is not a quality score.
A separate generic800-step no-trick control must reproduce18958 and its full hash.
After these pass, compare H0/H5000 on seeds0/2, all full5000steps, four concurrent
processes/four bound physical cores each,32decimalGB RSS/shared5s deadline. Exact
H0 controls must reproduce V80 (152383/152048). All action/schedule prefixes before
the first logged comparator reversal must match their controls. This includes
shortlist heap reversals, not only final assignment ordering. Report all outcomes,
final windows, unfinished work, ages and late latency. A valid full new peak counts
under the user's criterion; broader recommendation depends on both seeds.

The saved audit finds about4% unfinished-task work, not recoverable work. A separate
saved-trajectory lower-bound/opportunity audit will classify impossibility versus
slack. It is descriptive: trajectories, pool replenishment and future opportunities
change after a new policy, so its count is not a throughput bound. No calibrated
middle tier or feasibility-aware matching is part of this first mechanism.

The bound concerns completion by the current proposed holder. Later unopened
matching can rescue an initially impossible pair. The saved-audit first draft
incorrectly asserted no completed task could have had such an initial pair; it
failed on real data. The corrected audit checks every completed task's FINAL
holder bound and separately counts rescued initial pairs. Production ranking
is unchanged; no task is dropped or declared globally impossible.

Full-run decision, 2026-09-20 12:04 UTC: all three startup cases and the generic800
control pass with exact reference hashes. The corrected saved audit finds4024/
3980 chronological task-disjoint feasible-alternative witnesses on seeds0/2,
against9285/9232 unfinished late admissions whose initial-holder bound exceeds
remaining time. The still-feasible-but-unfinished counts are669/737. The sample
is large enough to justify the declared full OFF/ON matrix; it is not a predicted
gain. Exactly one completed late task per seed was rescued from an initially
impossible pair by retargeting; all completed FINAL-holder bounds pass.

V87 review hardening: reject cost-key1 with nonzero known horizon, because its
retention baseline differs from ordinary HRRN and would invalidate the first-
change diagnostic. The full V86 matrix already fixes cost-key0; its source and
results remain unchanged. New independent shelf fixture has Manhattan4 but BFS6
plus one repeated service: cached versus absent spatial tables change the tier.
A forced empty-shortlist fixture exercises sampled fallback at exact feasibility.
Both and the full suite pass; no additional policy factor is introduced.

Full result verified2026-09-20 12:43:45UTC:153614/153220, exact152383/152048 controls, gains1231/1172. All20kentries valid, exact pre-reversal prefixes3843/3828. New peak153614, targetgap1181. See results/known-horizon-full-v86/summary.md. No generic promotion or strict1s certification. Saved motion/work/bound replay follows before another policy decision.
