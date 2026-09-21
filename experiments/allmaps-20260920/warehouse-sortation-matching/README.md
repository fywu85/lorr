# Warehouse and Sortation matching-width check

Declared before runs on 2026-09-21. Selected trajectories spend18.64%/25.09% of
robot actions before pickup; that does not establish avoidable travel or a gain.
Test bounded assignment groups64 with32 groups against32 with64 groups: at most
2048 participants in either case. Keep existing candidate discovery, robot/task
protection, cadence, finite retarget budget, acceptance and solver work unchanged.
This comparison lost or tied on RANDOM/CITY; it has not been tested on these
large structured maps. It is a measured transfer hypothesis, not a claimed gain.

Both selected profiles require their explicit `--trick WAREHOUSE/SORTATION` flags.
Use seed0, all5000steps,10000robots,8bound physical cores per case, strict1000ms
complete entry and32decimalGB. Node-local logs are preserved after exit. Source
311d04800c369f227407b63641c1c4f0a7102186 is already fully qualified; unrelated new
features remain absent. Controls155173/150894 must reproduce full trajectories.
Sortation's selected control previously peaked at993.90ms, so a strict timeout
will be recorded as failure without accepting a partial run. Full inputs are
hashed, including Warehouse's newly declared single-instance manifest.

Both maps now have verified full results. Warehouse155173control/154829wider;
Sortation150894control/150863wider. All valid, and both controls repeat complete
selected trajectories. Keep current matching width32. Warehouse verification
needed two parser corrections for a disabled candidate cap; both failures and
the unchanged solver artifacts are preserved. No solver rerun was performed.
