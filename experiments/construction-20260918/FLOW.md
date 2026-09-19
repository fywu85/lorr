# Frozen guidance from observed traffic (experimental v15)

This optional generic policy records only adjacent movements actually observed
between consecutive simulator states. After a prescribed 128 observations by
default, it freezes one positive integer cost field. It never reads map names,
provided lane weights, task forecasts, or elapsed time to choose work.

For a directed edge, let a be the observed count in that direction and b the
opposing count. With at least 8 observations on the undirected edge and b > a,
the extra cost is ceil(strength * (b-a)/(a+b)); otherwise it is zero. Forward cost
is 1 plus this penalty. Every edge remains usable, and unchanged/unobserved/equal
flows remain neutral. Rotation/reflection of the observations rotates/reflects
the resulting field. These constants are workload-independent configuration.

Options: CGAR_FLOW_STRENGTH=1..8 (default 0, disabled), CGAR_FLOW_WARMUP=1..4096
(default 128), CGAR_FLOW_MIN_SAMPLES=1..65536 (default 8). Orientation guidance
must be enabled. A metric change invalidates all old oriented tables and unused
speculative tables before demand builds new complete reverse shortest paths.
The certified unweighted spatial distance, primary/recovery and protected-path
rules are unchanged. The field freezes once; there is no clock-selected partial
search or repeated periodic cache reset.

Candidate scoring uses learned costs only with a valid exact orientation table.
Every extra forward cost along the candidate, up to its first goal visit, is
charged consistently. Robots lacking the exact table retain the corrected
unit-action fallback. Integer bucket Dijkstra handles positive turn and forward
costs; compact tables retain their lossless 32-bit overflow fallback.

The full regression suite passes: 7,200 independent forward heap shortest-path
comparisons, 203,408 physical-progress bounds, observed-state learning, neutral/no-
evidence cases, fixed freeze, missing/duplicate observations, rotation/reflection
symmetry, cache invalidation and deterministic protected multi-threaded episodes.
The source archive reconstructs every tested source hash. The 200-step screen is
complete (job 8898398, four reserved cores, 8 GiB total). Strengths 1/2/4
all froze at step 128 after 998,691 observed moves and penalized 37,474 directed
edges. All 200-step runs passed: maximum entry times 0.510839359, 0.515175331,
and 0.517454741 seconds, versus 0.517035168 for the control. The disabled-flow
control has identical 200-step paths, schedules, events and tasks to v14. It rejects missed deadlines and never ranks throughput.
Full 5,000-step results are required before retaining or combining this policy.

The full comparison is queued from frozen v16 as job 8898410, with three flow
strengths and separate distance-scale ablations, held after the current v14
matrix/analysis. The scale remains 50 for every flow profile. Two concurrent
instances share a 24 GiB total reservation on eight disjoint physical cores.
Analysis 8898411 follows. The v16 default control and strength-1 flow exactly
match v15 over 200 steps. No full flow performance claim is made.
