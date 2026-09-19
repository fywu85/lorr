# Cost-based discovery and pickup CPU allocation

V43fixes a confirmed configuration bug: pickup workers were parsed after the
CPU-affinity check. Initialization now checks the largest worker count that can
run in any phase, including min(pickup thread ceiling, field quota, robot count).
This also applies when temporal planning is OFF. A one-field quota can validly
use a larger configured ceiling because only one worker can run; disabled fields
reserve none. GRID benchmarks independently bind each process to physical cores.

The optional boolean CGAR_PICKUP_FULL_COST_KEY requires enabled complete fields.
Default0 preserves v42. Value1 retains the best16/8tasks by complete pickup plus
approximate chain cost, then applies the existing HRRN ranking to those candidates.
This separates discovery from final age-weighted assignment. Oldest-task admission,
started tasks, fixed work, complete fields, unique assignments and all-robot fallback
remain. It introduces no map identities, supplied weights or task forecasts.

The original full scan deliberately ranked all candidates by HRRN; that behavior
was documented, not an accidental algorithm defect. Fable proposed the cost key
as a separate policy test. The full v42 matrix now confirms gains with16/64 fields on all six seeds, while
32 fields collapses seed0. This rules out a blanket claim that the original
age-aware discovery must hurt. Fable's proposed numerical rejection thresholds are heuristics.

Build8898877 passes all CGAR regressions, including actual one-CPU affinity
restriction with temporal OFF/ON, rejection of two possible workers, acceptance
when the quota limits actual concurrency, both shortlist keys with HRRN OFF/ON,
and fair admission beyond the cost shortlist with unchanged simulator metadata.
[Build provenance](build-provenance/v43/) independently reconstructs all22source/
test hashes; no production build remains active.

Screen8898884 passes all five profiles, max0.879275s and peakRSS5.224GB. Disabled
and32fields/default-key exactly reproduce v42prefixes;32fields/cost-key serial
and parallel prefixes match. No prefix throughput or full-memory claim is made.
[Screen and fingerprints](results/pickup-cost-key-screen-v43/).

Full 8898901/analysis 8898902 completed all eight cases, each with four reserved
physical cores and a 32 decimal GB RSS cap. Every full decision passes. All four
disabled/default32 controls exactly reproduce v42, including the32-field collapse.
All22 source/test hashes match the exact v43 commit 1661176.

| Fields / discovery key | Seed0 | Seed2 | Interpretation |
|---|---:|---:|---|
|0 / default|138963|139677|Healthy control|
|32 / default|70043|142735|Reproduces v42 collapse|
|32 / cost|135244|137131|Below healthy control on both seeds|
|64 / default (v42)|141829|142988|Now confirmed on six seeds|
|64 / cost|135773|79601|Loses both; seed2 collapses|

**Reject both cost-key profiles.** The32-field cost key rescues seed0 but loses
2.248% in the mean to the healthy disabled control; its whole empty robot-steps
rise 17.46–17.95% and age p90 rises 2,755–2,785 steps. The64-field cost key loses
6,056/63,387 tasks to original64, with only5,540 in seed2's final window. Its empty
travel and outstanding ages also rise. Comparing only with the collapsed32 mean
would conceal the regression. Retain default0 and the64-field reference.
[Full comparison and validation](results/pickup-cost-key-full-v43/comparison.json).
