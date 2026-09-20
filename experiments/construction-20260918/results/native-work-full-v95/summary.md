# More native search gives small, mixed returns

All six full5000-step/10000-robot runs pass source, trajectory, complete-entry,
resource, fixed-work and fairness checks. Both4M/two-round controls reproduce
the original winning V92 trajectories exactly. Frozen source bdf5e7a, binary
c49f8ccef14674ff718b0e94fa0f4c94ddf16c404fe37112eb20a449a6a21c6b.

| Global candidate cap | Regional rounds | Seed0 | Seed2 | Paired mean | Change vs control |
|---|---:|---:|---:|---:|---:|
|4M|2|154889|154556|154722.5|0|
|8M|2|154870|154762|154816.0|+93.5 (+0.0604%)|
|4M|4|154686|154576|154631.0|-91.5 (-0.0591%)|

The8M ceiling reaches the unchanged1M repair-attempt ceiling in sampled late
entries, typically before8M candidates. It is more work, not literally double
the executed candidates. Individual effects are-19/+206; final1000effects-11/+36.
Its two-seed mean is21above NMS154795, but neither run exceeds the existing
154889record and seed2 remains33below NMS. This small mixed effect does not
establish a robust quality improvement. Retain4M as the working reference.

Four regional rounds double the prescribed regional attempts to400000 per step.
Individual effects are-203/+20, with final1000effects-11/-1. Reject as a default.

Mean entry times are453.8/448.9ms for4M/two rounds,585.5/588.8ms for8M/two rounds,
and515.9/519.3ms for4M/four rounds. Maximums remain below1.284s and RSS below
12.736GB. These shared-host5s development measurements are not controlled CPU
speedups or strict1s certification. Each process has4distinct bound physical
cores; all24cores are disjoint and no CPU quota applies.

Across all six cases, no initially revealed task remains never assigned, every
task revealed by2500 completes, oldest never-assigned ages are184-189steps,
and no task is reassigned after pickup. The one-retarget limit holds. These
finite-run observations do not establish a starvation-freedom theorem.

Independent work accounting finds no decrease in unfinished work for8M: +11854/
+3554robot steps versus control. Cohorts and trajectories differ, so neither
this difference nor the throughput increment is a causal saving decomposition.

[Validation and exact controls](verification.json), [waiting tails](fairness.json),
[prescribed work](work-samples.json), [independent outcome accounting](../native-work-full-v95-outcomes/accounting.json).
