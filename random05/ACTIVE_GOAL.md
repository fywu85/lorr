# Active PILOT goal: all five RANDOM instances

Updated 2026-09-21 23:48 UTC following the user's explicit scope and baseline corrections.

**Maximize verified full-run combined-track throughput on RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04 and RANDOM-05. Make a substantial, evidence-driven effort on every instance, developing general improvements and separately labelled instance tricks. Always compare each instance with max(NMS, Kitty Knight).**

The RANDOM-03/04 ten-percent qualification is a milestone inside this broader campaign, not the entire objective. The earlier stored app goal mentions NMS2,580/RANDOM-04 target2,838; those are historical and superseded for current comparisons by NMS2,649/target2,914. Current user steering governs even if the app retains older text.

| Instance | Current selected best | Matched max(NMS, KK) | Lead | 10% reference milestone |
|---|---:|---:|---:|---:|
| RANDOM-01 | 733 | 692 | +5.92% | 762 |
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

The physical deadline gate now has an independently audited RANDOM-01 selected record of733 (+5.92% versus matchedmax692). Five paired planner seeds total3650 versus3625 (+0.690%), all positive; exact732 repetition is verified and exact733 repetition is queued. Fresh01V1 qualifies729 only. Other selected records remain1408/2620/2782/4242. The gate scores1403 on02 and raw2621/2782 on03/04; final all-five/replacement audit is pending. Original research49 allocation refusals remain retained.

Sixteen frozen full600-step RANDOM-01/02 trials compare existing complete-plan annealing temperatures0/0.25/1/4 on both general and selected trick profiles. Fixed budgets and strict limits are unchanged. This is meaningful sparse-instance planner work alongside dense coordination/scheduling experiments, not a presumed improvement.

The general auction gain has five paired planner seeds (+1.85% on03 and +4.31% on04). General04's face-cycle selected1616 repeats exactly, but five paired seeds give -0.101% (two gains/three losses); do not claim a robust mean improvement. All ten higher-startup-cap04/05 cases are audited and every intervention loses; controls2782/4242 are exact. Previous32 face-cycle,30 PIBT retry,14 prefix-search and8 sparse priority cases are fully audited, with negative outcomes/failures preserved. Keep substantive work across all five and pursue measured bottlenecks.

The campaign remains active. Further increases in selected maxima are useful evidence; they do not alone establish a repeatable gain, fresh-input transfer or that performance cannot be improved.
