# Persistent Fable review: independent assessment

Turn01 completed successfully on `claude-fable-5-1`, effort max, in the retained
session `1ebb1075-3538-49d1-93d1-a00c94fa256a`. The CLI reports two turns and
$7.7531845. The review is preserved verbatim; it is external advice, not a proof.

The tied-heading wait-seed rotation expressions match the actual adapter and
warrant an independent regression and controlled ablation. No throughput effect
is established yet. The cache/fallback confound in periodic refresh also needs a
separate control; the first full refresh result does not identify that mechanism.

Three review assertions need correction. Every compared flow profile explicitly
uses equal robot weights, so low rank does not shrink robot score weights. The
cumulative-field regression proves fields can become neutral and reverse; an
irreversible ratchet is not a code invariant. Outstanding-task age includes time
waiting unassigned, so it does not date a robot jam. Small gains from previously
tested search policies also do not prove all search neighborhoods are saturated.

The follow-up supplies these corrections, the independent connected-graph/motion
audit and the134511-task refresh result to the SAME session, without resending
unchanged source. No map-specific traffic template has been adopted.


Turn02 completed in the same verified session, one CLI turn,179.227seconds,
$1.79438025. Its5337-byte update contained ZERO changed source files. Fable
explicitly retracts the incorrect rank-weight, irreversible-ratchet and age-onset
claims, and weakens the search-saturation claim. It recommends isolating mature
field quality from cache flushes, and measuring multi-blocker rejections before
implementing a branching search. The second review is preserved verbatim.

Two cautions remain. Our opposite-turn counter permits intervening waits; it is
not an immediate-reversal count. A late-field transplant would use future-run
observations and is diagnostic only, never an eligible best-throughput entry or
a map-specific production policy. A flush-only control can isolate the cache
mechanism without introducing future observations.

V32 now independently reproduces128 tied-heading rotations on a blocked-robot
kernel fixture under4rotations/4cost scales. Strict improvement prevents these
rotations while retaining16useful orientation improvements. Full production
fixtures validate9600serial/four-worker actions with protection, warm reuse and
refreshed flow; rotation-attribution totals match executed actions. All regression
checks pass. The short production screen passes, including exact default equivalence. Seed2 full summaries show strict mode reaches121534 against70171 frozen control, but133672 against134519 with refresh512. Independent full trajectory analysis and seed0 remain pending; no additive gain is established.


Turn03 completes in the same session: one turn,397.255seconds,$3.95705,
12582bytes and four changed source excerpts. It supports a cache-only control,
and points out that a missing turn table disables BOTH weighted candidate scoring
and wait-seed orientation. The fixed-field control therefore tests the combined
cache effect; strict-mode arms help separate tied rotations. Seed0 is the clean
comparison because frozen-legacy seed2 already collapses. Genuine refresh logs
must confirm actual reset schedules. Exact reproduction and fixed work remain
required. A cache-only score near122k on seed2 is merely another recovery; it
would not reproduce the134.5k refresh gain.

We do not adopt the review's statement that matching refresh makes "the learned
field a red herring": cache-only still uses the FIRST learned field, so equality
would implicate later changes, not prove the initial field unnecessary. The proposed
5%,25% and one-third thresholds are heuristic screening suggestions, not established
statistical bounds. Planned follow-ups: interval256 and an offline congestion/dwell
audit before adding new costs. Scheduler-metric and branching suggestions remain
hypotheses; cached-only scoring may introduce cache-availability bias.


Turn04 completes in the same session: one turn,325.870seconds,$6.29122725,
48383payload bytes and five changed source excerpts. It retracts the claim that
tied seed turns explain most collapsed-run rotations: these are only0.62% of
late-window turns. It also accepts that actual traffic updates outperform a
cache-only reset on healthy seed0. Seed2 full confirmation is still pending.

The audit correctly labels its single best first-forward candidate, but that is
not the easiest improving candidate to repair. Before using it to choose a
branching policy, count the least-conflicted alternative over all improvements,
separate at least one physical step of cost from operation tie terms, and sample
at stride67 so refresh512 is not always observed at the same offsets. Its
reservations are the completed plan, not rejected search attempts. A free local
improvement is evidence of opportunity in that plan, not a measured task gain.

The suggested direct-cost assignment is already implemented. It slightly lost
in the older no-flow50k-repair study (107083 versus107457 on seed0), so a gain
is a hypothesis, not established evidence. Test it under refreshed flow with
and without existing chain-cost refinement to expose stale underestimate bias.
The task-length and empty-travel comparisons use different completed cohorts;
Fable's numerical split is illustrative and cannot establish that one-third
to one-half of the causal gap lies in assignment. Unopened-task exchange with
the free pool remains a separate hypothesis requiring simulator-contract and
protection checks. No new reassignment behavior has been enabled.


Turn05 completes in the same session: one turn,243.802seconds,$7.90448025,
17973payload bytes and three changed/new source excerpts. It prefers exchanging
unopened assignments with free tasks, and correctly identifies that a shorter
chain alone can pay for a longer pickup under the proposed total-cost rule.
Require an independent pickup-distance saving and consistent distance bases.
The simulator permits releasing unopened tasks; it rejects changing started tasks.
An incoming task and the released task must retain age and finite-retargeting
protections. Assignment validation alone does not establish liveness.

Several claims need qualification. Snapshot counts are not a1.75% hard ceiling
on a closed-loop throughput effect; removing a blockage can change later traffic.
The proposed transaction pass preserves the existing pinned robots and recovery
mechanism rather than altering the certificate itself. A pickup-saving condition
still changes the task mix, so it does not eliminate that confound. It also does
not price the released task's future pickup exactly; the suggested guards remain
a heuristic requiring full evaluation. Offline credited savings cannot be treated
as achieved system-wide benefit.

Fable's direct-cost prediction is now falsified: full HRRN0 loses2.559%, and
chain refinement preserves both HRRN0 trajectories exactly. HRRN stays enabled.
The bounded two-owner transaction prototype was developed independently while
this review ran. Its immutable input, separate reservations, undo journal, locked
placed robots and complete-score acceptance address the atomicity concerns.
One-owner and two-owner arms will measure the neighborhood effect directly;
no throughput result is inferred from kernel tests or the advisor's prediction.


Turn06 completes in the SAME session: one turn,391.765seconds,$10.10757575,
29493payload bytes and five changed/new source excerpts. The read-only source
review finds no counterexample to transactional atomicity, reservation ownership,
undo, fixed protection or fail-closed work/deadline behavior. It did not execute
code. It correctly distinguishes the one-owner transaction neighborhood from
the earlier one-owner repair: sibling backtracking, terminal aggregate acceptance
and recursion all change. Only the branch2/branch1 contrast isolates owner count.

The suggested comparison of a reconstructed score against a freshly summed
baseline avoids a possible fractional-rank floating-point drift mismatch. No
counterexample was supplied, and the tested equal-weight integer scores are
exact. Add a meaningful fractional-power regression before changing that check.
Multi-root commit/rollback/unlock coverage, edge-only conflicts and deeper size
cutoffs are useful next regression targets. The overlap estimate for root rank
windows is approximate; changing rank order prevents interpreting it as an exact
per-robot waiting bound. Optional pruning/cursor changes remain hypotheses.

The full transaction result is small: one-owner mean+0.262%, two-owner+0.339%;
two-owner loses to one-owner on seed0. The remaining four seeds are being tested.
The pickup5 seed0 record136149 also accompanies a seed2 late collapse to52444;
it cannot establish a robust pickup-weight benefit. Neither local scalar gains
nor independent pickup guards establish full-run system throughput or liveness.


Turn07 completes in the SAME session:286.965seconds,$12.4088865,49393payload
bytes and three changed excerpts, source24a576c. It found no concrete pool
safety counterexample and executed no code. The shared eligibility, proposed-ID
availability, one-to-one matching, exact BFS domain and both cost guards match
the implementation. A mid-pass timeout can leave private retarget records changed;
this is permitted only because Entry rejects the whole run. Timeout recovery
within a continuing run is not supported or claimed.

The pending-primary uncertainty is resolved by direct source inspection:
select_primary excludes only absent/reached goals and parked robots, and does
NOT skip locked or recovery robots. Started episodes excluded by the pool pass
are already immutable. The current and oldest unopened pending episodes stay
protected. The proposed locked-oldest counterexample therefore does not apply.

The claim that exchange does not weaken released-task fairness is not proven:
release changes waiting time and task mix even though the existing HRRN and
finite-retarget mechanisms remain. Likewise the finite task record prevents
repeated unopened-task redirection, not every conceivable robot-level cycle.
Static pickup distance ignores heading/tolls and future released-task costs;
no end-to-end throughput guarantee follows. Additional authoritative round-trip,
fresh-assignment contention and guard-boundary tests would improve coverage.
Full simulator runs already invoke its validator; complete outcomes are pending.

The v38 transaction suite independently covers two commit/rollback/reuse cases
(equal and fractional weights), four later-work-exhaustion cases retaining an
earlier complete commit, an edge-only reverse collision, exact displacement-size
cutoffs and512tiny instances enumerating57200candidate combinations.211returned
changes pass independent collision/protection/fresh-score replay. No fractional
score failure was reproduced; the production comparison has not been changed.


## Turn08: complete table admission and next direction

The same Fable5.1 max-effort session completed a read-only v40 source review at
2026-09-19T15:34:44UTC. It read five changed excerpts (92771-byte payload), ran
no tests and found no reproducible correctness counterexample. Its earlier
pending-primary concern is withdrawn. Actual reported cost is16.015591USD and
duration612.547s; the CLI's requested12USD setting did not act as a strict cap.
Visible findings and metadata are archived in turn08; raw protocol stays ignored.

Direct source checks support deterministic rank/admission, immutable compute
inputs, separate worker scratch, no mid-step trimming and safe candidate pointer
lifetimes. Mode2's admission refresh every32steps is an existing optional policy;
production uses mode1, so it does not explain current throughput. Changing mode2
is not a necessary production fix. Table insertion can exceed the configured
cache accounting until the next plan's trim, but measured RSS remains the
controlling32GB requirement. The counter prefetched_hits now includes intentional
batch admissions; do not label that counter speculative usefulness when both
features are enabled. The claimed universal exception guarantee needs qualification:
per-item compute failures and thread-creation failures are caught/joined, but a
worker's tiny initial bucket allocation sits outside its per-item catch. No such
allocation failure was observed; no broad all-exceptions proof is claimed.

Existing oracle tests cover eviction equivalence and an already-expired deadline;
they do not force timeout during a running chunk. The v40 batch1024 screen does
exercise a four-thread mid-computation timeout at timestep128 and exits124 through
CGAR's explicit timeout path. That is actual failure evidence, not a test of every
possible allocator/worker exception or private-cache retry after a failed run.

Do not adopt the review's linear coverage-to-throughput extrapolation or statement
that search-side changes are spent. Sampled coverage is not a causal response
curve, and two search operators do not exhaust the design space. Its approximate
assignment/motion share is not a validated causal split across different cohorts.
A completed batch512 seed2 has already lost throughput despite complete valid
execution; await the whole independently analyzed matrix for the policy decision.

The strongest new implementation hypothesis is to price and enumerate pickup
candidates with heading and the published traffic costs. Source confirms that
current local candidate search is unit BFS, and fallback estimates also ignore
heading/tolls. This mismatch is real; the claim that it causes seed2 collapse is
unproven. Before implementing, preserve current-task/fair admission and distinguish
local exact settled costs from fallback estimates, graph-domain differences,
fixed search bounds and scheduler-before-planner publication timing. The saved
runs do not record every historical candidate shortlist, so an exact16-candidate
counterfactual cannot simply be recovered from the aggregate audit. The suggested
5% shortlist-change gate and0.5–2% gain are hypotheses, not acceptance evidence.

A secondary test of graded tolls (strength4 at scale4) follows the measured margin
sensitivity and the losses from uniformly weak or stronger binary penalties.
It uses existing validated generic configuration, with no map rule. Full paired
results are required. Margin25 has now independently passed all six seeds with
mean136426.2 (+1.364%), all totals/final windows improved; this completed update
postdates the payload's two-seed status.
