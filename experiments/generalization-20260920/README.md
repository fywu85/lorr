# Generic solver transfer to the other nine instances

Requested by the user while warehouse optimization continues. GRID job8899287
is queued for all nine other MR24 instances concurrently, with one planner seed0.
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

Each process receives four distinct physical cores,36total, on an exclusive
EPYC9354 host. The complete scheduler+planner entry limit remains1second,
preprocessing30seconds, process RSS32,000,000,000bytes. The scheduler requests
288GiB aggregate virtual-memory allowance so its limit does not preempt the
per-process32GB check. Full job wall limit is2hours. No partial score is accepted
after a timeout or memory failure.

Initial submission found no immediately available exclusive host. The accepted
retry queues the identical resource request with immediate-availability checking
disabled; isolation/binding/deadlines remain. [Submission](submission.json),
[status](status.json). Raw output stays under
`runs/cgar-generalization-other9-v54-20260920` and is excluded from Git.

Seeds change planner tie-breaking, search exploration and annealing. Maps, starts
and task-file order stay fixed. Task reveal times and learned fields can change
indirectly through different executed decisions. The comparison therefore tests
the existing warehouse-selected parameters on new instances; it does not establish
that the parameters were selected independently of warehouse performance.

Analysis job8899288 is held behind the benchmark. It will verify source/binary
hashes, unchanged settings, generic activation, physical core allocation, all
complete-entry limits and action accounting before producing the per-instance
report. Invalid runs retain their failure status and no accepted task score.
[Analysis submission](analysis-submission.json).
