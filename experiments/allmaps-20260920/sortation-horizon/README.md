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


Completed strict full results: control150894, mean153963, p90margin154160. Selected154160 gains2.1644% over the control and0.9469% over publishedmax152714. Its maximum is986.967ms; the mean arm peaks995.886ms. All runs pass, and the control exactly repeats its prior full trajectory. This is one-seed evidence. Preserve the declared horizon assumption and report narrow runtime headroom. [Results](first-results.json).


Next declared comparison before submission: current2Mglobal candidate allowance versus1.5M and1M, retaining all regional search, p90horizon scheduling,8physicalcores and every other setting. Full5000steps/seed0. This explicitly changes fixed search work to test throughput/latency tradeoffs; it is not a timing-dependent cutoff. Earlier no-horizon1M lost only49tasks against2M and hadmoreheadroom. No assumption of an identical effect with horizon scheduling.
