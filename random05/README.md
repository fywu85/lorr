# Independent RANDOM-05 combined solver

Implementation from the colleague's development log supplied in this conversation.
The colleague's code and tuned field are unavailable; this is a new implementation.

The first version uses exact all-pairs oriented distances (about 43 MB on this map),
exact task-chain continuation values, joint greedy matching of eligible unopened
tasks, a two-step pipelined spatial PIBT policy, persistent randomized-priority
rollout portfolios, dead-end exit priorities, and 2x2 simultaneous cycle completion.

All 800 robots remain active. Started tasks cannot be reassigned. The first version
has no known-horizon triage, optimized traffic-assignment field, or local hill-climb.
Guidance is optional and requires --trick RANDOM-05.

Development uses fixed complete futures; R05_K controls their count, R05_DEPTH their
length, and R05_THREADS their parallel execution. The decision deadline remains a
hard error. A failed PIBT push commits its blocking robot to wait and propagates failure
to the requester. Completed independent movement chains are retained; every
output transition is collision-certified.

Tests cover exact chained orientation costs, collision/swap rejection, started-task
locking, and dense simulation with task turnover. The official simulator performs
a second independent validation during full runs.

See ../RANDOM05_PROGRESS.md for verified results and source revisions.
