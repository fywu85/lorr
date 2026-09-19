# Bounded unopened-task exchange, v37

Direct-cost dispatch can select shorter delivery chains while increasing empty
pickup travel. The new optional `CGAR_REASSIGN_POOL=1` lets an en-route robot
exchange its unopened assignment with an available task when pickup distance
improves on its own and pickup plus complete chain distance does not increase.
The default is off. Both released and incoming tasks keep their reveal times.
The simulator commits the proposed schedule; this pass does not mutate task data.

The pass runs every10steps, considering at most256eligible sources,2048BFS nodes
and16available candidates per source, with at most32one-to-one exchanges. Sources
must belong to existing unopened episodes; new assignments made in the same
scheduling call are preserved. Primary and pending-primary, recovery, pocket,
capacity, unfinished commitment and fair-admission protections are shared with
the existing swap pass. A20-step robot cooldown and a once-per-unopened-task
retarget record cover both incoming and released tasks. Released tasks remain
in the pool for later ordinary scheduling and are not reinserted in this pass's
candidate snapshot. Started tasks cannot be exchanged.

Incumbent pickup and all chain legs require complete cached spatial tables.
Missing evidence causes a skip; speculative comparisons neither build tables nor
promote LRU entries. Candidate pickups are found with bounded forward BFS in
exactly the oracle's domain for core goals: core cells and unclassified free
floor whose pocket is-1. FIFO discovery gives exact distances to reached cells.
The pickup saving must be at least four steps and ten percent of the incumbent
pickup distance. The independent total-cost guard cannot pay for a longer pickup
by choosing a shorter chain. No Manhattan/exact mixture or map template is used.
This is a static distance heuristic, not a prediction of congested travel time.
Cached-table availability may bias which exchanges are considered.

The guards do not account exactly for the released task's later pickup, remove
all changes in task mix, or establish an end-to-end liveness/throughput proof.
Complete benchmarks remain decisive. Deadline checks throw through the normal
entry failure path; elapsed time never selects a partially completed decision.

Build8898723 passes all existing regressions plus independent hand-counted
pickup14-to1, equal-chain1-to1, worse-pickup/shorter-chain rejection, worse-total
rejection, missing-table rejection, started/primary preservation, original task
state/age preservation, retarget prevention after cooldown, invalid configuration,
and an attractive alternative to a non-primary fair-admission task. Its archive
patch independently reconstructs all20requested source and test hashes.

Screen8898727 passes all three200-step cases: disabled, exchange, exchange plus
two-owner transaction repair. Disabled exactly reproduces the established prefix.
Maximum complete entries are0.7627/0.7635/0.7696seconds; RSS stays below3.69GB.
Short results are not used to rank throughput.

Full8898728/8898729 compares the same profiles on seeds0/2 with all5000steps, six concurrent
instances, four disjoint physical cores per planner and72GiB aggregate reservation.
Each process retains its own one-second decision and32GBRSS validity checks.
No full exchange result is available yet.

[Source archive](build-provenance/v37/),
[regressions and screen](results/pool-exchange-screen-v37/),
[configuration](pool-exchange-variants.json).
