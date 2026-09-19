# Loaded motion accounting

The latest confirmed pickup policy reduces empty travel. An independent action
replay now separates the remaining delivery time into forward moves, turns and
waits, to identify useful motion experiments.

The offline C++ analyzer classifies each action by the observed task assignment
and pickup/finish events. All50 million robot-steps per run fall into exactly one
idle/empty/loaded phase. Every completed task's loaded action count equals its
finish time minus pickup time. The prior hand-counted assignment fixture and a
new fixture with one loaded turn, one wait and one forward action both pass.

GRID job8898958 completed four full replays. The new output preserves every old
report field exactly, independently reproduces idle/empty totals, and matches all
CGAR production phase/action counters. Per-task components sum to measured loaded
elapsed time. Source and script SHA-256 values, map/log hashes and submission
metadata are archived. No planner code changed.

| Run | Tasks | Shortest chain | Extra forward | Turns | Waits | Loaded elapsed |
|---|---:|---:|---:|---:|---:|---:|
|64 fields, seed0|141829|241.76|9.12|11.19|15.31|277.39|
|64 fields, seed2|142988|241.81|8.20|10.36|14.10|274.46|
|32 fields, failed seed0|70043|235.29|35.57|44.04|77.82|392.72|
|Local KittyKnight|152981|233.84|4.59|5.68|8.41|252.52|

All action values are means per completed task. The shortest chain is the exact
unweighted physical-grid distance through its errands; extra forward is observed
forward actions minus that bound. Turns include necessary heading changes. These
components add within each run's observed completed-task cohort. Different solvers
complete different cohorts, and unfinished work is censored. Their differences
are observational, not additive counterfactual savings. KittyKnight used38.858GB
RSS versus CGAR's32GB limit.

The healthy CGAR runs show excess loaded turns, waits and forward detours relative
to this local reference. The failed32-field run has large increases in all three,
despite a shorter completed-task chain. This supports testing motion coordination
and traffic-cost strength alongside assignment quality; it does not identify a
single cause or establish that more search will help.

[Comparison and limits](results/loaded-motion-audit-v1/comparison.json),
[complete audit](results/loaded-motion-audit-v1/),
[cases](loaded-motion-cases.json), [C++](decompose.cpp), [driver](decompose.py).
