# Benchmark records

- [Strict-deadline fallback study and horizon calibration](../experiments/throughput-20260918-strict/README.md)
- [Reserved GRID CPU measurements and unchanged baseline](gridengine-20260918/README.md)
- [Sequential repeat under documented contention](mr24-sequential-repeat-20260918/summary.md)
- [Parallel MR24 results, September 18, 2026 — ten concurrent jobs](mr24-parallel-20260918/summary.md)
- [Sequential MR24 results, September 17, 2026](mr24-20260917/summary.md)
- [Original draft metrics](cgar-draft.json)
- [Regression test output](regression-20260917.txt)

The September 17 sequential suite completed all ten horizons with zero simulator errors or timeouts. Throughput improved on 7 instances and regressed on 3. See the report for the capacity restriction on RANDOM-05 and the warehouse regressions.

The September 18 parallel run used the identical production executable and started all ten instances together under the shared one-CPU quota. All ten completed with zero errors or timeouts, while the larger instances had lower throughput. See the parallel report for the per-instance comparison and resource context.
