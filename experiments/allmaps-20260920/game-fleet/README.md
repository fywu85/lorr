# Explicit GAME task-admission fleet selection

This adapts the active Kitty Knight LNS/LaCAM2 GAME configuration's nominal
2750-robot fleet and `tabu_locs` exclusion set. It is a map-specific trick,
requiring `--trick GAME` and the exact 6500-robot competition layout.

`CGAR_TRICK_GAME_ACTIVE_LIMIT=0` is the default and preserves existing behavior.
A positive value selects once, at the first scheduler call, which idle robots
may receive tasks. `CGAR_TRICK_GAME_TABU=1` additionally preserves robots whose
initial cells belong to KK's unchanged exemption set. The asset contains 11559
entries and 10882 unique cells; duplicates collapse as in the reference.
The optional cap can be exceeded when exemptions or held tasks require it.

This is a CGAR adaptation. Its independent deterministic SplitMix ordering uses
CGAR_SEED and robot ID, not KK's stateful random shuffle. Every robot remains
present in CGAR's movement and recovery system, including idle displacement.
No previously held or started task is dropped. The selection is not recomputed
when robots move. Temporal reservations, primary protection, recovery, and the
existing oldest-task admission rule remain in force. Fewer task-serving robots
can obstruct traffic; no throughput gain is assumed.

The generator and asset proof record the exact reference hashes. The copied
asset retains Kitty Knight's copyright and MIT license. Initialization receipts
record the requested limit and tabu option; the first schedule records counts,
asset hash, and deterministic mask fingerprint. Missing new receipt components
mean OFF for older frozen binaries, but an active experiment must prove activation.

Regression checks cover CLI/map gating, malformed values, exact set lookup,
deterministic selection, exemptions exceeding the nominal cap, held ownership,
deadline propagation, default-off action identity, stable selection after motion,
and 78000 valid production robot actions on the exact GAME map. Full build and
benchmark proof will be recorded after execution.

First full benchmark: selected rank-based GAME control, uniform2750, tabu2750,
and tabu4000, same seed0 and remaining settings. Use5000steps, strict1000ms,
four bound physical cores per run, and32decimalGB RSS. The control must reproduce
its prior whole trajectory. Keep losses and timeouts; never accept partial scores.
