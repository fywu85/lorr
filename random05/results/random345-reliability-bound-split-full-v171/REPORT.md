# IPO and binding-only full reliability attempt

This attempted repair is **insufficient**. Every original attempt is preserved.

| Instance | Full successes | Entry timeouts | Allocation refusals | Worst successful entry, ms |
|---|---:|---:|---:|---:|
| RANDOM-03 | 9/10 | 0 | 1 | 931.883 |
| RANDOM-04 | 4/10 | 6 | 0 | 842.618 |
| RANDOM-05 | 9/10 | 1 | 0 | 858.851 |

Shared verified allocations use16physical EPYC9354 cores/32workers, strict1,000ms entries and full800/1000/2000step horizons. No score is assigned to failed or refused attempts. Successful full runs pass independent action/assignment/event replay and memory/timing checks.

The subsequent exact SIMD and persistent-worker changes are separate implementation trials. None of these failures is overwritten or silently replaced.

[Independent audit](audit.json), [frozen protocol](../../experiments/random345-reliability-bound-full-v171-protocol.json).
