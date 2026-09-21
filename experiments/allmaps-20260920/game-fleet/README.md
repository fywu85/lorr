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

The complete regression suite passed at source f644acc. The first full benchmark
uses the newer15574 adapted-field/squared-chain/dispatch control, uniform2750, tabu2750,
and tabu4000, same seed0 and remaining settings. Use5000steps, strict1000ms,
four bound physical cores per run, and32decimalGB RSS. The control must reproduce
its prior whole trajectory. The verifier independently reconstructs the exact
mask from archived starts and the committed asset, and checks every actual
schedule of excluded robots for zero positive assignments. Keep losses and timeouts; never accept partial scores.

Full seed0 results: control15574, uniform2750=21742, tabu2750=21648,
tabu4000=17771. All pass strict1s; uniform peak835.91ms and10.562GB RSS.
Whole control matches the prior build. Uniform2750 disables3750 task recipients,
but all robots remain movable. Independent masks and excluded schedules pass.
The1000-step prefix incorrectly predicts a loss; full5000steps are needed.
See[verification](../results/game-fleet-full-v1/verification.json).
Nearby2000/2250/2500 sizes and paired planner seeds2/4 are now predeclared.

## Full follow-ups after the 3,250-robot result

The selected fleet reaches 23,977 / 23,917 / 23,888 tasks on planner seeds 0/2/4.
All complete the full 5,000 steps below one second per entry and exceed the
published NMS target of 23,274. This is not a matched local NMS comparison.
Fleet limits 3,125 / 3,375 / 3,625 score 23,556 / 23,938 / 19,642 on seed0; the
3,250 control repeats its exact earlier trajectory. Keep the selected limit.

Two separate matrices now use this stronger base. One tests geometric horizon
admission, with no margin / measured mean / p90 margin, against no known horizon.
The explicit 5,000-step horizon is a trick; bounds come only from revealed tasks,
and held/started tasks are preserved. The other tests pickup weights8/12 and
matching every step separately. Both retain full horizons, fixed search work,
four bound physical cores and strict1000ms; verify the exact control trajectory.
