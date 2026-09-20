# Full generic unopened matching comparison

Exact source [0196851](https://github.com/fywu85/lorr/commit/019685109769788dfcc318815e8d102a376982a9). Six full cases, all valid under the shared-host 5s development deadline and 32GB RSS. Both disabled controls exactly reproduce the confirmed reference; fixed planner work and distinct physical bindings verified.

| Quota | Seed 0 | Seed 2 | Paired mean effect |
|---|---:|---:|---:|
| OFF | 144,510 | 144,107 | control |
| 4 groups | 144,555 | 144,382 | +0.11087% |
| 64 groups | 144,967 | 144,511 | +0.29832% |

The64-group arm improves final-1000 totals by51/123 and reduces outstanding-age p90 by5/3. Empty work falls by125,579/137,209 robot steps. Its entries average401.22/388.48ms; maximum921.361ms, peakRSS11.915GB, average1.727/1.765CPU cores of four reserved. The policy remains default OFF until additional seeds are checked. No new generic single-run record:145,083 remains higher.

Full paired confirmation on seeds1/3/4/5 is submitted as8899409 with independent held analysis8899410. Do not promote from this first pair alone.

[Complete comparison](comparison.json) · [Source, resource and movement verification](verification.json).
