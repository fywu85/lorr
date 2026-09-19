# Continuing warehouse work

Active unbudgeted goal: >=152981tasks over5000steps/10000robots, repeatably over
six seeds, every complete schedule+plan entry<=1s, explicit timeout failure, RSS
<32000000000B, isolated GRID physical cores. No map identity/templates/supplied
weights/future tasks/map-specific fleet caps. Local KittyKnight used38.858GB;
not an official equal-resource comparison. Goal remains active and unmet.
Prior goal turns made progress: source/tests/builds, full evidence and pushed
checkpoints. No blocker. Maintain ../../WAREHOUSE_PROGRESS.md.

## Confirmed reference and resource answer

Graded strength4/scale4, margin25, refresh512, turn-build128,8GiBcache, ordinary
HRRN1/weight1 and pickup flow ON. warehouse-reference-variants.json.
Seeds0..5=138963/140002/139677/138671/139241/139773, mean139387.8333
(+0.316545%vs preceding graded/OFF138948). All six full totals and finalwindows
improve. All12runs valid, all six controls exactly reproduce the preceding
graded reference. Empty robot-steps fall1.46–2.15%; agep90 rises7–20steps.
Sourceeb5b3827b9daf9f91aba88c78bef7be0d17cd07c, frozenV41,21hasheschecked.
Mean241.5–284.0ms, median210.1–262.9ms,p99529.8–570.6ms,max0.872354s,
CPU1.171–1.204cores/4reserved,RSS11.901GB,21.36–24.91minutes.
results/graded-pickup-flow-six-seed-v41.json and
results/graded-pickup-reference-latency-six-seed-v41.json (audit8898919complete).
The user's latency/CPU/RAM question was answered before this promotion using
preceding graded/OFF229–277ms,1.17–1.21cores,11.94GB; final answer also
distinguished unconfirmed64-field candidate285–287ms,1.47cores,11.90GB.
Do not repeat the resource answer without new reason. Goal remains unmet.

WAREHOUSE_PROGRESS now has36strictly increasing full-run records. New records:
141725at17:42:42.270841UTC (16fields,seed2),142735at17:43:24.869257UTC
(32fields,seed2,rejected policy),142988at17:43:34.384352UTC (64fields,seed2).
All exactsource4872d048dd8155927074ac2a4d27a55756266fd5, validated22hashes.
confirmed-reference-progress.json now tracks five increasing six-seed milestones;
new pickup-flow milestone completion17:57:50.073730UTC, exactsourceeb5b382.
A user editor swap .WAREHOUSE_PROGRESS.md.swp appeared; leave it out of Git and
do not delete it. Avoid overwriting concurrent user changes in the markdown.

## Completed V41 and V42 full comparisons

V41sourceeb5b3827b9daf9f91aba88c78bef7be0d17cd07c, frozen
runs/cgar-pickup-flow-build-v41-r3-20260919.
- Binary50/build128 normal pickup ON137508/137923vs137060/136841, mean+0.559%.
  Directcost/weight5 ON138242/49732vsstable137603/136560; rejectcollapse.
- Graded margin25/build128 pickup ON138963/139677 vs exact OFF138402/139303,
  mean139320vs138852.5 (+0.337%), finalwindows+244/+6, full empty robot-steps
  down1.63%/1.46%, agep90 up18/20steps. All4valid and control hashes exact.
  results/graded-pickup-flow-full-v41/comparison.json; six-seedconfirmationnow
  completeandpromotedasabove.
- Independent full travel audit8898864 completed4cases; fixture/mapchecks and
  full empty-motion counters exact. Completed chain means242.3–242.6, loaded
  elapsed275.6–278.2; local KK233.84/252.52. Whole empty travel~10M likeKK.
  Different cohorts prevent causal/additive split. GRADED_PICKUP_TRAVEL.md and
  results/graded-pickup-travel-v1; first vs last assignment confounds documented.

V42source4872d04 all22source/test hashes, frozen
runs/cgar-pickup-full-build-v42-r1-20260919. All8full runs valid, peakentry
0.886371s andRSS11.937GB. Default control exact V41graded/pickupON.
- Fields16:139697/141725,mean140711 (+0.998%); final+70/+378, agep90−22/−15,
  whole empty robot-steps−4.39%/−4.48%. Confirmremaining4seeds.
- Fields32:70043/142735;seed0last1000=5433, agep903327. Rejectpolicy.
- Fields64:141829/142988,mean142408.5 (+2.217%); final+401/+670, agep90−12/−14,
  empty robot-steps−7.60%/−7.84%. Confirmremaining4seeds.
  Mean285.1–287.1ms,CPU1.466–1.473cores,RSS11.898GB,24.98–25.17min.
  Do not conflate with confirmed six-seed resources.
- Full quotas are not monotone in quality. No categorical causal claim against
  global HRRN discovery or complete fields. results/pickup-full-full-v42/comparison.json.

## Running jobs (actual IDs)

- 8898882 /8898883: V42fields0/16/64 remaining seeds1/3/4/5,
  start17:45:58UTC,research44,12concurrent x4cores,192GiBaggregate.
  Raw runs/cgar-pickup-full-confirm-v42-20260919, pickup-full-confirm-variants.json.
  Its0fieldcontrols should exactly reproduce V41pickupONconfirmation.
- 8898901 /8898902: V43cost-key full fields0/key0,32/key0,32/key1,64/key1,
  seeds0/2,8concurrent x4cores,128GiBaggregate. Raw
  runs/cgar-pickup-cost-key-full-v43-20260919. ExistingV42provides exact0/32
  default-key controls and64original comparison. All per-process caps32GB.
- 8898924 /8898927: fixed planning workers1/2/4 with4Mwork each onV43/64fields,
  seeds0/2,sixconcurrentcases x4physicalcores,96GiBaggregate;start18:03:54UTC
  research31. Raw runs/cgar-pickup-full-workers-full-v43-20260919.
  Screen8898912completed3/3valid,max0.913763s,RSS5.232GB; one-workerwholeprefix
  exactV42/64. This is not prefixqualityevidence. Full one-worker controls must
  exactly reproduce141829/142988. PICKUP_FULL_WORKERS.md andconfig.
- 8898903complete: all6V42full-travel cases archived in results/pickup-full-travel-v1.
  Fixture, simulator map hashes and all full empty-motion counters checked; exact
  V41pickupONcontroltravel reused. No diagnostic binaries/rawtrajectories archived.
  Fields64 completedchain241.76/241.81, loadedelapsed277.39/274.46; controls
  242.55/242.60 and278.18/275.98. Empty steps9.20M/9.28M vs9.96M/10.07M.
  Fields32failedseed0 has loadedelapsed392.72 (vs278.18), but completedcohort
  shorter235.29, substantialcensoring. No causaladditivecostdecomposition.

## Tested V43 checkpoint

Committed and pushed1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9; all22source/
test hashes independently verified against the commit. Previous4872d04 isexactV42.

Fable verified an actual omission: pickup workers were parsed after affinity
validation. V43moves the check and includes the max potential pickup workers
min(thread ceiling, field quota, robot count), even with temporal planning OFF.
V42benchmarks explicitly use4threads/4cores, so their allocation remains valid.
New optional CGAR_PICKUP_FULL_COST_KEY boolean requires fullfields; default0
preservesV42. Value1retains cheapest complete-pickup+approxchain16/8candidate
tasks then applies unchanged HRRN assignment/fairadmission. Allselectedfields
complete before publication, fixed quota, no clock-selected partial results.

Build8898877 full regressions pass. Newtests restrict actual affinity tooneCPU,
reject2workers with temporalOFF/ON, accept quota1/disabled withlargerceiling;
both HRRN anddirectcost, both shortlist keys, oldesttask beyondcostshortlist,
metadataunchanged. Existing32256independent distance checks etc also pass.
Frozen runs/cgar-pickup-cost-key-build-v43-20260919; build-provenance/v43
independently reconstructs all22requested source/test hashes from4872d04+patch.
No C++ build is active. Source has not changed since that build.
Screen8898884:5/5valid,max0.879275s,RSS5.224GB. 0/32defaultkey wholeprefixes
exactV42;32costkeyserial/parallel wholeprefixesexact. No short-prefixranking.
PICKUP_COST_KEY.md; results/pickup-cost-key-screen-v43/equivalence.json.

## Persistent Fable

Session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1max viaClaudeCLI;
explicit source-sharing permission persists. Turns01–10complete, noCLIprocess
live. Turn10source4872d04,4changedexcerpts30660bytes,209.132s,35.470251USD
actual despite requested12USD flag; that flag is not a reliable hardcap.
Visiblefinal/metadata/status/qualifiedassessment archivedin fable-flow-session/turn10.
No reproducible bucket/parallel/top-k bug; actual affinitygapfixed. GlobalHRRN
shortlist behavior was intentional, not an accidental bug. Cost-key optional
experiment/tests addresspolicy andtestcoverage. Probe share does not establish
selectedassignmentimpact; arbitrary10%falsifiers/rejectcompletefieldsgenerally
not accepted. Do not archive rawprotocol/hiddenreasoning. No need anotherCLI
review untilnew hardquestion/results justifyit. Source-spec currentlyV42ranges;
refreshranges beforeanyfutureV43diffreview.

## Other established negative evidence

8/16GiBcache identicalfulltrajectories137149/139037 bothsizes;197/217fewer
rebuilds with16GiB butguided/fallback countsidentical. Keep8GiB, donotclaim
cachecapacityneverbinds. Gradedmargin0rejected56537/139796. Fortyflowpublications
audit matchesmove/penalizedcounts/fullactions; failedseed firstrefreshonly3.87%
morepairchurn/7.50%moreflips, laterlower. No simpleglobalchurnprecursor; no
hysteresischange. No fullfieldhashinproductionlog, countmatchisnotvectorhash.
Batch128sixmean136957.33vsstronger137073.33; batch512seed2regressesdespite
100%sampledcoverage. Pool/transactions mixed6seeds,notpromoted. Four4Msearch
workers onolderbinary50/build32 only+0.204%2seeds,2.6cores; thisdoesnotprove
thatstrongerpolicies cannotbenefitfrommoreparallelsearch. Fullhorizonforquality.

## Next actions

1. V43source/results checkpoint1661176is pushed and22hashes verified.
   Fulltravel6follow-up is complete and can join the next result checkpoint.
2. V41gradedpickupall12valid,six-seedpromotion/history andallstepquantilesdone.
   Save these completed reports andworker-screen/config in the next checkpoint.
3. Evaluate V42sixseed16/64 andV43cost-keyfulloutcomes, exactcontrols, everyentry
   <=1s/RSS<32GB, completewindows/ages/travel. ExtendpromisingV43to4moreseeds.
4. Confirmresourceheadroombeforemorefixedparallelsearchontheneweststrongpolicy.
   Do not drop/falselycompleteactivegoal. No current blocker.
