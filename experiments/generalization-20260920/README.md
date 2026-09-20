# Generic solver transfer to the other nine instances

Requested by the user while warehouse optimization continues. GRID job8899301
completed for all nine other MR24 instances concurrently, with one planner seed0.
This is an initial transfer check, not multi-seed repeatability evidence.

The same verified generic warehouse configuration is used unchanged. No trick
flag, map-specific policy or new experimental score is enabled. Exact source:
[abbe36b](https://github.com/fywu85/lorr/commit/abbe36b723bb43de2478c07bcb88621d5238a23a).
All26production/test hashes and the frozen executable were checked before
submission. The build manifest's older dirty base is not used as the exact source.
[Source binding](source-provenance.json), [settings](variants.json).

| Instance | Robots | Full steps |
|---|---:|---:|
| SORTATION |10000|5000|
| CITY-01 |1500|3000|
| CITY-02 |3000|3000|
| GAME |6500|5000|
| RANDOM-01 |100|600|
| RANDOM-02 |200|600|
| RANDOM-03 |400|800|
| RANDOM-04 |700|1000|
| RANDOM-05 |800|2000|

Each process receives four distinct physical cores,36total, on a shared
EPYC9354 host. The development scheduler+planner entry limit is5seconds,
preprocessing30seconds, process RSS32,000,000,000bytes. The scheduler requests
288GiB aggregate virtual-memory allowance so its limit does not preempt the
per-process32GB check. Full job wall limit is2hours. No partial score is accepted
after a timeout or memory failure.

The original exclusive-host jobs8899287/8899288 were cancelled before execution
when the user relaxed the resource policy; their receipts remain in
[exclusive-request](exclusive-request/). The replacement uses the same binary,
settings and full horizons. [Submission](submission.json), [status](status.json).
Raw output stays under `runs/cgar-generalization-other9-v54-shared-20260920` and is
excluded from Git. Shared-host results do not certify the competition1s budget.

A later32-slot warehouse trial failed to receive its requested binding and briefly
overlapped some reserved cores on this host. That trial was cancelled and the
launcher now rejects such allocations before simulation. The transfer runs retain
this CPU-contention note under the user's shared-host authorization; fixed work
and explicit failures are preserved.

Seeds change planner tie-breaking, search exploration and annealing. Maps, starts
and task-file order stay fixed. Task reveal times and learned fields can change
indirectly through different executed decisions. The comparison therefore tests
the existing warehouse-selected parameters on new instances; it does not establish
that the parameters were selected independently of warehouse performance.

Analysis job8899303 completed on2026-09-20T05:38:51UTC and verified source/binary
hashes, unchanged settings, generic activation, physical core allocation, full
complete-entry limits and action accounting for every accepted result. Invalid runs retain their failure status and no accepted task score.
[Analysis submission](analysis-submission.json).

Seven instances finish validly under the shared-host5s development budget.
GAME times out at342 (5003.714ms,
temporal_region_repair); RANDOM-05 at41 (5000.342ms, same stage). No partial
quality scores are accepted. [Full results and timing](results/summary.md),
[verification](results/verification.json).

This is mixed transfer of the unchanged warehouse configuration. RANDOM-04's
mean entry is3.070s and maximum4.646s, so the warehouse work allocation does not
meet competition timing there. All measured process RSS values remain under32GB.
Other-map tuning is not applied in this initial transfer check, and there is no
new SoTA or six-seed claim.
