# Continuing warehouse work

Active unbudgeted goal: >=152981 tasks over5000 steps/10000 robots, repeatably on
six seeds, every complete schedule+plan entry<=1s, explicit timeout failure,
RSS<32000000000B, isolated GRID physical cores. No map identities/templates,
supplied weights, future tasks or map-specific fleet caps. Local KittyKnight used
38.858GB; this is not an official equal-resource comparison. Goal active and unmet.
Prior turns made concrete progress; no blocker. Maintain ../../WAREHOUSE_PROGRESS.md.

## Current confirmed reference

Graded strength4/scale4, margin25, refresh512, turn-build128,8GiBcache, HRRN1 and
pickup weight1, pickup flow ON,64 complete pickup fields/four field threads,
original age-aware key0, one planning worker4M with four preparation threads.
warehouse-reference-variants.json is explicit. Exact V42 source4872d048dd8155927074ac2a4d27a55756266fd5;
all22 source/test hashes verified. Frozen runs/cgar-pickup-full-build-v42-r1-20260919.

Seeds0..5:141829/143325/142988/141802/141988/142917, mean142474.8333, range
141802–143325, +2.214684% over preceding normal-pickup mean139387.8333.
All six totals/final1000 improve; empty robot-steps down7.43–7.84%, agep90 down5–14.
Quota16 also improves all six:139697/141925/141725/140734/140928/141878,
mean141147.8333 (+1.262664%); quota64 beats16 in every full total.
All18 paired runs valid; all six0-field full trajectories exactly match V41.
results/pickup-full-six-seed-v42.json; PICKUP_FULL.md.

Resources for64 across six seeds: mean272.4–317.4ms, median241.2–298.6ms,
p99557.3–602.0ms, max874.138ms, CPU1.425–1.494cores/4 reserved,
peakRSS11.934GB, full wall23.93–27.70minutes. All30000 entries<=1s.
CPU is whole-process user+system/wall (including startup/sim/logging), not an
instantaneous sample. results/full64-reference-latency-six-seed-v42.json,
audit8898959 complete. The user's resource question was answered with these new
confirmed figures in commentary. Do not keep repeating without a reason.

Progress now39 increasing full-run records, last143409 onfour-worker candidate,
not promoted. Previous confirmed high143325 completed18:13:40.691469UTC, exact
4872d04. Latest143340/143409 exact1661176 timestamps from full worker metadata.
confirmed-reference-progress.json has seven increasing six-seed milestones.
User editor swap .WAREHOUSE_PROGRESS.md.swp exists; exclude fromGit and do not
delete. Preserve concurrent user markdown edits with read/compare-before-write.

## Completed latest experiments

V43 exactsource1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9, all22 source/test
hashes verified. Frozen runs/cgar-pickup-cost-key-build-v43-20260919,
binaryf6d2a8ef5963e27a2408b503efb283ceb2d686155688d886fa81301a9e29313f.
Build8898877/all regressions complete; no C++ build active and no production
source changed since1661176. V43 fixes pickup CPU-affinity validation and adds
optionalcost discovery key. The default preserves V42 by full trajectory evidence.

- Cost-key full8898901/analysis8898902: all8 valid, all4default0/32 controls
  exactly reproduce V42. Fields32/key1=135244/137131 vs healthy0-field
  138963/139677: mean−2.248%, empty travel+17.46–17.95%, agep90+2755–2785.
  It rescues collapsed32/seed0=70043 but still loses to healthy reference.
  Fields64/key1=135773/79601 vs original64141829/142988; seed2final5540.
  Reject both cost-key profiles. PICKUP_COST_KEY.md and results/pickup-cost-key-full-v43/.
- Worker full8898924/analysis8898927:1worker141829/142988,2workers141432/142935,
  4workers143340/143409. Bothone-worker fullhashes exact V42/64, all6valid,
  disjoint fourphysicalcores each, noquota, all22source hashes checked.
  Fourworkers +0.67833% mean, finalwindows+60/+83, agep90unchanged,
  emptyrobotsteps+0.647%/−0.758%. Mean316.0–329.1ms,max912.607ms,
  CPU2.619–2.679cores,RSS11.885GB,27.39–28.47min. Two workers losesboth totals.
  Fourworkers is a candidate awaiting six seeds. PICKUP_FULL_WORKERS.md,
  results/pickup-full-workers-full-v43/comparison.json.
- New loaded action audit8898958 completed4 trajectories. decompose.cpp/.py
  now partition every50Mrobotsteps, assert conservation and agree with all CGAR
  phase/action counters. Completedtask loadedactions=finish−pickup; all old
  reportfields exactly preserved. Both handcountedfixtures pass. No planner change.
  Percompletedtask:64s0 chain241.758+extraF9.125+turns11.191+wait15.311=277.385;
  64s2 241.809+8.195+10.355+14.100=274.459; failed32s0
  235.286+35.573+44.036+77.821=392.715; KK233.836+4.594+5.678+8.411=252.518.
  Different completedcohorts, unfinishedworkcensored, necessaryturnsincluded;
  not causal/additive savings. LOADED_MOTION.md, results/loaded-motion-audit-v1/.

## Live jobs and next comparisons

-8898962/analysis8898963: one/four planning workers,4M EACH, current64policy,
 seeds1/3/4/5. Starts18:35:21UTC research57,8cases x4physicalcores=32cores,
 128GiBaggregate, eachprocess<32decimalGB. Raw
 runs/cgar-pickup-full-workers-confirm-v43-20260919.
 pickup-full-workers-confirm-variants.json. Fullcontrols must exactly match V42/64
 remainingseeds. Combine withfirstpair and inspect all totals/tails/age beforepromotion.
-8898971/analysis8898972: flow strengths2/4/6/8 atscale4,margin25,refresh512,
 turnbuild128,64fields,key0,one4M planningworker. Fullseeds0/2,8cases x4cores,
 128GiBaggregate,32decimalGB/process. Raw
 runs/cgar-pickup-full-flow-strength-full-v43-20260919.
 Screen8898964 complete4/4valid,max888.135ms,RSS5.206GB; controlfullprefix
 exactV42/64. Fullstrength4 must match141829/142988. No shortprefixqualityclaim.
 PICKUP_FULL_FLOW_STRENGTH.md andpickup-full-flow-strength-variants.json.

## Persistent Fable

Session1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1max viaClaudeCodeCLI;
explicit source-sharing permission persists. Turns01–10complete. Turn11running,
raw runs/fable-cgar-session/turn11, CLI started18:39:05UTC; sourceexactV43,
preparedbaseHEADc19a446 (report-onlychanges), threechangedexcerpts29819bytes.
Prompt fable-flow-session/followup-10.md asksatmosttwo concrete motionchanges,
currentloadedactionaudit, sixseed64gain, failedcostkey suggestion andnew4workers.
Includes caution thatgeometry alreadycompensatespastweightedturns/terminalwaits;
scaledflowcurrentlyrejectsphysicalturn!=1. Source-spec refreshed to V43functionranges.
No rawprotocol/hiddenreasoning inGit; archiveonlyvisiblefinal,metadata,status,
andqualifiedassessment. Turn10actualcost35.470251 despite12USDflag; nothardcap.

## Established negative evidence and guardrails

All new benchmark runs use complete prescribed work, no anytime partial outputs.
A timeout is a full-entry/run failure. Match full hashes before interpretingcrossbuild
controls. Full horizon forquality;200step screens onlyfeasibility/equivalence.
Heavy fulltrajectoryreplay onGRID. Reserve physicalcoresperprocess; aggregateRAM
mayexceed32GB but everyplannerRSSmustremainbelow32decimalGB.

Quota32seed0 collapse70043 is real and reproduced; quotaresponse nonmonotone.
Global age-aware discovery intentional; costkeytest does notsupersede confirmed64.
8/16GiB cache exactfulltrajectories137149/139037, saves197/217rebuilds without
changingguided/fallbackcounts. Gradedmargin0=56537/139796 rejected. Globalchurn
40publicationsnotpredictive ofcollapse; nofullfieldhashinproductionlog,countsnotvectorhash.
Batch128sixmean136957.33vs137073.33; batch512failsdespite100%sampledcoverage.
Pool/transactions mixed sixseeds, notpromoted. Earlier4workers+0.204% onbinary50/build32
is distinct fromcurrent64policy. Oldstrictwaitturns mixed two seeds. Weightedturn2
withoutflow lost, turn4laterdeadlinefailed; anyscaledflowinteraction isnewhypothesis.
Larger distancecoefficient256/1024 lost onearlyno-flowseed0; no knownscoringbug.
Guidereconnection/weightedguidevariants previouslycollapsed; do notrepeatsamepolicy.

Next: save completed reports/configs/history; assessFableturn11; collectlivefullruns,
validate and confirm promising policies. Currentcheckpointc19a446waspushed;
plannerexact1661176isalreadypublic. Goal remains active/unmet, not blocked.
