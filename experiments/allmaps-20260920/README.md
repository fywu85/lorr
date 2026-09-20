# CGAR across the ten 2024 instances

The user redirected the campaign on 2026-09-20: first finish Warehouse's strict
one-second validation, then improve the CGAR framework across the competition.
The previous 160,000 Warehouse aspiration is deferred. NMS is the primary
throughput target; KK supplies additional ideas and a comparison where stronger.
A 20% gain per instance is a research aspiration, not an established attainable bound.
The user clarified that throughput is primary and fairness is secondary: waiting
regressions must be disclosed but do not veto a faster configuration.

RANDOM-05 belongs to another agent. This campaign reads its progress log and
published experiment artifacts only. It does not modify, build, benchmark or
submit jobs in `random05/`, edit `RANDOM05_PROGRESS.md`, or alter that agent's jobs.
A future CGAR integration of a separately documented idea must retain CGAR's
ownership/progress protections and be evaluated separately; the standalone
RANDOM-05 results are not CGAR results.

## Starting evidence

[Historical generic transfer](../generalization-20260920/results/summary.md)
uses the same Warehouse-selected generic configuration on the other nine cases:
SORTATION146147; CITY-01 7305; CITY-02 13922; RANDOM-01 588; RANDOM-02 982;
RANDOM-03 1409; RANDOM-04 997. GAME and RANDOM-05 time out and have no accepted
partial score. These are full horizons, one planner seed, shared hosts, a5s
entry limit, and not competition qualification. The source predates later
unopened matching and Warehouse trick improvements. See the frozen
[baseline inventory](baseline-inventory.json).

Warehouse's best remains155173, source37f592a, with explicit `--trick WAREHOUSE`.
The running execution-only V123 validation must reproduce both full seed0/2
trajectories at a1000ms limit before claiming the one-second issue resolved.

## First diagnosis: regional work grows with congestion

The global temporal repair already has a4million-candidate budget. Regional
repair has only25000attempts per region per round, four regions and two rounds.
The saved RANDOM-04 step1000 expands those200000attempts into1186983284candidate
checks and spends4.122s in regional repair. GAME step200 uses399023139checks
and1.528s; it later fails at342. This is a concrete implementation-budget problem,
not evidence that random maps need more compute than Warehouse.

The new optional `CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT` passes the existing
finite-work limit into each region and round. Zero preserves prior behavior.
The last complete attempt may exceed the candidate count; no attempt is cut
midway. A clock deadline still raises a timeout and rejects the run. The option
is map-independent and does not automatically detect map names or density.
It changes the search budget when enabled, so quality must be measured again.

After regression tests, compare4M and8M regional limits on the eight instances
other than Warehouse and RANDOM-05. Keep identical generic settings elsewhere,
full competition horizons, and record failure rather than partial throughput.
Use short runs only for collision, source-equivalence and runtime diagnostics.
Then prioritize SORTATION, CITY, GAME and RANDOM-01..04 from their measured gaps.

[Published per-instance targets](TARGETS.md), [reference transfer review](TRANSFER_REVIEW.md),
[evaluation protocol](PROTOCOL.md), [cross-instance progress](../../CGAR_PROGRESS.md).
