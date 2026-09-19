# Distance versus operation preference (experimental v16)

The inherited temporal objective is corrected terminal distance times 50, minus
the operation index (0–128). At unit costs, the operation bonus can outweigh two
distance units. For example, distance 11 / operation 128 costs 422, preferred to
distance 10 / operation 0 at 500. This is a scoring choice, not a proven bug.
NMS's original scorer uses the same formula but consults weighted graph distances;
its source also contains map-specific guidance that this experiment does not use.

CGAR_TEMPORAL_DISTANCE_SCALE optionally changes that coefficient (1–4096,
default 50). The two test profiles use 256 and 1024. Both prefer a one-unit
terminal-distance improvement over any operation-index bonus for one robot.
They differ in multi-robot gains and annealing; stronger goal greediness could
also make necessary detours harder. No map structure or supplied weights enter
this option, and turn/learned-forward surcharges use the same coefficient.

The complete v16 suite passes: the actual scorer checks 33,282 dominance pairs,
7,200 independent shortest-path comparisons and 610,224 weighted physical-progress
bounds, plus protected progress and deterministic regional episodes. The completed
200-step screen passes: scale 256 peaks at 0.526716631 seconds and scale 1024
at 0.580728203. The default control and strength-1 learned flow exactly preserve
the v15 paths, schedules, events and tasks over all 200 steps. Only full 5,000-step runs rank throughput. Full job 8898428 (held after v14 and its analysis) compares three independently learned-flow strengths, the two
independent scale changes and one control, all from the same frozen build.

It reserves two concurrent instances, one disjoint physical core each, and
24 GiB total. The originally queued eight-core allocation was replaced before
starting because these profiles do not enable regional workers; the frozen build
and cases are unchanged. [Reallocation record](results/reallocated-guidance-v16/). Analysis 8898429 runs after it. [Screen evidence](results/scale-screen-v16/),
[default equivalence](results/scale-default-equivalence.json),
[full specification](guidance-full-variants.json). No full guidance/scaling score
is available yet.


The complete seed-0 full results are negative: scale 256 reaches **105,891**
(−1.46%), and scale 1024 reaches **105,811** (−1.53%), against **107,457**. All
finish 5,000 steps with zero errors/timeouts. Their maximum entry times are
0.567 and 0.615 seconds; RSS is 11.888 and 13.260 GB respectively. Final-1,000-step
counts are 21,727 / 21,641 versus 22,002. Moves away from the goal decrease, but
loaded turns rise from 3.832M to 4.993M / 5.087M and waits from 2.848M to
3.545M / 3.584M. Better short-term distance adherence does not improve this
full-run tradeoff. Both remain disabled; scale 50 remains the default.
[Full analysis](results/guidance-full-v16/).
