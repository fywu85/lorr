# Active PILOT goal: all five RANDOM instances

Updated 2026-09-21 22:26 UTC following the user's explicit scope and baseline corrections.

**Maximize verified full-run combined-track throughput on RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04 and RANDOM-05. Make a substantial, evidence-driven effort on every instance, developing general improvements and separately labelled instance tricks. Always compare each instance with max(NMS, Kitty Knight).**

The RANDOM-03/04 ten-percent qualification is a milestone inside this broader campaign, not the entire objective. The earlier stored app goal mentions NMS2,580/RANDOM-04 target2,838; those are historical and superseded for current comparisons by NMS2,649/target2,914. Current user steering governs even if the app retains older text.

| Instance | Current selected best | Matched max(NMS, KK) | Lead | 10% reference milestone |
|---|---:|---:|---:|---:|
| RANDOM-01 | 729 | 692 | +5.35% | 762 |
| RANDOM-02 | 1,408 | 1,256 | +12.10% | 1,382 |
| RANDOM-03 | 2,620 | 2,359 | +11.06% | 2,595 |
| RANDOM-04 | 2,782 | 2,649 | +5.02% | 2,914 |
| RANDOM-05 | 4,242 | 3,172 | +33.73% | 3,490 |

The table's10% values are useful yardsticks for all instances. Only RANDOM-03/04 were explicitly assigned that numeric qualification requirement; crossing a yardstick does not end work on the other instances or prove the open-ended throughput objective complete. References may strengthen with later valid measurements. Preserve previous runs and refresh margins against the stronger baseline.

## Execution and evidence

- Work across all five densities. Run broadly applicable changes on all five; conduct focused follow-ups where a mechanism helps. Give RANDOM-01/02 meaningful planner/scheduler work and keep pushing RANDOM-05 beyond4,242.
- Measure full600/600/800/1000/2000-step horizons on fixed input hashes. Use up to32 workers/32 GB, strict1-second entries and30-second preprocessing; preserve every timeout and allocation refusal. Declared work completes or fails.
- For each new selected record: independently replay movement, collisions, assignments and task events; record source commit, timestamp, exact settings, memory and latency. Repeat the exact configuration and test paired planner seeds before claiming reproducibility.
- Freeze source, configuration, planner seed, protocol and fresh task/start seeds before generation. Validate selected progress against repeated NMS and KK on each same fresh input/allocation, and state whether gains persist. Do not use held-out streams for tuning.
- Require explicit `--trick INSTANCE` for map-specific guidance, known-horizon rules and other tuned tricks. Keep general mechanisms and selected per-density presets clearly distinguished.
- Throughput is primary. Track completed and censored order waiting times as secondary measures; no fairness claim follows from a higher score.
- Keep [PILOT_PROGRESS.md](../PILOT_PROGRESS.md) current and [RANDOM05_PROGRESS.md](../RANDOM05_PROGRESS.md) as the detailed RANDOM-05 history. Preserve selected bests, weaker attempts, failures and all held-out inputs. Scope writes to those two files and random05/.

## Current next work

Qualify the general auction gains on RANDOM-03/04, complete stronger-baseline reporting, and start another five-instance planner/scheduler hypothesis with meaningful low-density and crowded-regime tests. The current selected records remain729/1408/2620/2782/4242. None of the new general auction results changes those selected records.

The campaign remains active. Further increases in selected maxima are useful evidence; they do not alone establish a repeatable gain, fresh-input transfer or that performance cannot be improved.
