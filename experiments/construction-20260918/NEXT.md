# Continuing warehouse work

Updated 2026-09-19 21:06:42 UTC. Active unbudgeted goal: meet/exceed local KittyKnight152,981
on full5,000-step/10,000-robot warehouse, repeatably across six seeds, every complete
schedule+plan<=1s or explicit failure, processRSS<32,000,000,000 bytes, isolated GRID
physical cores. Generic policies only; no map identities/templates/supplied weights,
hidden future tasks or map-specific fleet caps. KK used38.858GB, so not an official
equal-resource comparison. Goal active/unmet; concrete progress, no blocker.

## Current reference and new records

Working reference remains one4M planning worker/preparation4, graded strength4/scale4,
margin25/refresh512, turn-build128/cache8GiB, HRRN1/pickupweight1, pickup flowON,
64 complete pickup fields/four field threads, age-aware key0, warm/mixed/surcharge/
regional repairOFF. warehouse-reference-variants.json. ExactV42 source4872d048dd8155927074ac2a4d27a55756266fd5;
all22 hashes verified. Frozen runs/cgar-pickup-full-build-v42-r1-20260919.
Seeds0..5:141829/143325/142988/141802/141988/142917, mean142474.8333, range
141802–143325, +2.215% over preceding pickup-flow reference; all totals/tails/ages
improve. results/pickup-full-six-seed-v42.json. Resources: mean272.4–317.4ms,
p99557.3–602.0ms,max874.138ms,CPU1.425–1.494cores/4reserved,RSS11.934GB,
wall23.93–27.70min. All30k entries<=1s. CPU=(user+system)/whole-process wall.
User resource question already answered; do not keep repeating unprompted.

Highest observed six-seed mean143111.1667 is16workers (exactV43 source1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9):
143749/143241/143571/143643/141876/142587, +0.44663% vsreference,6.45% belowKK.
Three total/two tail regressions, so not promoted as working reference. All12valid,
all6controls exactV42. Mean326.1–392.0ms,max954.119ms,CPU6.267–7.680/16reserved,
RSS12.048GB. PICKUP_FULL_WORKER_SCALING.md; pickup-full-workers16-six-seed-v43.json.
Fourworkers also mixed:mean142742.1667, seed5loses3207total/1933tail,notpromoted.

NEW two-regional-round first pair:144510/144107 vs141829/142988, mean144308.5,
+1.33419%, tails+287/+120, agep90−3/−2. All6cases valid, all30k entries<=1s,
max953.192ms,RSS13.909GB,4physicalcores each,noquota. All22 hashes exactV44
f6d0ae448504762136c4f5d85f25d7d207207965, actualbinary verified; both disabled
controls full-hash exactV42. Loadedturns−13.44%/−5.12%,waits−12.82%/−4.68%,
empty+0.72%/−0.20%. Candidate mean401.2–419.4ms,CPU1.726–1.770cores.
One regional round mixed,mean141930(−.336%),tails−150/−576; not promoted.
PICKUP_FULL_REGIONS.md; results/pickup-full-regions-full-v44/comparison.json.
Jobs8899002/analysis8899003 complete, sampled regional work/retained scores verified.

Progress now42 increasing single-run records,7confirmed reference milestones.
New records144107 at2026-09-19T20:30:36.444606+00:00 and144510 at20:32:02.316251UTC,
both exactf6d0ae4. WAREHOUSE_PROGRESS.md and throughput-progress-provenance.json
updated with timestamps/links. No new six-seed reference yet. Preserve user editor
swap .WAREHOUSE_PROGRESS.md.swp; exclude, do not delete. Read/compare before edits.

## Live jobs

Regional confirmation8899053/analysis8899054: started20:38:54UTC research57,
seeds1/3/4/5 x disabled/two-round,8concurrentcases x4physicalcores=32,128GiB
aggregate,32decimalGB process cap. FrozenV44f6d0ae4. Config
pickup-full-regions-confirm-variants.json; raw
runs/cgar-pickup-full-regions-confirm-v44-20260919; future
results/pickup-full-regions-confirm-v44/. All4 controls must match V42 full hashes.
This is the priority confirmation. Assess all six totals/tails/ages/motion and deadlines.

Larger pickup quotas: full seed0 job8899064/analysis8899065 and seed2
job8899066/analysis8899067. Each matrix3concurrentcases:quota64/128/256,16field
threads,16physicalcores/process,48total,48GiBaggregate,32decimalGB process cap.
One global4M worker/prep4, regionalOFF. FrozenV46 (below). Config
pickup-full-quota-variants.json. raw runs/cgar-pickup-full-quota-full-s0-v46-20260919
and s2 analog; future results/pickup-full-quota-full-s0-v46 and s2 analog.
Control64 must match V42 full hashes despite16fieldthreads. No quality claim yet.
All three200-step prefixes are identical:64 may already cover most/all active demand.
Full results can establish equivalence or a later effect; do not assume more quota helps.

## Source, builds and checkpoints

Latest pushed exactV45 commitc339e8b4b3896c18234be0b1d4120a00ffcca519 contains
readonly audit, all regressions/screens,16worker six-seed evidence and finite probe.
All22 frozen source/test hashes match; exact-source-commit.json records this.
Frozen runs/cgar-service-audit-build-v45-20260919, binary
b3fbb35a2eec4968a5ea7bc5e8135c3d553597832eec14232b6a437661b3e18a.
Public fywu85/lorr; prior47a0b12 andf6d0ae4 also pushed.

V46 is committed/pushed as exact0b8361c1d28429f8fd9918a3470046d4414ad83f; all22
source/test hashes verified against that commit. Only production
change: generic CGAR_PICKUP_FULL_ROBOTS maximum64->256; defaults unchanged.
Quota257 remains invalid. Added real257-robot assignment fixtures at64/128/256,
one/four builder threads, correct field counts, unselected fallback robot, unique
nearest assignments and unchanged simulator metadata. All CGAR regressions pass,
including1542 serial/parallel production assignments. Build8899056 complete.
Frozen runs/cgar-pickup-quota-build-v46-20260919; archivebuild-provenance/v46;
binary65f7aa2c5695ebc923ef2c8170b479f3cc0cc111bc9ce6fbb1a122faf701d2ec.
The63-line patch reconstructs all22 source/test hashes fromc339e8b and matches current
workspace. No active build/no production source changes since freeze. All verified
regional/audit/quota evidence and the two new progress records were committed/pushed
with0b8361c. exact-source-commit.json records the V46 binding.

Initial quota screen8899044 usingV45 rejected128/256 at initialization because the
actual old cap was64. This was a setup mistake, not a performance failure. Its64case
was valid and exact-prefix. Preserve results/pickup-full-quota-screen-v45/.
Corrected V46 screen8899059 all3valid, max887.55ms,RSS5.229GB, disjoint16physical
cores each,noquota, all3fullprefixhashes exact reference0941646ba4985c0d4d18ad713e9563ac3edae7eac77aa50072c9fea59bcc0837.
Equivalent prefixes are not throughput evidence. PICKUP_FULL_QUOTA.md and
results/pickup-full-quota-screen-v46/equivalence.json.

New verify_full.py checks finished full matrices: exactcommit22hashes/binary,
5000entries<=1s,32GBRSS,50Mphaseactionconservation, matching fullmetadata,
physical-core nonoverlap during concurrent execution,noquota, then extracts rows.
Used successfully for regional and V45audit results. It does not itself check control
hashes or treatment-specific diagnostics; those are additionally checked in reports.

## Completed post-service audit and next scoring hypothesis

Full8899017/analysis8899019 complete: stride67, seeds0/2,74frames each; trajectories
exactV42 with141829/142988. All10k complete entries valid,max884.17ms,RSS11.697GB,
4physicalcores each,noquota,22sourcehashes exactc339e8b. Allframe/action partitions
conserve counts. POST_SERVICE_AUDIT.md; results/service-audit-full-v45/audit.json.
Only-known-next post-service conflicts average8.47/8.41 robots perframe,
7.32/7.70 afterstep1000 (0.073%/0.077%eligibleobservations). Finishing-only averages
23.88/22.80, mixed0. Otherconflicts~2347/2237. Arrivingknownnext~155/158 overall.
This specific class is small; not a bound on a different objective's throughput.
Audit uses easiest qualifying candidate, removes op ties, includes selectedFW, ignores
own reservations, counts service-slot/fixed/no-arrival owners asother, tests cellsand
edges, no RNG/cache/search mutation. Actions precede final seed-turn projection.
Different scores change its qualifying-candidate set; bucket movement alone does not
isolate occupancy improvements. Overlapping gains are not additive counterfactuals.

Finite goal_holding_probe.cpp found two3x7 native-score examples among987valid inputs:
native/witness costs−1040/−912 and−840/−564. Witnesscontinues knownnexterrands and
lets other robot firstFW; native holdsfirstgoal/turnsother. GOAL_HOLDING.md and
results/goal-holding-probe-v1.json. Pure score minimization, no warehouse gain claim.
Fable's old3x9 first-rotation claim failed exact minimization; it now acknowledges this.

Fable turn13 suggests common next-leg baseline B=min_heading D_next(currentgoal,h),
for arriving candidates only: d=−4U+D_next_star(endpoint)−B, plus ALLactualturn/toll
extras; native terminal-wait virtual rotation discount. Nonarriving candidates native.
Enable only for nonpinned, exact-oriented, nonguided robots with knownnext!=current,
resident nexttable via serial non-LRU peek afterbuilds and finite needed costs.
No nexttablebuilds/LRU effects. Candidate-dependent arrivalheading baseline creates
spurious credit; commonBavoids that specific bias. But arrivalbranch discontinuity,
cache-dependent eligibility and next-service approximation remain. Do not claimpaths
passingnextgoalandleavingaredominated under reservations/finitepool.

Offline continuation_probe.cpp is now complete; CONTINUATION_SCORE.md and
results/continuation-probe-v1.json archive source hashes, commands and output.
Independent Bellman distances + explicit weighted path replay/terminalwaitvariants
agree with the proposed commonB score on54,378 arriving candidates (U1/4/8,
turnextras0/1/2,directed tolls0/1);220,050 non-arriving cases use native fallback.
Fable's FRFFF/RFCFW physical pair reproduces−24/−16 withcommonB versus−24/−20
withcandidate-dependentbaseline. Our two native-holding examples now have exactjoint
commonB minima−1564/−1364, bothfirstFW (Aop80/Bop111), versusnative−1040/−840
(A58/B102). Neutral-tail separate toy minima−1552/−1356 alsofirstFW butA127/128
movesawayfromitsknownnextgoal. These are different objectives, not comparablequalityscores.
No production continuation/neutral policy implemented. Next useful local work:
implement one optional commonB policy (and keep neutral-tail a separate treatment),
defaultOFFexact, payallactualturn/toll extras, current/nexttablesimmutable, serial
non-LRU peeks, oneeligibilitychoiceperrobot, nonarrivingnative, repeatG=Nnative,
missing/nonfinite/guided/fallback/pinnednative, noextrabuilds. Add independentoracle
and serial/parallel/protected production tests before cold/full runs. The six-seed
regional confirmation is the immediate benchmark priority; choose its outcome as
context before deciding which policy to use as the next matched control.
Task_chain_cost is still spatial; cached oriented known-chain pricing remains untested.

## Persistent Fable

Session1ebb1075-3538-49d1-93d1-a00c94fa256a, lorr-warehouse-cgar-fable51,
Fable5.1max viaClaudeCodeCLI, explicit user permission persists. No live CLI process.
Turn12 terminalfailed error_max_budget_usd aftervisiblefindings; archivedqualified.
Turn13 successful2026-09-19T20:35:20.267191UTC,265.831s,no tools/denials,reported
sessiontotal61.3675335USD. New runner configurable per-turncap, default12, turn13cap24;
prepared capfixedinmetadata andusedonexecute. Cumulative reported costs should not
be presented as each turn's incremental spend. Archivedturn13review/metadata/status/
localassessment; rawprotocol stays ignored. Source-cache nowV45+goalholdingprobe;
V46 cap/test changes not yet sent. Next request would turn14/followup-13.md. No need
for another broad review now; use it for concrete hard scoring questions.

## Negative evidence and operation

Do not repeat unchanged negatives: fractionalturnextra1/2 andwarm/mixedstart loseboth
firstpair; strength2mixed,6/8lose; age-removingcostkey regresses/collapses; quota32seed0
collapse real; tablecache8/16GiB samefulltrajectories; orientationbatch128mixedsixseeds
and512fails; pools/transactionsmixedsix. Original no-flow tests are not current-flow
proofs. Details in FRACTIONAL_MIXED.md, PICKUP_FULL_FLOW_STRENGTH.md,
PICKUP_COST_KEY.md, PICKUP_FULL.md and earlier comparison reports.
Turn reversal/loaded action audits are associations, not additive causal savings.
>99.5%loaded turns come from planned actions; tiedfinalseedturns only54–83/fullrun.

All exec_command calls require sandbox_permissions=require_escalated (bwrapbroken).
No proactive subagents. Fullhorizon forquality;200steps onlyfeasibility/equivalence.
Heavy fulltrajectoryreplay onGRID; small logs/JSON/toyfixture work locally is fine.
Always complete fixed work or explicitlyfail; no anytime partial output. Reservephysical
cores perprocess; aggregate memory reservation may exceed32GB but processRSSmaynot.
Commit/push authorized; preserve deliberate PUBLICvisibility. Exclude binaries,
environments, rawtrajectories/protocol and user swap. Goal active/unmet, notblocked.
