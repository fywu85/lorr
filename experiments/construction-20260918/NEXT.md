# Continuing Warehouse work

Updated 2026-09-20 09:53 UTC. Preserve CGAR. Target NMS154,795: any valid full
5,000-step / 10,000-robot seed may qualify, including explicit `--trick WAREHOUSE`.
Keep 32GB process RSS, complete fixed work, and timeout failure without partial
quality scores. Shared GRID hosts and5s development deadlines are authorized;
strict1s checks remain separate. Preserve the other fork's random05 changes/jobs.

## Current best and validated reference

- Overall best **148,132 TRICK seed0**, static lanes + remaining potential +
  matching64, short preference OFF. Seed2=148,101. Exact source c696d5f, completed
  09:45:49.302966UTC, hash28d8a706680eedc892b0b12e7211a825756e3ae74856676f75baa72af4ffdd56.
  V72 all six full cases valid, four exact controls. Gains over score-only+710/+599,
  tails+140/+157; over matching-only+804/+897. Mean314.964ms/max976.717ms,
  RSS11.939GB,1.833CPU cores averaged of4reserved. Shared5s, not strict1s certified.
  `results/trick-score-match-full-v72/best-record.json`. Gap6,663, growth4.4980%.
- Prior overall147,502 potential-only seed2, source0f4183f, V68. Seed0=147,422.
  Keep both increasing records in the root log, plus V72's earlier148,101.
- Generic reference matching64 mean144,392.17, source0196851. Seeds0..5:
  144967/144869/144511/143340/144446/144220. All totals improve, mean+451,
  five tails improve; seed5tail−49 and seed3agep90+2 remain documented.
  `warehouse-reference-variants.json` selects this. Prior regional profile is
  `warehouse-regional-reference-variants.json`; old trick controls bind that file.
- Generic best single145,083 remaining-flow remains experimental OFF after six
  seed gains+573/+606/+108/−793/+223/−3. No reference promotion for that candidate.
- V66 mixed pickup grouping loses both full seeds144632/144398 versus144967/144511.
  Keep groups0. V65 short preference loses; keep OFF. V70 pickupweight5 with paid
  lanes+matching improves+129/+291 (147457/147495), both tails positive, no record.
  Short+weight5 recovers146873/146820 but is still worse than ordinary fairness.
- V69/V71 saved trajectory audits complete. Potential scoring reduces detours/waits
  and empty work, increases turns; completed shortest-chain means almost unchanged.
  Endogenous cohorts: do not claim causal savings or planner equivalence to NMS.

## Running comparisons and review

- Native V73 full8899463/analysis8899464 on research52: four arms x seeds0/2,
  32 bound physical cores. Paid/potential legacy controls, native with/without bands.
  Raw `runs/cgar-native-metric-full-v73-20260920`; results/native-metric-full-v73.
  Source06a8258, frozen build runs/cgar-native-metric-build-v73-20260920.
  Expected legacy controls146659/146566 and147422/147502. Matching/short OFF.
- V73 source implements explicit native20/200(+1bands), turn1 and raw potential
  scalar, schedule base20. All28 source/test hashes bound. Full regression passes
  617376 states,39302 macro scores,1416 service cases, independent pickup oracles,
  wide-distance fallback/eviction, integer/default bounds and CLI isolation.
  Four200-step screens valid with both exact legacy prefixes. Separate800-step
  generic matching control exactly reproduces c0f1c9fb... with no trick activation.
  See build-provenance/v73 and results/native-{metric-screen,generic-control}-v73.
- Combined pickupweight V74 full8899466/analysis8899467: unchanged V72 binary/source
  c696d5f, four cases weight1/5 x seeds0/2. Lanes/potential/matching64 ON, short OFF.
  Raw `runs/cgar-combined-pickup-full-v74-20260920`; results/combined-pickup-full-v74.
  Analyzer `tricks/pickup_weight/analyze.py --combined-score` checks exact V72
  control hashes. Older inactive native receipt fields normalize to OFF only.
- Persistent Fable review turn36 RUNNING, PID1049, session
  1ebb1075-3538-49d1-93d1-a00c94fa256a. Started09:46:06UTC. Source payload is authorized;
  review focus native metric units, bounds and isolation. Prompt followup-35.md.
  Read finished response.json/response.txt and qualify claims before adopting.
  Previous turn35 archived, recommends this bounded native fidelity comparison.

## Next actions

1. Review completed Fable36; fix actual defects with reproducing tests if any.
2. Verify both full comparisons, record ALL failures/results. Any increasing full
   best updates root WAREHOUSE_PROGRESS.md with exact source/time/hash/flags.
3. If native wins, consider its interaction with validated matching and pickup
   weighting after isolation checks. Current native selector deliberately rejects
   matching for this first experiment; adding compatibility requires tests/build.
4. Keep paired controls and separate generic confirmation. Do not mark goal
   complete below154795. Commit/push our work only; never stage random05 changes.

All evidence lives under experiments/construction-20260918/results. Build and
benchmark snapshots are immutable in ignored runs/. No partial prefixes are full
quality scores. Legacy analyzers may need explicit native-OFF receipt normalization
when used with newer benchmark tools; active jobs use frozen analyzer snapshots.

## Historical notes below (current status above supersedes old running labels)

## Current jobs and latest steering

User requested cost preference/long-task deferral as an explicit TRICK. Source
f939b3b, buildV58/8899307, full regression plus nine CLI checks pass. Selector
CGAR_TRICK_SHORT_TASKS removes HRRN and forced-oldest admission only after
--trick WAREHOUSE; independent CGAR_TRICK_LANES selects static lanes. Default flag
preserves earlier lane behavior. No started task is dropped. All four200-step
screens verified; both generic-control and lane-only hashes exactly reproduce.

Full factorial8899316 completed all eight valid cases (seeds0/2 × four arms),32 bound
physical cores on shared research50,5s deadline/32GB RSS. Analysis8899317 completed and independently verified all full hashes, fixed work, age/travel accounting and chain inventory;
raw runs/cgar-short-task-trick-full-v58-r2-20260920. Raw totals: control144510/144107, short136833/134580, lanes146659/146566, both137985/137934. All exact controls match. Short loses5.961%/5.902% without/with lanes; empty work+34.31%/+32.83%, unpicked4000 rises0→1356–1392. Keep OFF. Fable turn31 now reviews bounded next steps in the same session. The first full launch8899310
was cancelled for failed GRID binding (64-core mask for32 requested, overlapping
other-map cores), analysis8899311 cancelled too. This is an infrastructure failure,
not an algorithm score. Launcher rejects unapplied binding before any simulator.

Generic remaining-flow full0/2 OFF/ON8899305 and analysis8899306 completed successfully on research52; see the new peak and confirmation below. Other-nine generalization8899301/analysis8899303 completed:7 valid at5s, GAME/RANDOM-05 fail at342/41 in regional repair. RANDOM04mean3.070s is too slow for competition. Full report experiments/generalization-20260920/results/summary.md. Preserve their CPU
contention note from the cancelled overlapping trial. No new generic record.

Fable turn30 completed in the same session. Read the local assessment before
adopting claims. The task-pricingV59 sourceb50eebd passes the expanded full suite (60 hand cases, both native bases, active resident/missing quotes, unchanged work). Turn29 diagnostic fixes are implemented. Native/shadow200-step GRID8899323/analysis8899324 passed: both hashes exactly match reference,71 per-call snapshots cover t=129..199. Coverage grows0.34% after publication to31.51% for calls65..71;κ≈1.016–1.019; conditional rank changes observed, no quality claim. Full seed0 shadow8899328/analysis8899329 completed: exact144510/481323a5..., all5000valid, max959.305ms/RSS11.358GB. Coverage grows0.575% firstcall→54.170% after256; full4871rankreports through4999. See results/chain-shadow-full-v59. The active target is154795, not stale goal
metadata152981; shared5s runs are allowed but cannot claim strict1s certification.

V60-R3 job8899333 passed all regressions, including20 real scheduling publication/
flush/rebuild cases in all pricing modes/scales/native caches. Production sources
and binary exactly V59; source4964309. The first two fixture failures are archived,
not solver failures. Active three-arm screen8899334/8899339 passed, exactnative and128-step prefix.
Full8899340 now runs native0/resident-imputed2/ratio-only3 × seeds0/2 on shared
research50,24physicalcores verified,4percase,5s/32GB. Frozen source4964309.
Raw runs/cgar-chain-pricing-full-v60-20260920; held analysis8899341 writes results/chain-pricing-full-v60.

## Confirmed reference and frontier

New generic single-run best145,083 (source4524843, completed2026-09-20T05:28:00.961539UTC). Remaining-flow scoring improves full0/2 by573/108, paired mean+0.235953%; tails+72/+112 and agep90−2/−4. All20k entries valid at5s, max1004.262ms, peakRSS11.340GB. Both controls exact. Not promoted; reference remains regional. Confirmation8899325/8899327 is complete. All6paired effects+573/+606/+108/-793/+223/-3; mean144060.1667 (+119,+0.08267%),4wins2losses, remaining4mean+8.25. Keep defaultOFF/referenceunchanged. results/remaining-flow-six-seed-v56-shared.json. Shared5s,32physicalcores verified; all12fullvalid/exactcontrols.
The newly confirmed six-seed mean is143,941.1667, versus142,474.8333 (+1.02919%).
Seeds0..5:144510/143933/144107/143134/143934/144029. All six full totals improve;
five tails and ages improve, with seed1tail−108 and agep90+1. The mean is5.909% below
the earlier local KittyKnight measurement. Reaching the revised154795 target
requires7.5405% growth from the current mean (see TARGET.json). The user questioned
comparing143941 against the144k frontier; clarified
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

WAREHOUSE_PROGRESS.md has43 increasing single-run records and8 confirmed
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
Raw runs/cgar-deadline-duplicate-build-v50-20260919. Exact22source/test binding is cb6a666da8aee461d5a34cab01d7b7ccaa76a201; v50/exact-source-commit.json.
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


## Latest completed evidence (2026-09-19 23:40 UTC)

Work rebalance8899182/8899183 is complete; results/regional-work-balance-full-v48/
comparison.json verifies exact V48/8ba2484, five valid runs/25k entries, two exact
regional controls and all sampled cumulative audits. 3M/four rounds143056/143421,
mean−0.741%; both tails lose. 2M seed0 explicit timeout step36/1000.580ms;
seed2 valid143046,−1061 tasks. Both rejected. Reference remains4M/two rounds.

Offline chain-price replay8899212 and analysis8899213 complete. Archived in
results/chain-price-audit-v50; code chain_price_audit/. All60 full publications and
seven trajectory hashes exact. 3840 sampled known assigned tasks,3429 completed,
411 censored. Leave-one-seed-out MSE286.1083→270.4558(−5.4708%); all6 folds improve;
withinpublication permutation p1/201, early-assignment sensitivity−4.949%. Independent
NumPy SVD versus stdlib QR errors/coefficients match1e−8. This is predictive evidence,
NOT throughput or counterfactual assignment evidence; complete offline tables do not
establish resident production coverage. See README limitations and archived provenance.
Next implement/test optional A task-chain pricing using const resident-table peeks,
keeping original spatial cache calls/admission, complete fallback and current metric
units; compare raw covered costs and ratio-imputed missing costs against modeOFF.
No production A implementation yet; B heading coupling remains deferred.


## Latest decision after broad review (2026-09-20 00:14 UTC)

This section supersedes the earlier pending-run and implementation-next notes.
Full V50 jobs8899209/8899210 COMPLETE: strict verify_v50.py confirms22source/test
hashes, binary,10000valid complete entries,32GBRSS,physical isolation,noquota,
exact seed0/2 reference trajectories144510/144107 and every25sampled search frame
(50regionalround frames). Max919.194ms,RSS11.337GB; means384.13..406.12ms,
CPU1.715..1.768cores/4. No throughput gain. Archive deadline-duplicate-full-v50.

Fresh CURRENT regional reference replay jobs8899228..8899233 completed and verified:
all6 rawSHA agree with previous independent inputs,300M phase/actions conserved,
all4 hand fixtures pass. accounting.json: per-completion CGAR347.364vsKK326.838;
loaded detour/turn/wait gap12.3023, shortest-chain cohort8.0252, empty−1.3966,
unfinishedloaded+1.5952. Observational, no guaranteed causal savings. Report:
bottleneck_review/REPORT.md, source comparison and17 literature leads included.

Fable turns18/19 archived COMPLETE, same session1ebb1075-3538-49d1-93d1-a00c94fa256a,
claude-fable-5-1/max,no tools/denials. Turn19completed00:14:17.860547UTC;
81.30132875USD is cumulative session cost. PTY52269 closed, no live CLI.
Next optional reviewturn20/followup-19.md, retain24USD request cap, sourcecacheV50.
Turn18ratio-only control and coverage/basis qualifications recorded. Turn19changes
priority to KK guidance/rematching ablations before bigger CGAR planner rewrite.
Assessment rejects causal certainty about chain savings, cohort normalization,
5slothorizon, and oldstaticpickup opportunity being a recoverable bound.

Current production restored EXACTV50. An unbuilt/untested V51chainpricing draft is
saved ONLYin bottleneck_review/assignment-price-draft-v51.patch with DRAFT_STATUS.
Do not treat it as working source. New implementation should follow selected design
and tests, not blindly reapply it. No code build/live GRID job remains at this note.

Next: frozen KK factorial supplied/uniform weights x unopened rematching ON/OFF,
with contemporary native controls; these are resource-qualified competitor diagnosis,
not CGAR scores. CONFIG_PATH+empty map_weights_path already supports all-one weights;
rematchingoff needs an experiment-only scheduler patch keeping unopened assignments.
Do not mutate archive. Clock-limited KK needs replicate controls/effort records.
Then CGAR additive ordinary assignment x broader bounded rematching with all
fair/protected rules, separating pricing basis. Prior pool guard onlyallowedpickup
improvement,32exchanges/10ticks, so it does not settle that interaction. Mainmotion
changeconditional on current multiple-owner/boundary/preference opportunity audit,
notmoregeometricregions. Full5k0/2 then6forpromotion; reserve fresh seeds for final
claims. See report for detailed gates and caveats. Goal remains active/unmet.


## Latest live diagnosis (2026-09-20 01:05 UTC)

Supersedes previous no-live-jobs / next-Fable-turn notes. ProductionCGAR remains
exactV50, frontier144510 / sixseed143941.1667 unchanged. V51draft stillunapplied.
KKarchive unchanged. New experiment-only code and compact evidence:kk_ablation/.

KKbuildV2 job8899242 COMPLETE00:49:23.626873UTC, binary
4e4d11880896ad741a0735683e5572dc2e2bb0dbbefd6dcec82505c47dadb70a. Threefrozen
fileschanged:schedulereligibility, LNSdiagnostics, Entrytiming. Fiftyfixturecases
pass acrossnative/patched/modes/uniform+nonuniform, realTaskManagerlifecycle,
ownerchecks, invalidtoggle, actualconfigloader. All4corrected200screenspass.
InitialuniformV1screens exit139 fromOURmalformedscalarconfig; evidencepreserved.
Correctuniformconfigretainsconditionalarrayandemptieseachbranchvalue.

KKfullarray8899243.1–5 RUNNINGsince00:50:42UTC, raw
runs/kk-guidance-rematch-full-v2-20260920. Cases supplied_rematch-r0/research44,
supplied_keep-r0/research57, uniform_rematch-r0/research50,uniform_keep-r0/research38,
supplied_rematch-r1/research52. Each16physical/32SMT,noquota,exclusiveEPYC9354,
128GiBreservation,5000steps,1s nativeclocklimit. Thesearecompetitordiagnostics,
not32GB/completeentry-certifiedCGARscores. Fullanalysis8899246 heldon8899243,
frozenraw/support/analyze.py+raw/decompose.py/.cpp; outputresults/kk-guidance-rematch-full-v2.
It verifieshashes/allocations/50Maccounting, retainsactualRSS/errors/timeouts,
checkskeepmodezeroreassignment/startedprotection andlogsactualscheduler/LNSbudget.
Rawtrajectories/binaries/protocolnotcommitted. Expected~83minperfullKKrun.

CGARfullread-onlyblockeraudit8899240 RUNNINGresearch31 since00:42:32UTC,
rawruns/cgar-current-conflict-audit-full-v50-20260920. Twoisolated4coreseeds0/2,
CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE=67,exactV50binary. Heldanalysis8899241
usesbottleneck_review/collect_current_conflicts.py afternormalmotionanalysis.
Expected74frames/seed, exact144510/144107 trajectories, allsampledsearchcounters
unchanged. Auditclassifiespostrepaircandidateblockers,notfeasibledisplacements;
stationaryincludesusefulturns; no regionboundaryclassificationyet.

Fableturn20 COMPLETE00:38:21.718270UTC, samepersistent5.1/maxsession, no tools
orpermissiondenials, cumulativeUSD82.33881575. Archivedvisibleanswer+assessment.
Currentturn21/followup-20.md NEXT ifneeded; no liveCLI. Avoidreloadingcodebase.
Keepadditive×rematchinghypothesisconditionalonfullresults; no blindparameter
sweep or repeatofoldpersistentrouteguidance. Continuecurrentaudit andfullKK
analysis, thenimplementselectedgenericchange withstrictdeadline/fixedwork.


Task-pool audit8899247 COMPLETE01:11:04.641026UTC, raw
runs/cgar-kk-task-pool-audit-v50-20260920. results/task-pool-audit-v50 and
bottleneck_review/TASK_POOL.md verified all7original accountingreports identical,
allrawhashes vs earlierreplay, 4basefixtures+newownership/reveal/pickup/endfixture.
Atstep5000KKunassigned5025,chain527.639,age2706.898,p904877; CGAR6ranges5021–5044,
chain306.201–307.313,age100.5–104.0,p90209–214. KKunpicked>=1000:5130,>=4000:1642;
CGARnoneaged>=1000atanysample(0,1000,...5000). Supports persistent long-task
deferral, not a causal recovery estimate. Historical KK held1959unopened at5000,
mean427.74chain/1761.7age. Neverassigned4952vsunassigned5025:73released tasks.
Newofflinepool_audit.py generates augmented decompose.cpp inraw, keeps production
and original analyzer unchanged. Fable should see this before designing new
additive/rematching fairness/age controls. Stillwait fullKKfactorial, no newscore.


Current-blocker full8899240/analysis8899241 COMPLETE,strict auditchecked01:18:14.835419UTC.
results/current-conflict-audit-full-v50/audit.json all22files/exactV50binary,
10kcompleteentries,max928.438ms,RSS11.334GB,noquota/disjoint4physicalcores,
exact144510/144107trajectories+all25searchframes/50regionalroundframes.
74samples/seed. Last1000easiestphysical counts seed0/2:stationary808.67/827.07,
noqualifying319.93/322.27,unblocked2.80/3.33,oneowner322.93/332.20,
twoowner140.33/141.00,many22.60/28.27,protected0.067/0. Stationaryincludesrotations;
heuristicgain!=actualsavings,movable!=feasibledisplacement. Scalarunblocked39.1/40.1
mostlyoppreference. No regionboundaryprobe yet. CURRENT_BLOCKERS.md documents limits.

KKfull8899243 stillactive; analysis8899246andpoolanalysis8899251held. New
kk_ablation/analyze_pools.py usesvalidated historicalpool binary, checksalloriginal
accountingfields againstfirstanalysis,andwillwrite results/kk-guidance-rematch-full-v2/pools.
Bothanalysesfrozenwithhashes. Fableturn21RUNNING,samepersistentFable5.1/maxsession,
rawruns/fable-cgar-session/turn21,PTY84098,promptfollowup-20.md;sourcecacheV50unchanged.
Promptconsolidatesnewpoolagesandfullblockersandasksconditionalassignment/motiondesign,
retainedprogressguaranteesvsage-statistics,notnewmaptemplates. Wait/review/archiveit.


Fableturn21 COMPLETE01:23:46.685504UTC; PTY84098closed, no liveCLI. Archivedvisible
answer/metadata/status/assessment. SameFable5.1max/session,0sourcechanges,no tools
orpermissiondenials,cumulativeUSD83.4075485. Nextturn22/followup-21.md ifneeded.
Usefuldesign: task-centeredfresh-free-task retargeting, heading/traffic pickup +
nativechain, separateordinaryadditivefactor. Requiredcorrections: doNOTremovefinite
retargetlimit based on a local cost-decrease claim (positions/metricchange); don't
letprepassfree-agentassignmentsconfoundrematch-onlyarm; preserveprimary/recovery/
fair/startedprotects, proposed-scheduleavailability, completecommonmetricquotes,
cachecoverage/fixedwork/fullageandemptyaccounting. 256BFS×2048proposedworknotvalidated.

Fablemotionprobe: completejointcounterfactualroot+1/2owners, outsidefixed,same5slot
candidates/objective. Its assumedfrozendecisiondump DOESNOTexistyet; currentartifacts
aretrajectories/aggregateaudits. Needreadonlycapture andunchanged-decisionverification.
Null<=3robotsdoesNOTrejectlargergroup/horizon/MDPIBT; witnessdoesnotproveproduction
reachabilitywithoutfaithfulDFSpower/ordering/rollbackreplay. Readassessmentbefore
implementing. CanbuildthisboundeddiagnosticwhileKKfullrunsfinish; no blindpolicyport.

CurrentlivejobsONLY KK8899243.1–5, heldaccounting8899246, heldpools8899251.
BothCGARfullauditsandhistoricalpoolreplayCOMPLETEandvalidated. Best144510 and
sixmean143941.1667unchanged. NextcompletefullKKanalysis (~02:15UTCifhistoricalruntime),
interpretallcomponent/pool/budget/resourceoutcomes,thenchoosegenericCGARfactorial
ornewmotionmechanism. Usergoalactiveandunmet; no productioncodechangeinthisdiagnosticpass.


2026-09-20 02:10UTC: NMS follow-up COMPLETE. bottleneck_review/NMS_REVIEW.md and
results/nms-task-pool-audit/accounting.json replay154981 diagnostic tasks,
1timeout/39.744GB, all50Msteps. Original rawhash matches; initial timeout preceding
first schedule added10000implicit idle steps (old idle48814->58814, loaded residual
40339131->40329131); all other historical fields exact.5actionfixtures+poolfixture.
Initial attempt8899254 failed correct identity check; fixed replay8899256 passed,
failed attempt archived. NMS loaded detour/turn/wait16.754576 vsCGAR30.985046;
unpicked>=4000:1562,unassigned meanchain506.889,age2703.024. No causal savings.
Active NMS one-owner/129five-slot/equalpower already ported; try_echo_slam unused;
warehouse guidance is coordinate/lane-mask-specific, not generic online guidance.

Fableturn22 COMPLETE02:03:49.929979UTC, same5.1/maxsession,4sourceupdates,no tools/
denials,cumulativeUSD84.85065025; rawturn22,PTY57520 needs finalpoll ifstillopen.
Archivedvisibleanswer+assessment. Nextturn23/followup-22.md. It suggests fresh-task
coverage and degree2-chain guidance-coherence measurements; corrections inassessment:
no causal 'mostly outside kernel' claim, pre-scheduling retarget confounds arms,
rank holder bypickup-oldcost, eventual-assignee costs notsame-state counterfactual,
smallgroupwitness may use seed0 unavailable in ordinaryrepair. No blindpolicyport.

User subsequently AUTHORIZED map-specific tricks if genericgap proves difficult;
keepseparate tracks. Every trick MUST be enabled only by --trick <map-instance-name>
(e.g. --trick WAREHOUSE), with [trick] commit title and TRICK log entry. No flag
means generic. Not an environment-only override or automatic map detection.
EXPERIMENT_TRACKS.md is authoritative session policy; no trick implemented yet.
Fable22's old absolute guidance-import ban predates this steering. Preserve1s,
32decimalGB,validity,isolatedCPU,nohiddenfuturetasks in BOTH tracks. Neverreplace
generic frontier with trick-assisted score.

V52 readonly group snapshot source/regtests/build complete8899253, alltests pass.
Capture flags CGAR_TEMPORAL_GROUP_SNAPSHOT_COUNT=8, CONFLICT_AUDIT_STRIDE=67; defaultoff.
Snapshot-off/on200screen8899255 exact2888task trajectory a4bfe67f610e2451bb40e68da0e9fad0b75985d3f7701528334fee73f93cea8b,
max936.241/931.907ms,RSS5.269GB,4physicalcore each,all400entries valid.
Offline complete joint_probe.cpp+run_joint_probe.py screen8899257 passed6fixtures,
32groups67/134:16size2nonefeasible;16size3twofeasible,onescalarbetter,noneboth
scalar+fullunitaggregatepotentialbetter. Earlysample only; noqualityconclusion.
Needfull0/2capture+exactreferencehash/counter/deadline/sourceverification, then
samecompleteprobe. Buildfrozen runs/cgar-group-snapshot-build-v52-20260920,
build-provenance/v52 archived. No productionpolicychange.


V52 source/test exact commit f1c3c8abc4717ec921ca6e241366b63f77cea3e3 (22hashes).
Full capture8899258 RUNNING since02:11:02UTC onresearch31,8physical total,4/seed,
rawruns/cgar-group-snapshot-full-v52-20260920. Held verification+completeprobe8899259,
frozen raw/support/analyze_group_snapshots.py +5analyzerdependencies+verify_full.py+
joint_probe.cpp. Outputresults/group-snapshot-full-v52. It validates allcompleted
referencehashes/counters,5000entries,32GB,physicalbinding,74frames<=16groups each,
thenexhaustivelychecksallcapturedgroups. Invalidfullruns stayvisible, no partialscores.
Fable22 PTY57520 closed, no liveCLI. NextFable23mustinclude newusertrickauthorization
and mandatory --trick <map-instance-name> gate; donotrepeatoldabsoluteimportban.

KK8899243 all5tasks FINISHED by02:14:44UTC: suppliedrematch r0/r1=152872/152786,
suppliedkeep142510,uniformrematch24574,uniformkeep23620. EVERYrun1timeout,
0planner/scheduleerrors,38.18–38.85GB; all diagnostic-only. Account8899246running,
pools8899251held. Initial suppliedrematch decomposition succeeds with frozen
pre-initial-gap-fix analyzer; do not assume NMS's missing-initial-schedule bug
also affects KK or prematurely rerun/modify frozen jobs. Needfullaction/cohort/
budget analysis before interpreting the strong supplied/uniformcontrast.


KK accounting8899246 COMPLETE02:16:52.653965UTC, pools8899251 COMPLETE02:18:56.106192UTC.
All5rawhashes/frozenconfigs/source/binary/allocations,4actionfixtures,50M-step
identities and pool-original-field equality pass. kk_ablation/RESULTS.md fulltable.
Suppliedrematch loadedoverhead18.830/18.987,empty65.734/65.794; suppliedkeep18.183,
empty88.852. Uniformrematch loadedoverhead408.202,empty546.639,unfinished890.299;
uniformkeep479.220/687.006. Uniform+rematch has similarmeanremainingLNSbudget
.877vs.874s and MORE mergediterationrecords40.9kvs21.1k; countsnotallattempts.
Guidancewholepolicyinterventionstrong, motion-onlycausalitynotidentified. Shorter
uniformcompletedchains189.35 doNOTpreventcollapse. All5one-timeout/RSS>32GB remain
nonqualifying; originalvalidKK152981targetunchanged. Poolunpicked>=4000:
suppliedrematch1632/1643,keep1377,uniformrematch5360,uniformkeep4268.

Fableturn23 RUNNING same5.1maxsession, rawruns/fable-cgar-session/turn23,
PTY62925, promptfollowup-22.md,0sourceupdates. Promptincludesexactnewusertrick
permissionandmandatory --trick WAREHOUSE gate, fullKKaction/accounting/budget
results,poolsstillpendingatprompttime. RequestsONEgenericnextchange andONEfirst
trick design, notesforwarduint8cost<=16,turn/base<=16,Dialbuckets,auto16/32bitcache,
NMS20/200forward1turn vs CGARpaidforwardextra/scale4/op*4,staticfieldpublication
andpickupcouplingconfounds. No trickimplemented. Awaitreview,archiveassessment.
Nextturn24/followup-23.md onlyafterturn23complete. No otherliveCLI.


## 2026-09-20: explicit trick activation and first isolated candidate

User requires `--trick <map-instance-name>` as the only trick activation path.
Implemented experimental `--trick WAREHOUSE`: native NMS lane directions at
CGAR forward4/16 and turn4; exact occupancy/dimensions/name checks, no env-only
activation, no learned publications over the static field. Generic policy remains
selected when no flag is passed. Separate TRICK metadata/results/profile labels.
This is not a new score. Full generic snapshot jobs8899258/8899259 still pending.

Fable turn23 completed02:24:34UTC, persistent session unchanged; archived its
visible answer and local assessment. Native field extraction8899262 rejected
traversable S/E cells in our harness; corrected8899263 passes all280,000cells/directions
and orientation checks. Both provenance/failure records are retained under
tricks/nms_warehouse. Build/regressions8899264 running from frozenV53 sources.
After build: CLI rejection fixtures, generic/trick paired200-step startup screens,
then full runs only for valid candidates. Generic full trajectories must remain
identical; no promotion from a short screen. First trick changes the metric,
cold-start/cache lifecycle and pickup quotes together; separation arms later.

V53 build8899264 passes all regressions; five CLI failure checks pass. Generic
200-step8899265 completes seeds0/2 at2888/2857 with all400entries<1s; analysis8899268
is complete. TRICK8899266 fails both at entry0
in pickup_flow_candidates at1000.201/1000.189ms; no score. Fable turn24 reviewing
minimal fixed startup change via persistent CLI, raw runs/fable-cgar-session/turn24,
PTY33150. Next V54 keeps generic initial dispatch, then static pickup from tick1.

Full generic snapshot8899258/analysis8899259 complete: exact0/2reference trajectories,
all10,000entries valid<1s, unchanged work counters,22source/test hashes exactf1c3c8.
2,368complete groups;44have both scalar and aggregate guidance-potential improvement
(22of1,184each group size),8of480final-window groups. Not a population estimate,
physical-work gain or production reachability proof. Report/witnesses archived.
V53 source+tests exact6e0612f; generic200-step seed0hash exactV52reference, seed2
2,857tasks, all400entries pass. V54 build8899269 now running with generic tick0
dispatch and tick1 static pickup admission; no default/generic policy change.

V54 build8899269 passes all regressions. TRICK200-step8899270 passes both seeds:
3,709/3,710tasks; max entries994.717/987.554ms; RSS5.996/5.965GB; unique expected
planner receipts and result track labels verified. These are startup results only;
no full-run score or frontier promotion. Next full generic/trick0/2 on isolated
four-core EPYC allocations, maintaining1s and32decimalGB.
Fable24 completed same session; source review finds no concrete defect, supports
startup fix; adopted two-sided receipt validity guard. Raw/archive turn24.
Generic degree2corridor topology check passes4fixtures:63.13%free cells and66.64%
edges incident to such chains, all chains2or3nodes. Enough coverage to measure
traffic/coherence; no claim of field incoherence yet.

## Active handoff: V54 full comparison (2026-09-20 02:59 UTC)

Source+all26production/test hashes exactabbe36b723bb43de2478c07bcb88621d5238a23a,
committed/pushed. V54startup analysis8899273 completes both cases: exact generic
initial assignments (all10,000robots), correctunique field receipts/result labels,
no learned publications. Cold0..80/0..100max994.717/987.554ms; not a fullscore.

- Generic full8899271, raw runs/generic-warehouse-full-v54-20260920, research44.
- TRICK full8899272, raw runs/trick-nms-warehouse-full-v54-20260920, research50.
- Both launched02:54:33UTC, seeds0/2 parallel, fourphysicalcores/case, EPYC9354,
  noquota,1scomplete entries,32decimalGB; expect~03:25–03:30UTC dependingpolicy.
- Held independent analyses8899274/8899275, frozen helpers in each raw directory.
  Check results/{generic-warehouse-full-v54,trick-nms-warehouse-full-v54}/
  trick-verification.json and verification.json. Generic full trajectories must
  exactlymatchV44reference. The trick has no promoted full result.
- No live Fable call. Turn24 archived, samepersistent session, nextturn25 /
  followup-24.md. Full snapshot+newcorridorcoverage were not in turn24payload.
- Generic next diagnosis: observed traffic/field sign coherence over topology-
  derived degree2straightchains. Static coverage63.13%cells/66.64%incidentedges;
  lengths2–3only. Need movement-weighted coverage and disagreement BEFORE pooling.
- Keep all new map-specific experiments behind --trick WAREHOUSE; [trick] commits
  and TRICK log entries. No-flag generic frontiers unchanged.

## Generic flow coherence replay queued (2026-09-20 03:10 UTC)

Previous goal turn made progress: explicit trick gating, verifiedV54startup,
full no-flag/TRICK matrices queued, and completed generic exhaustive-group
diagnostics. The generic target is still unmet and separate from TRICK.

New offline FlowGuidance/traffic replay has frozen sources and8historicalcases
(sixgeneric+KK+NMS),3physicalcores/max3concurrentprocesses. GRID8899278 is queued;
raw runs/flow-coherence-v55-20260920. Initial submissionfailed because all exclusive
hosts were occupied; retry only changed immediate-availability verification,
not resource isolation. Do not resubmit a live queued job. Expect sourcefixtures,
400Maction conservation, all60genericpublicationcountermatches and validgeometry.
Results target results/flow-coherence-v55/verification.json. No result yet.

Fable turn25 is running via persistentCLI, raw runs/fable-cgar-session/turn25,
PTY58092; next promptisfollowup-25.md after this completes. Asked it to audit the
diagnostic and the conditional generic degree2chain pooling operator, with
minimum mean evidenceperedge preserved. Suppliedfullsnapshots+startupfacts.
No generic production policy has changed in this turn.


## Current handoff: verified full V54 / coherence V55 (2026-09-20 03:36 UTC)

All GRID jobs8899271/72/74/78/79complete; no live GRID workload. Generic full0/2
exact144510/144107trajectories; TRICK146659/146566, all20kentriespass1s, RSS11.914GB.
TRICKtail−93/+50 means no consistent sustained-rate gain. Separate TRICK records
in WAREHOUSE_PROGRESS; generic frontier unchanged. All26source/testfiles exactabbe36b.
TRICKanalysis8899275archive-filename failure retained, alias+verifier-only8899279
passes. Frozen benchmark/source/analysis inputs unchanged; no benchmark rerun.

V55replaypasses400Mactions/60fields/sevenfixtures. Zero mixed toll signs across
ALL60short-chainfields; latepoolingchangesonly202–240directedcosts. Drop short-chain
pooling. Allresults archived, FLOW_COHERENCE.mdupdated. Cross-junction circulation
stillunmeasured. No new generic production change yet.

Fableturn25complete/archived. Turn26running samepersistentCLI, PTY98363, prompt
followup-25.md; no sourceupdates. Sent fullTRICK/generic/coherence facts. Proposed
GENERIC default-off score ablation: omit only paidforwardextra from local score,
keep SAME learned potential/turns/opscale/assignment/work/protectedprimary. Reject
next-errand/guideroute combinations initially. Currentcode NOT YET edited; awaiting
review while archiving. Nextturn27/followup-26.md after completion if useful.

Fable26completed; visible review/metadata and qualified assessment archived. It
supports the remaining-flow ablation as a cheap test, with neutral/negative prior.
No live CLI; nextturn27/followup-26.md. Implementing the narrow generic factor now.


## V56 generic candidate awaiting isolated build (2026-09-20 03:50 UTC)

Implemented default-off CGAR_TEMPORAL_REMAINING_FLOW: omit only paidforwardextra
from temporal objective. Protected construction/oracle/scheduler unchanged.
Reject statictrick/guide/next-errand/paid-progress audit/branch combinations.
Four productionfiles+regressionfile changed; build8899283QUEUED, no test result.
All eligible exclusivehosts occupied; PEthreaded reports0availableslots. Same
frozen4core/16GiBrequest enqueued with-w n, originalavailabilityfailurepreserved.
Do not resubmit or edit frozenproduction sources while job pending/running.
Raw runs/cgar-remaining-flow-build-v56-20260920; pendingarchivebuild-provenance/v56.

Afterbuildpasses: archivebuild/allocation/log; bind26sourcehashes toexactcommit;
runremaining-flow-variants.json seed0,200steps,two4corecases inparallel onEPYC9354;
remaining_flow/analyze.py freezes/verifiescode,receipts,genericlabels,full128action
andassignmentprefix,equalworklimits,exactdefault200hash. Thenfull0/2x2arms16cores,
5ksteps,1s/32decimalGB; analyze.py supportsfullvalid/failurearchiving andexactcontrols.
No new benchmark has been submitted or scored yet. REMAINING_FLOW_SCORE.md.


## Latest pending work (2026-09-20 04:02 UTC)

User requested a quick generalization check while warehouse continues: queued
GRID8899287, nine otherinstances allconcurrent, seed0, fullcompetitionhorizons,
36physicalcores total/4each, exclusiveEPYC9354,1s/32decimalGB. EXACTverifiedV54
binarybace95.../sourceabbe36b, unchangedwarehouse-referenceprofile,no --trick,
no new remaining-flow option. Rawruns/cgar-generalization-other9-v54-20260920;
request/status/sourcebinding inexperiments/generalization-20260920. Initial
availabilityfailurepreserved, same resource request queued with-w n.

Answered user randomness question: CGAR_SEEDcontrols PIBTties, temporalroots,
horizons/displacementchoices/annealing and regional search streams. Inputs/task
sequence fixed; reveal times/learnedfields differ indirectly. Sixseeds areplanner
realizations on sameinstance, notnewworkloads; testedfixedworkrepeatability.

Fable27completed/archived, samepersistentCLI, no livecall. Found no scoring or
protected-constructiondefect; adopted realactiveadapterdifference fixture and
specificconfigurationguardchecks. Corrected its false claim that regionaltests
neverassertnonzeropenalties; theyalreadydo. Retainingturnpayments isintentional.
Build8899283cancelledWHILEQUEUED, noexecution. Strongertest-only revision frozen
and queuedas8899285, rawruns/cgar-remaining-flow-build-v56-r2-20260920, archive
build-provenance/v56-r2. Productionhashesidenticaltofirstv56. Do NOTeditfrozen
source/testfiles while pending. No v56testresult or benchmark yet.

Generalization analysis8899288heldbehind8899287; frozenhelpers/sourcebinding
checks, fullper-instancevalidity, resource/actionaccounting and results/summary.md
underexperiments/generalization-20260920. No rerun or retuning planned before
readinginitialtransferresults. Current questionaboutRNG answeredincommentary.

Latest persistent review: turn31 / followup-30.md, started2026-09-20T06:10:31UTC,
same session1ebb1075-3538-49d1-93d1-a00c94fa256a, modelclaude-fable-5-1/max,
explicitly read-only/no tools. Raw runs/fable-cgar-session/turn31; PTY45080.
It is reviewing the VERIFIED unfair-selection results and whether bounded unopened
retargeting deserves one isolated TRICK comparison. Do not treat this pending
review as a recommendation already received. Generic full pricing8899340 runs
independently. No pool/TRICK guard change or new retarget component implemented.

2026-09-20T06:29UTC: Fable31 complete, no live CLI. Public visible findings/metadata/
qualified assessment in fable-flow-session/turn31. The CLI result review.md is only
a final segment; emitted-findings.md contains the complete visible answer. Reviewer
withdraws turn30 recommendation to simply enable old pool exchange. Its cap16000/
run, residual-pool timing and unit metric make it weak here; not a mathematical
impossibility proof. Broader holder bidding is deferred pending generic tests;
no TRICK reassignment guard has changed. Next review would be32/followup-31.md.

Generic global-worker + region combination: no production change; V60 source4964309.
Screen8899342/8899343 passes, exactnative/completeworkerparticipation/regions.
Full8899344 compares1vs4globalworkers, each4Mquota, followedbyunchanged4regions2rounds,
seeds0/2; sharedresearch52,16physicalcores,4percase. Raw
runs/cgar-global-region-workers-full-v60-20260920; analysis writes
results/global-region-workers-full-v60. IncreasingworkersalsoadvancesRNGmore;
whole-policytest, not equal-work speedup or isolatedbest-initial-plan mechanism.
Existing genericpricing8899340/8899341 remains live on sharedresearch50/24cores.
