# Active PILOT goal: all five RANDOM instances

Updated 2026-09-22 02:50 UTC following current run audits.

**Maximize verified full-run combined-track throughput on RANDOM-01, RANDOM-02, RANDOM-03, RANDOM-04 and RANDOM-05. Make a substantial, evidence-driven effort on every instance, developing general improvements and separately labelled instance tricks. Always compare each instance with max(NMS, Kitty Knight).**

The RANDOM-03/04 ten-percent qualification is a milestone inside this broader campaign, not the entire objective. The earlier stored app goal mentions NMS2,580/RANDOM-04 target2,838; those are historical and superseded for current comparisons by NMS2,649/target2,914. Current user steering governs even if the app retains older text.

| Instance | Current selected best | Matched max(NMS, KK) | Lead | 10% reference milestone |
|---|---:|---:|---:|---:|
| RANDOM-01 | 739 | 692 | +6.79% | 762 |
| RANDOM-02 | 1,408 | 1,256 | +12.10% | 1,382 |
| RANDOM-03 | 2,646 | 2,359 | +12.17% | 2,595 |
| RANDOM-04 | 2,782 | 2,649 | +5.02% | 2,914 |
| RANDOM-05 | 4,302 | 3,172 | +35.62% | 3,490 |

The table's10% values are useful yardsticks for all instances. Only RANDOM-03/04 were explicitly assigned that numeric qualification requirement; crossing a yardstick does not end work on the other instances or prove the open-ended throughput objective complete. References may strengthen with later valid measurements. Preserve previous runs and refresh margins against the stronger baseline.

## Execution and evidence

- Work across all five densities. Run broadly applicable changes on all five; conduct focused follow-ups where a mechanism helps. Give RANDOM-01/02 meaningful planner/scheduler work and keep pushing RANDOM-05 beyond4,302.
- Measure full600/600/800/1000/2000-step horizons on fixed input hashes. Use up to32 workers/32 GB, strict1-second entries and30-second preprocessing; preserve every timeout and allocation refusal. Declared work completes or fails.
- For each new selected record: independently replay movement, collisions, assignments and task events; record source commit, timestamp, exact settings, memory and latency. Repeat the exact configuration and test paired planner seeds before claiming reproducibility.
- Freeze source, configuration, planner seed, protocol and fresh task/start seeds before generation. Validate selected progress against repeated NMS and KK on each same fresh input/allocation, and state whether gains persist. Do not use held-out streams for tuning.
- Require explicit `--trick INSTANCE` for map-specific guidance, known-horizon rules and other tuned tricks. Keep general mechanisms and selected per-density presets clearly distinguished.
- Throughput is primary. Track completed and censored order waiting times as secondary measures; no fairness claim follows from a higher score.
- Keep [PILOT_PROGRESS.md](../PILOT_PROGRESS.md) current and [RANDOM05_PROGRESS.md](../RANDOM05_PROGRESS.md) as the detailed RANDOM-05 history. Preserve selected bests, weaker attempts, failures and all held-out inputs. Scope writes to those two files and random05/.

## Current next work

Selected records are **739 / 1408 / 2646 / 2782 / 4302**. Every selected run passes independent full replay and strict limits. These are development-input maxima, with qualification reported separately.

- RANDOM-01: observed-progress horizon correction.25/span32 raises735to739. Exact repetition matches all six fields. Five paired planner seeds736/739/726/732/732 versus732/735/725/729/730 total3665versus3651 (+0.3835%, all positive). Largest qualification entry118.656ms. Fresh01V1still qualifies729only.
- RANDOM-02: selected1408 and general1397remain. All observed-progress mixtures and third-repair-order variants lose. Preserve that negative evidence; do not import the new01/03settings here.
- RANDOM-03: progress correction.25/span32 raises2621to2646 (+12.17% versus matched max2359). Mean484.103/max676.507ms,RSS469.467MB. The off2621control matches all six archived fields. The exact2646repeat matches all six fields. Five paired planner seeds2611/2606/2611/2646/2610 total13084versus13029 (+0.4221%, all positive); largest entry730.523ms. Frozen fresh03V3qualifies2646:2660/2622 versus max(NMS,KK)2461/2380 (+9.110%aggregate, below10%); prior2620recipe2639/2612 gives a positive0.590%paired aggregate. All12runs pass strict checks and independent full replay.
- RANDOM-04: selected2782remains132short of2914. All guidance-iteration and new progress-span alternatives lose. The existing2782profile repeats exactly and has+0.340%aggregate over eight paired seeds, but extensionseed11has a preserved first-step timeout. General auction gains have five positive paired seeds; general face-cycle selected1616has a slightly negative paired mean.
- RANDOM-05: committed-motion terminal credit.5 raises4254to4302 (+35.62% versus matched max3172). Full replay and strict checks pass; mean582.723/max830.909ms,RSS491.999MB. The disabled4254control is six-field exact. Exact4302repeat and additional paired seeds are pending; disabled-credit seed3control failed at step0/1045.313ms and has no score. No complete four-pair qualification is claimed. The preceding4254field80recipe has all-four-positive paired qualification (+2.2134%). General05separately rises2226to2314atcredit1, with an exact repeat and+3.4599%aggregate over four paired planner seeds (three gains/one loss), all strict-valid. FreshV6qualifies4175only and is NMS-only.

The12-case progress-transfer experiment is complete and independently audited. Source163's third repair order and source162's larger current-profile repair budgets lose on every tested profile. The04/05flow-model batch is now complete: all10runs pass replay/strict checks, both controls are six-field exact, and every alternative loses. No record came from that experiment. See [flow-model report](results/random45-flow-model-split-full-v162/REPORT.md).

Source164/30a56647 terminal credit is complete: all16runs independently replayed/strict-valid; seven controls six-field exact. General reactive01=647/643,02=1106/1101,03=1634/1619,04=1616/1600,05=2226/2314 (credit0/1). Selected04at0/.5/1scores2782/2747/2769; selected05scores4254/4302/4250. Only05improves. General01/02 reactive baselines remain distinct from their stronger windowed records. General2314qualification is frozen in4d94594a (exactseed5repeat, paired6/7/8); selected4302qualification is frozen in5e47a72f (exactseed0repeat, paired1/2/3). Both include their original selecting pair in the four-seed aggregate, retain every failure and use full strict horizons. [Complete report](results/random12345-terminal-pending-split-full-v164/REPORT.md).

Source165/513cbe89 adds default-off parallel continuation evaluation outside already-active search teams. Original branch reduction order, complete work and sequential local proposals are preserved. Full regression62.69s passes; compiled/test sources and binary hashes are verified. Eighteen full local-refinement cases across all five general reactive densities and selected04/05 are frozen in919612ec and submitted. Nine exact controls cover unchanged trajectories and serial/parallel local refinement. This is a bounded interaction test, not a claimed throughput/runtime gain. [Manifest](experiments/random12345-local-refinement-full-v165.json).

[Replay/runtime snapshot](results/current-bottlenecks-v162/REPORT.md) motivates different sparse/crowded priorities; it does not identify causally recoverable throughput. Continue substantial work across all five densities and preserve every failed/lower trial.

Fresh03V3 inputs50021/50022 were generated only after protocol3fc121ad and remain excluded from all tuning. Selected maxima, paired planner-seed gains and fresh-input qualification remain separate. The dashboard and density ledger lead with matched max(NMS,KK); historical published comparisons stay separate.

The user deferred renaming random05/ to pilot/ until the end of development. Only a read-only naming inventory was performed; keep current paths during this campaign.

Fresh RANDOM-03 V3 is complete and independently audited: candidate2660/2622 versus prior2639/2612; NMS2461/2444 and2361/2380; KK2084/2093 and2094/2130. The candidate gains9.110%over max(NMS,KK) and0.590%over its predecessor. All12original runs pass source/input/runtime/bound-affinity/deadline/memory and full replay checks. This misses the10%fresh milestone. Both inputs remain excluded from tuning. [Full report](RANDOM03_FRESH_VALIDATION_V3.md).

General05record2314and selected4302are independently verified/promoted. All seven off controls are six-field exact. General2314exact/paired qualification is complete (+3.4599%); selected4302qualification is incomplete with a retained baseline startup failure. [Record evidence](results/random12345-terminal-pending-split-full-v164/GENERAL05_RECORD.md).

All92chronological selected RANDOM-05 frontier/waiting rows pass the complete audit after4302promotion. Fourteen source165local trials are audited and negative across all five general profiles and selected04. Four selected05cases remain; general2314qualification is complete, while selected4302qualification has a retained baseline timeout.


Current follow-ups: source165local refinement loses on all five general reactive profiles and selected04; fourteen runs are audited, four selected05cases remain. The04serial/parallel five-proposal pair is six-field exact but loses to2782. Frozen source162layout interactions (12cases) test03softened prices/04admission; fixed startup-work budgets (9cases04and4cases05) target observed first-step failures. All use full horizons, preserve explicit trick flags and retain original failures. No new held-out stream has been generated.
