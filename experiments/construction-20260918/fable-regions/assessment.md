# Assessment of the approved v11 review

Claude Code CLI completed the read-only review with `claude-fable-5-1`, maximum
effort. The user explicitly approved the exact source/test payload after two
automatic-review rejections. The review covers frozen v11, not later compact
storage or prefetch. The reviewer ran no tests; local validation follows below.

## Confirmed scoring defect and fix

P1 is reproduced by executable regressions. With the old weighted fallback,
a one-corner route at turn cost 8 and a two-corner route at cost 4 offer no
positive gain over waiting, despite an unblocked route. The local fallback
estimates only the next useful turn; charging it for future corner turns mixes
incompatible cost bases.

Build v14 selects one basis per robot: use weighted turns when a complete exact
orientation table reaches its starting state, and unit turns otherwise. The
production fallback helper stays in unit-action units. Candidate path surcharges
use the same basis. The certified spatial potential is unchanged. All 64
rotated-corner cases (exact/fallback, costs 1/2/4/8) credit progress and the real
temporal kernel selects a moving plan. The complete regression suite passes.

The cost-2/4 full jobs had not started. They were first replaced by a compact
parallel matrix; that replacement was held immediately upon receiving P1.
Its source will not be silently changed. A new frozen build and deadline screen
precede a separately recorded full replacement. Earlier weighted-turn cold
results describe the defective fallback and cannot isolate turn-cost quality.

## Regional diagnostics and validation

P2 identifies a real testing/observability gap, not a demonstrated collision or
race. Build v14 adds retained/reverted repair-batch counters, crossing-path counts,
pre/post and per-round scores, and phase timings. The global diagnostic reports
the global score before regional repair. Eighteen of 24 dense cases actually
improve, searching 1,297,256 candidates in total; a no-op implementation fails.
One-round cases independently verify crossing paths stay fixed and changed paths
stay inside their starting region. A production-adapter episode compares 4,800
robot actions with one versus four regional threads, exactly.

Freshly reconstructing the initial plan makes the round-0 comparison use the
same summation order on both sides, addressing H5's possible floating-point
false alarm. Masks now also outlive worker destruction. Neither change adds a
clock-selected stopping condition. Every worker still joins and a timeout fails
the whole decision.

## Hypotheses and remaining checks

H1 is unproven. The review says Dial table construction costs the same for all
turn costs; this is too strong. Different edge weights can change relaxation
order, queue occupancy, stale work and the distance range traversed. Lower entry
times after P1 alone would not isolate the cause. The new phase/metric counters
help distinguish candidate preparation and repair effects; weighted-oracle work
has not yet been separately timed.

H2 (regional annealing starts too hot), H3 (more primary recovery), and H4
(two-round deadline margin) remain experiments or measurements. The completed 200-step
screen records exact/fallback coverage, retained regional changes and per-round
scores; it never ranks throughput. Six profiles passed; cost 8 still failed
explicitly. The 25k-global plus two 25k-regional profile peaked at 0.727 seconds,
versus 0.893 for 50k-global/two-rounds, and retained 7/8 regions at step 200.
The six passing profiles are running as full job 8898387. Actual full memory,
complete trajectories, sustained throughput and six-seed validation remain
required before promotion. The three unit-cost variants retain every earlier
50-step path, schedule, event and task.
