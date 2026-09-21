# Complete oriented chain potential in new-task assignment

CGAR's current task pairing uses weighted approach plus a cached spatial chain
estimate. PILOT's documented scheduler uses oriented whole-chain costs. CGAR now
already owns a complete exact static oracle for the small maps, so this experiment
reuses it for assignment without importing a different scheduler.

General CGAR_SCHEDULER_CHAIN_POTENTIAL=1 scores actual pairs by complete oriented
remaining-chain potential plus (pickup_weight-1)*the existing approach estimate.
The oracle already includes one approach. This accounts for heading coupling and
one service per physical action, including repeated errands. Endpoint-bucket
ordering, initial dispatch, candidate budgets, oldest admission, held/started
ownership, matching and finite retarget limits stay unchanged. Unreachable static
quotes explicitly count a fallback to the old estimate; timeout remains failure.
No additional distance tables, live repricing, future task visibility or map test.
An existing chain/window/common-future oracle is required. The mechanism works on
generic static maps; weighted tricks still require their named --trick flag.

Independent full action/service shortest paths determine the expected production
assignment in heading, repeated-service and pickup-bias fixtures. Tests also check
unchanged explicit-zero and initial dispatch, preserved simulator metadata,
oldest-task admission and started tasks, no added table builds, actual valid
execution and strict rejection without a complete oracle. Full regression and
frozen40file/binary provenance must pass before running the matrix.

Declared comparison on each RANDOM instance: current selected profile versus
only this flag enabled, seeds4/2/2/0/10 respectively. Full600/600/800/1000/2000steps,
four physical cores/case, strict1000ms,32decimalGB, node-local logs. Controls are
693/1215/1982/2059/3065; each whole trajectory must match its current reference.
The composition currently being tested on RANDOM05 is a separate experiment and
is not included here. Repeat positive gains on additional seeds before claiming
reliability. No edits to the independent RANDOM05/PILOT workspace.
