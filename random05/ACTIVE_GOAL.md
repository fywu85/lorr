# Active PILOT goal: all five RANDOM instances

Updated 2026-09-22 00:32 UTC following the user's all-instance scope and deferred rename.

**Maximize verified full-run combined-track throughput on RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04 and RANDOM-05. Make a substantial, evidence-driven effort on every instance, developing general improvements and separately labelled instance tricks. Always compare each instance with max(NMS, Kitty Knight).**

The RANDOM-03/04 ten-percent qualification is a milestone inside this broader campaign, not the entire objective. The earlier stored app goal mentions NMS2,580/RANDOM-04 target2,838; those are historical and superseded for current comparisons by NMS2,649/target2,914. Current user steering governs even if the app retains older text.

| Instance | Current selected best | Matched max(NMS, KK) | Lead | 10% reference milestone |
|---|---:|---:|---:|---:|
| RANDOM-01 | 735 | 692 | +6.21% | 762 |
| RANDOM-02 | 1,408 | 1,256 | +12.10% | 1,382 |
| RANDOM-03 | 2,621 | 2,359 | +11.11% | 2,595 |
| RANDOM-04 | 2,782 | 2,649 | +5.02% | 2,914 |
| RANDOM-05 | 4,254 | 3,172 | +34.11% | 3,490 |

The table's10% values are useful yardsticks for all instances. Only RANDOM-03/04 were explicitly assigned that numeric qualification requirement; crossing a yardstick does not end work on the other instances or prove the open-ended throughput objective complete. References may strengthen with later valid measurements. Preserve previous runs and refresh margins against the stronger baseline.

## Execution and evidence

- Work across all five densities. Run broadly applicable changes on all five; conduct focused follow-ups where a mechanism helps. Give RANDOM-01/02 meaningful planner/scheduler work and keep pushing RANDOM-05 beyond4,254.
- Measure full600/600/800/1000/2000-step horizons on fixed input hashes. Use up to32 workers/32 GB, strict1-second entries and30-second preprocessing; preserve every timeout and allocation refusal. Declared work completes or fails.
- For each new selected record: independently replay movement, collisions, assignments and task events; record source commit, timestamp, exact settings, memory and latency. Repeat the exact configuration and test paired planner seeds before claiming reproducibility.
- Freeze source, configuration, planner seed, protocol and fresh task/start seeds before generation. Validate selected progress against repeated NMS and KK on each same fresh input/allocation, and state whether gains persist. Do not use held-out streams for tuning.
- Require explicit `--trick INSTANCE` for map-specific guidance, known-horizon rules and other tuned tricks. Keep general mechanisms and selected per-density presets clearly distinguished.
- Throughput is primary. Track completed and censored order waiting times as secondary measures; no fairness claim follows from a higher score.
- Keep [PILOT_PROGRESS.md](../PILOT_PROGRESS.md) current and [RANDOM05_PROGRESS.md](../RANDOM05_PROGRESS.md) as the detailed RANDOM-05 history. Preserve selected bests, weaker attempts, failures and all held-out inputs. Scope writes to those two files and random05/.

## Current next work

The selected records are 735 / 1408 / 2621 / 2782 / 4254. The physical deadline gate is qualified across five RANDOM-01 planner seeds: 3650 versus 3625 (+0.690%), all positive. Exact 733 repetition passes. Mild complete-plan annealing then raises the selected seed to 735; its exact repeat passes, and five planner seeds total3651 versus3650 (+0.027%, three gains/two losses). This is a repeatable selected best with negligible paired mean gain. Fresh01V1 still qualifies the earlier 729 profile only.

On RANDOM-03 the gate raises the selected seed from 2620 to 2621 and repeats exactly. Five planner seeds total13029 versus12999 (+0.231%, four gains/one loss), a modest gain without new fresh-input qualification. The gate loses on02/05 and is neutral on04. All ten valid original/replacement runs are independently audited; both original research49 allocation refusals remain preserved, and all five disabled controls match their complete archived traces.

Current work covers every instance:
- RANDOM-01/02: all16annealing cases and735qualification are audited. General01 is unchanged and all nonzero02 temperatures lose. Source163's third repair order loses on all five tested profiles (general01/02 and selected01/02/03), even with full iterations on01; all12runs pass replay and all five controls are exact. Leave the optional feature off.
- RANDOM-03/04/05: all12guidance trials pass replay; all three controls are exact. Iterations20/10/40/80 produce03=2621/2527/2600/2602;04=2782/2570/2761/2771;05=4242/3864/4193/4254. Only05improves. The4254run averages579.781ms, peaks874.301ms, uses490.107MB, and is34.11%above matched max3172. Seven frozen exact-repeat/paired-seed cases are running on source162. No paired or fresh qualification is yet claimed.
- General03/04 auction gains have five paired planner seeds (+1.85%/+4.31%, all positive). General04 face cycles reach a selected1616 and repeat exactly, but their paired mean is -0.101%; do not call that a robust improvement. All higher startup-cap04/05 trials lose.

The dashboard now leads with matched local max(NMS,KK). Its renderer also derives campaign targets from the current reference file, fixing a stale RANDOM-04 table that reverted to2580/2838 on refresh. The correct reference/threshold are2649/2914.

The campaign remains active. Selected maxima, repeatable planner-seed gains and fresh-input qualification remain separate evidence levels. No held-out stream was generated or used for tuning in this work.

The user deferred renaming random05/ to pilot/ until the end of development. Only a read-only naming inventory was performed; keep current paths during this campaign.
