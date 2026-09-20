# Late rematching can worsen individual cutoff bounds

Both verified best-profile trajectories are independently replayed unchanged. This is a diagnostic, not a new throughput result.

| Visible holder change | Seed 0 | Seed 2 |
|---|---:|---:|
| Observed task retargets | 15803 | 15725 |
| Physical bound fits → cannot fit | 47 | 63 |
| Physical bound cannot fit → fits | 35 | 44 |
| Rescued-bound tasks subsequently completed | 21 | 14 |
| Diagnostic p90 tier worsens | 148 | 167 |
| Diagnostic p90 tier improves | 168 | 180 |

All boundary crossings occur in the final 1,000 steps. A fitting lower bound does not guarantee a feasible timed route or completion, while a bound beyond the remaining horizon rules out completion for that unchanged holder. No worsened-to-impossible task completes. Other changes rescue tasks, so these counts cannot be added up into the benefit of a new policy. Rejecting a whole permutation cycle can remove its beneficial transfers too.

The diagnostic p90 model uses complete spatial BFS at first admission and only earlier single-holder completions. Its estimate differs from production cached/Manhattan quotes. Some worsened-tier tasks still complete (62 /68); the tier is a heuristic. Every comparison uses the old holder at the instant of reassignment, not its first-admission position. Fresh permutations within a single scheduling entry are unobservable here.

The corresponding physical-work replay finds only466,086 /466,358 loaded forward moves above completed chains' shortest spatial distances. Completed loaded work also includes877,922 /877,274 turns and1,127,938 /1,133,208 waits. These are different descriptive components, not wholly recoverable work. The dominant remaining overhead therefore does not consist of arbitrarily long loaded detours.

Seven targeted fixtures and all existing physical-work fixtures pass; both complete 50-million-robot-step accounts match their frozen reference totals and raw hashes. One reserved GRID core, no CPU quota. Completed 2026-09-20T19:39:18.088495+00:00, job 8900541. No production change is part of this audit.

Decision: test a separately gated, default-OFF whole-cycle cutoff-tier veto against exact current-profile controls. Preserve ordinary fairness and the one-retarget limit. Full benchmarks, rather than these offline counts, decide whether to keep it.

[Accounting](accounting.json), [fixtures](fixtures.json), [source/input hashes](request.json), [protocol and limitations](../../match_horizon/README.md).
