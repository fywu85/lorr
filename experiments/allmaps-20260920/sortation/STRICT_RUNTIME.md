# Strict Sortation qualification with eight cores

The full150353-task native-field record averaged525ms but peaked1152ms on four
cores under the five-second development limit. That is not yet a1s result.
Keep every fixed search work limit, ordering, scheduling policy and field fixed.
Raise candidate preparation and full-pickup worker counts from4 to8, reserve
eight physical cores (within the competition32-core budget), and enforce1000ms.
The5000-step full run must complete and its whole trajectory must equal150353
before calling this a pure runtime qualification. No timed partial work.

This uses the original df94a520 frozen source/binary rather than a new solver
variant. The matrix verifier now requires the explicitly declared core count,
default4 or8, preserving affinity, no-quota, no-overlap and32decimalGB checks.
Shared hosts remain allowed. One full run first; a timeout stays a failed run.

The eight-core preparation run failed at timestep0 after1000.289ms in
`temporal_repair`; no throughput accepted. More preparation threads alone do
not qualify the record. The failure is preserved in results/sortation-strict-prep8-v1.
Next screen200steps with fixed global candidate budgets2M/1M/500k/250k, retaining
all regional work. This is a latency screen only: never compare its truncated
task counts with full competition throughput. Select the largest budget that
passes with useful deadline headroom, then run the complete5000steps. Lower
fixed work is declared before each run; timeout behavior remains an error.

The full strict runs are complete: global2M=150333, max972.92ms,mean452.13ms,
RSS12.46GB; global1M=150284,max887.27ms,mean340.66ms. Both use8physical cores,
full5000steps, no errors/timeouts and fixed regional work. Select2M for strict
throughput; retain150353 as the slower5s development maximum.
[Full verification](../results/sortation-global-strict-full-v1/verification.json).

Next full strict experiment transfers the two useful scheduling patterns:
age/HRRN pickup8 plus64-group matching (Warehouse-like), and direct pickup4/8
plus64-group matching (CITY/RANDOM-like). The150333 control retains identical
settings. Four runs in parallel, eight physical cores each,64scheduler slots
for32physicalcores and128GiB total reservation; each process still32decimalGB.
Any step timeout is failure, never an accepted partial throughput score.
