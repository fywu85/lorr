# Reservations after service

CGAR_TEMPORAL_SERVICE_AUDIT_STRIDE defaults to0 (disabled). A positive stride up to
4096 requires temporal planning and samples the final complete reservation table.
The const audit consumes no RNG, changes no reservations, and performs no table
lookups, speculative search or policy changes. It checks the existing deadline;
a missed deadline remains an explicit failure of the complete entry.

For every selected path, the first slot at its current goal is its predicted service
slot. Service occurs at most once per robot per physical tick in TaskManager. Slots
strictly after that first hit are called post-service reservations. They are observed
planned reservations, not a claim that the owner can or should vacate them.

Every ordinary positive-weight goal-bearing robot is eligible, including robots
whose selected first action moves forward. Candidate gain uses distance/turn cost
after removing the operation-index term and must reach one full scaled unit. This
is a heuristic objective, not elapsed travel time. A candidate can satisfy this
condition while losing under the full native scalar score because of its op term.

For each eligible robot, the audit chooses the cheapest candidate in the easiest
blocker class: free, only post-service conflicts, or another conflict. Equal cost
uses operation index. The classes plus no improving candidate partition all eligible
robots. Self reservations are excluded; repeated conflicts do not multiply robots.
A conflict in the owner's service slot, a pre-service conflict, an owner with no
service in the window, or any fixed owner goes in the other class. Cell and edge
reservations are both checked.

Post-service-only candidates are split by their owners: all have a visible next
errand, all finish their task, or both types occur. The selected search action is
split into wait/turn/forward, and the candidate's first-forward flag and physical
objective gain are recorded. Search actions precede any occupancy-preserving final
seed-turn projection; these are not counters of executed actions. Different robots'
gains can overlap and cannot be added as recoverable throughput or score bounds.

## Validation and current runs

Build8899006 passes every CGAR regression. Thirteen hand-counted fixtures cover
known/finishing/mixed owners, service-slot exclusion, fixed owners, edge-only and
repeated conflicts, a free non-best candidate, and each selected-action category.
Other checks cover own reservations, nine unit/tie boundaries, invalid settings,
explicit interruption, unchanged subsequent RNG/search and4,800 protected
serial/parallel production decisions. The248-line patch reconstructs all22 recorded
source/test hashes independently. [Build and reconstruction](build-provenance/v45/).

Screen8899012 compares disabled and stride67. Both complete200-step trajectories
exactly match the prior V44 reference. Both are valid; maximum879.95ms and
peakRSS5.193GB. The enabled case emits exactly the67/134 frames; all robot/action
partitions conserve counts. This is a timing/equivalence check, not a full-run
prevalence estimate. [Screen and assertions](results/service-audit-screen-v45/equivalence.json).

Full8899017/analysis8899019 samples seeds0/2 with stride67, four disjoint physical
cores per process,32GiB aggregate reservation and32decimalGB per-process cap.
Stride67 samples74 frames over5,000 steps and is coprime to the512-step flow cycle.
Both full trajectories must exactly reproduce the confirmed reference before
interpreting the counts. No continuation policy is enabled.
[Configuration](service-audit-full-variants.json).
