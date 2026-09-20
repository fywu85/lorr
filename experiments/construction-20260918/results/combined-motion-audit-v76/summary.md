# TRICK: movement accounting for the148k combination

All six saved full runs are replayed, accounting for300million physical robot
steps. Both independent fixture suites and field-mismatch rejection pass. Input
and trajectory hashes match the verified V72 runs. This replay adds no solver run.

| Policy / seed | Tasks | Completed shortest chain | Empty steps/task | Loaded overhead/task | Loaded turns/task | Loaded waits/task |
|---|---:|---:|---:|---:|---:|---:|
|trick_score0_match1-s0-r0|147328|242.252|65.653|21.891|5.929|11.185|
|trick_score0_match1-s2-r0|147204|242.265|65.741|22.025|5.935|11.284|
|trick_score1_match0-s0-r0|147422|242.223|65.781|21.624|6.637|10.906|
|trick_score1_match0-s2-r0|147502|242.251|65.678|21.528|6.612|10.885|
|trick_score1_match1-s0-r0|148132|242.275|64.368|21.396|6.566|10.829|
|trick_score1_match1-s2-r0|148101|242.292|64.401|21.416|6.564|10.834|

The combined policy has nearly the same completed-chain length as either control.
Relative to score-only, empty work per task falls about1.3–1.4steps and loaded
overhead falls0.228/0.112steps. Relative to matching-only, loaded overhead falls
0.495/0.609steps. Lower detours/waits coexist with more turns than matching-only.
The measurements are consistent with complementary assignment and routing effects.
They do not causally allocate the throughput gain, because cohorts and traffic
change together. Short-task preference remains OFF.

[Accounting and input hashes](accounting.json), [full quality validation](../trick-score-match-full-v72/summary.md).
