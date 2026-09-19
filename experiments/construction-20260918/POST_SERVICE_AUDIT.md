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

Full8899017/analysis8899019 is complete. Both full trajectories exactly reproduce
the confirmed V42/64 reference:141829/142988 tasks. All10,000 complete entries are
valid and<=1s; max884.17ms, peakRSS11.697GB, disjoint four-physical-core allocations,
no CPU quota. All22 frozen source/test hashes match exactc339e8b. Stride67 yields
74 frames per run, coprime to512; all robot/action partitions conserve counts.

| Observed candidate class | Seed0 mean robots/frame | Seed2 mean robots/frame |
|---|---:|---:|
|Only post-service owners with a known next errand|8.47|8.41|
|Only post-service owners finishing their task|23.88|22.80|
|Mixed post-service owner types|0|0|
|At least one other conflict|2,347.22|2,237.19|
|Unblocked qualifying candidate|394.11|375.55|

After step1,000, the known-next class averages7.32/7.70 robots per sampled frame,
about0.073%/0.077% of eligible observations and0.306%/0.326% of observations with a
qualifying improvement. Known-next arrivals in selected plans average160.5/161.9.
This measured class is small and tempers expectations for this specific mechanism.
It is not an upper bound on a different joint objective, which can also change arrival
headings or earlier decisions. The larger finishing-owner class has no revealed
next errand to use. Counts repeat robot observations across sampled frames.

Changing the score also changes which candidates qualify as improving, so comparing
this audit across different objectives would not isolate occupancy changes by itself.
No production continuation policy has been enabled. [Full audit, frame data and
validation](results/service-audit-full-v45/audit.json).
