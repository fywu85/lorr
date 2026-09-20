# Full fresh-pickup audit: exact observation, modest remaining matching opportunity

Both full seeds reproduce **155,120 / 155,056** tasks with audit OFF and ON. Their complete trajectories match the V99 reference, and every real diagnostic line is unchanged after excluding explicit timing fields. All four runs, 20,000 complete decisions and **200 million independently accounted robot steps** pass. This is a validated diagnostic, not a new solver or score.

| Final counter | Seed 0 | Seed 2 |
|---|---:|---:|
| Eligible fresh holders | 153,950 | 153,856 |
| Positive cycles before acceptance/horizon filters | 8,177 | 8,254 |
| Positive raw guidance-cost reduction | 677,962 | 676,534 |
| Cycles passing the original acceptance threshold | 202 | 195 |
| Feasibility-tier edges excluded | 252,249 | 252,567 |
| Guarded, task-disjoint qualifying cycles | **145** | **141** |
| Tasks in those cycles | 343 | 325 |
| Guarded raw guidance-cost reduction | **67,170** | **59,575** |
| Portion observed in the final 1,000 steps | 52,043 | 47,322 |

The qualifying cost equals 3,358.5 / 2,978.75 forward-guidance units after dividing by 20. Most appears near the configured cutoff, so the full run was needed. These are **not physical robot steps, recoverable throughput or a global matching bound**. Grouping is limited, eligibility is protected, and the audit keeps the scheduler's selected task set. No task is reused across its qualifying witnesses. The 460 / 454 missing-field observations concern only eligible holders; V104 counted all newly free robots and therefore had 461 / 454.

The horizon guard matters: it excludes a new holder that would worsen that task's current feasibility tier under the same frozen p90 model. The unfiltered accepted-cost totals (101,755 / 100,023) cannot be described as equally safe opportunities. The audit retains the original four-unit and 10% acceptance margins; relaxing them is a different untested policy.

Ordinary HRRN, forced-oldest admission, held/started protection and the one-retarget cap are unchanged. Every first-half task completes, no initially revealed task remains never assigned, oldest never-assigned ages remain 188 / 193, and no reassignment occurs after pickup. Finite observations do not prove starvation freedom.

Audit-ON entries average 448.297 / 452.574 ms, maxima 0.985063 / 0.991991 seconds, RSS 11.946 / 11.950 GB, about 1.51 CPU cores of four reserved. Shared-host timings vary: they do not demonstrate a speedup from observation, and the OFF seed0 peak is 1.025017 seconds. All satisfy the permitted 5-second development deadline and 32-decimal-GB cap; strict1s certification remains separate.

Source `778ac779aefd3d7f4557e399ea7544b1cebd8cd0`; binary `ef0973f7577b1e10d22d55d76060605f67ec3ee3ffa318ad4d0119fb9a360f15`. All28source/testhashes, 16 disjoint physical cores, no CPU quota and all25cumulative audit samples are checked. Full verification 2026-09-20T19:21:46.787476+00:00; independent accounting 2026-09-20T19:22:23.518503+00:00.

**Decision:** retain the read-only tool and prioritize the separate, bounded five-step matching-cadence trial. This modest conditional residual does not yet justify a new fresh-only assignment pass. Keep the current 155,120 record and ten-step reference unchanged pending measured policy results.

[Full verification](verification.json), [audit samples](work-samples.json), [fairness](fairness.json), [independent work accounting](../fresh-pickup-native-full-v105-outcomes/accounting.json), [physical work](work-budget.json).
