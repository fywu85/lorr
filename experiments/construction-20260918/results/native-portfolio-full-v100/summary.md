# Smaller parallel searches trade throughput for lower latency

All six full runs and exact one-worker controls pass. Splitting the prescribed global search limits across two or four workers reduces latency, but both settings lose task throughput on both seeds. Retain one worker for the throughput reference.

| Global workers | Candidate threshold / attempts per worker | Seed 0 | Seed 2 | Mean tasks | Mean entry across seeds |
|---|---|---:|---:|---:|---:|
| 1, exact control | 4M / 1M | 155,057 | 154,727 | 154,892 | 453.313 ms |
| 2 | 2M / 500k | 154,710 | 154,669 | 154,689.5 | 333.897 ms |
| 4 | 1M / 250k | 154,583 | 154,687 | 154,635 | 272.173 ms |

Two workers lose 347 / 58 tasks; four lose 474 / 40. Observed mean entry time falls about 26% / 40%, and full wall time averages 38.80 / 28.83 / 23.66 minutes for one / two / four workers. Average CPU usage rises from about 1.50 to 2.04 / 2.55 cores, with four reserved in every case. These are shared-host whole-policy observations, not same-trajectory controlled timing measurements.

The aggregate configured thresholds stay 4M candidates and one million repair attempts, but executed work is not identical. Each worker constructs a plan; complete final attempts can overshoot the threshold; either stopping condition can bind; RNG consumption and search diversity change. All prescribed workers complete before deterministic selection. No partial result or anytime fallback is used. Sampled global counters expose only the selected worker, not all workers' summed work.

All 30,000 complete entries satisfy the 5-second development deadline and 32 decimal GB RSS. Maximum entry across the matrix is 1.012774 seconds; peak RSS 12.571 GB. Four-worker maxima are 0.801751 / 0.797551 seconds, but the runs were not strict-one-second certification. Twenty-four physical cores on research52 are disjoint, with no CPU quota.

Every first-half task finishes in every arm, with no initially revealed task left never assigned and no post-pickup reassignment. Oldest never-assigned ages are 187 / 187 for two workers and 188 / 185 for four, versus 190 / 185 for the controls. Outstanding-age p90 changes are 0 / -1 and +1 / 0. Ordinary fairness and one-retarget protection remain unchanged.

Independent accounting verifies 300 million robot steps. Empty-work changes are +82 / -2,556 for two workers and +1,888 / -2,128 for four; unfinished-work changes are -1,428 / +2,064 and +6,034 / +488. These small mixed accounting differences do not explain the score change causally.

Source `27be6e312fdd79ad310583d33eeb2a4781d75b97`, binary `37694eacdc5c829c2f77673c4e4cf6070d1fca2cd8512b91cab6672249385752`. Validation completed 2026-09-20 17:29:11 UTC; independent accounting completed 17:29:51 UTC. This uses the preceding p90 / pickup-weight-5 profile, with explicit `--trick WAREHOUSE` and known horizon 5,000.

A separate comparison now keeps each worker's original 4M / 1M limits and adds parallel workers on the new weight-8 profile. That increases aggregate compute and tests a different question; this negative result does not imply it will win.

[Verification](verification.json), [fairness](fairness.json), [independent accounting](../native-portfolio-full-v100-outcomes/accounting.json), [next experiment](../../native_portfolio/FULL_PER_WORKER.md).
