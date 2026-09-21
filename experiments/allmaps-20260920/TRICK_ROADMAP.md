# CGAR next experiments across ten instances

Updated 2026-09-21T13:24:01.461718+00:00
Targets: +10% over max(published NMS,KK) on RANDOM01–05; stretch +5% on the other
five. Throughput first, fairness secondary. These are historical targets rather
than matched competitor runs. Every selected profile requires --trick INSTANCE.

| Instance | Selected full strict1s | Next work |
|---|---:|---|
| WAREHOUSE | 155173 | Wider matching loses154829 versus155173; keep the qualified eight-core profile. +5% remains a stretch. |
| SORTATION | 150894 | Wider matching loses150863 versus150894; keep current profile. Deadline headroom remains narrow. |
| CITY-01 | 8440 | Wider matching, horizon selection, lower and higher lane prices all lose; keep price16. |
| CITY-02 | 16839 | Price24 gains3.6849% across three seeds; selected16839seed4. Nearby price comparison remains a possible next step. |
| GAME | 24447 | New margin target met; retain the explicit fleet/rank tricks. |
| RANDOM-01 | 693 | Retained-tail refresh replicates; exact assignment and turn/wait price interactions lose. |
| RANDOM-02 | 1215 | Guidance strengths25/50/75 versus100 are running on selectedseed2. Earlier window and scheduling changes lose. |
| RANDOM-03 | 1982 | Delay3 gains3.29% across three seeds; guidance strengths25/50/75 versus100 now running on selectedseed2. |
| RANDOM-04 | 2321 | Motion commitments replicate. Crowd16 adds0.76% aggregate with one losing seed; selected2321seed4. Guidance-strength comparison running. |
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
