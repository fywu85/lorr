# Deferred candidate selection stores (component experiment)

The production TemporalPibt search writes selected_[r] for every considered path,
including paths rejected by reservation conflicts or visitation/random gates. The
prototype passes the candidate path directly to the blocker query and writes the
selection only immediately before reserving the accepted/recursive path. A robot
being built has already had its own reservation removed. Reservation changes,
recursive order, random draws, score arithmetic, candidate order, work budgets and
deadline checks remain unchanged. All unsuccessful returns still restore the old
selection. The patch stays outside production pending measurement.

The existing component driver covers cold global search, region-filtered search,
unequal-weight valid warm starts, and region-filtered search with real deadline
callbacks. Two fixture seeds and eight alternating A/B pairs per mode verify plan,
score-bit and work-counter checksums plus independent vertex/edge validity. Both
binaries use identical compiler settings on the same bound physical core. Process
CPU and wall time are recorded separately on the shared host. This establishes
component equivalence/timing only, not warehouse quality or an entry speedup.

Probe8899794 completed128observations with exact checksums/work and0.71–1.55%
lower median paired processCPU across eight settings. See
[full evidence](../results/deferred-selection-probe-v93/summary.md). The patch now
moves to production regression and exact warehouse timing validation; no complete
solver speedup or new throughput score is claimed from this component result.

V93 sourcebdf5e7a and frozen binaryc49f8cce pass the complete regression suite,
all28source/test hashes, the exact800-step generic control and native same-core
A/B/A200-step trajectory plus work-counter equality. The latter measures wall
+0.190% andCPU+0.117% against mean brackets, so establishes no solver speedup.
The component gain is not resolved at entry scale in that prefix. Test-only
instrumentation proves all eight fixtures actually revisit construction ancestors,
with identical output and counters. See [warehouse timing](../results/deferred-selection-timing-v93/summary.md)
and [ancestor coverage](../results/deferred-selection-ancestor-v93/coverage.json).
The next full quality matrix will require exact frozen control trajectories.
