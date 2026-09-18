Perform the independent, read-only Fable 5.1 Max review requested by the repository owner. Focus ONLY on generic CGAR improvements measured on the full WAREHOUSE instance (10,000 robots, 5,000 steps). Do not edit files, run commands or delegate. Use only repository source and compact results; do not read secrets or environments.

The owner wants to match or beat KittyKnight/NMS on WAREHOUSE without map-specific geometry, map-name switches, pretrained map weights or per-map lane tricks. Target CGAR peak memory is 32 GB (even though the archived competition VM specification lists 128 GB). CGAR is NOT anytime: prescribed bounded work must finish or throw an explicit Timeout; never return an interrupted search as success. Current successful CGAR runs use one reserved physical core; references use 32 logical / 16 physical cores and approximately one second per decision. Parallel fixed-work computation would be permissible if justified and explicitly allocated.

Baseline CGAR: tasks 36,353 / 37,511 / 36,294 across seeds 0/1/2, horizon 5,000. Previous optional orientation guidance (demand cache 512 MiB + turn-first): 40,902 / 40,984 / 40,764. Newly enlarged 8 GiB LRU orientation cache: 43,446 / 44,174 / 42,600; independent seeds 3/4/5 also all beat 512 MiB (42,532 / 43,184 / 43,121). Peak total memory 14.3 GiB. Increasing to 20 or 24 GiB cache produced IDENTICAL full trajectories to 8 GiB, peak actual memory below 19 GiB. Thus cache coverage alone is not the remaining large gap.

Short temporal sequence refinement was implemented in this turn and is currently EXPERIMENTAL. Five-step, two-pass refinement looked better at 200 steps but fell below baseline over 5,000 steps (35,441 / 34,849 / 35,474). Three-step seed 0 collapsed to 14,214. The five-step prototype generates 189 generic action sequences; begins with CGAR's complete one-step feasible joint action padded by waits; then performs strictly improving bounded displacement repairs and executes first actions. It protects primary, transaction participants and cells, pocket service, parked robots, capacity bootstrap. It originally retained ordinary unfinished turn/move commitments, and created a commitment even if its selected path began with a wait before a later forward move. Very many commitments expire. A delayed-acquisition variant is being tested; exact oriented terminal costs alone appear insufficient.

The frozen CURRENT source under review is the v6 snapshot, not historical files (the working source may advance while you review):
- runs/cgar-sequences-build-v6-20260918/sources/cgar/cgar_planner/cgar.cpp
- runs/cgar-sequences-build-v6-20260918/sources/cgar/cgar_planner/cgar.hpp
- runs/cgar-sequences-build-v6-20260918/sources/cgar/cgar_planner/action_sequences.hpp
- runs/cgar-sequences-build-v6-20260918/sources/cgar/tests/cgar_regression.cpp
Its optional new variants include repairs with up to three blockers and fixed depth/candidate budgets; relaxing ordinary core commitments while preserving primary/recovery/pockets; and a wait-based initial joint plan whose protected forward dependencies are closed using the valid baseline. These are running full warehouse seed-0 tests. Do not mistake unmeasured prototypes for proven gains.

Read the active compact evidence as useful:
- experiments/sequences-20260918/results/warehouse-full-v2/{metrics,paired-results,run-summaries}.json
- experiments/sequences-20260918/results/warehouse-cache-confirmation-v4/paired-results.json
- experiments/sequences-20260918/results/warehouse-memory-full-v4/paired-results.json
- experiments/motion-20260918/README.md (matched-goal NMS uniform guidance achieved 8,047 arrivals vs CGAR 4,159, controlling assignment; caveats recorded)
- experiments/pibt-equivalence-20260918/README.md (bare native PIBT under the rotation adapter loses badly; commitments help)

Compare NMS implementation as needed: nms/Solution/Planner/PIBT/pibts.cpp, the operation pool and oriented heuristic code under nms/Solution/Objects, plus settings.hpp. Do not recommend copying its map-specific switches. Inspect source for actual mechanisms rather than attributing gains to labels.

Deliver a concise but technically substantive review: (1) any correctness or guarantee-preservation defects in v6, with file/line references and concrete failure scenarios; (2) the strongest reasons this search is far from NMS despite its longer horizon; (3) rank the next three generic changes by expected impact, state an implementable fixed-work version and one decisive validation each. Challenge our diagnosis when unsupported. Clearly separate proved code facts, measured findings, and hypotheses. Do not claim the full reference benchmark results: those jobs are still running.
