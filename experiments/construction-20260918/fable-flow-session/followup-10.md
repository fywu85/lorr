Continue the SAME persistent lorr-warehouse-cgar-fable51 session, turn11. Fable5.1
max effort, read-only. Source below is incremental context. No tools or test claims.

We need a fresh code-grounded diagnosis of residual loaded-motion inefficiency.
Target unchanged: generic5000-step/10000-robot warehouse >=152981 on six seeds,
complete schedule+plan<=1s, fixed work or explicit failure, RSS<32decimalGB,
isolated physical cores, no map IDs/lane templates/task forecasts/fleet caps.

Updates and corrections since turn10:
- Your actual affinity bug is fixed in V43: check after pickup parsing includes
  min(fieldquota,threadceiling,robotcount), also temporalOFF. Real restricted-CPU
  regression tests pass, as do all previous regressions. All22source/test hashes
  match1661176; v42baseline exact4872d04. Current source diff includes the fix and
  optionalcost shortlist; default0 preserves v42, verified by full trajectories.
- V41graded normalpickupON confirmed six seeds mean139387.8333, +0.317%.
- V42original age-aware whole-field discovery is now confirmed on ALL SIX seeds:
  quota16:139697/141925/141725/140734/140928/141878, mean141147.8333 (+1.263%).
  quota64:141829/143325/142988/141802/141988/142917, mean142474.8333 (+2.215%).
  Both improve every full total/final1000/agep90.64 beats16 every full total.
 64fields is current reference. Empty robot-steps fall7.43–7.84%, agep90 falls5–14.
  All18runs valid<=1s/RSS<32GB; all six controls exactly match V41. Thus a blanket
  rejection of complete fields or original age-aware discovery is not supported.
- Quota32 still collapses seed0:70043 vsseed2142735, fullvalid; nonmonotone response.
- Your separate cost-based discovery suggestion has now been tested in V43;
  old/fair-task admission remains. key32=135244/137131, both BELOW healthy disabled
  138963/139677 (mean−2.248%), with empty travel+17.46–17.95%, agep90+2755–2785.
  key64=135773/79601 vsoriginal64141829/142988; seed2final5540. Reject both keys.
  All four key0 disabled/32 controls exactly reproduce. A mean rescue against a
  collapsed32control is not enough. Key0 is deliberate age-aware policy, not bug.
- Parallel full4M motion workers on current64policy just finished:
  1worker141829/142988,2workers141432/142935,4workers143340/143409.
  Four gains+0.678%, finalwindows+60/+83, agep90unchanged. All6valid and controls
  exact. Four-worker remaining4seeds running. CPU2.62–2.68cores of4,mean316–329ms,
  max913ms,RSS11.885GB. Default confirmed1worker sixseedmean272–317ms,p99557–602ms,
  max874ms,CPU1.425–1.494,RSS11.934GB. No clock-selected partial plans.

New independent loaded action replay: each of50Mrobotsteps assigned one phase,
allCGARproductionphase/actioncounters match, everycompletedtaskactions=finish−pickup,
bothhandcountedfixturespass, alloldreportfieldspreservedexactly. PerCOMPLETEDtask:
case                 tasks   shortestchain extraForward turns  waits  loadedElapsed
64fields seed0        141829  241.758       9.125        11.191 15.311 277.385
64fields seed2        142988  241.809       8.195        10.355 14.100 274.459
32fields FAILED s0    70043   235.286       35.573       44.036 77.821 392.715
localKittyKnight      152981  233.836       4.594        5.678  8.411  252.518
Components add WITHIN each cohort. These are different completed cohorts; unfinished
work is censored, necessary turns included. Not additive causal savings or a
matchedcohortclaim. KKused38.858GB vsour32cap. Motion inefficiency is distributed.

In progress separately: existing strengths2/4/6/8 atscale4,margin25,refresh512,
turnbuild128,64fields,key0,1worker4M. Deadline screen then full0/2 andsixseedifuseful.
This is a policyinteraction test on current reference, not repeat of old binaryflow.
Previous negatives still hold: largeguide-routevariants collapse, cache8->16GiB
exacttrajectories, batch512fails despite100%sampledcoverage, oldstrictwaittiechange
mixed two seeds. Globalflowchurndidnotpredictcollapse. Avoid repeating these blindly.

Please prioritize at most TWO concrete, implementable motion changes with a small
synthetic counterexample or line/function basis and a bounded validation plan.
Look at TemporalGeometry::cost, action pool/ties, metric/fallback consistency,
rollinghorizon restart/weights and warm reuse, and how complete workers are compared.
Are excess turns/waits likely an objective issue or coordination/search issue?
Separate demonstrated bugs, designed approximations and untested hypotheses.
A potential next option is weighted future-turn cost on scaledflow: currently
initialize rejects scale!=1 withphysicalturn!=1, though oracle init multiplies
turncost*scale and TemporalGeometry::cost ALREADY compensates actual prefix turns
and terminalwait rotations. Do NOT claim the old manufactured-turn-progress bug
without a new counterexample. If this combination is plausible, specify every
metric/bounds/fallback/scheduler consistency check needed, and a minimal test.
Use the cached source context; no need to restate the whole architecture. No arbitrary
numerical falsifier thresholds or causal claims from coverage/meanthroughput alone.
