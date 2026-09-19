# Offline audit of revealed task-chain prices

This is an information audit before changing assignment. It uses saved reference
trajectories to reconstruct the exact production flow publisher. At each publication,
all logged step/sample/move/penalized-edge/publication counters must match. Tasks are
priced against the field available at ASSIGNMENT, not pickup: scheduling precedes
that timestep's flow observation, and recorded schedule steps are one-based. A field
published at planner timestep p is first available to assignments recorded at p+2.

Choose 64 tasks per published field by a deterministic seed/publication/task-id hash,
before looking at pickup/completion outcomes. Exclude warmup and one-stop tasks by
features known at assignment. Preserve unfinished sampled tasks as censored records.
Reuse production certificate, spatial and oriented shortest-path implementations.
The oriented sum minimizes the starting heading separately for each known leg; it
is a relaxed lower bound, not exact full-chain heading coupling. No planner policy
or future-task access is introduced.

Compare out-of-seed prediction of realized loaded time using unit shortest-path
chain length versus adding oriented excess, with assignment-time/leg-count controls
and a permutation null within seed/publication. This uses selected assigned tasks,
so it cannot establish counterfactual assignment gains. Complete offline tables also
do not model production cache availability. Record those limitations even if the
extra feature predicts well. Heavy replay and path computation run only on GRID.


## Completed six-seed audit

GRID replay 8899212 finished 2026-09-19T23:31:51.084691UTC; analysis 8899213
finished 23:35:52.796786UTC. All 60 full-run flow publications and all seven input
trajectory fingerprints (including the pipeline prefix) match their saved controls.
The sample contains 3,840 assigned tasks: 3,429 completed labels and 411 censored tasks.

Leaving one whole seed out, adding the assignment-time oriented excess reduces
mean squared error from 286.1083 to 270.4558 (**5.471%**) and mean absolute error
from 10.9522 to 10.4342 steps. Each of the six held-out seeds improves. The primary
model controls unit chain length, assignment age/time and remaining leg count.
The within-seed/publication permutation comparison gives p=1/201 over 200 shuffles;
limiting to completed assignments through step 4,000 retains a 4.949% MSE reduction.
Independent NumPy SVD fits reproduce all six fold errors and coefficients within
1e-8, checking the separate standard-library QR/residualization implementation.

This supports testing the feature in assignment. **It is not a throughput gain.**
The completed, already-assigned cohort is selected and censored; it does not cover
counterfactual free tasks. The baseline is exact unit BFS, not the scheduler's
sometimes approximate cache price. Complete offline oriented tables do not prove
production cache coverage. Independent fits check arithmetic, not those limitations.

[Results and provenance](../results/chain-price-audit-v50/prediction.json),
[independent arithmetic check](../results/chain-price-audit-v50/independent-fit-check.json).
