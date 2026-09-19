# Continuing warehouse work

Updated 2026-09-19 21:59:37 UTC. The active, unbudgeted goal remains unmet: reach the local KittyKnight
reference of152,981 tasks repeatably across six full5,000-step/10,000-robot warehouse
seeds. Every complete scheduler+planner entry must finish within1s or fail explicitly;
process RSS must stay below32,000,000,000bytes. Use isolated GRID physical cores and
generic policies, with no map identity/templates, hidden future tasks or fleet caps.
KittyKnight used38.858GB, so this is not an official equal-resource SoTA comparison.
The user hopes for progress by midnight but explicitly says to take the time needed.

## Confirmed reference and frontier

Best single run remains144,510. No new single-run record was set this turn.
The newly confirmed six-seed mean is143,941.1667, versus142,474.8333 (+1.02919%).
Seeds0..5:144510/143933/144107/143134/143934/144029. All six full totals improve;
five tails and ages improve, with seed1tail−108 and agep90+1. The mean is5.909% below
KittyKnight. The user questioned comparing143941 against the144k frontier; clarified
that this is six-seed confirmation of the prior regional gain, not a new peak.

warehouse-reference-variants.json now enables4regions/4threads,2rounds and25,000
attempts per region per round at1000ppm, after one global4M worker. Other settings:
preparation4, graded strength4/scale4, margin25/refresh512, turn-build128/cache8GiB,
HRRN1/pickupweight1, pickupflowON,64completefields/4fieldthreads, age-aware key0.
Warm/mixed/surcharge/next-errand OFF. Exact V44 source:
f6d0ae448504762136c4f5d85f25d7d207207965; all22 hashes and binary verified.
Frozen runs/cgar-fractional-mixed-build-v44-20260919.
results/pickup-full-regions-six-seed-v44.json validates all12 paired runs/60k entries,
all six exact V42 controls and every sampled regional counter. Candidate mean entry
401.2–419.4ms, max962.689ms, CPU1.726–1.770cores/4reserved, RSS13.909GB,
full34.50–35.93minutes. Lower loaded turns/waits are associations, not causal savings.

WAREHOUSE_PROGRESS.md has42 increasing single-run records (unchanged) and8 confirmed
milestones. Latest milestone completed2026-09-19T21:14:15.691307UTC. Preserve the
user's .WAREHOUSE_PROGRESS.md.swp; exclude from Git and never delete it. Read and
compare progress-file contents before editing because the user may have it open.

Quota64/128/256 full0/2 runs produce exactly identical V42/64 trajectories, including
the former four-field-thread control:141829/142988. All30k entries valid, max892.703ms,
RSS12.159GB, exact V46 source0b8361c1d28429f8fd9918a3470046d4414ad83f. Retain64
fields/four threads; larger quotas provide no observed benefit on these two seeds.
results/pickup-full-quota-full-v46.json. This does not prove64 never binds elsewhere.

## Live full experiments

1. Next errand: matrix8899141, analysis8899143, raw
   runs/cgar-next-errand-full-v47-20260919, future results/next-errand-full-v47.
   Seeds0/2 x ON/OFF,4cases x4physical cores =16,64GiB aggregate reservation.
   V47 frozen binary; OFF must exactly match regional totals144510/144107 and hashes.
   The source is committed/pushed as3946d3340ddae324b3ba77eb6a1d23b1418f0fbc.
2. Regional temperature: matrix8899147, analysis8899148, raw
   runs/cgar-regional-peak-full-v48-20260919, future results/regional-peak-full-v48.
   Seeds0/2 x1000ppm/0ppm, both with read-only peak audit.4cases x4cores =16,
   64GiB aggregate. The1000ppm controls must exactly match the regional reference.
3. Eight regions: matrix8899150, analysis8899151, raw
   runs/cgar-regional-eight-full-v48-20260919, future results/regional-eight-full-v48.
   Seeds0/2 x4regions/8regions, matching4/8repair threads, both auditON and1000ppm.
   All4cases reserve8physical cores each =32,64GiB aggregate. Regional attempts rise
   from200k to400k per entry; this is a work/policy experiment, not equal-work speedup.
   Four-region full trajectories must match the prior four-core regional reference.

All cases have the32decimalGB process RSS cap and1s complete-entry deadline.
After each matrix and analysis finish, use verify_full.py with its exact committed
source, then additionally check control hashes and25 cumulative feature-counter frames.
Compare full totals, final1,000 tasks, outstanding ages, loaded/empty motion and limits.
Useful candidates require the remaining four seeds before reference promotion. Short
prefixes are only feasibility/equivalence checks; no quality ranking from them.

## V47 next-errand mode

CGAR_TEMPORAL_NEXT_ERRAND is optional/default0. It uses only an already-revealed,
distinct next errand for nonpinned robots with exact oriented, nonguided current costs
and an already-resident complete next table. Serial non-LRU peeks follow all builds.
One metric decision covers the entire candidate set. Arriving choices use common
B=min_heading D_next(currentgoal,h), then−4U+D_next_star(endpoint)−B plus every actual
turn/toll extra; native op term and paid terminal-wait rotations remain. Nonarriving,
missing/unreachable/guided/current-fallback/G=N/pinned cases retain native scoring.
No next table builds, hidden futures, reservation release or partial plans. Keeping
next as terminal after visiting/leaving it is a deliberate surrogate. Final seed-turn
projection still uses the current goal. Eligible includes far-away robots; changed
candidate counts are not altered executed-action counts.

Build8899126 passed all regressions; all22 hashes reconstruct and match exact3946d33.
Archive build-provenance/v47; raw runs/cgar-next-errand-build-v47-20260919.
Binary4e7fac8db72a4c4ac24f8c673b54cb32ba74d1cd30b851f38d8eaf666a946467.
Tests include54,378 independent weighted production scores, two exhaustive joint
fixtures, six exact native fallbacks,46 real task services, repeated-location service,
changing flow/caches,320 serial/parallel robot actions and4,800 protected decisions.
Screen8899133: both valid; OFF prefix exact a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b,
ON prefix0eea594b7d0264f9924673e30067f80514df62327a6ba4075f01b608255926d5.
Max OFF961.196ms/ON959.324ms,RSS5.273GB. ON at200: known709016,eligible114178,
unavailable594838,arriving153885,changed122104. CONTINUATION_SCORE.md and screen report.

## V48 read-only regional peak audit

Build8899144 passed every regression; all22 source/test hashes reconstruct from3946d33
plus build-provenance/v48/source.patch. No active build. Raw
runs/cgar-regional-peak-audit-build-v48-20260919. Binary:
7c46c79d3e4885dae07d9f2830365652d954d0b3fb2da1cf86ef87e31511458c.
V48 is committed as exact8ba2484dd5f1b9bfc719f5f1e8f4533d7543a7bd; all22 source/test hashes
match that commit. build-provenance/v48/exact-source-commit.json records the binding.

CGAR_TEMPORAL_REGION_PEAK_AUDIT defaults0. It records initial/maximum/final/returned
scalar score at COMPLETE attempt boundaries, never changes search/RNG/work/output,
and marks incomplete audits unusable after an exception. Cumulative diagnostics every
200steps cover ALL intervening batches. lost_peaks counts maxima above returned;
lost_improvements counts the subset whose final does not improve initial. Record
peak/final gain sums, discarded gain and maximum-attempt positions. No snapshots or
best-state retention were added. Tests analytically known0/100/1000 scores found18
actual transient peaks, verified64 unchanged later search continuations, timeout
propagation,72 audited regional cases at1000/100/0ppm and4,800 protected decisions.

Screen8899145: all3 valid;1000ppm auditOFF/ON exactly reproduce regional prefix,
max959.70/960.32ms.0ppm max953.31ms,RSS5.303GB. Both audited variants execute1,600
batches/40M attempts, with ZERO lost peaks/discarded gain. This weakens a retention
hypothesis; full late behavior still needs measurement. Larger0ppm scalar gain is
not throughput evidence. REGIONAL_PEAKS.md; results/regional-peak-screen-v48.

Eight-region screen8899146: both valid, eight disjoint physical cores per process.
Four-region trajectory and all peak counters exact four-core screen; max975.44ms.
Eight regions max974.06ms,RSS5.289GB;3,200batches/80M attempts; one lost peak of256
scalar units. REGIONAL_EIGHT.md; results/regional-eight-screen-v48. No quality claim.
collect_cold.py now also retains the cgar-regional-peaks marker in screen evidence.

## Persistent Fable and follow-ups

Session1ebb1075-3538-49d1-93d1-a00c94fa256a, lorr-warehouse-cgar-fable51,
Fable5.1/max through Claude Code CLI; explicit user permission to share source persists.
Turn14 completed2026-09-19T21:40:57.435658UTC after242.668s, correct model/session,
no tools or denials. Reported74.9763435USD is cumulative session cost, not turn cost.
Per-turn cap24. Archived visible review/metadata/status/local assessment in turn14;
raw protocol ignored. No live CLI request. Source-cache is V47 plus regional header;
V48 changes not sent. Next request would turn15/followup-14.md if useful.

Fable found no demonstrable V47 bug, with no tests run by Fable. It correctly cautioned
that many accepted moves/rollbacks do not prove discarded improvements; hence V48.
Its temperature0 claim 'never reverts' was qualified: equal final scores still count
as reverted, and worsening proposals still consume existing RNG draws. Its thermal
phase account is qualitative, not evidence of exact monotonicity.

Remaining useful V47 tests: already-on-current-goal assignment and whole-set fallback
for a geometrically valid endpoint in an excluded pocket. Source inspection confirms
TurnDistanceOracle limits predecessor cells to core or the goal's pocket, so finite
current-goal distance does not guarantee a geometric endpoint is finite. The current
fallback is conservative before admissibility filters; existing kind5 tests the
separate infinite-current-goal case. Add focused tests if continuing that policy.

Only implement best-state retention if full audit counts justify it. A higher scalar
score still needs throughput validation. Alternative future hypothesis, unlaunched:
physical-score dominance (distance_scale129 vs50) under the new flow/regional policy;
older256/1024 no-flow losses do not establish the current interaction. Avoid blind
parameter sweeps; inspect the three full comparisons first.

## Evidence to preserve

V45postserviceaudit full0/2 exactV42trajectories,all10kvalid,max884.17ms,RSS11.697GB.
After1000known-next-onlyblockerclass7.32/7.70robotspersnapshot(~0.073%eligible),
finishing-onlylarger23/frame. Smallclassnotthroughputbound;objectivechangesaudit
candidateuniverse. POST_SERVICE_AUDIT.md andresults/service-audit-full-v45/audit.json.
Offlinegoalholdingandcontinuationprobes checkedindependently. Commonbaselinescore
54,378weightedcases; FRFFF/RFCFWphysical−24/−16 versusrejectedbaseline−24/−20.
Neutral-tailseparatehypothesisonlytwojointfixtures,no productionimplementation.
CONTINUATION_SCORE.md,GOAL_HOLDING.md. V47regressionusesactualproductionhelper.

Do notrepeatunchangednegativeevidence:4/16globalworkersmixed6seed;fractionalturn
extras1/2 andwarm/mixedlosebothfirstpair;strength2mixed,6/8lose;age-removingcostkey
regresses/collapses;quota32seed0collapse;orientationcache8/16GiBsamefulltrajectories;
orientationbatch128mixedsix/512fails;pool/transactions mixedsix. Originalnoflow
resultsarenotproofundernewflowpolicy. >99.5%loadedturnsplanned,seedturnties54–83/full.

## Operation

All exec_command require sandbox_permissions=require_escalated (defaultbwrapbroken).
No proactive subagents. Fullhorizonforquality;200stepsonlyfeasibility/equivalence.
HeavyfulltrajectoryreplayonGRID;smalllogs/JSON/toyworklocallyfine. Allfixedworkfinishes
orentryfails. Physicalcoresisolated;aggregatejobmemorymayexceed32GB,processRSSmaynot.
verify_full.py validates22source/testhashes,actualbinary,5000timings,32GBRSS,
50Mactionconservation,noquota/coreoverlap;addcontrolhashandfeaturecounterchecks.
Commit/pushauthorized,deliberatelyPUBLICfywu85/lorr. Excluderawruns/env/binaries/
protocol/swap. Sourcefilesfrozenbeforebuild;do noteditwhilebuildlive.
