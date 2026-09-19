# Fixed turn-table rebuild allowance

The ordinary refreshed-flow control has complete turn-aware metrics for only
75.69% /75.72% of robots on average in its every200-step samples (seeds0/2).
The minimum samples cover only1682/1614 of10000robots; maximum samples cover all.
These are sampled coverage figures, not every-step estimates or causal task gains.
A traffic-field change invalidates old tables, and the planner builds at most32
new turn tables in each complete step. Missing tables use the complete fallback
heuristic; they are not partially built distance tables or partial action sets.

The warehouse has38586free cells. A compact table physically uses308688bytes.
Eviction conservatively accounts for wide tables of617376bytes, so the8GiB
cache permits more than13000tables, above the10000current robot goals. At most16384new tables can be built within
a512-step refresh interval at the existing limit. Thus raising cache memory alone
cannot remove this rebuild-rate limit. More exact guidance could help, but it can
also change traffic feedback; full paired runs are required.

V39 makes this prescribed count configurable as CGAR_TURN_BUILD_LIMIT,0–256,
default32. No elapsed-time condition changes the count. Tables still complete
before entering the cache, and a complete entry exceeding one second fails.
The initial full experiment compared32/64/96/128 after cold-screen validation.
There is no map-specific rule or new memory setting.

[Sampled baseline coverage](results/turn-build-limit-baseline-coverage.json).


Build8898734 passes the full regression suite, including15complete small-map
steps at limits0/1/16/32/64 and240exact default32robot actions. Counts never
exceed the configured limit; invalid values are rejected. Both the source and
tests are archived with an independently reconstructed patch.

Screen8898739 passes32/64/96/128with maximum entries0.782/0.808/0.847/0.881s,
RSS below5.21GB, and exact default32trajectory reproduction. Short completion
counts do not rank the configurations. Full8898743/8898744 completed all four on seeds0/2
concurrently,32physical cores and160GiBaggregate reservation. Each planner has
four disjoint cores and an individual32GBRSS limit; the larger aggregate is
for eight simultaneous processes, not a higher per-planner budget.
[Archive](build-provenance/v39/), [screen](results/turn-build-limit-screen-v39/).


All eight full runs validate. Means at32/64/96/128 are134515/136302.5/136600/
136950.5; each larger count beats both controls. Count128 gains1.811% in the
paired mean, with final windows28902/28689 versus28215/28214. Its mean complete
entry latency is251.0–264.0ms, maximum870.9ms, and peak RSS11.905GB. Count96
seed0 sets the single-run record137490; count128 has the better paired mean.

Every200-step exact-metric samples rise from75.69%/75.72% at32 to97.43%/97.40%
at128. Even128 still has minimum samples covering5169/5094 robots after resets.
These are sampled temporal coverage measurements, distinct from the cumulative
PIBT orientation counters also archived. The result supports investigating better
coverage; it does not establish coverage as the unique cause of task gains.

Count128 versus32 confirmation runs seeds1/3/4/5 in8898756/8898757, eight parallel
cases with four physical cores each. Production and test hashes for the completed
matrix exactly match commit5c7f0faa7968c0dcd5c7f8e1ec3d2ba7b6534a27.
[Full comparison](results/turn-build-limit-full-v39/comparison.json).
