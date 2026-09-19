# Continuing warehouse work

Updated 2026-09-19 23:21:30 UTC. The active, unbudgeted goal remains unmet: reach the local KittyKnight
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

## Completed comparisons and live experiments

V47 next-errand ON loses both full seeds0/2:143529/144090 versus exact144510/144107,
mean−0.345787%, tails−204/−55, agep90+1/−5. All20k complete entries valid, max967.454ms,
RSS12.013GB. Exact3946d33. results/next-errand-full-v47/comparison.json. Keep OFF.
V48 temperature0 loses both:143600/144083, mean−0.323612%, tails−315/+75, ages0/−2.
All20k valid, max968.764ms, RSS11.380GB, exact regional controls. Final cumulative
reference audit9/10 lost peaks out of40000batches, discarded600/662scalar units.
Zero-temperature loses no peaks but does not improve throughput. Keep1000ppm;
no best-state retention implemented. results/regional-peak-full-v48/comparison.json.
V48 eight regions loses both:144096/143553, mean−0.335393%, tails−103/−154,
ages−1/+2. All20k valid,max982.765ms,RSS11.945GB. Four-region controls AND cumulative
counters reproduce the four-core reference. Eight regions loses5/6 peaks out of80k
batches; extra work does not yield extra throughput. Keep four regions. Completed
22:35:09.487641UTC. results/regional-eight-full-v48/comparison.json.

Scale513 is rejected. Full8899167/analysis8899168 completed23:04:27.701703UTC.
Seed0 failed at2230,1003.435ms,temporal_region_repair. Seed2valid142171 versus exact
144107, task−1936/tail−281/age+6/turns+17.51%/waits+21.77%. Both50 controls exact.
All15k completed entries meet1s/RSS; no full-pair mean, no partial quality result.
results/regional-score-scale-full-v48/comparison.json. verify_full.py now optionally
preserves explicit timeout failures while still checking all valid entries/source/
CPU bindings. Strict default still rejects any failed case. compare_failed_regional.py
checks exact controls and feature activation, suppresses means for incomplete arms.

Work rebalance: screen8899177, raw
runs/cgar-regional-work-balance-screen-v48-20260919, three V48 profiles:
global4M/regions2rounds control,3M/4rounds,2M/4rounds. Four regions/threads,25k
attempts/region/round,1000ppm,peakauditON,nextOFF.12cores/48GiB aggregate.
Screen complete and verified (max962.235/969.932/993.561ms), exact regional control,
all22 source hashes, binary, physical bindings and work counters. Full8899182 /
analysis8899183 started22:50:01UTC, raw runs/cgar-regional-work-balance-full-v48-20260919,
six simultaneous0/2 xthreeprofile cases,24cores/96GiB aggregate. Archive will be
results/regional-work-balance-full-v48; use compare_regional.py CANDIDATE_LIMIT control4000000.
2Mseed0 FAILED in the full run at timestep36,1000.580ms,planning_complete. The other
five cases continue; keep failure visible, no partial-plan fallback. Analyze via
compare_failed_regional.py if completion retains a failed case. REGIONAL_WORK_BALANCE.md; regional-work-balance-variants.json.

All cases have the32decimalGB processRSS cap and1s complete-entry deadline.
compare_regional.py calls verify_full.py for every complete entry, all22source/test
hashes, actual binary, RSS, CPU allocation and50M action conservation. It additionally
checks exact confirmed controls,25 cumulative feature-counter frames and changed
configuration keys. It now supports3-arm work-rebalance via CANDIDATE_LIMIT, allowing
REGION_ROUNDS to vary too. Useful first pairs require remaining four seeds before
reference promotion. Short screens are feasibility only.

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
V47 tests include54,378 independent weighted production scores, two exhaustive joint
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

## V49 tests and persistent Fable

V49 changes ONLY cgar/tests/cgar_regression.cpp. Build8899162 passed all regressions;
all21 production sources and binary match V48 exactly, all22 hashes reconstruct from
f888c87 plus build-provenance/v49/source.patch. No build live. Raw
runs/cgar-next-errand-edge-build-v49-r3-20260919. Exact22 binding is db720efa8c6b266af13ba7767521465e776c847a, recorded in v49/exact-source-commit.json.
The real7x9 excluded-pocket fixture proves finite next distance at currentG but
infinite distance at a geometrically valid arriving endpoint: whole candidate set
falls back exactly to native. The own-cell fixture shows nativeCR versus nextW,
service index advances once and the next plan uses updated metadata. Independent
weighted oracle ranks WFRFW−409 ahead of RFCFW−385. Earlier builds8899154/8899155
failed a mistaken test expectation of CR, not production validity; archives explain
the correction. Regression marker native_fallback_cases=7, excluded_pocket_endpoint=1,
own_cell_service=1. CONTINUATION_SCORE.md and build-provenance/v49*/.

Session1ebb1075-3538-49d1-93d1-a00c94fa256a, lorr-warehouse-cgar-fable51,
Fable5.1/max via Claude Code CLI; explicit user permission to share source persists.
Turn15 completed22:31:20.411231UTC, correct model/session, no tools/denials. Reported
76.655394USD is cumulative session cost. Visible review/metadata/status/qualified
assessment archived in turn15. Source-cache now includes V48 production and V49 test
context. No live CLI; next turn16/followup-15.md if useful,24USD request cap.

Fable recommended the bounded global/regional work rebalance now screening. It also
confirmed the optional finishing-only neutral-tail identity native−(4-last_hit)*U*S,
but no production neutral policy is implemented and it is lower priority. Its claim
that next/neutral scoring cannot change the subject robot's own first action is
false: V49 is a counterexample. Four table peeks do not recover exact full-chain
arrival-heading coupling, and its sub-ms estimate is unmeasured. Repeating a geometric
partition can still change repair. These qualifications are in turn15/assessment.md.
Do not treat review speculation as test evidence. Do not prioritize a retention
journal: full lost-peak counts are tiny. Higher scalar scores are not throughput.

## V50 efficiency candidate and next hypothesis

Only production edit since V49 is temporal_pibt.hpp: remove the duplicate root
callback (counter0) inside build, because construct/repair already checked immediately
before attempt. Nonzero16-displacement callbacks, completion checks and fixed work
stay unchanged. Build8899199 PASSED allregressions, archive build-provenance/v50,
all22 hashes reconstruct fromdf3f8f192a527fdace697399d6775eea263d52bd. No build live.
Raw runs/cgar-deadline-duplicate-build-v50-20260919. Exactcommit binding pending.
Same-core A/B/A200step timing8899204 is complete at
runs/cgar-deadline-duplicate-timing-v50-20260919. All use confirmed referenceprofile:
original_a(V48),optimized(V50),original_b(V48), four physicalcores sequentially.
Collected/verified results/deadline-duplicate-timing-v50/equivalence.json: allthree
hashes EXACTa4bfe67f..., all22sourcehashes/binaries, samefourcores sequentially/noquota,
all600entries valid. V50wall100.640s vs102.609/102.353 (−1.796%);max905.37ms vs953.13/967.95ms.
Full8899209/analysis8899210 nowrunning0/2,8cores/32GiB aggregate, raw
runs/cgar-deadline-duplicate-full-v50-20260919. Archivefuture results/deadline-duplicate-full-v50.
Use strict verify_full thencheck bothrefhashes/totals144510/144107 andsampledsearchcounters. This is efficiency, not new throughput.

Component job8899186 uses1000robot synthetic45x45 cases and gprof. Clocked regional
median time0.9792s/20trials vs0.9363 without callbackclocks. Source-level line samples
concentrate in blocker, but no cache/TLB measurements or warehouse speedup follows.
Local perf launcher cannot run (perf_4.19 missing; local paranoid3), no systemsetting
changed. Reservationlayout8899187 and equal-owner-first8899188 each180observations
exactchecksums/counters but no useful speedup; neither adopted. Duplicate-rootcheck
probe8899194 completes96observations, allsearchchecksums exact; clocked12/12pairs
faster, medianpairedratio0.974838 (ratioofmedians0.973290). No-clockmodes0.14–1.10%
slower. Files kernel_profile/,reservation_layout/,deadline_duplicate/, results/*probe-v49.

Fable turn16 archived: completed22:57:25.273155UTC, same5.1max/session,notools/denials,
cumulative78.0940595USD. It accepted allpriorcorrections. Proposed frozenreal-decision
replay, reservationlayout, setupcopies, clockchanges and deterministic candidatecaps.
Only measured duplicateclockcandidate proceeds; otherideas unimplemented. Review
expects cachegain/100ms onlyspeculatively; assessment documents limits.

Fable turn17 COMPLETE23:17:38.556153UTC, same session, archivedturn17; raw
runs/fable-cgar-session/turn17,PTY38941 mayneedclosedpoll,
promptfollowup-16.md,1source delta(V50header),6094bytes,24USDcap. Proposed a new
ASSIGNMENT hypothesis, no productionimplementation yet: A) whole revealedtaskchain
sum of minheading oriented/toll costs from resident complete next-goal tables,
whole-task nativefallback ifmissing/infinite; B) optional exact first-pickup-heading
coupling using all4distances from existing complete forwardfields. Pairsubtotal
min_h[W*F(first,h)+D_next(first,h)], laterleglowerbounds independent. No extraDijkstra,
LRU changes, hiddenjobs or claimofexactfullchain. Keep native task_chain_cost calls
for unchanged spatialadmission bookkeeping; consistent publishedmetric units and
whole-chain coverage bias are important. Retaining4headings costs~72MB at64fields.
Fable favors Aonly initially, with an offline prediction gate from reconstructedpublished
fields and savedcompleted-task trajectories. It proposes ratioimputation forcoverage
bias; this canreduce meanmismatch butcannot eliminate nonrandommissingness. Its
uniform-cost assignment-invariance andfairrobot claims needqualifications; see
turn17/assessment.md. NextCLIturn18/followup-17.md ifneeded. SourcecacheV50header.
Nextusefullocalwork: boundedGRID offline taskchainpricing audit, reuseproductionflow
publisher andvalidatepublicationcounts, sample deterministicallybeforeexaminingdurations,
assessheld-out-seed loadedtimeprediction pluspermutation null/censoring limitations.
No productionchainpricing, headingretention orimputation implementation yet. Neutral-tail and regional
candidate-cap policies remain unimplemented. Allactualwarehousecells arecertcore
(pockets0), so expandingoracle pocketdomain cannothelpthisbenchmark.

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
