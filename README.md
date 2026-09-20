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

[WAREHOUSE throughput progress](WAREHOUSE_PROGRESS.md) tracks every new full-run best with UTC completion time, linked commits and benchmark evidence, alongside confirmation across seeds.

On 20 September 2026, an explicitly enabled Warehouse TRICK reached **155,173 tasks**,
378 above the user-specified NMS target of 154,795. It completes 5,000 steps with
10,000 robots, zero errors/timeouts, seed 0, source
[37f592a](https://github.com/fywu85/lorr/commit/37f592a48bdb24b936f8d0be8be22ddd405b4ddb).
The late matching-cycle guard scores 155,173 / 155,056 / 155,090 / 155,008 on
seeds 0–3, mean 155,081.75: three paired wins, one loss, mean gain 33 tasks.
It assumes a configured 5,000-step horizon and requires
`--trick WAREHOUSE`. Ordinary fairness remains enabled; every first-half task
finishes, with no formal starvation-freedom claim. The preceding guard-OFF profile
scores 155,120 / 154,999 / 155,056 / 155,020 on seeds 0–3, mean 155,048.75.
These are solver seeds on one fixed input. The generic six-seed reference remains
144,392.17. Shared-host 5s development results are separate from strict 1s
certification. [Verified result and exact configuration](experiments/construction-20260918/results/match-horizon-native-full-v110/summary.md).

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

The [movement diagnosis and NMS comparison](experiments/motion-20260918/README.md)
isolate a movement gap using identical robot positions and goals, including an
NMS reference with uniform action costs and no category-specific fleet limits.
An optional profile, `CGAR_ORIENTATION_GUIDANCE=2 CGAR_TURN_FIRST=1`, adds turn-aware
route costs with a bounded cache. Across six seeds it improves WAREHOUSE by 10.7%,
CITY-01 by 6.2%, and RANDOM-02 by 9.9%, but SORTATION averages -1.5%. Defaults remain
unchanged. The report retains all full-horizon results, rejected prototypes,
CPU allocations, and a follow-up audit of simulator timing outliers.

The [PIBT specialization study](experiments/pibt-equivalence-20260918/README.md)
adds an optional policy that matches the pinned `Kei18/pibt2` reference on 308
cases and 8.76 million robot decisions. Across 300 full-horizon diagnostic runs,
CGAR's existing movement commitments explain much of its advantage over spatial
PIBT with a direct turn adapter. Default trajectories remain identical. The
study uses an explicit 10-second decision cap for exact-distance construction;
it retains the preceding one-second failures and is not a competition-budget
comparison.

The [full WAREHOUSE sequence and cache study](experiments/sequences-20260918/README.md)
focuses subsequent work on that instance and uses all 5,000 steps for iterations.
An optional 8 GiB orientation-cache profile improves mean completions by 17.3%
over default CGAR across six planner seeds, using about 14.3 GiB total peak RSS.
Full KittyKnight/NMS references, the remaining throughput gap, unsuccessful
sequence prototypes, and a second Fable 5.1 Max CLI review are retained.

The [temporal construction study](experiments/construction-20260918/RESULTS.md)
ports and independently checks NMS-style construction and fixed-work repair inside
CGAR's protection rules. The optional equal-weight 50k profile averages 107,413
warehouse tasks over three seeds, versus 43,407 for the paired cache control.
Serial and parallel portfolios produce identical full trajectories. A precise
timing repeat stays below 0.506 seconds per entry and about 15.2 GiB total memory;
six-seed confirmation and the user-revised 154,795-task target remain outstanding.
The study also records initialization-failure fixes, a further Fable CLI review,
and an exact travel audit that identifies the remaining loaded-motion gap.

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
