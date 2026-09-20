# V60 scheduler publication regression

The final R3 build passes all regressions, including20 controlled scheduler cases
across both native caches, two metric scales and all five pricing modes. Executed
moves change the published field; goal-free publication flushes resident tables,
all active modes use unchanged native prices while uncovered, and normal planner
preparation rebuilds quotes against the new field. Scheduling builds no tables.
Production sources and the executable are byte-identical to V59.

The first two builds failed a test-fixture assumption: holding the robot still
preserved its movement commitment, so the second goal's table was not prepared.
R3 executes the committed move before changing that goal. Those failed source
snapshots and test logs are retained in v60 and v60-r2; they are not solver failures
or benchmark results. No production change was needed.
