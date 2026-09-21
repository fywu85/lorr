# CGAR next experiments across ten instances

Updated 2026-09-21T11:33 UTC
Targets: +10% over max(published NMS,KK) on RANDOM01–05; stretch +5% on the other
five. Throughput first, fairness secondary. These are historical targets rather
than matched competitor runs. Every selected profile requires --trick INSTANCE.

| Instance | Selected full strict1s | Next work |
|---|---:|---|
| WAREHOUSE | 155173 | Qualified eight-core profile; bounded wider assignment comparison running. +5% remains a stretch. |
| SORTATION | 150894 | Bounded wider assignment comparison running; runtime alternative150865 retains more deadline headroom. |
| CITY-01 | 8440 | Wider matching and known-horizon transfer lose; keep current profile. |
| CITY-02 | 16315 | Budget2 gain replicates; lower lane prices, horizon triage and wider matching lose. |
| GAME | 24447 | New margin target met; retain the explicit fleet/rank tricks. |
| RANDOM-01 | 693 | Retained-tail refresh replicates; exact assignment and turn/wait price interactions lose. |
| RANDOM-02 | 1215 | Delay, tail refresh, annealing, wider matching and exact chain assignment lose. |
| RANDOM-03 | 1982 | Delay3 gains +3.29% across three seeds; exact assignment and turn/wait price interactions lose. |
| RANDOM-04 | 2059 | H20 is best selected seed, H15 has better mean; task caps lose. One-action motion commitments running. |
| RANDOM-05 | 3130 | Task cap700 gains65 on seed10; seeds0/2 replication and nearby caps queued. One-action motion commitments running on prior uncapped control. |

RANDOM-01 improved at matched work with uniform guidance (+2.69% over three seeds),
then a refreshed retained tail (+1.88%, all three positive). More attempts and
larger groups lost; extreme work failed its deadline. RANDOM-03 delay selection
gains28/82/78 tasks on seeds0/2/4 (+3.29%aggregate). Annealed repair retains the best
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

All ten selected trajectories now have reconciled [movement accounting](bottlenecks/README.md). RANDOM01–03 rotate24–41% of robot actions; RANDOM04/05 wait25–38%; CITY moves forward92–94%. These are descriptive shares, not throughput ceilings.
