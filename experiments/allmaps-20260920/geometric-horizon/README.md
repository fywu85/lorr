# Explicit known-horizon selection on maps with pockets

`CGAR_TRICK_HORIZON_MANHATTAN=1` requires an explicit `--trick INSTANCE` and a
positive `CGAR_TRICK_KNOWN_HORIZON`. It selects an obstacle-free physical lower
bound: sum the Manhattan distance of every remaining leg, with at least one
simulator tick per service stop. Ignoring obstacles, turns, other robots and
certificate restrictions makes this a lower bound even when the core excludes
pockets. The existing cached spatial bound remains the default and still rejects
restricted cores. No task stream or future unrevealed task is inspected.

The existing horizon scheduler uses this bound only to rank new assignments.
It does not abandon held or started tasks; oldest admission is unchanged, and
if all candidate tasks are impossible it still assigns one. Past the configured
end it resumes ordinary ranking. Optional completion margins use only observed
single-holder completions; a percentile margin is an empirical heuristic, not a
physical guarantee. Reassigned tasks remain excluded from the training sample.
The matching horizon guard and read-only pickup audit use the same selected bound.

Validation includes independent orientation/action BFS with obstacles, repeated
services and partially completed chains, plus real700-robot RANDOM04 actions.
Explicit zero and a distant horizon must preserve the complete reference trace.
The original pocket restriction must still reject the unmodified spatial mode.
Complete regression and measured throughput results are required before promotion.

After building, compare the selected profile against physical-bound-only, learned
mean-margin, and learned90th-percentile-margin variants on RANDOM04/05. Use each
case's full known horizon, strict1000ms entries, four physical cores,32decimalGB.
These are explicitly declared tricks. Keep any throughput or runtime failures.
