# Warehouse winner on two additional solver seeds

Full seeds1/3 complete154713/154776tasks, respectively82/19below NMS154795.
Both5000-step/10000-robot runs are independently verified:10000complete entries,
zero errors or timeouts, all28source/test hashes, exact frozen binary, disjoint
physical CPU bindings and32decimalGB RSS. The configuration is unchanged from
the winning mean-margin profile and requires --trick WAREHOUSE. These are CGAR
random seeds on the same fixed input, not additional generated map/task instances.

| Solver seed | Tasks | Final1000 | Oldest never assigned at end | Oldest unfinished |
|---|---:|---:|---:|---:|
|1|154713|33790|190|1090|
|3|154776|33785|185|1076|

Every task revealed by2500 completes by5000 in each run. No task is reassigned
after pickup; at most one unopened retarget occurs. Maximum observed reveal-to-
first-assignment waits are692/697steps. Final never-assigned ages are snapshots,
not historical maximum waits; finite runs do not prove starvation freedom.

Mean entries452.148/441.954ms; maximum1.017122/1.000873s; peak RSS11.918/11.947GB;
average CPU1.495/1.509cores of4reserved. Wall38.72/37.84minutes. Shared5s
development, not strict1s competition certification. No new best or policy change.

Source bdf5e7a145626f87d0321eb8df8ab08b9ec3cb4f, binary
c49f8ccef14674ff718b0e94fa0f4c94ddf16c404fe37112eb20a449a6a21c6b.
This later implementation defers stores for rejected search candidates. Its
full same-seed controls are being checked in the separate V95 work matrix.

[Full verification and timestamps](verification.json), [waiting audit](fairness.json),
[exact configurations](spec.json), [independent unfinished-work replay](../native-frontier-seeds13-v95-outcomes/accounting.json).
