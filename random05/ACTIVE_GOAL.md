# Active PILOT goal: all five RANDOM instances

Updated 2026-09-22 06:02 UTC.

**Maximize verified full-run combined-track throughput on RANDOM-01 through RANDOM-05. Give each instance substantial development effort, and compare every headline result with matched max(NMS, Kitty Knight). The latest user steering makes RANDOM-01 and RANDOM-04 the main priorities.** Neither priority instance has a demonstrated inherent ceiling.

The RANDOM-03/04 ten-percent qualification is a milestone within this broader campaign. The stored app goal has older NMS-only wording and a superseded RANDOM-04 target of 2,838; current comparisons use 2,649 and a target of 2,914. Do not falsely complete or recreate the goal to change that text.

| Instance | Selected maximum | Matched max(NMS, KK) | Lead | 10% reference |
|---|---:|---:|---:|---:|
| RANDOM-01 | 745 | 692 | +7.66% | 762 |
| RANDOM-02 | 1,408 | 1,256 | +12.10% | 1,382 |
| RANDOM-03 | 2,646 | 2,359 | +12.17% | 2,595 |
| RANDOM-04 | 2,799 | 2,649 | +5.66% | 2,914 |
| RANDOM-05 | 4,302 | 3,172 | +35.62% | 3,490 |

The all-instance ten-percent references are useful yardsticks; only RANDOM-03/04 were explicitly assigned that numeric qualification requirement. Crossing a yardstick does not complete the open-ended goal. General bests remain 727 / 1397 / 1634 / 1616 / 2314. These five densities share one map layout; this is not unseen-map validation.

## Evidence and limits

- Use full horizons 600 / 600 / 800 / 1000 / 2000, strict 1,000 ms entries, 30-second preparation and 32 decimal GB. Use 16 bound physical EPYC9354 cores / 32 SMT workers, verifying affinity and no CPU quota. Shared hosts are allowed.
- Complete the declared fixed work or fail. Do not turn PILOT into a deadline-truncated search or emit partial assignments.
- Independently replay movement, collisions, assignments and task events before promoting a valid selected run. Pin source, timestamp, settings, latency, memory and completed/censored waiting metrics.
- Distinguish a selected maximum, exact reproduction, paired planner-seed behavior, runtime robustness and fresh task/start validation. Preserve every original timeout, refusal and losing trial.
- Keep general mechanisms separate from tuned map, admission and known-horizon presets, which require explicit `--trick RANDOM-0N`. Throughput is primary; no fairness claim follows from a higher score.
- Freeze fresh protocols before generation. Inputs 50001–50012 and 50015–50022 are excluded from tuning; 50013/14 remain reserved and ungenerated for RANDOM-04.
- Scope writes to `random05/`, `PILOT_PROGRESS.md` and `RANDOM05_PROGRESS.md`. Public `fywu85/lorr` pushes are authorized. Preserve other work and repository visibility. Naming cleanup to `pilot/` is explicitly deferred.

## Current qualification

**RANDOM-01:** Source167 forecast assignment at radius8 reaches 745 on planner seed4, with mean70.901/max119.472ms. Five paired seeds give 740/739/742/739/745 versus 736/739/726/732/732: **+1.0914%**, four gains and one tie. All pass strict checks and full replay; maximum qualification entry119.523ms. The earlier740 and742 trajectories repeat exactly. The exact745 repetition also matches all six fields. The larger-radius follow-up does not beat745; general01 variants lose. Fresh01V1 qualifies the older729 profile only. [Qualification](results/random01-record740-split-full-v167/REPORT.md).

**RANDOM-02:** Selected1408 and general1397 remain. Fresh02V1 qualifies1408 at+10.17% over matched max. The new general forecast radius4 run fails at step2/1016.576ms and remains in the evidence. No cause is inferred solely from that timing.

**RANDOM-03:** Selected2646 is exact and gains+0.4221% over five paired seeds. Frozen freshV3 gives2660/2622 versus matched maxima2461/2380: **+9.109688%**, below the10% fresh milestone. All12 fresh runs pass independent checks; both inputs remain excluded from tuning. [Fresh report](RANDOM03_FRESH_VALIDATION_V3.md).

**RANDOM-04:** The selected maximum remains 2,799, 115 tasks below 2,914. Its selected implementation now uses source168, `R05_MATCH_SKIP_ZERO=1` and `R05_COMPACT_IDLE=1`: mean474.550/max664.721ms. Five contemporaneous off/on planner-seed pairs score2799/2771/2761/2761/2766 and match every complete trace field; optimized maximum across the five is669.091ms. A separate exact repetition peaks657.644ms. All runs pass independent replay and strict limits. The original source166 first-attained record and startup failures remain preserved; this is a runtime replacement, not a throughput increase. The preceding length0.25 recipe's five-pair gain remains separate; no new claim that length0.5 improves its average is made. All six new guidance layouts lose; retain layout15/one flip. No fresh04 qualification yet. [Runtime report](results/random12345-zero-update-split-full-v168/REPORT.md), [layout comparison](results/random04-chain-layout-split-full-v168/REPORT.md).

**RANDOM-05:** Selected4302 repeats exactly. Three valid pairs improve+1.0477%, but the disabled-credit seed3 control fails startup at1045.313ms; the full four-pair qualification is not achieved and no historical score substitutes for that failure. General2314 repeats and gains+3.4599% across four pairs. FreshV6 qualifies4175 only and is NMS-only. All92 chronological selected frontier/waiting rows remain audited. [Detailed history](../RANDOM05_PROGRESS.md).

## Next work

The source168 all-five runtime comparison is complete: 18 independently audited runs, 16 historical exact comparisons and nine exact off/on pairs. Every declared search step remains; no throughput change is claimed. All 29 RANDOM-01 forecast cap/weight trials and eight RANDOM-04 layout trials are also complete and audited. Retain RANDOM-01 radius8/max32/weight1 and the current RANDOM-04 layout. No benchmark jobs remain active.

Continue prioritizing RANDOM-01/04 throughput while keeping all five in scope. Next diagnose which end-of-run effort goes to unfinished orders and whether current horizon suppression abandons physically short remaining chains. Distinguish optimistic physical bounds, observed traffic delays, and admission/release waiting. The existing suppression auditor only reconstructs the older hop-only estimate; it must be extended or replaced before applying it to current guided/progress estimates. No change or recoverable-throughput claim is justified yet. Sustained traffic progress remains a RANDOM-04 target; its earlier common-order diagnosis found more loaded steps than NMS and most of its final lead emerging in the last100 steps. [Diagnosis](results/random04-task-transitions-v166/REPORT.md).

Current records, timestamps and sources are in [PILOT_PROGRESS.md](../PILOT_PROGRESS.md), [the density ledger](RANDOM_PROGRESS.md) and [the record manifest](random-frontiers.json). Operational continuation details are in [NEXT.md](NEXT.md).
