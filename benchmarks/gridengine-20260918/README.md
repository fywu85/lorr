# Reserved GRID evaluation — 2026-09-18

GRID uses Grid Engine: `/opt/n1ge/bin/lx24-amd64/qsub`, `qstat`, `qconf`, and `qacct`. The tested CPU queue is `debian.q`, with the `threaded` parallel environment. Its `$pe_slots` allocation rule places all requested slots on one host. `-binding linear:N` binds N physical cores, including their SMT siblings; the runner selects one logical CPU from each distinct physical core. `exclusive=true` prevents other scheduled jobs from sharing that host.

The interactive host exposes 24 CPUs but imposes a shared one-CPU account quota. A reserved job ran outside that quota: ten separately pinned workers consumed 30 CPU seconds in 3.015 wall seconds, equivalent to **9.95 concurrent cores**. See [measurements](cpu-probe.json) and [allocation metadata](cpu-metadata.json). This demonstrates genuine concurrent CPU execution, not merely ten processes sharing one CPU.

CGAR's current scheduler and planner compute serially, so the default allocation is **one physical core per concurrent instance**. The simulator creates a worker thread but does not parallelize CGAR's algorithm. The archived [competition specification](../../cgar/Evaluation_Environment.md) allowed 32 logical CPUs (16 physical cores with SMT) and 128 GB RAM per submission. That is an available resource ceiling; this smaller allocation is sufficient for the measured implementation. The GRID CPU model differs from the competition VM, so these are not identical-hardware performance claims.

```sh
python3 tools/benchmark_gridengine.py --output runs/cgar-grid-new --jobs 10
```

The launcher snapshots the executable, records source hashes and explicit configuration, checks CPU topology and quota, and pins each run to a separate physical core. It submits with `-pe threaded 10 -binding linear:10 -l exclusive=true,h_rt=01:00:00,h_vmem=8G`. Grid Engine multiplies the memory resource by slot count: the earlier ten-slot 4G request produced a 40 GiB process address-space limit. The current 8G-per-slot default requests 80 GiB for ten slots, not a separately enforced 8 GiB cap per native instance. The measured full-run peak RSS across the study was 6.24 GiB; a separate single-slot job with an actual 8 GiB address-space limit completed SORTATION successfully. Per-instance peak RSS is now measured with Linux `wait4`. Resource reservations do not eliminate clock-frequency variation or all shared-system effects; successful strict-deadline runs are checked for trajectory repeatability.

## Unchanged production baseline on reserved cores

Before changing the fallback or deadline contract, the original production binary (`653b4f3f5247e50ee1829559aa117b01476ef914e42571f0fa51426795d0c5f3`) ran all ten archived horizons concurrently on ten physical cores. All finished with zero planner errors, schedule errors and simulator timeouts. The suite took about 175 seconds. This isolates an unchanged-code comparison on better provisioned hardware; it is not a code-improvement result.

| Instance | Completed tasks |
|---|---:|
| RANDOM-01 | 457 |
| RANDOM-02 | 574 |
| RANDOM-03 | 645 |
| RANDOM-04 | 634 |
| RANDOM-05 | 1032 |
| GAME | 5848 |
| CITY-01 | 6053 |
| CITY-02 | 6637 |
| WAREHOUSE | 30600 |
| SORTATION | 37965 |

[Results](isolated-original/summary.json) · [Binary and source provenance](isolated-original/spec.json) · [Allocation](isolated-original/allocation.json)

RANDOM-05 retains the existing capacity restriction: 31 parked robots, 769 active robots, and exclusion of tasks with outside-core stops. The [subsequent matched strict-deadline study](../../experiments/throughput-20260918-strict/README.md) measures the fallback repair, CPU/RAM use, repeatability and shorter evaluation horizons.
