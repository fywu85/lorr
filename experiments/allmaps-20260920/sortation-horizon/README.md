# SORTATION known-horizon transfer

Declared before submission on 2026-09-21T13:31:58.969426+00:00. Current SORTATION uses ordinary
HRRN admission and no known horizon. WAREHOUSE improved with a configured run
length and observed task-duration residual margin. Test whether that transfers:
control versus configured5000horizon with mean versus90th-percentile residual
margin, both also applied to bounded unopened matching. Keep the routing field,
pickup weight8, HRRN, matching width32/groups64 and every fixed planning limit.
Existing held/started tasks and CGAR protection remain authoritative.

No map geometry or task-stream fitting is added. This is an explicit
--trick SORTATION horizon assumption and must remain labelled as such. Source
6fc0cc50 is already qualified; native SORTATION asset unchanged. Full5000steps,
10000robots, seed0,8bound physical cores per arm, strict1000ms complete entry,
32decimalGB RSS, local log spool. Control150894 must reproduce its whole trace.
Its previous max994ms is close to the limit: any timeout is retained as failure.
Replicate improvements before claiming a seed-robust effect.
