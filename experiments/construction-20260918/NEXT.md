# Continue CGAR across all ten competition instances

Updated 2026-09-21T05:54:40.459053+00:00. User scope supersedes the obsolete formal Warehouse160k goal:
all ten competition instances, target NMS, throughput primary, fairness secondary.
Pursue general CGAR improvements and explicit instance tricks together. Individual
seeds may set records; mark seed selection separately from algorithmic improvement.
Every trick requires --trick INSTANCE and labelled commits/logs.

Ownership: never edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
PILOT_PROGRESS.md, that agent's jobs or held-out streams50001..50008. Our CGAR runs
use runs/cgar-*. Frozen reference study233f5bf is read-only. No internal subagents.
Fable CLI session1ebb1075-3538-49d1-93d1-a00c94fa256a authorized; turn46 lacked
credits, so do not retry without an availability change.

All shell calls need require_escalated. Python3.7; quoted Python edits; no apply_patch.
Heavy builds/tests/raw analysis on GRID. Shared main/index: explicit owned paths,
git commit --only. Never remove locks, amend, force, or stage other work.
Commit/push authorized; public repository stays public. Last pushed checkpoint1633edf;
new completed reports are ready for the next owned checkpoint.

## Selected full strict1s records

WAREHOUSE155173; SORTATION150894; CITY01=8427; CITY02=16315; GAME23977;
R01=635; R02=1197; R03=1902; R04=1999; R05=2915. All below32decimalGB.
Warehouse/Sortation8cores; others4. All current selected rows are TRICK. Warehouse
has exclusive-host qualification and independent100M-action replay. Others have
simulator validation plus complete movement/waiting reconciliation. Published NMS
scores are historical targets, not matched local comparisons.

Canonical selected-full-results.json contains every configuration/source/binary/
seed/timestamp/evidence. CGAR_PROGRESS.md concise overview; BEST_HISTORY.md linked
history; selected-results-checks-20260921-0550.json reconciles all ten rows with
original verification artifacts (Warehouse uses exact_production_source_commit).
Do not rewrite old failed reports or unaccepted timeout scores.

## New source, frozen build and tested behavior

Production source90df94f0d1ca1e380acd1b4a9cf1a7cc851c4638, build complete:
runs/cgar-regional-peak-build-v2-20260921, binary
92595db9fcfea8cbe89e5e65950578c8837128a97a5f0c63d7912c540120f0a5.
All37source/test hashes matchcommit; full regressionpassed. Proof regional-search/
build-v2/checks.json. Earlier96b1037 implements mechanism;90df fixes diagnostic
lost_improvements to compare returned score. Both default behavior and full default
control trajectories preserved. No production or test edits currently pending.

CGAR_TEMPORAL_REGION_KEEP_PEAK=1 is general and defaultOFF. Save complete improving
reservations, continue identical prescribed search work, then optionally restore
the highest score. Timeout always throws even after visiting a peak. Equal scores
keep legacy rule. Regression:64analyticseeds,18restoredknownpeaks,300unchanged
attempts/candidatework,128disabledRNGcontinuations, timeout-after-peak, strictparser,
4800serial/parallelproductionactions and reconstructedregionalmerges.

## Results since1633edf

GAME3250 active fleet:23977/23917/23888 seeds0/2/4, all above published23274.
Versus2750, +10.07%mean. All6500robots remain movable; only3250getnewtasks.
This deliberately changes fairness; no starvation-free claim. Independentmask/
assignmentauditpasses.3125/3375/3625=23556/23938/19642;3250 controlwholetrace repeats.

SORTATION matching64=150894 vs150333 control, max993.90ms, little timingmargin.
Directpickup4=150081. Directpickup8 FAILEDstep0 1002.725ms regionalrepair;
no accepted partialscore. Runtimefollowup global1M/peakvariants running.

R04 turn2 controls:temp1000 OFF/ON retention=1947/1589; temp5000=1924/1999.
Seed2:1904/1869/1949/1938; seed4:1796/1721/1987/1876 (same order).
Means1882.33/1726.33/1953.33/1937.67. Retention gives bestindividual but loses
mean vs hot control. Do not promote universaldefault. Coldretention has more
loadedturns/waits/backtracking and expiredcommitments962 vs392; hotretention206.
That is an association, not proof safeguards cause or cure congestion.
Extra globalworkers8/16equal/32equal/32extra=1999/1873/1985/1982, allvalid, keepexactcontrol.
R03 retention1902->1898, leaveoff. Exact remaining-chain orientation DP and coherent
multi-step motion remain structural candidates; prior partial-next-errand and
one-turn promise were not those mechanisms. Read random-transfer/REVIEW.md.

R05 rounds6/8/10/12=2741/2877/2861/2915; bestmax895.64ms, no peakretention.
8rounds+peak=2898,max672.45ms. Controlwholetraceidentical. Twelve-round result is
single seed; earlier4vs8 seed0/2/4=2806/2809/2718 ->2877/2865/2836 allpositive.
Eight-core followup: parts4/round12/8M control; parts8/round12/4M; parts8/round16/4M;
last+peak. Same32Mnominalcandidatecapperround for parts4/8, actualworknotguaranteed
identical due completeconstruction/attemptovershoot. Eight boundcores allarms.

R02 cadence1/budget4:1188/1189/1197 vs1160/1113/1135 control, allpositive.
Horizon atseed0:control1188/bound1176/mean1193/p901169. Keepseed4no-horizon1197.
CITY02 budget2:16315/16199/16117 vs16169/16171/15675, allpositive.
R01 no-horizon control621/635/631; mean-margin horizon626/633/633. Best635seed2
therefore has no known horizon; field/scheduler tricks remain. Declaredseedscan
6,8,...,36 unchangedprofile, full600steps, fourparallelcases(16physicalcores).
Keep all results; user accepts maxima but no general average-gain inference.

Geometric known-horizon source51aab58 supports pocketmaps with an independently
verified Manhattan+service lowerbound; held/started tasks preserved. EarlierR05
control FAILEDstep0, identicalretry2806 exactpreviouswholetrace. Failurecause
unresolved;17matrixcross-host/coreaudit found no sharedboundphysicalgroups in22
overlapping-timehostpairs, not proof about other workloads/cache/OS. Keep failures.

## Pending matrices to collect

Raw runs/cgar-SUFFIX-20260921; reports experiments/allmaps-20260920/results/SUFFIX.
All below use current90df94f frozenbuild. Controls must match prior wholetrajectory.

| Suffix | Matrix | Verifier |
|---|---|---|
| sortation-dispatch-runtime-full-v1 | 8901673 | 8901674 |
| random05-priority-diversity-full-v1 | 8901677 | 8901678 |
| game-fleet-horizon-full-v1 | 8901687 | 8901688 |
| game-fleet-dispatch-full-v1 | 8901689 | 8901690 |
| city02-geometric-horizon-full-v1 | 8901709 | 8901710 |
| random05-parallel8-full-v1 | 8901713 | 8901714 |
| random05-regional-peak-seeds-full-v1 | 8901717 | 8901718 |
| random01-seed-scan-full-v1 | 8901722 | 8901723 |

SORTATION full5000steps,8cores; GAME full5000,4cores; CITY02 full3000,4cores;
R05full2000,4cores except parallel8 uses8; R01full600,4cores. Jobs may finish before
next turn; check reports and immutable analysis logs before submitting duplicates.
No exec/wait/PTY sessions pending at this checkpoint.

## Resource and evidence rules

research44/57 queue logical slots: --scheduler-slots-per-core2 for newmatrices.
Fourcorecase8slots x4GiB/slot; eightcorecase16slots x2GiB/slot. Exact physicalbinding
and noquota guards stay. Verifiers2slots/bind1physical/6GiBslot. Sharedhostsallowed.
Always fail timeout; no quality fallback. Inspect completed flags, rawfailures and
source/binary hashes, then compare controls and promote only fullvalid improvements.

GAME earlierfrozenbuild runs/cgar-game-fleet-build-v1-20260921 requires
build-attested.json, sourcef644accc7e00ceda7a455b7a45ed6eee4f970f8d, binary
0a83dc762511ea924c6433b634007bfd3e5227cb5a2ef9ea491f3794c6a0a30f. Originalrequest
raced anotheragentindexlock;36frozenhashes match f644acc; preserveattestation.
Tuning/finite-retarget build runs/cgar-rematch-budget-build-v1-20260921,
sourceaad422ff1160e1daae02eccf575209da1e4edd3a, binary
dfec054298b7eff48c1634e70df377ae95542f08f5ddd07ec2990afcb4478997.

Checkpoint ownedpaths only: CGAR_PROGRESS.md, allmaps/, thisNEXT, anychangedowned
analysis/buildhelper. Stage resultdirs only when verification.json+summary.md
exist and declaredfull_horizonstrue. Keep completedfailedcases withtasksNone.
Use git diff --check, git add explicitpaths, git commit --only explicitpaths.
Push with GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 and credentialhelper
!/user/fw2449/.local/bin/gh auth git-credential. Neverforce/amend.
