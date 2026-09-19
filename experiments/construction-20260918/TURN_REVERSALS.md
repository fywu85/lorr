# Opposite-turn audit

Seven complete warehouse trajectories were replayed on GRID job8898983. The
analyzer counts opposite quarter-turn transitions within the same robot, task and
phase. Adjacent transitions are back-to-back turns. Stationary transitions also
allow intervening waits, but reset on a forward action or task/phase boundary.
The latter count includes the former; transitions can overlap.

All four hand-counted fixtures pass, including R,W,C,R (two stationary transitions,
one adjacent) and a pickup boundary that excludes the preceding empty-phase turn.
Every old report field is unchanged for the three overlapping loaded-motion audits.
All50 million robot-steps are accounted for in each trajectory, and every phase's
forward/turn/wait count agrees with production metrics for all six CGAR runs.
The analyzer source/script hashes match the replay provenance. No planner changed.

| Run | Tasks | Loaded turns/task | Loaded waits/task | Adjacent reversals/task | Stationary reversals/task |
|---|---:|---:|---:|---:|---:|
|One worker, seed0|141829|11.191|15.311|0.498|0.524|
|Four workers, seed0|143340|10.334|13.830|0.357|0.376|
|One worker, seed2|142988|10.355|14.100|0.409|0.432|
|Four workers, seed2|143409|10.193|14.119|0.353|0.371|
|One worker, seed5|142917|10.367|14.209|0.401|0.424|
|Four workers, seed5|139710|11.777|15.974|0.647|0.697|
|Local KittyKnight|152981|5.678|8.411|0.092|0.110|

Action and transition means refer to completed tasks. Four workers reduce
reversals in seeds0/2, where throughput improves; seed5 has more reversals, turns
and waits as throughput falls. This is an association, not evidence that blocking
all reversals will help. Some turns accommodate traffic, and overlapping events
cannot be summed as unique wasted actions. Different completed cohorts and
unfinished work prevent interpreting differences as additive causal savings.
KittyKnight also used38.858GB RSS versus CGAR's32GB limit.

These counts provide a diagnostic for the fractional-turn and mixed-start
experiments. Full completed-task throughput and late-run behavior remain the
acceptance criteria; a smaller reversal count alone is insufficient.

[Comparison and verification](results/turn-reversal-audit-v1/comparison.json),
[all reports and provenance](results/turn-reversal-audit-v1/),
[cases](turn-reversal-cases.json), [analyzer](decompose.cpp), [driver](decompose.py).
