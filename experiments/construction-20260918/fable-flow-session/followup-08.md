Continue the same persistent lorr-warehouse-cgar-fable51 session. This is turn09,
a read-only incremental review of the new optional pickup-flow policy. Use only
the supplied source updates and retained context; run no tools. Do not claim tests
were executed by you. Request remains Fable5.1 with max effort through Claude CLI.

Current confirmed warehouse reference is margin50/demand-turn-build128: six seeds
137060/137271/136841/137227/136949/137092, mean137073.3. Full scheduling+planning
means235–270ms, max873ms, process CPU1.18–1.21cores of4reserved, peak12.013GB.
Full5000-step/10000-agent target152981 remains unmet. Generic policies only; no map
identity/templates/supplied weights/future tasks. RSS below32decimalGB, complete
one-second decisions, explicit whole-entry timeout failure, no partial solutions.

Results since turn08:
* Parallel batch128 confirmed six seeds138003/137388/136701/137477/134785/137390,
  mean136957.3, minus0.085% against stronger demand128. Four wins/two losses;
  seed4 loses2164. Not promoted. Prior serial/parallel exact trajectories and
  ~6% wall-time reduction remain real for the batch policy. Coverage is not a
  monotone quality metric; batch512's100% sampled exact guidance still loses.
* Binary margin25 + demand128 seeds0/2=137149/139037, mean138093. Both totals and
  final windows improve; four remaining seeds running.
* Graded strength4/scale4/margin25 at demand128=138402/139303, mean138852.5,
  plus0.550% over binarymargin25. Seed2 final1000 is153tasks lower. Four remaining
  seeds running. Graded margin0=56537/139796: reject seed0 late collapse.

V41 implements your last suggested optional pickup alignment experiment. It is
built and ALL regression checks passed (by us on GRID, not by you), including
32256 independent costs from repeated graph relaxation, all headings, directed
costs/scales1/2/4/8, obstacles/disconnection, only settled cell endpoints, fixed
queue-pop work, timeout injected during search and scratch reuse, and scheduler
heading/toll/scaling/fair/started/cold-default cases. Four200-step deadline screens
pass; disabled default entire prefix trajectory equals prior build128. Eight full
5000-step tests are running: flow-enabled/disabled crossed with HRRN1/pickup1 and
HRRN0/pickup5, all margin50/build128, seeds0/2. Historical pickup5 collapse used
build32, so this is not a matched reproduction of that older collapse.

Please review the code, especially:
1. Forward Dijkstra correctness, queue-pop bound including stale entries, scratch
   lifetime/generation reuse, positive cost bounds, settled-only endpoint minima,
   timeout propagation and new memory/work exposure.
2. Cost units in Pair/HRRN (chain and age scaled; normalized reported pickup),
   complete cached-turn-table estimates versus scaled approximate fallback, and
   deterministic tie/shortlist/oldest-task/started-task protections.
3. Last-published snapshot: scheduler intentionally runs before plan's new
   observation/publication; no lifecycle move or double-observation. Physical
   free graph/core restriction versus cached certificate-domain tables is an
   explicit approximation, not claimed equivalent in pockets. Look for concrete
   unintended metric-lifecycle bugs beyond that stated boundary.

The heading fixture was corrected after discovering real idle warmup rotations;
it uses actual independently replayed heading. Toll fixture deliberately ingests
an independently replayed physically valid FW/CR/CR trace to isolate published
field ranking. Those prescribed observations are not claimed as offered actions;
full simulator tests validate actual planner offered/executed trajectories.

Return at most three prioritized, actionable findings with exact function/line or
a minimal counterexample. Distinguish correctness bugs from approximations and
unmeasured improvement ideas. If no reproducible bug, say that. Do not make broad
causal claims from the aggregate throughput alone. If a next experiment beyond
current full runs would be useful, give one bounded hypothesis and falsifying test.
