# PILOT campaign: all five RANDOM instances

Updated 2026-09-22 08:17 UTC.

**Status: paused at the user's request. Reliability repair and final reporting are complete.**

No automatic throughput search, benchmark submission or fresh-input generation is authorized by the unfinished broader goal. The completed work is logged with source and evidence links; the final scoped commit and push complete the handoff.

The broader objective remains maximizing verified full-run combined-track throughput on RANDOM-01 through RANDOM-05, always compared with matched **max(NMS, Kitty Knight)**. The latest development priority was RANDOM-01 and RANDOM-04; neither has a demonstrated inherent ceiling. The stored app goal's NMS-only wording and old RANDOM-04 target of 2,838 are superseded by this log. Do not mark that goal complete or recreate it merely to edit its wording.

| Instance | Mean ± sample SD (10 planner seeds) | Selected best | Matched max(NMS, KK) | Best lead | 10% reference |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 738.7 ± 3.2 | 745 | 692 | +7.66% | 762 |
| RANDOM-02 | 1,394.9 ± 6.0 | 1,408 | 1,256 | +12.10% | 1,382 |
| RANDOM-03 | 2,616.5 ± 14.7 | 2,646 | 2,359 | +12.17% | 2,595 |
| RANDOM-04 | 2,773.2 ± 15.1 | 2,799 | 2,649 | +5.66% | 2,914 |
| RANDOM-05 | 4,230.1 ± 46.5 | 4,302 | 3,172 | +35.62% | 3,490 |

The final selected cohort has **50 successful full runs / 50,000 planning calls**, all independently replayed under strict 1,000 ms entries and 32 decimal GB. Worst entry is **885.879 ms** and peak RSS **500.015 MB**. RANDOM-01/02 reuse their already valid frozen observations; RANDOM-03 uses source173 and RANDOM-04/05 source174. Three incorrectly bound allocations were refused before any solver launch, then retried once each with identical settings. Their refusals remain in the audit.

This is sample SD (n−1) across planner seeds on fixed archived inputs, with development-selected trick presets. It does not measure fresh task/start or unseen-layout variability. All 29 available prior successful full trajectories for the repaired RANDOM-03/04/05 cohorts match exactly; RANDOM-04 seed8 has independent replay but no prior successful full reference. Original and intermediate deadline failures remain preserved. [Frozen statistics and runtime report](results/random45-reliability-dynamic-split-full-v174/REPORT.md).

## Evidence and limits

- Full horizons: 600 / 600 / 800 / 1000 / 2000. Strict 1,000 ms entries, 30-second preparation, 32 decimal GB; 16 bound physical EPYC9354 cores / 32 SMT workers, verified affinity and no CPU quota. Shared hosts are allowed. Observed timing is not a guarantee under arbitrary contention.
- Complete the declared fixed work or fail. Runtime repairs do not return partial solutions, relax deadlines or alter search budgets, objectives, seeds or horizons.
- Independently replay movement, collisions, assignments and task events. Pin source, timestamp, binary, input, settings, latency, memory and completed/censored waiting metrics.
- Separate selected maxima, exact reproduction, planner-seed statistics, runtime qualification and fresh-input validation. Keep failed attempts and losing trials.
- General bests remain 727 / 1397 / 1634 / 1616 / 2314. Current selected presets use explicit `--trick RANDOM-0N` for tuned guidance, admission and known-horizon rules. Throughput is primary; fairness is not established.
- Five densities share one map layout. Non-random maps remain unevaluated placeholders; frozen distance storage was estimated above the 32 GB budget.
- Inputs 50001–50012 and 50015–50022 remain excluded from tuning. RANDOM-04 inputs 50013/14 stay reserved and ungenerated. Freeze future protocols before generation.
- Scope edits/staging/commits to `random05/`, `PILOT_PROGRESS.md`, `RANDOM05_PROGRESS.md`. Public `fywu85/lorr` pushes are authorized. Preserve other shared work and repository visibility. Naming cleanup is deferred. No subagents or Fable retries without new authorization/information.

## Remaining qualification on an explicit future resume

RANDOM-01's selected 745 recipe has five paired seeds (+1.0914%, four gains and one tie). Fresh V1 qualifies the older 729 recipe only. RANDOM-02's 1,408 recipe has fresh validation at +10.17% versus matched max.

RANDOM-03's 2,646 recipe has five positive paired seeds (+0.4221%). Fresh V3 yields 2,660 / 2,622 versus matched maxima 2,461 / 2,380: +9.109688% aggregate, below the 10% fresh milestone. All twelve fresh runs are independently audited. [Fresh V3](RANDOM03_FRESH_VALIDATION_V3.md).

RANDOM-04 remains 115 tasks below the 2,914 yardstick and has no fresh qualification. Its former source166/source168 implementations and all startup failures are retained. The current source174 replacement changes runtime only. The preceding length0.25 recipe's five-pair +0.5714% gain is separate; length0.5 has not shown a paired mean gain over it.

RANDOM-05's 4,302 recipe still has only three valid terminal-credit comparison pairs (+1.0477%); its disabled-credit seed3 control failed at startup. The declared four-pair qualification remains incomplete, regardless of the new ten-seed candidate reliability results. Fresh V6 qualifies 4,175 only, NMS-only. The longest completed selected order is 1,986 steps, with initial orders still unfinished at 2,000; eventual maximum latency is censored.

The source170 diagnosis is complete: exact 745/2,799 controls and all 1,600 suppression masks validate. RANDOM-01's suppressed loaded steps already exceed the physical deadline lower bound. RANDOM-04 has 119 opened unfinished orders, 21 within a five-action physical lower bound; this ignores traffic and is not a throughput recovery estimate. [Diagnosis](results/random14-triage-diagnostic-split-full-v170/REPORT.md).

[PILOT dashboard](../PILOT_PROGRESS.md), [density ledger](RANDOM_PROGRESS.md), [selected manifests](random-frontiers.json), [handoff](NEXT.md). Resume only on explicit user request.
