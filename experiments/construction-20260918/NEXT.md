# Continue CGAR across ten instances

Updated 2026-09-21T11:33 UTC. User asleep and explicitly requests continuous
iteration. Targets: +10% over max(published NMS,KK) RANDOM01–05; +5% stretch on
other five. Throughput primary, fairness secondary. Selected seed maxima count;
distinguish them from replicated improvements. Active formal Warehouse160k goal
is obsolete; do not stop while current scope remains. No internal subagents.

Ownership: NEVER edit/stage/build/collect/interrupt random05/, RANDOM05_PROGRESS.md,
PILOT_PROGRESS.md or their jobs/builds/held-out streams. Read-only immutable git
source references and algorithmic transfers into CGAR are authorized. Fable CLI
persistent session1ebb1075-3538-49d1-93d1-a00c94fa256a authorized, Fable5.1max.
Turn47 failed credits at2026-09-21T10:31:58Z; no findings. Do not retry without
availability change. Compact receipt allmaps/fable-followup/turn47.

## Execution and ownership

All shell calls require_escalated (default bwrap fails), Python3.7. Heavy builds,
regressions and raw analyses run on GRID. Shared main/index: explicit owned paths,
git commit --only; no amend/force/lock deletion. Public repo deliberate, push authorized.
GRID research44/57: scheduler-slots-per-core2;4physical=8slotsx4GiB;
8physical=16slotsx2GiB. Disjoint physical binding/no quota;32decimalGB process cap;
strict1000ms whole entry. Shared hosts authorized, do not claim exclusive.
Timeout is failure, never partial best. No production/test edits during live builds.

## Frontier

WAREHOUSE155173; SORTATION150894; CITY01 8440; CITY02 16315; GAME24447;
R01 693; R02 1215; R03 1982; R04 2059; R05 3065. All full-horizon TRICK, strict1s,
32GB. WH/Sortation8physical; others4. Only GAME meets the new margin target.
Canonical selected-full-results.json; latest all10 proof112321; BEST_HISTORY,
CGAR_PROGRESS and TARGETS. No starvation-free claim. R05 parks31capacityrobots,
GAME fleet limits. WH exclusive qualified +100Mactionreplay; others simulator and
complete action/wait accounting. Historical competitor targets, not matched reruns.

Replicated gains: R01 uniform field+2.69%/three seeds, then refreshedkeep10+1.88%
allthreepositive; R03 delay3 +3.293% allthreepositive1967/1982/1948 vs1939/1900/1870.
R04 H20 seed0 best2059 but H15 better mean. R05 chainmode1 +3.59%three-seedmean,
32starts helps, horizonmean modest; selectedseed10=3065. Exactbest profiles under
history-rollout/best-random01, rolling-window/best-random02,
window-delay/best-random03, common-futures/best-random04,
chain-potential/best-random05-horizon (all JSON trick_selected envs).

## Recent rejected directions

Wider matching: R01 all693 wholetraceidentity; R02 1194/1154/1154 vs1215;
R03 all1938 vs1982. CITY01 8440/8440/8435/8434 and CITY02
16315/16315/16260/16203 atwidth32/64/128/256. CITY01 horizon
bound8410/mean8427/p908357 vs8440. Allvalid, no promotion.
Exact whole-chain assignment: R01 693->677; R02 1215->1200; R03 1982->1779;
R04 2059->1814; R05 3065->3055. All10valid, controls repeat full trajectories.
R05 chain-seed+window: control3065; H10keep0=3008; H20keep0=2896;
H20keep10refresh=2985. Lower wait mostly became more turns, no promotion.
R04 terminal chain1908; paidchain1714; paid+strict1683 vs2059. Allvalid.
Prior delayR02, annealing, history/annealR03, startupassignment, regionalroots,
R05 crowdedwindow alllose. Do not repeat unchanged arms.

Movement audit all10 at bottlenecks/README: R01/2/3 turn24.23/30.08/40.65%;
R04/5 wait25.14/38.15%; CITY forward94.07/92.32%. Descriptive, not causal ceilings.

## Current builds and jobs

No active builds, no source edits pending. All four new builds below FULL40source/
test hashes + binary + whole regression qualified. Proof in respective build-v1.
- chain-window source d28d451ac96dd654413a714ffa809f8bdf20983c,
  binary46d5130133429a474dfe2537a1fe9d075dc5ae27d121ee4add5dc1d86d97dc92.
- scheduler-chain source e4cfcc4dace2f92139c8496dc8c21a708bc2ab12,
  binaryb0d408f171f94b1cc9990350d18bed7994d3611bdfd57bba4bc9d8b614d3a441.
- chain-paid source311d04800c369f227407b63641c1c4f0a7102186,
  binary8af0745f67e572fcbb29dc6d3797583f3a69b654425bbf4f3f88a04e36cf9b17.
- task-cap source0962671fee93801d104a0f35d723274ecc9994c7,
  binary78026b67d063a7b13e7379df0125dac8e08292627801a9078bc1ae957aed2e25.
Raw: runs/cgar-NAME-build-v1-20260921. Full source commits in build manifest.

Pending (verify ACTIVE_JOBS):
- R05 paidchain four arms 8902716/verifier8902717: control3065/paid/strict/paid+strict.
- R04 task-cap 8902736/37: control2059/cap600/650/675, seed0.
- R05 task-cap 8902738/39: control3065/cap600/700/750, seed10.
- R01 action-prices 8902740/41: seed4 2x2turn20/40 wait40/20, current693.
- R03 action-prices 8902742/43: seed2 same2x2, current1982.
Cap uses qualified096; action-prices qualified311. Fullhorizons,4physical/case,
strict1s32GB, node-local log spool. Source free for next independent change.

Taskcap CGAR_TRICK_RANDOM_TASK_CAP is explicit RANDOM01–05only, default0 unchanged.
Caps new admissions only, preserves held/started including external over-cap
assignments, goal-less robots remain movable; existing R05 parking separate.
30000productionactions/464completions regression, off/zero/fullfleet identity,
idle blocker displaced, held/started protected. Analyzer reconstructs every active
assignment and completion, checks cap/no dropped held tasks, peak/admitted counters.

PILOT immutable source review copied d51bdbf2c4e6830f8cabacb52ac9cfa8234aa663
engine.cpp/hpp to /tmp/cgar-pilot-readonly-d51; provenance
pilot-transfer/source-review-20260921-1112.json. No independent run data touched.
True two-phase pipeline remains NOT transferred: promises movement one action
ahead, idle robots pre-rotate while future move is chosen, next-frame starts from
exact predicted occupancy. Current CGAR windows/common futures are forecasts.

## Helpers and checkpointing

/tmp/cgar_qualify_build.py --raw runs/... --proof experiments/allmaps-20260920/.../build-v1
verifies40source/test hashes against manifestcommit, binary, full regressions,4cores/noquota.
/tmp/cgar_submit_profile.py --instance INSTANCE --seeds ... --profile JSON --suffix SUFFIX
--build runs/... --source FULLHASH --local-log-spool [--inputs MANIFEST] [--cores8].
Alwaysoverride old defaultbuild339718b3. CITY manifests city-game/inputs-CITY-01.json,etc.
Sequential ACTIVE_JOBS writers. /tmp/cgar_refresh_frontier.py --report SUFFIX repeatable
--note ... updates canonical/target/history/proofs and removes completed jobs; inspect
all completed jobs first. Does NOT update exact best profiles or narrative.
/tmp/cgar_commit_checkpoint.py ownsCGAR_PROGRESS,NEXT,Fablehelper,allmaps/** only;
resultdirs requireverification, gitignoredrawlogs excluded. Explicitadd+commit--only.
NFSnegativecache: listparent/retry; no solverrerun forvisibility. Last push04b28c82;
source311/096 and subsequent reports still require checkpoint+push.
Authorized push: GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 git -c
'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main

tools/benchmark_cgar.py is frozen runnerhash
f5049ddd0f2ac55f01f2493076f9538bca1acaa817beda5aff8a6a0c890234bc.
Do not edit while queued matrices await dispatch. --local-log-spool archives full
logs after exit including failures; strict time limits include actual logging.
EarlierR02timeout2632ms retained; cause not proven, localspoolrepeatvalid1164 loses.
