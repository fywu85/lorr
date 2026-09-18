# PIBT specialization and CGAR layer diagnosis, 18 September 2026

CGAR now has an executable PIBT compatibility profile that matches the pinned upstream on **308 cases and 8,756,494 robot decisions**. The shared recursive assignment code serves both this profile and the existing CGAR policy. All 30 full-horizon default runs reproduce the previous baseline exactly.

The throughput result is that **ordinary spatial PIBT plus a direct turn adapter is substantially weaker than current CGAR**. Persistent movement commitments account for much of the recovery. Certificate-related routing and capacity handling also help the dense random cases when combined with commitments. The default remains the strongest of the tested profiles by the three-seed mean on every instance. No production policy is promoted by this study.

## Reference and the scope of equivalence

The reference is [Kei18/pibt2](https://github.com/Kei18/pibt2), the AIJ-2022 implementation, pinned at [`faab5b916649549f1cd563df8dbf6e4f6382f631`](https://github.com/Kei18/pibt2/tree/faab5b916649549f1cd563df8dbf6e4f6382f631). Its grid-pathfinding dependency is pinned at `f444df84459258d7b4d8ceffdbbf2e201b042a68`. Selected upstream files are vendored with both MIT license notices; [upstream.json](upstream.json) records every file hash. The algorithm in `pibt.cpp` is unchanged. CMake adds only a friend declaration to a generated header copy to expose stepping for the harness.

`CGAR_PIBT_REFERENCE=1` selects upstream's elapsed/initial-distance/random priority ordering, exact spatial distances, west/east/north/south/wait candidate order, shuffled ties, and empty-cell preference. It disables ordinary commitments and defaults certificates and transactions to off. This profile includes every traversable component, with no certificate-based capacity pruning. The production default still uses its existing policy. Both call [the shared recursion](../../cgar/cgar_planner/pibt_kernel.hpp).

There are two movement models. Native PIBT moves to an adjacent cell or waits in one step; it has no orientation. The native harness compares CGAR's spatial proposals with upstream, including checking the stepping wrapper against the original complete `PIBT::run` on all 144 native cases. The rotation harness adapts upstream proposals independently: turn toward the target, then cancel forward dependency chains that terminate at a stationary robot. CGAR uses its existing action conversion and safety code. Matching this adapter is not a claim that upstream PIBT itself handles LoRR rotations.

The 288 small cases span six 8-by-8 topologies, 25%/75%/100% occupancy, eight seeds and both movement models, for up to 256 decisions. Twenty full-fleet probes replay the existing MR24 snapshots at steps 100 and 500 for 100 decisions. Fixed goals remain fixed even after arrival; idle snapshot robots receive their start cell as a goal. This differs from the earlier NMS replay's goal-retirement protocol, so arrival totals are not directly comparable. Obstacles, adjacency, vertex collisions and edge swaps are independently validated. [Small results](results/small/results.json), [snapshot results](results/snapshots/results.json).

These are exact behavioral checks of a specialization, not a general liveness proof. Full occupancy, bridges and disconnected inputs deliberately test behavior outside usual progress assumptions. Switching off CGAR's certificates and recovery does not preserve their progress guarantees automatically. For lifelong use the existing CGAR scheduler remains; a changed or retired target cell resets native priority age and initial distance. That is an explicit LoRR extension, not a reproduction of upstream's MAPD scheduler. There is no map-name or category-specific rule.

## Full-horizon results

All **300/300** full-horizon runs completed with zero planner errors, schedule errors or timeouts. Ten instances, seeds 0–2 and ten profiles are retained. Values below are mean completed tasks over the same three seeds. Horizons are the archived per-instance horizons, not uniformly 1,000 steps.

**The decision limit is 10,000 ms for every profile in this diagnostic study.** Exact initial distance construction failed at 1,000 ms on CITY-01, CITY-02 and GAME in the earlier screen. Thus these are not competition-budget results. A timeout remains a failed run; no partial solution is returned. Default CGAR's 30 full trajectories match its previous 1,000 ms runs, and every default wrapper measurement here is below one second. [Baseline equivalence](results/baseline-equivalence.json).

| Instance | Steps | PIBT + turns | + commitments | + all tested CGAR layers | Current CGAR |
| --- | --- | --- | --- | --- | --- |
| WAREHOUSE | 5000 | 13,733.0 | 35,699.0 | 35,191.3 | 36,719.3 |
| SORTATION | 5000 | 18,157.7 | 54,615.7 | 55,964.7 | 66,581.3 |
| CITY-01 | 3000 | 2,076.3 | 4,902.7 | 4,863.7 | 6,133.0 |
| CITY-02 | 3000 | 2,419.0 | 5,665.0 | 5,620.7 | 6,729.7 |
| GAME | 5000 | 1,513.7 | 4,941.3 | 4,806.7 | 5,846.3 |
| RANDOM-01 | 600 | 256.7 | 267.0 | 405.3 | 451.3 |
| RANDOM-02 | 600 | 315.3 | 369.3 | 513.7 | 558.0 |
| RANDOM-03 | 800 | 249.0 | 519.0 | 598.0 | 649.7 |
| RANDOM-04 | 1000 | 77.3 | 352.7 | 570.0 | 632.0 |
| RANDOM-05 | 2000 | 51.7 | 377.7 | 985.3 | 1,067.3 |

`PIBT + turns` is CGAR's compatible spatial policy with the direct LoRR adapter and CGAR scheduling. It is not an official pibt2 competition submission. `+ commitments` adds only persistence of chosen movement targets. `+ all tested CGAR layers` also adds ticket ordering, certificates and recovery; it still retains reference candidate ordering and exact distances. Its residual differences from default include turn tie-breaking, random-number consumption and the distance-build policy, so the remaining gap cannot be attributed to one of these alone.

## What individual layers contribute

Each entry is the arithmetic mean of three paired percentage changes against the named control, not a confidence interval or a competition aggregate. Effects are not additive. Full per-seed values, ranges, outstanding-task ages and parked counts are in [effects.json](results/effects.json).

| Instance | Commitments alone | Tickets alone | Certificate alone | Certificate with tickets + commitments | Recovery with tickets + certificate + commitments |
| --- | --- | --- | --- | --- | --- |
| WAREHOUSE | +159.9% | +3.9% | +0.0% | +0.0% | +0.1% |
| SORTATION | +201.3% | -6.1% | +0.0% | +0.0% | +1.7% |
| CITY-01 | +136.2% | +1.8% | +3.3% | +0.3% | -0.5% |
| CITY-02 | +134.8% | -4.8% | -6.1% | -0.2% | -1.2% |
| GAME | +226.9% | +2.3% | -2.8% | -2.3% | +0.8% |
| RANDOM-01 | +15.5% | -6.2% | +39.4% | +53.9% | +3.0% |
| RANDOM-02 | +18.5% | +3.7% | +4.1% | +16.8% | +4.4% |
| RANDOM-03 | +108.4% | -6.5% | +0.8% | -1.9% | +14.6% |
| RANDOM-04 | +365.7% | +51.0% | -16.4% | +54.3% | +6.6% |
| RANDOM-05 | +638.5% | +27.4% | +2.2% | +155.4% | -2.2% |

The first three columns add one mechanism to bare PIBT. The fourth compares `pibt_tickets_certificate_commitments` against `pibt_tickets_commitments`. The fifth compares `pibt_all` against `pibt_tickets_certificate_commitments`. The two certificate controls were run in a separate reserved allocation after the first 240 runs to isolate recovery; they use the same executable, horizons, inputs, seeds and 10-second cap. No runtime speedup is inferred across those allocations.

Certificates here include topology-based routing, pocket locks and capacity handling, not merely a boolean validity check. On RANDOM-05, certificate-enabled variants park 31 robots and exclude tasks with stops outside the retained core; bare variants keep the full fleet and task set. Its certificate effect therefore combines motion changes with different service coverage. The code applies this capacity rule generically to the graph, not by map name. Recovery-disabled certificate controls still perform the capacity bootstrap required by that mode; the recovery contrast toggles the optional `CGAR_TXN` mechanism, not every possible vacancy shift.

## Movement explains much of the difference

Without commitments, spatial PIBT may choose a different destination after each physical turn. Commitments retain the target long enough to execute the turn-and-move sequence. The matched intervention produces much more actual forward movement and less turning on the large instances. This supports persistence as a useful mechanism; we did not directly count abandoned turn targets, and not every wait is avoidable.

| Instance | Profile | Forward | Turning | Wait | Turn-blocked forward |
| --- | --- | --- | --- | --- | --- |
| WAREHOUSE | pibt | 13.7% | 25.1% | 61.3% | 24.6% |
| WAREHOUSE | pibt_commitments | 39.2% | 15.9% | 44.9% | 14.9% |
| WAREHOUSE | baseline | 43.0% | 12.7% | 44.3% | 15.3% |
| SORTATION | pibt | 15.5% | 27.5% | 57.0% | 23.8% |
| SORTATION | pibt_commitments | 47.9% | 17.6% | 34.5% | 12.4% |
| SORTATION | baseline | 60.0% | 12.0% | 28.1% | 11.0% |
| GAME | pibt | 5.8% | 34.2% | 60.0% | 34.9% |
| GAME | pibt_commitments | 32.6% | 24.9% | 42.5% | 23.0% |
| GAME | baseline | 34.9% | 19.4% | 45.8% | 26.0% |
| RANDOM-04 | pibt | 3.8% | 30.8% | 65.5% | 28.3% |
| RANDOM-04 | pibt_commitments | 17.4% | 17.6% | 65.1% | 12.1% |
| RANDOM-04 | baseline | 25.9% | 22.0% | 52.1% | 21.1% |
| RANDOM-05 | pibt | 1.7% | 31.7% | 66.6% | 27.9% |
| RANDOM-05 | pibt_commitments | 10.3% | 10.6% | 79.0% | 8.3% |
| RANDOM-05 | baseline | 22.3% | 19.0% | 58.6% | 20.9% |

Percentages pool all robot steps across the three seeds. Forward, turning and wait sum to 100%; turn-blocked forward is a subset of wait. Commitment and recovery participation overlap actions and are retained separately in [movement.json](results/movement.json). The existing trajectory analyzer independently checks diagnostic action counts against simulator paths.

The practical next experiment is a fixed amount of search over short action sequences, including turns and waits, with occupancy checked at every future step. Retain persistent destinations, primary/recovery protection and the generic capacity rules while improving coordination. Ordinary PIBT equivalence is now a regression reference; it does not reproduce NMS's orientation-aware multi-step search or establish parity with NMS.

## Runtime, failures and CPU allocation

The one-second, 200-step screen retained **24 successes and six explicit failures**: both `pibt` and `pibt_all` timed out on CITY-01, CITY-02 and GAME at timestep 0 in `distance_table`. All six exited 124 at approximately 1,000 ms; unavailable throughput is not recorded as zero or used as a successful partial run. [Failure excerpts](results/screen/failures.json), [all screen outcomes](results/screen/run-summaries.json).

| Profile | Valid/runs | Max wrapper s | Runs >1 s | Total wrapper/control | Peak RSS GiB |
| --- | --- | --- | --- | --- | --- |
| baseline | 30/30 | 0.382 | 0 | 1.00x | 6.234 |
| pibt | 30/30 | 3.804 | 9 | 1.11x | 6.229 |
| pibt_tickets | 30/30 | 3.662 | 9 | 1.08x | 6.225 |
| pibt_commitments | 30/30 | 3.831 | 9 | 1.18x | 6.232 |
| pibt_tickets_commitments | 30/30 | 3.776 | 9 | 1.17x | 6.224 |
| pibt_certificate | 30/30 | 3.669 | 9 | 0.74x | 6.230 |
| pibt_recovery | 30/30 | 3.558 | 9 | 0.72x | 6.223 |
| pibt_all | 30/30 | 3.561 | 9 | 0.92x | 6.230 |
| pibt_tickets_certificate | 30/30 | 3.534 | 9 | separate allocation | 6.225 |
| pibt_tickets_certificate_commitments | 30/30 | 3.558 | 9 | separate allocation | 6.231 |

Wrapper time includes scheduling, planning and simulator wrapper overhead. Each reference profile exceeded one second on nine full runs, the three affected instances across three seeds; all stayed below the explicit ten-second cap. A 10-second allowance does not cause extra search or early return: the prescribed computation either finishes or raises `Timeout`. The production default limit is unchanged.

For WAREHOUSE, the current default completed all 5,000 steps with 10,000 robots in 114.2–118.5 seconds per seed (mean 116.3 seconds), using about 6.2 GiB RSS on one reserved physical core. These are process wall times, excluding GRID queue time and compilation. Three such seeds can run concurrently when three cores and sufficient memory are reserved.

Each full matrix used 30 distinct GRID physical cores, six simultaneous suites of five instances, an exclusive allocation and no CPU quota. Each CGAR process was pinned to one physical core; it did not use 30 cores itself. Both matrices used AMD EPYC 9354 processors. Requested memory was 8 GiB per slot; the aggregate process address-space limit is not an independent 8 GiB limit for each child. [Full allocation](results/full/allocation.json), [control allocation](results/controls/allocation.json).

The reference comparison used exclusive four-core and ten-core allocations on AMD EPYC 7532 processors, with each upstream/CGAR pair run sequentially on the same physical core. Upstream precomputes a distance table per robot before its timed steps; CGAR caches tables by goal and builds them in its first planning calls. Summed costs for the two snapshots of each instance are:

| Instance | Upstream preparation s | Upstream spatial planning s | CGAR preparation s | CGAR full planning s |
| --- | --- | --- | --- | --- |
| WAREHOUSE | 13.1009 | 1.0942 | 0.0096 | 8.3937 |
| SORTATION | 16.8159 | 1.2185 | 0.0124 | 10.8900 |
| CITY-01 | 2.9460 | 0.1025 | 0.0117 | 2.5380 |
| CITY-02 | 5.8544 | 0.2045 | 0.0123 | 5.1347 |
| GAME | 14.6245 | 0.8686 | 0.0154 | 10.0043 |
| RANDOM-01 | 0.0033 | 0.0041 | 0.0003 | 0.0144 |
| RANDOM-02 | 0.0066 | 0.0086 | 0.0003 | 0.0298 |
| RANDOM-03 | 0.0129 | 0.0174 | 0.0003 | 0.0578 |
| RANDOM-04 | 0.0223 | 0.0305 | 0.0003 | 0.0996 |
| RANDOM-05 | 0.0256 | 0.0352 | 0.0003 | 0.1132 |

Do not compare upstream's warm step time with CGAR's cold step time alone. The columns also have different scopes: the upstream step timer covers its spatial solver, while CGAR's includes progress bookkeeping, conversion to actions and safety checks. Graph/input creation, trace validation and JSON I/O are excluded. These measurements locate costs, not a claim of equal whole-program speed. They show why exact-distance preprocessing must be considered separately when enforcing a one-second decision limit. [Per-case timings](results/snapshots/results.json).

## Every tested profile

All rows below use the same three seeds. The full flag definitions are in [variants.json](variants.json) and [control-variants.json](control-variants.json).

| Variant | WAREHOUSE | SORTATION | CITY-01 | CITY-02 | GAME | RANDOM-01 | RANDOM-02 | RANDOM-03 | RANDOM-04 | RANDOM-05 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| baseline | 36719.3 | 66581.3 | 6133.0 | 6729.7 | 5846.3 | 451.3 | 558.0 | 649.7 | 632.0 | 1067.3 |
| pibt | 13733.0 | 18157.7 | 2076.3 | 2419.0 | 1513.7 | 256.7 | 315.3 | 249.0 | 77.3 | 51.7 |
| pibt_tickets | 14252.3 | 17016.7 | 2113.3 | 2300.7 | 1545.0 | 239.3 | 326.3 | 232.7 | 112.7 | 65.7 |
| pibt_commitments | 35699.0 | 54615.7 | 4902.7 | 5665.0 | 4941.3 | 267.0 | 369.3 | 519.0 | 352.7 | 377.7 |
| pibt_tickets_commitments | 35189.3 | 55204.3 | 4874.3 | 5699.7 | 4882.0 | 276.0 | 435.0 | 533.0 | 346.7 | 402.0 |
| pibt_certificate | 13733.0 | 18157.7 | 2144.3 | 2264.7 | 1469.3 | 338.0 | 328.0 | 251.0 | 64.0 | 52.7 |
| pibt_recovery | 13995.3 | 16581.0 | 2140.7 | 2387.0 | 1475.3 | 361.0 | 353.7 | 261.0 | 99.7 | 86.7 |
| pibt_all | 35191.3 | 55964.7 | 4863.7 | 5620.7 | 4806.7 | 405.3 | 513.7 | 598.0 | 570.0 | 985.3 |
| pibt_tickets_certificate | 14252.3 | 17016.7 | 2021.7 | 2294.3 | 1489.3 | 346.0 | 316.7 | 248.0 | 82.0 | 69.3 |
| pibt_tickets_certificate_commitments | 35189.3 | 55204.3 | 4888.3 | 5686.3 | 4768.7 | 393.7 | 492.3 | 522.7 | 534.7 | 1007.3 |

## Reproduction

The harness builds two separate executables so the upstream and LoRR type definitions cannot collide. It verifies vendored hashes, captures the CGAR/driver source hashes, and checks that sources do not change during execution. The existing CGAR regression suite also passes. [Build and regression evidence](results/build-provenance/), [reference build provenance](results/small/build.json).

```sh
python3 experiments/pibt-equivalence-20260918/equivalence.py \
  --output runs/new-pibt-equivalence
# After the first GRID job completes:
python3 experiments/pibt-equivalence-20260918/equivalence.py \
  --output runs/new-pibt-snapshots --snapshots \
  --build-dir runs/new-pibt-equivalence/build --jobs 10
python3 experiments/assignment-20260918/build.py --output runs/new-pibt-build
# After the build job completes:
python3 tools/benchmark_matrix.py --output runs/new-pibt-full \
  --binary runs/new-pibt-build/lifelong --source-manifest runs/new-pibt-build/build.json \
  --variants experiments/pibt-equivalence-20260918/variants.json \
  --seeds 0 1 2 --parallel-suites 6 --jobs-per-suite 5 --time-limit-ms 10000
python3 experiments/motion-20260918/submit_analysis.py \
  --input runs/new-pibt-full --output runs/new-pibt-full-analysis --hold-job JOB_ID
```

Repeat the matrix with `control-variants.json` for the 60 additional controls. The shorter failed/successful screen uses `screen-variants.json`, `--horizon-profile experiments/pibt-equivalence-20260918/screen-200.json`, seed 0 and `--time-limit-ms 1000`. Every output directory must be new. Submission commands return before their jobs finish. The configured workspace compiler/environment is required by the GRID runners; the experiment CMake project itself is also usable directly.

`python3 experiments/pibt-equivalence-20260918/report.py` regenerates this report and derived JSON using only committed compact results. `collect.py` packages the local run evidence after GRID analysis has produced `results/full` and `results/controls`; its path arguments support alternate raw run locations. Binaries, full trajectories and raw logs remain ignored under `runs/`.
