# Continue CGAR across all ten competition instances

Updated 2026-09-21T06:59:14.874103+00:00. Latest user instruction explicitly supersedes the obsolete
formal Warehouse160k goal. Target at least10% above max(published NMS,KK) on all
five RANDOM instances; aim for5% above that maximum on the other five, explicitly
a stretch goal for Warehouse. Throughput primary, fairness secondary. Individual
seeds may set records. Continue general CGAR improvements AND instance tricks.
All tricks require --trick INSTANCE and labelled commits/logs. User is asleep and
asks continuous iteration; do not stop just because a promising transfer loses.

## Ownership and execution rules

Never edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
PILOT_PROGRESS.md, its jobs or held-out task streams. Read-only PILOT reference is
explicitly authorized; frozen source facts are now in allmaps/pilot-transfer/.
Our CGAR runs use runs/cgar-*. No internal subagents. Fable CLI persistent session
1ebb1075-3538-49d1-93d1-a00c94fa256a is authorized; turn46 lacked credits. Do not
retry without an availability change. No new availability message arrived.

All shell tools need require_escalated; default bwrap fails. Python3.7, quoted
Python edits; apply_patch fails. Heavy build/test/raw analysis belongs on GRID.
Research44/57 have64physical/128logical cores. Use scheduler-slots-per-core2:
4physicalcase=8slots x4GiB,8physicalcase=16slots x2GiB;32decimalGB process cap.
Full1s entry enforcement, fixed complete work, timeout fails with no partial score.
Exact physical affinity and no-quota guards remain. Shared hosts are allowed.

Shared main/index: explicitly owned paths and git commit --only; never remove
locks, amend, force, or stage other work. Commit/push authorized; repo stays public.
Push uses GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 and
-c 'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential'.

## Current selected records and new targets

| Instance | Best | New target |
|---|---:|---:|
| WAREHOUSE |155173|162535|
| SORTATION |150894|160350|
| CITY-01 |8427|8868|
| CITY-02 |16315|17847|
| GAME |24447|24438|
| RANDOM-01 |647|757|
| RANDOM-02 |1197|1386|
| RANDOM-03 |1902|2568|
| RANDOM-04 |1999|2802|
| RANDOM-05 |3027|3355|

Only GAME meets the new margin target. All current records use explicit tricks,
strict1s and32decimalGB. Warehouse/Sortation8cores; others4. Warehouse has exclusive
host and independent100M-action replay qualification. Other rows use simulator
validation plus complete movement/waiting accounting. Published scores are
historical, not matched local comparisons. All20NMS/KKvalues were verified against
the frozen official response. KK sets higher baselines on CITY01/02 and RANDOM01/02.

Canonical allmaps/selected-full-results.json holds source, binary, seed, UTC,
configuration and proof. BEST_HISTORY.md records every best. CGAR_PROGRESS.md now
compares to max(NMS,KK) and newtargets. Active targets are in active-targets.json.
Latest checks: selected-results-checks-20260921-0656.json.

## Implemented and qualified this turn

Source7783736fec8dd929316fdaa2bd6d0b4fdd92c94c adds complete oriented remaining-chain
potential, generic defaultOFF. Build runs/cgar-chain-potential-build-v1-20260921;
binary37c9ae5d6e55c6698037872626ce2991b6629e3f23904764cf1eca60d038e8fa.
All38source/test files matchcommit, full regressionpasses. Proof allmaps/
chain-potential/build-v1/checks.json. No production or regression edits pending.

New chain_potential.hpp precomputes all4arrivalheadings for every cell pair in the
static guidance graph,64*nfree^2bytes (42.93MB for819). Exact static weighted
travel+orderedservice costs, not congested travel times. Core/goal-pocket entry
and escape from another pocket are represented. DP retains all remaining revealed
stops. Starting on a goal requires a real action; at most one errand per action.
No partial preprocessing lookup; configured memory cap rejects oversize tables.

CGAR_TEMPORAL_CHAIN_MODE:1scoreonly,2priorityonly,3both;0default.
CGAR_TEMPORAL_CHAIN_MB default512,max8192; THREADS default1,max32.
Modes2/3require TEMPORAL_ORDER2. Dynamic learned metrics, guide routes, old partial
next-errand and native neutral-tail are incompatible. Static flagged fields allowed.
Scoring uses actual end heading and advances services along5postactioncells.
Primary/recovery/parked paths remain fixed. Missing task record uses visible goal;
unreachable chain is counted and falls back to old score. Mode0 consumes no RNG.
Config and cumulative chain counters are verified by analyze_matrix.py.

Tests: independent heap shortest paths on action/service product graph agree at
162900states;24228temporalpaths;171heading-coupled tails;26280unreachable states.
Memory cap, parallelpreprocess, timeout/no partial state, repeat stops, suffix stage,
strict parser. Production8000serial/parallelactions,1180service events, modes0..3,
disabled trajectory identity, replaced tasks, protected primary/pocket recovery.
All tested full default control trajectories reproduce too.

Other frozen production remains90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638:
runs/cgar-regional-peak-build-v2-20260921,
binary92595db9fcfea8cbe89e5e65950578c8837128a97a5f0c63d7912c540120f0a5.
37files/fullregressionqualified. Existing tests/profiles remain frozen.

## New full-run evidence

Chain scoring/order:
R01 seed8:control638,score636,order2control633,both644. Replication seed0/2/4:
621/635/631 ->645/647/595. Bothmeans629; report variance and selected647, not a mean gain.
R02 seed4:1197control,score1061,order2control1109,both1025:alllose.
R03 seed0:1902control,score1491,exactorder1677,both1203. Loaded waits51252->94815
under score-only; expiredcommitments100->580. Association, not proven cause.
R04 seed0:1999control,score1971,exactorder1951,both1775. Keepoldprofile.
R05 seed0:2956control,score3027,exactorder2850,both2958. Score-onlywins+2.40%,
originalpriority retained. Replication pending. Do not enablechainglobally.
Full factor diagnosis and frozen settings in allmaps/chain-potential/.

GAME pickup12:24326/24360/24447 versus23977/23917/23888 on seeds0/2/4,allpositive.
Same3250active receivers;6500robots remain movable, half receive no new tasks.
Deliberate fairness tradeoff, no starvation-free claim. Controls wholetraces repeat.
GAMEhorizon23977control ->23948bound/23867mean/23819p90:alllose.

R05 constructiondiversity:8startsx500kcontrol2877;16x250k2901;32x125k2956;
32x500k2921. Caps firstthree nominal4Mtotal, actualconstruction/overshootdiffer.
8vs32repseed0/2/4:2877/2865/2836 ->2956/2900/2889,allpositive.
Peakretention OFF/ON at8rounds:2877/2865/2836 ->2898/2781/2827; mean-0.84%.
Eightcores:4parts/12rounds2915wholetrace identicalto4cores;8parts/12rounds2874;
8parts/16rounds2946(max799.65ms);+peak2918. Composition with32starts pending.

SORTATION lowerglobalwork:control150894(max989.60ms),1M150760(max892.44),
1M+peakT1000same150760,1M+peakT5000=150780(max900.70). Runtime alternative recorded,
not promoted over150894. Parallel start comparison pending.

CITY02horizon16315 ->16287/16241/16212:alllose. Objective/priority fullmatrix:
16315control,remaining-potential15233,chainpriority16013,both14241:alllose.
R03morefinite rematching:1902control, budget2=1776,budget4=1804,budget4cadence1=1824.
R03diverse starts:1x8M1902;2x4M1859;4x2M1741;4x8M1724:alllose.
R02turnprices:20control1197,12=1159,16=1181,24=1028:alllose.
All original failures and negatives remain; no rejected partial throughput.

## Pending owned jobs

See allmaps/ACTIVE_JOBS.json. Raw runs/cgar-SUFFIX-20260921;
reports allmaps/results/SUFFIX. Check directfiles/parentlistings if NFS reports
missing immediately after analyzer completion. Do not resubmit the solver.

| Suffix | Matrix | Verifier | Cores/case | Source |
|---|---|---|---|---|
| sortation-parallel-starts-full-v1 |8901857|8901868|8|90df94f|
| random05-composed8-full-v1 |8901886|8901888|8|90df94f|
| random05-chain-potential-seeds-full-v1 |8901946|8901947|4|7783736|

Sortation uses factor-analysis-r2; others factor-analysis. All ownmatrixprofiles
are predeclared/frozen; controls must reproduce prior full trajectories. R05
composition is still based on2956(nochain),32starts,8global/prep/pickupthreads;
control4parts/8rounds,8parts/12,8parts/16,4parts/12. Fixed32Mnominalregional cap/round.
R05chainrepseeds2/4 control32starts/8rounds with mode0 versusmode1.

## Analyzer correction and prevention

The initial R01/02/03/04chain/SORTparallel/CITYobjective analyzers accidentally used
an R05 input manifest. Instance-set validation rejected them before interpreting
results. Actual solver inputs/runs were correct. Original requests/logs preserved;
only owned held verifiers canceled, corrected factor-analysis-r2 attempts use
exact original per-instance frozen manifests in chain-potential/inputs/.
GAMErep verifier hold was mistyped, immediately corrected with qalter before
execution; correction copied into its completedreport.

analyze_matrix.py now checks declaration instances, profiles, seeds, trick and
cores BEFORE creating attempt directories/submitting jobs. Two manual invalid-input
checks fail early and create no files/jobs. This analyzer-only edit is pending
checkpoint; all previously executed analysis copies remain immutable.

## Next substantial work: PILOT transfer

Read allmaps/pilot-transfer/NEXT_TRANSFER.md. Explicit userauthorization.
Read-only frozen de0b763:window.cpp/engine.hpp and ac5cbc9:engine.cpp/engine.hpp
were reviewed and fingerprinted. Latest PILOT R01/02use20stepwindow,no guidance;
keep6/10,512/2048LNSiterations. R03uses20stepwindow,keep14,8192iterations,annealing4,
tunedflow. R04 uses coherentpipeline depth16,mutation0.1,8064futures/10continuations;
R05uses16320futures/18continuations. Those selectedrecords use16physical/32SMT.
Do not conflate this with CGAR's larger5stepconstructionportfolio.

Implement a general CGAR-seeded rolling-window repair layer for sparse/medium
RANDOM first: complete validated seed paths, exact task-stage action costs, bounded
A*grouprepair, fixed completed attempts, atomicrollback for unsuccessful repair,
retainedhistory checked against executedstates/tasks. Preserve protected first
moves/supporting chains, recovery/pocket/intent restrictions. Reconcile actual
wait-seed rotations, next_ locations, commitment/movementcounters and history.
No globaldefaultchange before full tests and fullbenchmarks. This is a design,
NOT yet implemented; no half-written source remains.

A readonly next-move deferral audit could isolate whether5stepforecasts repeatedly
postpone advertised forward actions under unchanged tasks/protections. Current
waiting association alone is not proof. Crowdedpipeline/commonfuturecontinuations
remain a separate majortransfer; don't relabel the old after-turn promise as that.

After any promotion refresh selected-full-results, BEST_HISTORY, CGAR_PROGRESS,
active-targets and the check snapshot. Commit only completed own reports/profiles,
not in-progress analyzer fragments. Push checkpoint, continue active unlimitedgoal.
