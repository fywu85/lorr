I ran nothing. This is a read of the shadow delta and your verified tables.

## Correctness findings

**Mutation.** I found none.
- The shadow call works on a copy of the proposal.
- It has its own `Stats`, its own cursor and its own ledger.
- It skips `prune_reassignment_records`.
- It returns before the commit block. That block holds the only writes to `reassigned_tasks_`, `last_reassignment_` and `agents_`.
- Oracle access is `peek` and `value` only.
- `include_budget` changes one line of eligibility. Primary, next-primary, recovery, commitment, cooldown, fair and started exclusions are untouched.

**Side effects of running after the real commit.** These are intended, but worth stating.
- Robots moved this tick are excluded by cooldown.
  - The shadow therefore never sees a just-moved task.
  - It measures the policy "budget 2 with cooldown 20".
- Moved robots also cannot nominate `oldest`.

**One new failure mode.** A timeout inside the shadow fails the run. The audit-OFF run would not have failed at that point. This is acceptable.

**Concrete fix.**
- A cycle with one protected row and several unprotected rows books its whole saving to the budget. Part of that saving is reachable by the real pass under a different grouping.
- Add one counter for the saving of witness cycles where every row is protected.
- Report both numbers.
- Your row split alone does not separate the savings.

**Label.** In shadow `work`, `match_budget_protected` counts holders that were included. In the real stats the same field counts holders that were excluded.

## Measurement qualification

Cumulative S2 at stride 100 and cumulative S1 are not the same kind of quantity.

- **What each number is.**
  - S1 is a complete flow. It covers 500 passes, each clearing what it finds, and the budget lets each task count once.
  - S2 at stride 100 is about 50 snapshots of a stock.
- **Why snapshots undercount.**
  - A protected opportunity lives only until pickup.
  - I would guess that is a few tens of ticks after the first move. This is unmeasured.
  - Most protected task-lives therefore fall between samples.
- **Coverage.**
  - Anchor exhaustion also limits each pass to a subset of residents.
  - The pool is larger once protected holders are included.
- **Fix.**
  - Run the full audit at stride 10, which your guard allows.
  - The whole-run ledger then makes S2 a complete, task-disjoint count of first excluded witnesses, the same kind of object as S1.
  - The extra work is one more matching pass per 10 ticks.
- **If you keep stride 100.** Label S2 as snapshot sums and do not divide it by S1.
- **Either way.** S2 stays descriptive. It is chronologically biased, it is measured on the unexecuted trajectory, and it is in guidance units.

## Budget 2 under ordinary fairness

Verified rows, native bands:

| Step | Empty work per task | Tasks |
|---|---|---|
| First-move matching | down 0.84 / 0.80 steps | +458 / +436 (about +0.3%) |
| Weight 5 added | down a further 1.3 / 1.2 steps | +482 / −18 |

- Weight 5 also raised the completed chain by about 0.65 steps.
- Empty work is already 60.3–60.5 steps per task, below your unmatched NMS diagnostic.
- A second move acts on holders with shorter remaining legs that were already optimized once.
- My conjecture is that it yields less than the first move.
- If so, it adds at most a fraction of +0.3%, against a gap of 2,412 tasks.
- Under ordinary fairness, budget 2 is not a gap-closer.
- It stays worthwhile only inside the short arm, conditional on the short results and on a stride-10 S2 measured there.

## Ranked conditional next mechanism

**1. Short preference.**
- The full results are pending.
- If short wins, adopt it and let the in-arm S2 decide on budget 2.

**2. Known-horizon admission, if short loses or S2 is small.**

- **Why.**
  - It is the highest-upside narrowly controlled step I can size from your own accounting.
  - For bands + matching + weight 5 on seed 0, tasks × (empty + chain + overhead) is about 48.6M of the 50M robot-steps.
  - About 1.36M steps, or 2.7% of robot time, produce no completion.
  - I attribute that mostly to work in flight at the end. That attribution is unverified.
- **Zero-host check first**, from existing replays:
  - robot-steps after each robot's last completion;
  - the share of assignments made after about t=4,600 that completed.
  - If the censored work is small, stop here.
- **Minimal principled version.**
  - **Selector.**
    - Use an explicit `--trick` selector that carries the horizon value, for example 5000.
    - Print `known_horizon=… assumption=configured` in the receipt and mark it in every result.
    - Past the configured tick the policy reverts to ordinary behaviour and logs the violation.
  - **Scope.** The rule touches only new assignments of unopened tasks to free robots.
    - Unchanged:
      - started tasks;
      - primary and recovery robots;
      - fair admissions;
      - held unopened tasks;
      - matching;
      - retarget budgets.
    - No task is deleted. If a robot has only infeasible candidates, it still takes one as now.
  - **Rule.** Compute LB = spatial BFS pickup + the existing spatial chain, with Manhattan as the fallback where BFS falls back today.
    - This is a valid lower bound on ticks.
    - Native distance is not a valid bound, because contraflow edges cost 200 for one tick.
    - A pair is provably useless when now + LB > H.
    - Inside `better`, rank provably useless pairs after all others, and keep the existing order within each class.
  - **Optional second tier.** It needs no tuned constant.
    - Let ρ be the running ratio of realized task durations to LB over this run's completed tasks.
    - Rank "ρ·LB ≤ remaining time" first, then "LB ≤ remaining < ρ·LB", then provably useless.
  - **Isolation.** The rule is inert until the first provable demotion.
    - The control prefix must therefore match exactly up to that tick.
    - Log the tick.
  - **Arms.** Run the best profile with the selector off and on, on seeds 0 and 2.
  - **Rejection.**
    - Reject it if tasks do not rise on both seeds.
    - Also reject it if censored work does not fall.
  - **Limits.**
    - No hidden-task access.
    - No claim of general or competition validity.
