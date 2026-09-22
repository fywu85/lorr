# PILOT paused handoff

Updated 2026-09-22 08:17 UTC. The user requested wrap-up, standard deviations, reliability repair before reporting, then stop. That bounded work is complete. The final scoped commit/push and paused app goal close this handoff. **Do not resume throughput search automatically.** No benchmark jobs remain outstanding from the final campaign batches.

## Completed checkpoint

Best scores: **745 / 1408 / 2646 / 2799 / 4302** against matched **max(NMS, KK) = 692 / 1256 / 2359 / 2649 / 3172**. Best leads: **7.66 / 12.10 / 12.17 / 5.66 / 35.62%**.

Ten planner seeds per instance give mean ± sample SD:
**738.7 ± 3.2 / 1394.9 ± 6.0 / 2616.5 ± 14.7 / 2773.2 ± 15.1 / 4230.1 ± 46.5**.
All 50 selected full runs / 50,000 planning calls pass strict 1-second entries, 30-second preparation, 32 decimal GB and independent replay. Worst entry885.879ms; peak500.015MB RSS. This is development-profile planner-seed variability on fixed archived inputs, not fresh-input uncertainty.

[Final report](results/random45-reliability-dynamic-split-full-v174/REPORT.md), [frozen protocol](experiments/random12345-final-reliability-seeds10-v174-protocol.json), [selected manifests](random-frontiers.json). Source/input/binary hashes and all exact runtime flags are pinned there. RANDOM-01/02 retain original sources167/132. RANDOM-03 selects source173 (`8870903e3693583223cc13132d56785ce8465f76`), RANDOM-04/05 source174 (`2d893f96e78d3fcc092c37a03a7b1888880b306e`). First-attained sources/timestamps are preserved separately. No throughput gain is attributed to the runtime repair.

Final runtime implementations use portable IPO, exact SIMD Hungarian scans, a synchronous persistent worker and OpenMP binding. RANDOM-04/05 also use dynamic scheduling of independent fixed work; RANDOM-05 adds passive OpenMP waiting and existing redundant-matching skips. Every budget/objective/seed/horizon is unchanged. All 29 available prior successful full trajectories for repaired03/04/05 match all six fields;04seed8 is the first successful full observation and has independent replay.

## Evidence to retain

- `random12345-wrapup-seeds10-split-full`: original frozen attempt, five startup timeouts. RANDOM-01/02 ten valid observations are reused in the final cohort.
- `random345-reliability-bound-split-full-v171`: IPO/binding alone insufficient, seven timeouts and one preflight refusal.
- `random345-reliability-persistent-split-full-v173`: RANDOM-03 all ten valid and exact;04/05 still fail three early deadlines, plus three preflight refusals. This attempt qualifies03only.
- `random45-reliability-dynamic-split-full-v174`:17 valid full runs (seven04, ten05), three04 binding refusals before solver launch.
- `random04-dynamic-allocation-retry1-split-v174`: one predeclared identical retry for each refused04seed7/8/9, all valid. The refusals remain in the original audit; no started failure is silently replaced.
- SIMD/static-dynamic/worker regressions pass; source173/source174 IPO builds take69.58s/64.35s for their tests. Eighteen source174 startup controls pass and reproduce exact prefixes. Short probes are not throughput records.

## Constraints for a future explicit resume

All five instances stay in scope; the most recent priorities were RANDOM-01/04. General bests727/1397/1634/1616/2314. RANDOM-04 needs115tasks to2914; RANDOM-01 needs17to762. RANDOM-03 freshV3 is+9.109688% versus matchedmax, below10%. No inherent ceiling is established. Do not mark the broad app goal complete; its old NMS-only RANDOM-04 target is stale.

Only edit/stage/commit `random05/`, `PILOT_PROGRESS.md`, `RANDOM05_PROGRESS.md`. Public `fywu85/lorr` pushes are authorized; preserve visibility and shared CGAR changes. No directory rename or subagents. Fable provider-credit failure has no new resolution; do not retry blindly.

Full horizons600/600/800/1000/2000; strict1000ms entries,30000ms preparation,32decimalGB. Reserve16boundphysicalEPYC9354cores/32SMTworkers, verify actual affinity/no quota; shared hosts allowed. `research35` refused the expected binding in final jobs; do not count such allocations as solver outcomes. Complete fixed work or fail; no time-truncated partial plan.

Tuned guidance, admission and horizon rules require explicit `--trick INSTANCE`. Throughput primary; retain completed/censored waiting metrics. Inputs50001–50012 and50015–50022 remain excluded from tuning;04inputs50013/14remain reserved/ungenerated. No fresh inputs were generated during reliability repair. See [ACTIVE_GOAL.md](ACTIVE_GOAL.md) for qualification limits and the completed diagnosis.
