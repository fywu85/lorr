# Physical cores and scheduler slots

Research44 and57 expose64physical cores /128logical CPUs, and their GRID queues
advertise128slots. A16slot job asking for16physical bindings can start even when
fewer than16physical cores remain. GRID then sometimes declines the binding
and exposes all64cores. Our allocation guard rejects these jobs before any run.
Rejected jobs and verifier failures remain in their immutable run directories.

New matrices may explicitly use `--scheduler-slots-per-core 2`. Sixteen physical
cores then reserve32queue slots while still requesting `-binding linear:16`.
With `--memory-gib-per-slot 4`, this reserves the same128GiB total as the old
16slot/8GiB request. Each benchmark still receives four distinct physical cores
and retains the32decimalGB process limit. This accounting does not let the
algorithm use the sibling hardware thread or double its physical-core budget.
The allocation guard still requires exactly the declared physical binding and
checks the larger reservation. Existing specs default to factor1 for replay.

Evidence collectors on these same hosts now reserve two scheduler slots and
bind one physical core, with6GiB per slot (same12GiB total). Their exact one-core
and no-quota checks remain. This addresses logical-slot under-accounting; it is
not a guarantee that GRID will never decline another binding request. Existing
factor1 jobs must still be considered when packing concurrent work.
