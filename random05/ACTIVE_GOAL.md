# Active PILOT goal: all five RANDOM instances

Updated 2026-09-22 00:59 UTC following the user's all-instance scope and deferred rename.

**Maximize verified full-run combined-track throughput on RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04 and RANDOM-05. Make a substantial, evidence-driven effort on every instance, developing general improvements and separately labelled instance tricks. Always compare each instance with max(NMS, Kitty Knight).**

The RANDOM-03/04 ten-percent qualification is a milestone inside this broader campaign, not the entire objective. The earlier stored app goal mentions NMS2,580/RANDOM-04 target2,838; those are historical and superseded for current comparisons by NMS2,649/target2,914. Current user steering governs even if the app retains older text.

| Instance | Current selected best | Matched max(NMS, KK) | Lead | 10% reference milestone |
|---|---:|---:|---:|---:|
| RANDOM-01 | 739 | 692 | +6.79% | 762 |
| RANDOM-02 | 1,408 | 1,256 | +12.10% | 1,382 |
| RANDOM-03 | 2,646 | 2,359 | +12.17% | 2,595 |
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

Selected records are **739 / 1408 / 2646 / 2782 / 4254**. Every selected run passes independent full replay and strict limits. These are development-input maxima, with qualification reported separately.

- RANDOM-01: observed-progress horizon correction.25/span32 raises735to739. Exact repetition matches all six fields. Five paired planner seeds736/739/726/732/732 versus732/735/725/729/730 total3665versus3651 (+0.3835%, all positive). Largest qualification entry118.656ms. Fresh01V1still qualifies729only.
- RANDOM-02: selected1408 and general1397remain. All observed-progress mixtures and third-repair-order variants lose. Preserve that negative evidence; do not import the new01/03settings here.
- RANDOM-03: progress correction.25/span32 raises2621to2646 (+12.17% versus matched max2359). Mean484.103/max676.507ms,RSS469.467MB. The off2621control matches all six archived fields. The exact2646repeat matches all six fields. Five paired planner seeds2611/2606/2611/2646/2610 total13084versus13029 (+0.4221%, all positive); largest entry730.523ms. No fresh2646qualification is claimed. Fresh03V2qualifies2620only and gives+8.852%aggregate over fresh max(NMS,KK), below10%.
- RANDOM-04: selected2782remains132short of2914. All guidance-iteration and new progress-span alternatives lose. The existing2782profile repeats exactly and has+0.340%aggregate over eight paired seeds, but extensionseed11has a preserved first-step timeout. General auction gains have five positive paired seeds; general face-cycle selected1616has a slightly negative paired mean.
- RANDOM-05: field80updates raises4242to4254 (+34.11% versus matched max3172). Exact repetition passes. Four paired planner seeds4254/4189/4252/4253 versus4242/4028/4183/4128 total16948versus16581 (+2.2134%, all positive), mean4237.0. Largest qualification entry919.594ms, RSS494.387MB. All91chronologicalfrontier/waiting rows pass. FreshV6qualifies4175only and is an NMS-only comparison.

The12-case progress-transfer experiment is complete and independently audited; its three declared controls are full-trace exact. Source163's optional third repair order loses on all five tested profiles; leave it disabled. Its full regression passed62.13s. No source164implementation exists.

The frozen2646qualification is complete. No PILOT benchmark jobs remain active at this checkpoint. Preserve every outcome and continue substantial work across all five densities. [Replay/runtime snapshot](results/current-bottlenecks-v162/REPORT.md) motivates different sparse/crowded priorities; it does not identify causally recoverable throughput. A group repair rejection bound is only a possible runtime hypothesis and needs correctness/compatibility work before implementation.

No held-out input was generated or used for tuning. Selected maxima, paired planner-seed gains and fresh-input qualification remain separate. The dashboard and density ledger lead with matched max(NMS,KK); historical published comparisons stay separate.

The user deferred renaming random05/ to pilot/ until the end of development. Only a read-only naming inventory was performed; keep current paths during this campaign.
