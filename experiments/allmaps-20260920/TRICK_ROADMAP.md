# CGAR next experiments across ten instances

Updated 2026-09-21T10:14:13.450223+00:00
Targets: +10% over max(published NMS,KK) on RANDOM01–05; stretch +5% on the other
five. Throughput first, fairness secondary. These are historical targets rather
than matched competitor runs. Every selected profile requires --trick INSTANCE.

| Instance | Selected full strict1s | Next work |
|---|---:|---|
| WAREHOUSE | 155173 | Preserve qualified eight-core profile; +5% remains a stretch. |
| SORTATION | 150894 | Runtime alternative 150865 has more measured deadline headroom. |
| CITY-01 | 8440 | Pickup grouping gain replicates; examine finite assignment restrictions. |
| CITY-02 | 16315 | Budget2 gain replicates; lane penalty reductions lose. |
| GAME | 24447 | New margin target met; retain the explicit fleet/rank tricks. |
| RANDOM-01 | 693 | Retained-tail refresh replicates; annealed repair factors running. |
| RANDOM-02 | 1215 | Delay bias and tail refresh lose; annealed repair factors running. |
| RANDOM-03 | 1967 | Replicate delay3 improvement; annealing is tested separately. |
| RANDOM-04 | 2059 | Global future H20 is best selected seed, H15 has better three-seed mean; crowded-window check running. |
| RANDOM-05 | 3065 | Regional futures and startup lose; crowded-window check running. |

RANDOM-01 improved at matched work with uniform guidance (+2.69% over three seeds),
then a refreshed retained tail (+1.88%, all three positive). More attempts and
larger groups lost; extreme work failed its deadline. RANDOM-03 delay selection
adds 28 tasks at seed0; replication is pending. Annealed repair retains the best
complete plan only after all declared attempts finish, with deadline exceptions.

All regional-future arms lose on RANDOM04/05; keep them off. The global future
selector changes no actions on RANDOM05. A true two-phase movement pipeline has
not been transferred. Current window/future layers retain CGAR primary/recovery
and protected actions; they are not renamed PILOT or NMS solvers.

New matrix runs may explicitly spool logs under node-local /tmp and archive them
after process exit, including failures. The frozen runner hash and sink mode are
verified. This reduces exposure to shared-filesystem stalls while keeping strict
entry deadlines and complete diagnostics; it does not erase earlier timeouts.

[Frontier](selected-full-results.json), [history](BEST_HISTORY.md),
[jobs](ACTIVE_JOBS.json), [PILOT transfers](pilot-transfer/NEXT_TRANSFER.md).
