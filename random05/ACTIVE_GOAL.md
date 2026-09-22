# Active PILOT goal: all five RANDOM instances

Updated 2026-09-22 05:09 UTC.

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

**RANDOM-04:** Selected2799 is115 tasks below2914. Whole-chain length price0.5, seed0, passes full replay and repeats exactly; its original run peaks984.795ms and seed1 fails startup at1046.301ms. That original configuration is not robust. Exact optional-column compression completes candidate seeds0/1 at2799/2771, max848.301/859.270ms. Their total5570 is one task below compressed0.25 controls2776/2795; this is not a paired throughput gain. The additional paired seeds 2/3/4 are complete, with three startup timeouts: 1039.572 / 1144.865 / 1041.974 ms, including two controls. The three completed pairs 0/1/3 total 8,331 candidate versus 8,347 control (-0.1917%); full five-pair qualification fails. Compression alone has not made runtime robust. The preceding0.25 recipe reaches2795, repeats exactly and improves+0.5714% across five pairs. No fresh04 qualification yet. [Runtime evidence](results/random04-chain-runtime-split-full-v166/REPORT.md).

**RANDOM-05:** Selected4302 repeats exactly. Three valid pairs improve+1.0477%, but the disabled-credit seed3 control fails startup at1045.313ms; the full four-pair qualification is not achieved and no historical score substitutes for that failure. General2314 repeats and gains+3.4599% across four pairs. FreshV6 qualifies4175 only and is NMS-only. All92 chronological selected frontier/waiting rows remain audited. [Detailed history](../RANDOM05_PROGRESS.md).

## Next work

The exact 745 repetition and compressed RANDOM-04 qualification are complete. Preserve all three audited startup failures and the unsuccessful paired comparison. Address startup matching/search cost next. Report every result and preserve failures. Keep RANDOM-01/04 as the main new development priorities; retain all five in scope for general transfer.

The completed source167 all-five forecast comparison has19 valid runs and one retained general02 timeout; all eight disabled controls are exact. It improves the selected sparse01 profile without a broad generalization claim. R04 diagnosis on common completed orders finds more loaded steps than NMS, while most of the final score lead appears in the last100 steps. Target earlier and sustained traffic progress, and distinguish admission/release waiting from travel. [Diagnosis](results/random04-task-transitions-v166/REPORT.md).

Current records, timestamps and sources are in [PILOT_PROGRESS.md](../PILOT_PROGRESS.md), [the density ledger](RANDOM_PROGRESS.md) and [the record manifest](random-frontiers.json). Operational continuation details are in [NEXT.md](NEXT.md).
