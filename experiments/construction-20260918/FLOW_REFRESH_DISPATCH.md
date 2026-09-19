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


## Full comparison complete

All eight runs and independent analyses pass. Both ordinary controls exactly
reproduce previous full trajectories. Direct-cost assignment scores131,199 /
130,947, mean131,073, losing2.559% against the134,515 control mean. Its final
1,000-step counts fall to26,695 /26,652, versus28,215 /28,214. HRRN remains on.

Refinement gives the exact same full trajectories as unrefined direct cost on
both seeds. With HRRN it scores134,508 /134,519: just three fewer tasks on
seed0 and an exact seed2 trajectory. This does not support chain-estimate
refinement as a throughput improvement. Fable's predicted positive direct-cost
effect is falsified in these two refreshed-flow runs. A separate travel audit
will distinguish task-mix and movement changes without claiming causal additivity.

[Full scores, task ages, motion and equivalence](results/flow-refresh-dispatch-full-v33/comparison.json).


## Travel explanation

The independent travel audit8898716 passes its fixture and full replay. Direct
cost reduces completed-chain shortest distance to235.265 /235.270, versus
242.324 /242.320 with HRRN. Completed loaded elapsed time falls to279.193 /
279.163 steps, versus284.622 /284.599. However, total empty robot-steps rise
to12,049,483 /12,115,340 from10,298,959 /10,302,897: about17.0% /17.6% more.
Different completed cohorts prevent treating these as an additive causal split,
but the empty-travel increase motivates testing the existing pickup weight3/5
under refreshed flow. This is a global objective parameter, not a map rule.

[Independent travel evidence](results/dispatch-travel-v1/).


Pickup-weight screen8898719 passes HRRN1/weight1 and direct-cost weights1/3/5.
Both weight1 controls match their previous fingerprints. Max entries are
0.7800 /0.7748 /0.7870 /0.8083 seconds. Full matrix8898721 /8898722 runs all
eight seed0/2 cases concurrently on32 physical cores under128 GiB aggregate
reservation. Each planner has four disjoint cores and individual1s/32GB checks.
[Pickup profiles](flow-refresh-pickup-variants.json),
[pickup screens](results/flow-refresh-pickup-screen-v33/).


The full pickup-weight matrix8898721/8898722 is also complete. All eight cases
are valid; all four weight1 control trajectories exactly reproduce the preceding
factorial. Direct-cost weight3 gives134397/134194 (mean134295.5), still0.163%
below the HRRN baseline. Weight5 gives136149 on seed0 but collapses on seed2
to52444, including only4447 tasks in the final1000 steps. Both are rejected.
The seed0 peak is retained with its completion timestamp and exact v33 source
commit in WAREHOUSE_PROGRESS.md; it is not a robust leading policy.
[Full pickup evidence](results/flow-refresh-pickup-full-v33/comparison.json).
