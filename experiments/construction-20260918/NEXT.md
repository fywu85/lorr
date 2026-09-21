# Continue CGAR across ten instances

Updated 2026-09-21T14:06:57.742781+00:00. Latest user prioritizes RANDOM02-04 and pushing SORTATION/CITY01/CITY02 above max(NMS,KK); latest PILOT source review explicitly authorized.

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
| CITY-01 | 8453 | 6 | 764.785 |
| CITY-02 | 16840 | 8 | 802.558 |
| GAME | 24447 | 4 | 826.520 |
| RANDOM-01 | 693 | 4 | 488.915 |
| RANDOM-02 | 1215 | 2 | 359.375 |
| RANDOM-03 | 1982 | 2 | 662.922 |
| RANDOM-04 | 2398 | 4 | 548.105 |
| RANDOM-05 | 3610 | 10 | 729.825 |

All are full-horizon TRICK profiles. Warehouse and Sortation reserve8 physical cores; others4. GAME and RANDOM05 meet the selected-seed margin target. Published baselines are historical targets, not matched competitor reruns. No starvation-free claim. RANDOM05 parks31 capacity robots and its best now adds cap700. GAME restricts the active fleet. Warehouse has exclusive qualification and100M-action replay; other results have simulator plus complete movement/wait audits.

Canonical files: selected-full-results.json, BEST_HISTORY.md, active-targets.json, CGAR_PROGRESS.md. Latest all10 proof: selected-results-checks-20260921-140445.json. Exact RANDOM best profiles: history-rollout/best-random01.json, rolling-window/best-random02.json, window-delay/best-random03.json, committed-work/best-random04.json, move-promises/best-random05.json and city-seed-frontier/best-city01.json / best-city02.json.

Motion commitments: RANDOM04 controls2059/1901/1843 -> mode2 2237/2298/2215 on seeds0/2/4, +16.319% aggregate, all positive. Forward-only gains11.529%. RANDOM05 seed10:3065 ->3391mode1 ->3571mode2, then3610 with cap700. Cap composition adds39 (+1.09%) on one seed; no replicated cap claim. R05 uncapped replication is complete:3532/3531/3571 versus3043/3015/3065 on seeds0/2/10, +16.5625% aggregate, all positive. All three mode1 scores also exceed3355. Cap composition remains single-seed evidence.

## Finished negative comparisons

- RANDOM04 H20/no-futures:2237/2298/2215 vs2239/2219/2270; H20 retains the best and a22-task aggregate advantage. Terminal/paid chain under mode2 give2231/2044 versus2239control. KK-field/turn2 gives2298; turn20=1908, existing PILOT-field=2056, both1987.
- Window+motion commitments lose on all sparse selected seeds: RANDOM01 693/687/688, RANDOM02 1215/1197/1172, RANDOM03 1982/1848/1942 (control/mode1/mode2).
- New window/scheduler2x2 also loses: R01 693/691/685/686, R02 1215/1212, R03 1982/1982/1872/1883 (control/horizon mean/more rematching/both). All controls reproduce full trajectories.
- Cap700 without commitments gives +1.129% aggregate over seeds0/2/10, one loss. Nearby caps625/650/675 lose to700. Superseded by current commitment+cap profile.
- Sortation wider matching gives150863 versus150894 control; both valid. Warehouse155173control/154829wider is fully verified; current width32 remains.
- CITY01 stronger lane prices16/24/32/48 give8440/8413/8409/8394, all valid. Lower prices also lost earlier. Keep16. CITY02 price24 improves all three seeds:16830/16754/16839 versus16315/16199/16117, +3.6849% aggregate. Selected16839seed4. Prices32/48 on seed0 were16526/16615. All control trajectories reproduce exactly.

Older exact assignment, sparse action prices, regional-future roots, and various window/history/annealing changes lost. Consult feature logs before repeating unchanged arms. Earlier replicated sparse gains: R01 uniform field +2.69%, refreshedkeep10 +1.88%; R03 delay3 +3.293%, each across three seeds.

## Builds and source

Latest fully qualified production source766e2508919fdec01bb4aaefce15825deedde04a implements generic optional CGAR_WINDOW_MERGE=1. All41 source/test hashes, binary and full regression passed. Raw runs/cgar-window-merge-build-v1-20260921, binary1f68798699416623c41aa17c7f53b04ff5b9931003f7d3bddabdfeca7c9b051a, proof window-merge/build-v1. No active build or production edits pending.

Every window worker finishes first. Starting from its existing best, visit all other workers in index order; connect every cross-parent vertex/head-on conflict across the full horizon and adopt only improving complete components. CGAR protected prefixes/paths, pocket permissions and first-cell commitments remain validated. Exact integer gains and complete donor counts are reconciled; default0 preserves old work/RNG/actions, late failure publishes no answer/history. Tests:11 independent quadratic graphs,20 complete component mixtures, isolated head-on/cycle/complementary fixtures, protected/promise gate, unchanged worker work, serial/parallel identity, late failure; production14400actions/2147services.

Nearby CITY costs20/28 are implemented and fully qualified atf22bc68bb6ea5c3ecc6234fe23838a5ca10ac5e9. Raw runs/cgar-lane-neighbors-build-v1-20260921; binary989748d723ffb90d6ac4ec6d380a510d43ef46ecbfb276cacd498953c90333b0, proof lane-neighbors/build-v1. Explicit adapted CITY/GAME fields only. All old field identities and masks preserved; independent geometry/hash and real pickup/planning tests include new costs. Default16 unchanged.

Reference-strength source6fc0cc50 remains qualified, but every25/50/75 treatment lost on RANDOM02/03/04. See reference-strength/first-results.json; keep100. Its frozen build is used for SORTATION horizon arms because that asset is unchanged. General crowd-cost sourceb6d526fc, movement-promises sourcee66daa2f and window-composition44986ddf remain qualified; existing best profiles are independently reproducible.

## Pending jobs

- sortation-horizon-full-v1: matrix8902917, verifier8902918, source6fc0cc50, 8 physical cores/case.
- random04-committed-work-replicate-full-v2: matrix8902948, verifier8902949, sourcef22bc68b, 4 physical cores/case.

Warehouse verification is complete after two preserved parser failures: missing cap defaults to0, and the compiled solver emits no cap diagnostics at0. The third analysis verifies both unchanged solver outputs. No solver was rerun. Evidence and failures are in warehouse-sortation-matching.

## Next bounded work

1. Collect SORTATION known-horizon transfer and RANDOM04 committed-work replication. RANDOM04 seed4 rises2321->2358 with8rounds and2398 with16workers+8rounds; maximum0.549s. Both positive treatments are replicating on seeds0/2. Additional workers alone lose2278. Current best is committed-work/best-random04.json.
2. Completed CITY neighbors all lose: CITY01price16/20=8440/8434, CITY02price24/20/28=16839/16254/16611. Original verifier omitted legal20/28; same simulation outputs reanalysed after parser-only whitelist fix, original failures retained. Fixed-profile CITY seeds6/8/10/12 give8453/8433/8437/8427 and16719/16840/16704/16741. CITY01selected8453 now passes8445by8; CITY02selected16840 remains157short. This is selected-seed evidence, not an algorithm effect. Generic window merge loses: R02control1215,merge1173,wide1189; R03control1982,merge1962,wide1859. All controls match complete prior trajectories. Keep merge0. Explore causal blocker neighborhoods next; not implemented yet.
3. Further source hypotheses from PILOT: cached displacement costs excluding a head-on destination, goal-local guidance, complete joint-movement proposals with bounded matching swap repairs. Do not infer measured gains from their presence in source. Evaluate interaction with CGAR primary/recovery and current motion commitments before any transfer. Sparse worker-component merge was already present in the frozen reference but not previously transferred.

Latest immutable PILOT snapshot: /tmp/cgar-pilot-readonly-027df4d9/*.cpp and engine.hpp, commit027df4d97ad737ad6319354abe8b92afc2840473. Source-only review/provenance: pilot-transfer/source-review-20260921-1328.json. No independent agent working files or live outcomes were read or changed. Earlier source d51bdbf2c4e6830f8cabacb52ac9cfa8234aa663 remains archived.

RANDOM04 crowd16 replication remains modest:2176/2304/2321 versus2237/2298/2215, +0.7556% aggregate with one loss. CITY02price24 replicates +3.6849% across all three seeds. Reconciled current movement shares are in bottlenecks/README.md and its latest linked snapshot.

## Helpers and checkpointing

/tmp/cgar_qualify_build.py --raw runs/... --proof experiments/allmaps-20260920/FEATURE/build-v1 verifies every source/test hash, commit, binary, full regression and allocation.
/tmp/cgar_submit_profile.py --instance INSTANCE --seeds ... --profile JSON --suffix SUFFIX --build runs/... --source FULLHASH --local-log-spool [--inputs MANIFEST] [--cores 8]. Always override its old default339718b3. CITY inputs: city-game/inputs-CITY-01.json etc. Warehouse: warehouse-sortation-matching/inputs-WAREHOUSE.json. Sequential ACTIVE_JOBS writers.
/tmp/cgar_refresh_frontier.py --report SUFFIX (repeatable) --note ... only clears the requested completed reports. It updates canonical/targets/history/proofs, but not exact best-profile files or narrative. Inspect results before calling.
/tmp/cgar_commit_checkpoint.py owns only CGAR_PROGRESS, NEXT, Fable helper and allmaps/**. Result directories need verification.json; ignored raw logs excluded. Update commit message. Poll exec_command session until commit finishes before dependent push/build.

Authorized push: GIT_ASKPASS=/bin/false GIT_TERMINAL_PROMPT=0 git -c 'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main

Frozen benchmark runner now has SHAc15523cffd8968d1542d5b937763e11b8cb6570d5d3f427b9676ed709f9ee119. Previous runner3a2c... boundary is recorded in lane-neighbors/runner-upgrade-boundary.json: every preexisting case had loaded its old runner before the change. Do not edit the live runner while any queued matrix still has an unlaunched case. Metadata.json appears after runner code/hash and assets load; remaining running processes retain those originals.

NFS negative cache: list parent and retry; do not rerun a solver merely for visibility. Local log spool archives full logs after every exit including failures. Strict entry time still includes logging. Earlier2632ms timeout is retained; its cause is unproven.
