# General mechanisms and explicit instance tricks proceed together

Updated2026-09-21UTC after the user's renewed instruction. Throughput is primary;
waiting/ownership metrics are reported separately. All instance-specific
configuration and assets require `--trick INSTANCE` and labelled commits/logs.
A successful trick may update the overall best, including an individual seed.
The independent RANDOM-05 agent's source and jobs are outside our edit scope.

| Instance | Current record and next comparison |
|---|---|
| WAREHOUSE | Preserve155173 strict1s; no current retuning. |
| SORTATION |150353 at5s. Strict full runs with1M/2Mglobal work and8cores underway. |
| CITY-01 |8386 pickup8 seed6,34below published8420. Pickup10/12/16 factors underway. |
| CITY-02 |16159 pickup12. Test16,16+short preference,12+pickup grouping. |
| GAME |14664 adapted field+squared chain ranks. Native/adapted x dispatch full comparison underway. Study KK fleet selection separately. |
| RANDOM-01 |621 NMS arrows+pickup4; generic613 remains. Replicate and inspect scheduler tradeoff. |
| RANDOM-02 |1160 NMS arrows+pickup4; higher pickup8 and extra proposals lose. |
| RANDOM-03 |1890 KK forward+general chain ranks+pickup4; generic1613. Replicate and measure fixed-work scaling. |
| RANDOM-04 |1645 squared ranks, no field. NMS/KK fields lose the1622seed0 control. |
| RANDOM-05 |2608 existing field/direct4/matching. New NMS arrows lose; squared ranks mixed. General joint-prefix or exact-chain ideas remain. |

Read-only source facts, with [frozen fingerprints](trick-review-source-hashes.json):

* NMS uses `GraphGuidance(env)` for CITY/GAME and reads the first arrow layout
  from `guidance_map_random.txt` for RANDOM. Its random graph is2 on preferred
  forward edges and6 on other forward directions, with turn/wait2. This is not
  the20/200 CITY/GAME field or the standalone RANDOM-05 field already borrowed.
  Preserve those units or document any adaptation explicitly.
* NMS squares rank-based robot power for GAME and RANDOM-04/05; CITY and sparse
  RANDOM retain linear power. CGAR implements this explicit selector with full regression proof.
  It improves RANDOM04 across three planner seeds and GAME on seed0,
  but RANDOM05 replication is mixed; retain linear there.
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

Further KK inspection: the active GAME LNS/LaCAM2 path uses `tabu_locs`,
not its separate top-level uniform selector. It excludes robots starting in
`brc202d_tabu_locs.txt` from the random disabling pool, and disables at most
N-2750 eligible robots once. Therefore the configured cap can leave more than
2750 active if fewer candidates are available. Active input/source selection
must be preserved when testing a corresponding CGAR trick.

KK guidance arrays use E/S/W/N/stay order, the same physical directions as CGAR.
RANDOM02 forward0.9/1.1 and uniform stay1 are a simple exact transfer candidate.
RANDOM03/04 and CITY have location-varying stay/turn costs; CGAR's current constant
turn oracle cannot reproduce those by copying forward costs alone. GAME forward
weights include100000, requiring explicit handling beyond the current8-bit field.
Any simplified adaptation must be labelled and compared separately.
