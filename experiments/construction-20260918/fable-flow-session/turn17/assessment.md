# Local assessment of Fable turn17

Successful same-session Fable5.1/max read-only review completed
2026-09-19T23:17:38.556153UTC. Fable ran no tools or tests. The root callback change
was inspected and found consistent with its two callers; actual regression and
same-core equivalence evidence is local, not Fable evidence.

A revealed-chain assignment price differs from both the failed five-step next-errand
planner score and the prior spatial-table refinement. Fable recommends testing the
relaxed oriented chain first and deferring first-pickup heading coupling. Its claim
that age largely fixes the completed mix is a hypothesis, not a measured bound.

A mixed covered/uncovered basis can systematically discount missing-table tasks.
Ratio imputation may reduce the mean mismatch but cannot remove selection bias:
coverage depends on destinations, chain length and cache lifecycle. It is a possible
ablation, not a mathematically required fix. The proposed oriented excess should
first be examined against realized loaded time on held-out seeds, with publication
and completed-task censoring recorded. Predictive improvement would not establish
causal throughput improvement, and absent prediction would weaken, not disprove,
the policy hypothesis. No assignment policy has been implemented yet.

Fable's tests need two qualifications. Adding necessary turns can change assignments
even with uniform tolls, so ON and native OFF need not agree. Unit-scaling invariance
holds only when all relevant costs and surcharges scale proportionally and rounding
is accounted for. Its fair-admission robot-invariance claim applies to a fixed task,
fixed available robot set and the independent chain term A: it does not apply to B's
robot-dependent heading regret or to runs with earlier changed assignments.

All complete forward-field headings can provide first-pickup coupling at W=1;
reverse-to-first tables cannot. For W!=1, service-boundary rotation weighting is
an intentional modeling choice. Repeated-location service still takes simulator
ticks even though both old/new distance estimates omit them. Swapped field buffers
are safe only after all worker completion and with no retained stale aliases.

A full-field reconstruction from saved trajectories would be an offline measurement,
not a source of future knowledge in planning. Reuse the production flow publisher,
verify its logged publication counters, bound the sampled task set deterministically,
and run the replay/shortest paths on GRID. A/B/A and full V50 verification remain
separate efficiency evidence. No speedup beyond the measured screen is assumed.
