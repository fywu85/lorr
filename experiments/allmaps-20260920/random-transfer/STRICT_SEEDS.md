# Strict dense-map replication

Declared after the full seed0portfolio comparison. Test the one-worker chain-rank
control, eight-worker control and noise50without persistence on both RANDOM-04
and RANDOM-05, planner seeds0/2/4. Same archived task/start inputs, not independent
input streams. Keep the frozen source5d3c5bf binary and all fixed work parameters.
Enforce1000ms per entry,4physical cores per process,32decimalGB on shared
EPYC9354hosts. Full1000/2000steps determine outcomes; failures retain no partial
score. A strict seed0run should reproduce its earlier5s whole trajectory.

Selected prior seed0counts: one worker1249/1847, eight workers1330/1989,
noise50cold1306/2036. Retention variants lost to the selected controls and remain
off. These three configurations test whether the small extra benefit from
noise on RANDOM-05 repeats; do not substitute a new winning seed into a frozen
claim about a mean. Record fairness, but throughput is primary.
