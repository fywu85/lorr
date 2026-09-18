# LoRR 2024: CGAR migration

This workspace contains the 2024 League of Robot Runners competition sources for
KittyKnight and No Man's Sky, the main-round evaluation inputs, and a CGAR/PIBT
migration from the KNAPP ground-progress method into the competition start-kit.

- `cgar/`: modified start-kit, CGAR planner and scheduler, and regression tests.
- `kk` and `nms`: relative links to the selected competition submissions.
- `mr24`: relative link to the ten main-round benchmark instances.
- `tools/benchmark_cgar.py`: benchmark runner with source and executable hashes.
- `benchmarks/`: compact benchmark records; full trajectories remain in local `runs/`.

Read [the migration notes](cgar/cgar_planner/README.md) for the recovery witnesses,
persistent task epochs, scheduling changes, and the capacity-mode restriction.
When the fleet exceeds the certified core capacity, outside robots park and tasks
with outside stops are excluded. This restriction matters for RANDOM-05.

## Build and test

The tested Linux environment uses GCC 11.4, Boost 1.74, and C++17. Create a local
Conda environment, then configure and build:

```sh
conda env create --prefix ./env --file environment.yml
source ./activate.sh
cmake -S cgar -B cgar/build -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" -DBUILD_TESTING=ON
cmake --build cgar/build --parallel 4
ctest --test-dir cgar/build --output-on-failure
```

The tests cover graph-certificate premises, pocket evacuation, persistent primary
tickets, task replacement, excess-capacity parking, full 10,000-robot scheduling,
fair admission, sparse fallback quality, cache refinement and eviction, consistent
stall observations, and explicit timeout failures. Simulated actions are independently checked
for obstacles, duplicate destinations, and edge swaps.

## Benchmark

[Full sequential MR24 results](benchmarks/mr24-20260917/summary.md) include all ten instances and per-instance comparisons with the draft. The [ten-way parallel evaluation](benchmarks/mr24-parallel-20260918/summary.md) uses the same production executable and records the effect of sharing the one-CPU quota across concurrent jobs.

[Throughput diagnosis and proposed improvements](experiments/throughput-20260917/README.md) isolate the large-fleet scheduling regression with controlled prototypes and holdout checks. Those diagnostic prototypes were separate from the production planner. A [follow-up Fable 5.1 Max review](experiments/throughput-20260918-fable/README.md) was completed through Claude Code CLI.

```sh
python3 tools/benchmark_cgar.py --output runs/cgar-new-run --jobs 1
```

The runner uses all ten MR24 instances, their archived simulation lengths, 1000 ms
per decision, and 30000 ms preprocessing. Each output directory must be new. Use
`--instances RANDOM-01 RANDOM-02` to run a subset. Sequential execution is the
default for the interactive host, whose account quota is one core. For actual parallel execution on GRID, use `python3 tools/benchmark_gridengine.py --output runs/cgar-grid-new --jobs 10`; it reserves one physical core per instance.

The [sequential repeat](benchmarks/mr24-sequential-repeat-20260918/summary.md) documents contention, the [GRID report](benchmarks/gridengine-20260918/README.md) verifies reserved parallel CPU execution, and the [strict-deadline study](experiments/throughput-20260918-strict/README.md) evaluates the first fallback repair and shorter horizons.

The [scheduler and distance-consistency study](experiments/throughput-20260918-next/README.md)
adds 360 matched CGAR runs, a reconstruction of the original CGAR core, and
KittyKnight/NMS references on reserved CPU allocations. The tested changes did not
justify a new default: HRRN remains enabled, fallback sampling remains 64, and the
three distance-consistency switches default off. A 1,000-step screen can reverse
the full-run conclusion, so candidate defaults still need full-horizon validation.
[NMS transfer priorities](experiments/throughput-20260918-next/NMS-transfer.md)
focus next on unopened-task reassignment, pickup weighting and orientation costs.

The [NMS-inspired assignment study](experiments/assignment-20260918/README.md)
implements global pickup weighting and bounded swaps of unopened tasks. It tests
both ideas separately and together, plus direct weighted-cost ranking. Reassignment
protects primary tickets, recovery witnesses, started tasks and fair admissions.
The default remains unchanged; measured throughput/waiting-time tradeoffs and
experimental settings are documented in the report.

Before values come from the existing draft runs and are preserved in
[benchmarks/cgar-draft.json](benchmarks/cgar-draft.json). These are single-run
throughput comparisons; passing the simulator does not prove starvation freedom.
Generated binaries, the Conda environment, logs, and full trajectories are ignored.

## Upstream provenance

The source trees are vendored snapshots, including the local build fixes already
present in this workspace. Existing nested Git histories are preserved locally;
clones of this repository receive ordinary source directories.

| Tree | Upstream revision |
|---|---|
| `Code-Archive/` | [MAPF-Competition/Code-Archive](https://github.com/MAPF-Competition/Code-Archive/tree/59e340d6ded3855c6a03973cb76efb69486db77a), selected 2024 team trees |
| `Benchmark-Archive/` | [MAPF-Competition/Benchmark-Archive](https://github.com/MAPF-Competition/Benchmark-Archive/tree/25ffd5b6a39b6fe30e5bc6cb5e22720a9531ea8a) |
| `cgar/` | [MAPF-Competition/Start-Kit](https://github.com/MAPF-Competition/Start-Kit/tree/72b409f52f4ca76a29b9dce849ed5f624dae0293), with the local CGAR migration |

The selected KittyKnight submission is `da7d428b40cc1425b381adefe9709d4accbfdfe1`;
the selected NMS submission is `44950d1b61b7482b4529a9dc127fa9d220962bdc`.
The optional `tools/knapp_certificate_probe.cc` uses a separate KNAPP checkout and
is not needed to build or run CGAR. Upstream license and participant notices are
retained in their source directories.
