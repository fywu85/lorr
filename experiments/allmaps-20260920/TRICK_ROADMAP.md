# CGAR next experiments across ten instances

Updated 2026-09-21T14:06:57.742781+00:00
Targets: +10% over max(published NMS,KK) on RANDOM01–05; stretch +5% on the other
five. Throughput first, fairness secondary. These are historical targets rather
than matched competitor runs. Every selected profile requires --trick INSTANCE.

| Instance | Selected full strict1s | Next work |
|---|---:|---|
| WAREHOUSE | 155173 | Wider matching loses154829 versus155173; keep the qualified eight-core profile. +5% remains a stretch. |
| SORTATION | 150894 | Known5000horizon with mean/p90 residual margins is running; wider matching lost. Deadline headroom remains narrow. |
| CITY-01 | 8453 | Selected seed6 now8453, eight above published8445. Nearby price20 loses; continue route/dispatch improvements, do not claim a robust margin. |
| CITY-02 | 16840 | Selected seed8 adds one task;157short of16997. Price24 replicates +3.6849%;20/28lose. Investigate dispatch/route interactions at the stronger field. |
| GAME | 24447 | New margin target met; retain the explicit fleet/rank tricks. |
| RANDOM-01 | 693 | Retained-tail refresh replicates; exact assignment and turn/wait price interactions lose. |
| RANDOM-02 | 1215 | Guidance weakening and complete-worker merging lose. Study PILOT causal blocker neighborhoods for more useful bounded repairs. |
| RANDOM-03 | 1982 | Delay3 replicates; guidance weakening and merging lose. Causal blocker selection is the next bounded source-transfer hypothesis. |
| RANDOM-04 | 2398 | Seed4 now2398 using16workers/8rounds, +3.32%, max0.549s. Replicating both positive work arms on seeds0/2. |
| RANDOM-05 | 3610 | Margin target met on seed10 with movement commitments and cap700; uncapped commitments gain16.56% over three seeds. Cap interaction gains39 on one seed. |

RANDOM-01 improved at matched work with uniform guidance (+2.69% over three seeds),
then a refreshed retained tail (+1.88%, all three positive). More attempts and
larger groups lost; extreme work failed its deadline. RANDOM-03 delay selection
gains28/82/78 tasks on seeds0/2/4 (+3.29%aggregate). Annealed repair retains the best
complete plan only after all declared attempts finish, with deadline exceptions.

All regional-future arms lose on RANDOM04/05; keep them off. The earlier global future
selector changed no actions on RANDOM05 before movement commitments. One-action motion commitments now transfer the next occupied cell; a full two-phase
pipeline has not been substituted for CGAR. Current window/future layers retain CGAR primary/recovery
and protected actions; they are not renamed PILOT or NMS solvers.

New matrix runs may explicitly spool logs under node-local /tmp and archive them
after process exit, including failures. The frozen runner hash and sink mode are
verified. This reduces exposure to shared-filesystem stalls while keeping strict
entry deadlines and complete diagnostics; it does not erase earlier timeouts.

[Frontier](selected-full-results.json), [history](BEST_HISTORY.md),
[jobs](ACTIVE_JOBS.json), [PILOT transfers](pilot-transfer/NEXT_TRANSFER.md).

All ten selected trajectories now have reconciled [movement accounting](bottlenecks/README.md). RANDOM01–03 rotate24–41% of robot actions; RANDOM04/05 wait25–38%; CITY moves forward92–94%. These are descriptive shares, not throughput ceilings.
