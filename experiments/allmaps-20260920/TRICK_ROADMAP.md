# General mechanisms and explicit instance tricks proceed together

Updated2026-09-21UTC after the user's renewed instruction. Throughput is primary;
waiting/ownership metrics are reported separately. All instance-specific
configuration and assets require `--trick INSTANCE` and labelled commits/logs.
A successful trick may update the overall best, including an individual seed.
The independent RANDOM-05 agent's source and jobs are outside our edit scope.

| Instance | Active trick work / next controlled comparison |
|---|---|
| WAREHOUSE | Preserve155173 strict1s record and its native guidance; no current retuning. |
| SORTATION | Native NMS field reached150353, close to152714 target; reduce1152ms peak and verify strict1s. |
| CITY-01/02 | Native and adapted NMS parity fields plus native/short-preference comparison, full strict1s runs submitted. Then compare KK's `paris_weight_014.w` if needed. |
| GAME | Same explicit NMS field comparison submitted. Next distinguish NMS squared priority weighting from field effects; KK's `brc202d_weight_002.w` and2750-agent active limit are separate candidates. |
| RANDOM-01 | Keep a no-field control: KK also selects no weight file at100 agents. Study scheduler-specific tuning before assuming lanes help. |
| RANDOM-02 | Compare unchanged NMS arrow field and KK's `random_weight_101.w` after adding explicit fleet/map gates. |
| RANDOM-03 | Compare NMS arrow field and KK's `random_weight_040.w`; guidance can matter at this density. |
| RANDOM-04 | The transferred800-robot field did not beat the generic mean. Keep separate tuned-field branch. NMS squared priority weighting and KK's `random_700_w002.w` are next candidates. |
| RANDOM-05 | Selected CGAR field+matching reaches2608. Promises/short-preference screen lost; measure native service-tail scoring separately. A coherent joint motion prefix or full-chain metric is a later general transfer. |

Read-only source facts, with [frozen fingerprints](trick-review-source-hashes.json):

* NMS uses `GraphGuidance(env)` for CITY/GAME and reads the first arrow layout
  from `guidance_map_random.txt` for RANDOM. Its random graph is2 on preferred
  forward edges and6 on other forward directions, with turn/wait2. This is not
  the20/200 CITY/GAME field or the standalone RANDOM-05 field already borrowed.
  Preserve those units or document any adaptation explicitly.
* NMS squares rank-based robot power for GAME and RANDOM-04/05; CITY and sparse
  RANDOM retain linear power. Current CGAR dense profiles use linear power.
  This is a precise instance-specific comparison still to implement, not an
  already measured gain.
* KK's active planner selects the first config threshold with `n_agents >=
  actual_agents`. RANDOM files therefore differ at100/200/400/700 robots.
  Its800-robot selection is a different planner (`Shadocks`), not a guidance
  change to borrow and relabel as CGAR.
* KK's active GAME config caps agents in use at2750. That is a separate fleet
  selection trick; it cannot be conflated with removing fairness alone, and its
  behavior must be understood before adapting it around CGAR protections.
* KK's `greedy_matching2` includes idle/unopened tasks and uses its planner's
  oriented pickup metric plus the summed task-chain metric. CGAR's bounded local
  rematching is not that full-pool scheduler. Full joint matching remains a
  general mechanism to evaluate without silently dropping started ownership.

Known-horizon triage is allowed as a labelled trick, but CGAR's current horizon
scheduler requires an unrestricted full core for its lower bound. RANDOM and
irregular CITY/GAME have pockets. Do not bypass that check; construct a valid
free-space bound or declare a separate heuristic first.

No claim that a reference trick transfers successfully is made from source
inspection alone. Keep individual ablations and full-run controls, including
negative results. Dense after-turn promises reduced reversed turns but lost
throughput, and short-task preference also lost that seed0 screen.
