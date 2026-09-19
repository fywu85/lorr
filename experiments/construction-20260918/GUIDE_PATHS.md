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

Full **8898527**, with analysis **8898528**, is queued after the existing v20
matrix/analysis. It compares control and those three batch-512 profiles over
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
