# Neutral-tail startup and activation checks pass

Both 200-step / 10,000-robot runs pass the shared 5-second deadline and 32 decimal GB limit on disjoint four-core allocations. OFF reproduces the previous complete startup trajectory exactly (`b9fef3086ca4d7146f1e79f8bd7c43585ebef7a05598e2f65216404c650fa2b1`). ON changes the trajectory and reprices 993,923 of 1,611,455 goal-serving candidate evaluations. The configured 5,000-step horizon does not affect these startup decisions.

Counts are 3,853 / 3,851; these are feasibility checks, not a full-run quality ranking. All 28 source/test hashes match `e757965ad5869b9004e63da4c0bab52ecdb64549`; binary `c7f7b32063284b42c4cf50894df983b7d1af3489b2f009854b092a270430a0b4`. Validation completed 2026-09-20 17:23:01 UTC.

The [generic 800-step check](../native-neutral-tail-generic-v101/comparison.json) also passes exactly: 18,958 tasks with the unchanged reference trajectory. The full ON/OFF comparison uses the newly verified pickup-weight-8 baseline, while this startup screen used weight 5. All other scoring components remain fixed within each comparison.

[Screen verification](comparison.json), [configuration and semantics](../../tricks/neutral_tail/README.md).
