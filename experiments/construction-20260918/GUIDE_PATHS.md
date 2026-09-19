# Online intended-route guidance

[Chen et al., AAAI 2024](https://ojs.aaai.org/index.php/AAAI/article/view/30054)
use congestion-aware current-goal guide paths with PIBT. Their
[Guided-PIBT implementation](https://github.com/nobodyczcz/Guided-PIBT) offers a
routing direction worth testing. Their cardinal-move model and ten-second
experimental decision limit differ from LoRR rotations and our one-second limit;
published scores are not directly comparable. No external code was imported.

## CGAR prototype, frozen v21

`CGAR_GUIDE_ROUTES=1` enables an independently implemented optional score guide.
It requires temporal planning, unit turn costs, and frozen executed-flow guidance
disabled. The default remains off. Every decision still completes construction,
prescribed repair, all regional/portfolio workers, and full collision validation.
Deadline overrun fails the decision; elapsed time never selects a partial answer.

Routes use only current robot goals and the generic certified core graph. An
orientation-aware A* charges each turn one base unit and each forward edge:

`base + opposite_weight * reverse_uses + load_weight * (forward_uses + reverse_uses)`.

Counts include remaining intended forward edges, not executed traffic or future
tasks. Goal changes, loss of eligibility, or off-route execution remove all stale
contributions. Observed forward progress removes consumed prefixes. Admission
rotates through missing routes fairly, with a fixed number of attempts and a
fixed expansion limit. A limited or unreachable search publishes no guide; that
robot uses the existing complete temporal planner and goal metric.

A nearby waypoint supplies unit-action distances from a complete reverse BFS in
a bounded local box. The box covers the route prefix and every five-action
candidate. Its distance table is cached until the waypoint is reached or passed.
The remaining route suffix is an additive potential; actual-goal completion keeps
the original reward. Task priorities still use the original goal distance.
Protected primary, recovery, pocket and supporting actions retain their original
fixed paths. Guide routes reserve no actions. Waiting rotations use the same
metric as candidate scoring. Warm suffix reuse remains independently validated.

Options and defaults:

| Option | Default | Meaning |
|---|---:|---|
| `CGAR_GUIDE_BATCH` | 128 | Maximum route attempts per decision |
| `CGAR_GUIDE_EXPANSIONS` | 4096 | Maximum expanded states per attempt |
| `CGAR_GUIDE_LOOKAHEAD` | 8 | Route actions to the next waypoint |
| `CGAR_GUIDE_BASE_COST` | 16 | Positive unit-action routing cost |
| `CGAR_GUIDE_OPPOSITE_COST` | 1 | Marginal opposing-flow cost |
| `CGAR_GUIDE_LOAD_COST` | 0 | Marginal total-flow cost |

## Validation and pending measurements

Build **8898524** passed the complete regression suite. New checks compare routes
against independent forward Dijkstra, verify exact remaining-edge count
conservation through progress/goal changes/deviations/protection, cover 784
candidate orientations, exercise fixed-work admission fairness and unreachable
routes, and propagate deadline failure. Production fixtures validate 9,600 robot
actions with identical serial/parallel regional decisions, with warm starts both
off and on, plus primary/pocket/capacity tests. The archived patch reconstructs
all frozen source hashes.

Screen **8898525** runs control, unit routes without congestion cost, opposing
weights 1/4 with batch 128, and opposing weight 1 with batch 256. It uses 200
steps, one reserved EPYC 9354 physical core, 8 GiB reservation, and the exact
one-second deadline. It tests feasibility and guidance coverage only. No full
throughput result or improvement is established for this prototype.

All five initial screen cases passed. Maximum entry times were 0.509 seconds
(control), 0.558 (unit/b128), 0.512 (opposite1/b128), 0.510 (opposite4/b128),
and 0.564 (opposite1/b256), with peak RSS below 4.81 GB over 200 steps.
The disabled control exactly preserves all four prior trajectory fields.
At step 200, active guides number 4,128 / 3,574 / 3,827 / 6,177 in the four
enabled profiles. A batch of 128 leaves much of the fleet on the baseline metric.
This coverage finding motivates screen **8898526**, queued after the first screen:
unit/opposite1/opposite4 with a fixed batch of 512. This is a mechanism and
deadline test, not a choice based on prefix throughput.

The batch-512 screen also passed: maximum entries 0.645 / 0.703 / 0.816 seconds
for unit/opposite1/opposite4, with peak RSS below 4.82 GB. Active guides at step
200 were 8,639 / 7,331 / 7,305. The opposing-cost cases hit their fixed expansion
limit in 321/512 and 327/512 attempts at that sampled step. Guide preparation
costs 0.189/0.187 seconds there. These are feasibility diagnostics only.

Full **8898527**, with analysis **8898528**, was initially queued after v20.
While still pending it was moved behind v24 full/analysis **8898535/8898536**
to prioritize the fixed-work guidance test. It compares control and those three batch-512 profiles over
all 5,000 steps, two independent single-core instances, 24 GiB total reservation,
and the same EPYC 9354/one-second/memory requirements.

## Search efficiency follow-up, frozen v22

A separate revision reuses already cached complete unit-orientation distances as
A* lower bounds, falling back to the spatial metric when absent. No extra table
build or time-selected admission is introduced. Even at search weight 1 this can
change which equal-cost route is selected, so policy preservation is not claimed
for enabled guidance. The original v21 full matrix remains frozen for comparison.

`CGAR_GUIDE_HEURISTIC_WEIGHT=1` is the default. Fixed weights 2/4 favor progress
in the route search; they still publish only complete current-goal routes and
obey the same expansion cap. They are alternative routing policies, not an
elapsed-time early-return mode. Candidate scoring still uses the unit-action
waypoint metric, and the full temporal solve/validation remains mandatory.
Diagnostics now distinguish goal, protected/ineligible, and deviation resets.

Build **8898529** passes the full suite, including four weighted-route bounds
checked against independent Dijkstra and the existing serial/parallel production
fixtures. Its source patch reconstructs all frozen hashes. Screen **8898530**
compares disabled control, unit routes, and opposing-cost routes at search
weights 1/2/4, all with batch 512. No v22 performance result is established yet.

The completed v22 unit-guide screen reports 8,693 active guides at step 200,
107,516 expanded route states, and 0.0368 seconds of guide preparation. Of 472
invalidations, 47 follow goal changes and **425 follow route deviations**. This
motivates a separate bounded reconnection experiment: reconnect nearby deviations
to a still-current route using its local distance field, with exact flow-count
updates and complete path validation. It is not yet implemented in v22.

All five v22 screens pass, and disabled guidance preserves the previous exact
control trajectory. Maximum entries are 0.524 / 0.649 / 0.721 / 0.582 / 0.627
seconds for control, unit, opposite1 weights 1/2/4. At step 200, weight 2 solves
all 512 admitted route searches in 96,051 expansions, with 9,086 active guides
and 0.0375 seconds of preparation. Weight 4 also solves all 512, using 61,433
expansions and 8,692 active guides. These select different routes; lower search
work does not establish better throughput. Evidence: `results/guide-search-screen-v22`.

## Bounded reconnection, frozen v23

`CGAR_GUIDE_RECONNECT_STEPS=0` remains the default. A positive fixed action limit
(up to 128; experiment 16) allows an eligible robot with an unchanged goal to
rejoin its still-current route after a nearby deviation. The connector descends
the prior complete local distance field until first meeting the remaining route,
then aligns with the outgoing orientation and preserves the full suffix.
Reaching the final goal does not require a particular terminal orientation.

Only the replaced prefix is removed from intended-edge counts; connector edges
are added, and shared suffix counts remain intact. Preparation completes before
counts/path replacement. A failed or too-long connector changes no route and
uses ordinary invalidation/admission. A deadline exception fails the decision.
Goal changes and protected/ineligible robots never reconnect stale guidance.
A fresh complete local window covers the resulting route prefix and every
five-action candidate. This is a guidance update, not an action reservation;
the ordinary complete temporal search and collision checks still apply.

Build **8898531** passes all regressions. New fixtures validate current-goal
suffix completion, every physical transition, exact flow conservation during
reconnection and execution, fixed connector limits, goal/protection invalidation,
and a forced interrupted connector that publishes no partial flow/path changes.
An additional 4,800 production robot actions validate nonzero reconnection and
identical serial/parallel decisions with warm starts enabled. The frozen patch
reconstructs every source hash.

Screen **8898532** compares the v22 weight-2 control against reconnection with
batch 512, batch 128, load cost 1, and search weight 4. It retains the same
200-step feasibility-only purpose, one isolated EPYC 9354 core, 8 GiB reservation,
and exact one-second deadline. Full throughput remains unmeasured.

The first reconnection screen exposes a feasibility failure: batch 512, load 0,
search weight 2 exits 124 at timestep 90, `temporal_repair`, 1001.413 ms.
It receives no throughput score; see `results/guide-reconnect-v23-first-failure`.
Batch 128 and load-cost-1 profiles complete their 200-step screens. At step 200,
batch 128 retains 9,996 guides and reconnects 2,040 deviations, needing only 26
new-route attempts. This saves route rebuilding but can increase temporal
conflict-search work; it is not evidence of better throughput. The final weight-4
case also passes, with maximum entry 0.863 seconds. Complete evidence is in
[guide-reconnect-screen-v23](results/guide-reconnect-screen-v23/).
The disabled-reconnection control exactly preserves the v22 weight-2 trajectory.


## Fixed-work follow-up, frozen v24

V24 changes failure diagnostics only. A temporal worker now records search work
before propagating a timeout; workers still join and the entire decision fails.
No failed case returns a partial plan or receives a throughput score. Build
**8898533** passes all regressions and its patch reconstructs all requested hashes.

Screen **8898534** reproduces the batch-512 reconnection failure at timestep 90:
preparation 0.111459 seconds (including guides 0.044973), search 0.836690 seconds,
69,229,435 candidate inspections, 1,174,478 recursive calls, maximum depth 144,
and 48,162 of 50,000 repair attempts reached. Entry time is 1001.438 ms. The
measured bottleneck is temporal conflict search, not the route-preparation phase.

The follow-up uses the existing fixed candidate-work threshold of 4,000,000,
with a 1,000,000-attempt ceiling. Construction completes before repair; the
threshold is checked only between complete repair attempts and can be exceeded
by the final attempt. It does not depend on elapsed time. A global deadline
overrun still fails the decision. All five 200-step cases pass:

| Profile | Maximum entry seconds | Peak RSS bytes |
|---|---:|---:|
| Disabled-guide 4M control | 0.811717422 | 4,675,309,568 |
| Opposite 1, weight 2, batch 512 | 0.830158231 | 4,651,270,144 |
| Reconnect 16, batch 128 | 0.801487935 | 4,459,360,256 |
| Reconnect 16, batch 512 | 0.820137948 | 4,368,236,544 |
| Reconnect 16, batch 512, load 1 | 0.819225129 | 4,492,075,008 |

These are feasibility results, not throughput rankings. The disabled-guide
control exactly matches the previous v20 4M screen. Evidence:
[screen](results/guide-work-screen-v24/),
[equivalence](results/guide-work-default-equivalence.json).

Full **8898535**, analysis **8898536**, follows v20 **8898517/8898518**. It tests
these five frozen profiles at seed 0 over all 5,000 steps, two independent
single-core EPYC 9354 instances and 24 GiB total reserved memory. V21 full
**8898527/8898528** then follows unchanged. The verified pending-job dependency
update is recorded in [guide-queue-update.json](results/guide-queue-update.json).
No full intended-route throughput result is established yet.


## Retained-route refinement, frozen v25

`CGAR_GUIDE_REFINE_BATCH=0` defaults off. A positive batch (up to 4,096) scans
pre-existing eligible routes in a separate fair rotation. A sparse exact count
of the selected robot's remaining edges is subtracted only when evaluating
routing costs; shared flow is not mutated during search. This handles repeated
edges as well as ordinary simple routes. The fixed expansion limit and heuristic
weight still apply. Only a complete current-goal route with strictly lower cost
is installed; limited searches and equal-cost alternatives retain the old route.
The old cost includes alignment from the robot's actual orientation. Newly
admitted routes are not immediately refined in the same decision.

Build **8898541** passes all regressions. New checks verify an independently
measured five-action improvement after opposing flow disappears, exact count
conservation, no self-congestion penalty, equal-cost retention, fixed-count
fairness, limited/timeout retention, and protected/changed-goal handling. An
additional 4,800 production actions compare serial/four-thread decisions with
nonzero refinement and warm starts. The first build's erroneous four-action
fixture expectation is retained separately; it was not benchmarked.

## Correct route-to-go windows and bounded connectors, frozen v26

Fable's emitted findings identify a reproduced metric defect: a candidate that
passes a cached waypoint is penalized for being beyond that waypoint. The
standalone old/fixed probe prefers FFFWW before the fix and FFFFF afterwards.
The new window seeds **every remaining route state inside the box**, with its
remaining suffix length, and computes the minimum unit-action cost to any such
continuation. This permits both forward crossing and bypass/rejoin candidates.
Sorted seed distances and a FIFO of unit-edge relaxations avoid a priority heap.
Distances use int storage because full suffix offsets can exceed uint16 range.
The local box size, cache refresh condition, prescribed work and global deadline
failure contract are unchanged. See the [independent review assessment](fable-guides/assessment.md).

The corrected potential can legitimately point along a parallel row toward a
farther join. Reconnection therefore now uses an independent shortest-connector
BFS within its fixed action limit and the existing local box, terminating at an
aligned remaining-route state, or the final goal in any orientation. All complete
suffix and flow-conservation checks remain in force. Its explored-state count is
recorded separately. The original nearby-connector regression caught this issue
in the first v26 build and is retained unchanged in the correction.

Build **8898546** passes the complete suite, including 3,912 oriented window
states and four rotated candidate-ranking checks. Its source patch reconstructs
all requested hashes. Screen **8898547** tests disabled control, unit routes,
congestion routes, reconnection, and refinement batch 64 at the existing fixed
4M work threshold. All use one reserved EPYC 9354 core, 8 GiB, 200 steps, and
one-second complete decisions. It is a feasibility screen, not a throughput rank.

The affected v21/v24 full matrices and their analysis jobs
**8898527/8898528/8898535/8898536** were verified pending and canceled before
execution; no running job was interrupted. Their frozen source/specifications
and completed screens are preserved. They are superseded by corrected-window
full tests after feasibility validation. [Scheduler record](results/guide-pending-matrices-superseded.json).


All five v26 screens pass. Maximum entries are 0.808614661 (disabled control),
0.848701178 (unit routes), 0.829822722 (opposing-cost routes), 0.807486145
(reconnection), and 0.827943872 seconds (refinement 64). Peak RSS ranges from
4.68 to 4.80 GB. The disabled control preserves every previous trajectory field.
At step 200 the refinement case has 9,996 guides, reconnects all 2,713 observed
deviations in 49,583 connector-state visits, and improves 28 of its 64 prescribed
route refinements. These are nonvacuous mechanism/feasibility observations,
not throughput evidence. [Screen records](results/guide-window-screen-v26/),
[exact control equivalence](results/guide-window-default-equivalence.json).

Corrected full **8898550**, analysis **8898552**, follows the independent flow
confirmation **8898544/8898545**. It compares these same five profiles at seed 0,
5,000 steps, two single-core EPYC 9354 instances and 24 GiB aggregate memory.
The nearby flow-setting full matrix **8898554/8898555** follows it.


## First completed corrected-guide full pair

The v26 control again completes 109,244 with its exact prior full trajectory.
Unit-cost guides (opposite/load costs zero) complete only **39,066**, despite
all 5,000 exact entry samples passing, max 0.935855331 seconds, RSS
11,996,459,008 bytes and zero errors/timeouts. Its five successive 1,000-step
counts are 17,230 / 10,425 / 5,438 / 3,195 / 2,778; outstanding task age p90 is
4,505 versus 850 for control. It accumulates 13,619,939 loaded waits and
9,415,952 loaded turns, versus control's 2,772,123 and 3,675,264.

At the last sampled step it maintains 7,164 guides, invalidates 473 route
deviations, and rebuilds 512 routes, with only four goal resets. This is
consistent with repeated displacement and route concentration, but does not
isolate a single cause. The waypoint fix resolves a real scoring error; it does
not establish a throughput gain. Congestion-aware/reconnection/refinement full
cases are still running and must finish before drawing conclusions about them.
Only the two completed cases were analyzed, using a read-only subset on GRID.
[Full-horizon first-pair evidence](results/guide-window-first-pair-v26/).

## Completed corrected-guide comparison

All five v26-r1 cases finish 5,000 steps with zero errors/timeouts, every entry
sample below one second, and RSS below 12.015 GB. None of the guide variants is
an improvement over the 109,244-task control. The corrected waypoint metric is
a verified correctness fix, not evidence that route guidance helps throughput.

| Policy | Tasks | Final 1,000 | Task age p90 | Max entry s |
|---|---:|---:|---:|---:|
| 4M control | 109,244 | 22,456 | 850 | 0.865250 |
| Unit routes | 39,066 | 2,778 | 4,505 | 0.935855 |
| Opposing-cost routes, h2 | 42,051 | 3,480 | 4,335 | 0.973808 |
| Reconnect 16, batch 128 | 28,071 | 4,105 | 5,000 | 0.887376 |
| Reconnect + refine 64 | 33,507 | 3,650 | 4,701 | 0.911235 |

The full control exactly matches prior 4M trajectories. The large turn/wait
increases in the complete unit/opposing/refinement diagnostics are consistent
with route concentration or reduced route flexibility, but do not isolate either
cause. Reconnection is especially poor and its oldest outstanding tasks reach
age 5,000. No guide profile is promoted.

Strict analysis job 8898552 caught a missing final movement-log sample for the
reconnection case: stdout stopped at step 4,800 although the independently saved
trajectory and all 5,000 entry timings are complete. The simulator normally
uses `_exit(0)` without flushing C stdout. V29 explicitly flushes periodic movement
diagnostics. Its build 8898621 passes the full regression suite and its archived
patch reconstructs every source hash.

Recovery job 8898620 uses explicit `--allow-incomplete-movement`. Strict analysis
remains the default. Recovery retains independently derived full task, action,
age and trajectory metrics and labels the incomplete optional diagnostics as
`partial_movement_phases` with observed step 4,800. It does not invent final
counts. Seven analyzer tests check strict rejection, incomplete/mixed samples,
corrupt counters and full action crosschecks. The original failure is retained.
[Full evidence](results/guide-window-full-v26/),
[first analysis failure](results/guide-window-analysis-first-failure/),
[flush build](build-provenance/v29/).

A separate four-case load-cost screen 8898605 passes all 200-step deadlines
(max 0.834 seconds, RSS below 4.878 GB). At the step-200 sample without
reconnection, load cost 1 and A* weight 2 complete 205/512 searches (307 limited),
while weight 4 completes 511/512. The latter has 8,721 active guides. These are
coverage/work observations only, not throughput rankings. Both reconnection plus
refinement load-cost screens also pass. No full load-cost score exists yet.
[Load-cost feasibility evidence](results/guide-load-screen-v26/).
