# Assignment choices under refreshed flow

Fable turn04 suggests direct-cost assignment instead of age divided by estimated
cost. Both are existing generic policies. A prior no-flow50k-repair test slightly
favored HRRN,107457 versus107083 on seed0; this experiment tests the interaction
with refresh512. It crosses HRRN0/1 with chain-cost refinement0/1, retaining
pickup weight1, no global samples, independent oldest-task admission and all
planner settings. Refinement can replace cached Manhattan chain estimates when
exact tables become available; it is not an oracle over future tasks.

The four200-step screens all pass. Default exactly reproduces the v30 control.
Refinement matches its own HRRN arm on this prefix, but full equality is unknown.
No prefix-throughput ranking is used.

Full job8898684, analysis8898685: all eight cases (four arms, seeds0/2) run
concurrently on exclusive research46 with32physical EPYC9354 cores, four disjoint
cores per process, and128GiB aggregate reservation. Every planner still has the
32,000,000,000-byte RSS validity limit and complete one-second decisions.

[Configuration](flow-refresh-dispatch-variants.json),
[screens](results/flow-refresh-dispatch-screen-v33/),
[Fable assessment](fable-flow-session/assessment.md).
