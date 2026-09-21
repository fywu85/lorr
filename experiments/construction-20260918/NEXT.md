# Continue CGAR across ten instances

Updated 2026-09-21T13:02:09.256814+00:00. The user is asleep and explicitly requests continued iteration.

Targets: +10% over max(published NMS, KK) on RANDOM-01 through RANDOM-05; stretch +5% on the other five. Throughput is primary, fairness secondary. Selected-seed maxima count, but distinguish those from replicated gains. The old formal Warehouse160k goal text is obsolete; continue the latest all-map scope. No internal subagents.

## Ownership and authorization

Never edit, stage, build, collect or interrupt random05/, RANDOM05_PROGRESS.md, PILOT_PROGRESS.md, or the independent agent's jobs, builds and held-out streams. Read-only immutable git source review and transfers into CGAR are authorized. Public repository is deliberate. Owned commits and normal pushes are authorized. Shared main/index: explicit paths and git commit --only; no amend, force, lock deletion or foreign staging.

Fable CLI session 1ebb1075-3538-49d1-93d1-a00c94fa256a, Fable5.1 max, is authorized. Turn47 failed credits at 2026-09-21T10:31:58Z; no findings. Do not retry without an availability change. Receipt: allmaps/fable-followup/turn47.

## Execution

All shell calls require_escalated (default bwrap fails); Python3.7. Heavy builds, regressions and raw analysis run on GRID. Hosts research44/57, slots-per-core2: four physical cores = 8 slots x4GiB; eight physical =16 slots x2GiB. Disjoint binding, no CPU quota, 32 decimal GB process cap, strict1000ms complete entry. Shared hosts are authorized; do not claim exclusivity. A timeout fails the entry, never returns a partial best. No production/test edits during live builds; frozen binaries permit independent later edits.

## Frontier

| Instance | Tasks | Seed | Max entry ms |
|---|---:|---:|---:|
| WAREHOUSE | 155173 | 0 | 944.766 |
| SORTATION | 150894 | 0 | 993.898 |
| CITY-01 | 8440 | 2 | 752.710 |
| CITY-02 | 16830 | 0 | 809.831 |
| GAME | 24447 | 4 | 826.520 |
| RANDOM-01 | 693 | 4 | 488.915 |
| RANDOM-02 | 1215 | 2 | 359.375 |
| RANDOM-03 | 1982 | 2 | 662.922 |
| RANDOM-04 | 2304 | 2 | 304.051 |
| RANDOM-05 | 3610 | 10 | 729.825 |

All are full-horizon TRICK profiles. Warehouse and Sortation reserve8 physical cores; others4. GAME and RANDOM05 meet the selected-seed margin target. Published baselines are historical targets, not matched competitor reruns. No starvation-free claim. RANDOM05 parks31 capacity robots and its best now adds cap700. GAME restricts the active fleet. Warehouse has exclusive qualification and100M-action replay; other results have simulator plus complete movement/wait audits.

Canonical files: selected-full-results.json, BEST_HISTORY.md, active-targets.json, CGAR_PROGRESS.md. Latest all10 proof: selected-results-checks-20260921-130003.json. Exact RANDOM best profiles: history-rollout/best-random01.json, rolling-window/best-random02.json, window-delay/best-random03.json, future-crowd/best-random04.json, move-promises/best-random05.json and stronger-lanes/best-city02.json.

Motion commitments: RANDOM04 controls2059/1901/1843 -> mode2 2237/2298/2215 on seeds0/2/4, +16.319% aggregate, all positive. Forward-only gains11.529%. RANDOM05 seed10:3065 ->3391mode1 ->3571mode2, then3610 with cap700. Cap composition adds39 (+1.09%) on one seed; no replicated cap claim. R05 uncapped replication is complete:3532/3531/3571 versus3043/3015/3065 on seeds0/2/10, +16.5625% aggregate, all positive. All three mode1 scores also exceed3355. Cap composition remains single-seed evidence.

## Finished negative comparisons

- RANDOM04 H20/no-futures:2237/2298/2215 vs2239/2219/2270; H20 retains the best and a22-task aggregate advantage. Terminal/paid chain under mode2 give2231/2044 versus2239control. KK-field/turn2 gives2298; turn20=1908, existing PILOT-field=2056, both1987.
- Window+motion commitments lose on all sparse selected seeds: RANDOM01 693/687/688, RANDOM02 1215/1197/1172, RANDOM03 1982/1848/1942 (control/mode1/mode2).
- New window/scheduler2x2 also loses: R01 693/691/685/686, R02 1215/1212, R03 1982/1982/1872/1883 (control/horizon mean/more rematching/both). All controls reproduce full trajectories.
- Cap700 without commitments gives +1.129% aggregate over seeds0/2/10, one loss. Nearby caps625/650/675 lose to700. Superseded by current commitment+cap profile.
- Sortation wider matching gives150863 versus150894 control; both valid. Warehouse155173control/154829wider is fully verified; current width32 remains.
- CITY01 stronger lane prices16/24/32/48 give8440/8413/8409/8394, all valid. Lower prices also lost earlier. Keep16. CITY02 price24 improves16315 to16830 (+515,+3.16%);32=16526 and48=16615. Replication on seeds2/4 is pending.

Older exact assignment, sparse action prices, regional-future roots, and various window/history/annealing changes lost. Consult feature logs before repeating unchanged arms. Earlier replicated sparse gains: R01 uniform field +2.69%, refreshedkeep10 +1.88%; R03 delay3 +3.293%, each across three seeds.

## Builds and source

No active build and no production source edits pending. Latest production source b6d526fcf4d1e39b102b1083bdf30ac0c7e6962f is fully qualified (all40 source/test hashes, binary, full regression). Raw: runs/cgar-future-crowd-build-v1-20260921; binary9efc77eac135d4632736adefc99f0e8f6774299b4e0df4b3ce1add1c861969e9. Proof: future-crowd/build-v1.

CGAR_FUTURE_CROWD_COST0..255 adds pairprice times terminal Chebyshev2 unordered pairs across all robots to each completed forecast. Default0 does not count pairs and preserves RNG/actions. All branches complete or throw timeout. Counter O(cells+12*robots). Independent490-layout geometry tests; analytically known stay/separate root costs;14400 production actions,2001 services, serial/parallel identity, protection and movement-promise composition. Diagnostics reconcile base+pair cost. New first_cells equality guard also included in common-future roots.

Stronger lanes source59c73dc82fb0cbaa2a2d2f7021461b7cbe8ed4fe, binary86a0c0586a705a6e4c4a10317000b0bd5759ea800c72512c1fdf2d943f286bef, fully qualified in stronger-lanes/build-v1. Prices24/32/48/64 only explicit adapted CITY/GAME lanes. Independent full oriented heap at64; all old fields/default16 preserved.

Earlier qualified source44986ddf54fc381f478ba5116b9660389c3fb693 composes window and movement commitments; e66daa2f065e2b9589f1e2c972c3437f2e636f2d implements motion commitments alone. Frozen builds in matching runs/cgar-NAME-build-v1-20260921 paths. All hashes in feature proofs and ACTIVE_JOBS.

## Pending jobs

- city02-stronger-lanes-replicate-full-v2: matrix 8902894, verifier 8902895, source 59c73dc8, 4 physical cores/case.
- random04-future-crowd-replicate-full-v2: matrix 8902896, verifier 8902897, source b6d526fc, 4 physical cores/case.

Warehouse verification is complete after two preserved parser failures: missing cap defaults to0, and the compiled solver emits no cap diagnostics at0. The third analysis verifies both unchanged solver outputs. No solver was rerun. Evidence and failures are in warehouse-sortation-matching.

## Next bounded work

1. Collect CITY02 lane24 replication (seeds2/4) and RANDOM04 crowd16 replication (seeds0/4). R04 crowd16 selectedseed2 improves2298 to2304 (+6 only); weaker priority weights lose1974/1535. Promote only verified full records; tiny selected-seed gains are not robust claims.
2. Prepare explicit RANDOM reference-strength interpolation toward uniform20. This is NOT implemented yet. Existing NMS costs20/60; KK03 ranges2..62; KK04 ranges6..64. Low-cost routes can favor long detours. Hypothesis: 25/50/75% of the existing field contrast may improve physical throughput. Keep turns fixed and masks/topology unchanged; default100 byte-identical. Reference1/2 only, explicit --trick RANDOM instance, native static metric, reject simultaneous uniform mode. Generate immutable hashes/FNV from verified base assets. All oracles/pickup remain on the same transformed metric. Need parser, runner, diagnostic identities and independent oracle/integration tests. Prior whole-PILOT-field substitution lost; this is a different contrast factor.

Immutable PILOT source copies: /tmp/cgar-pilot-readonly-d51/engine.cpp and engine.hpp, commitd51bdbf2c4e6830f8cabacb52ac9cfa8234aa663; source review proof pilot-transfer/source-review-20260921-1112.json. Do not inspect independent current results.

## Helpers and checkpointing

/tmp/cgar_qualify_build.py --raw runs/... --proof experiments/allmaps-20260920/FEATURE/build-v1 verifies every source/test hash, commit, binary, full regression and allocation.
/tmp/cgar_submit_profile.py --instance INSTANCE --seeds ... --profile JSON --suffix SUFFIX --build runs/... --source FULLHASH --local-log-spool [--inputs MANIFEST] [--cores 8]. Always override its old default339718b3. CITY inputs: city-game/inputs-CITY-01.json etc. Warehouse: warehouse-sortation-matching/inputs-WAREHOUSE.json. Sequential ACTIVE_JOBS writers.
/tmp/cgar_refresh_frontier.py --report SUFFIX (repeatable) --note ... only clears the requested completed reports. It updates canonical/targets/history/proofs, but not exact best-profile files or narrative. Inspect results before calling.
/tmp/cgar_commit_checkpoint.py owns only CGAR_PROGRESS, NEXT, Fable helper and allmaps/**. Result directories need verification.json; ignored raw logs excluded. Update commit message. Poll exec_command session until commit finishes before dependent push/build.

Authorized push: GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 git -c 'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main

Frozen benchmark runner now has SHA5c322b1ca675ed8c0bb8b2a1732be409106e465734461401753a1ab59f61f8eb. Previous runnerf504... boundary recorded in stronger-lanes/runner-upgrade-boundary.json: every old case had loaded its old runner before the change. Do not edit the live runner while any queued matrix still has an unlaunched case. Metadata.json appears after runner code/hash and assets load; remaining running processes retain those originals.

NFS negative cache: list parent and retry; do not rerun a solver merely for visibility. Local log spool archives full logs after every exit including failures. Strict entry time still includes logging. Earlier2632ms timeout is retained; its cause is unproven.
