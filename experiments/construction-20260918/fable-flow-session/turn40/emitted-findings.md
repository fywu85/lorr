I ran nothing. This is a design read against the simulator semantics you described and your horizon audit. I agree with your qualification that stride 10 does not make the bounded, chronological shadow complete.

## Bound and off-by-one

I found no error in the rule as specified.

- **Tick arithmetic.**
  - The move of iteration t precedes `check_finished_tasks`, and one errand is serviced per tick.
  - So the tick count is `max(1, d) + Σ max(1, legₖ)`, which equals your `max(1, pickup) + chain + repeats`.
  - An admission at tick t that needs k ticks completes at the end of tick t+k−1.
  - The last counted check follows the move of tick H−1, so `LB ≤ H − now` is feasible, including equality.
  - Confirm one fact from a saved trace: the latest recorded completion belongs to iteration H−1. Print `last_counted_tick=H-1` in the receipt.
- **Soundness.**
  - With core equal to free and no pockets, BFS, Manhattan and the clamped chain cache are all lower bounds.
  - A demoted pair is therefore provably useless under the configured H.
  - Mixing BFS and Manhattan changes only completeness, never correctness.

Three implementation notes:

1. **Where the tier is a function of.**
   - It depends on (robot, task).
   - It therefore has to be computed inside `pair_for` and carried on `Pair`.
   - It has to apply in the main `better` sort, in `retain_better`, and in the fallback loop's `min_element` and sampled improvements.
   - Only `fair_admission` keeps the baseline comparator.
   - If the fallback loop stays on the baseline comparator, it undoes the retention of feasible tasks for robots that lost their candidates to conflicts.
2. **Lookup cost.**
   - In the final window the full-field scans evaluate on the order of 100k pairs per tick.
   - Use `c_t = chain + repeats`, cached per task.
     - If `c_t + 1 > R`, the pair is useless for every robot.
     - Otherwise test Manhattan first.
     - Peek the spatial table only when Manhattan passes.
   - Taking the larger of the available bounds keeps the rule sound and keeps lookups few.
   - Measure the added time in the window before trusting it.
3. **Reversal detection.**
   - It has to live inside the comparator wrapper.
   - Record any comparison where the tiers differ and the baseline order flips.
   - That way retention-heap comparisons count as well as the final sort.
   - The earliest recorded tick is then a safe lower bound for the exact-prefix assertion.

## Fairness

- **No structural problem.**
  - Forced-oldest admission is untouched. It fires about once per 10,000 admissions.
  - HRRN aging continues inside each tier.
  - No task is dropped, and no held or started task changes.
  - Old long tasks are demoted only once they are provably useless. Their extra wait is bounded by the window.
  - Label that effect in the result.
- **One interaction to count, not change.**
  - Matching inside the window minimizes pickup cost, not feasibility.
  - A passenger row can therefore move an LB-feasible task onto a holder for whom it is LB-infeasible.
  - Add a read-only counter of such moves in the window.
  - Leave matching unchanged in this first mechanism, as you planned.

## Is the two-seed OFF/ON matrix warranted?

Conditionally yes. One zero-host number should come first.

- **What the audit already shows.**
  - About 4.0% of robot time goes to tasks that never finish.
  - Between ticks 4600 and 4899, about 5,660 (seed 0) and 5,620 (seed 2) admissions do not finish.
  - The gap is 2,412.
  - The rule therefore has enough admissions to act on.
  - As you say, those are not recoverable-work estimates.
- **The missing number.** On the saved trajectories, split the late non-finishing admissions into:
  - (a) provably useless at admission, meaning LB > R;
  - (b) LB-feasible but unfinished, which are slack failures.
- **The opportunity count for (a).**
  - Walk the admissions in time order.
  - Count those for which an unassigned task with LB ≤ R existed at that tick.
  - Remove each chosen alternative from the pool as you go.
  - The result is a sequential opportunity count. It is not a throughput estimate.
- **The decision.**
  - Run the matrix if that count is of the order of the gap.
  - If it is a few hundred, the provable tier cannot matter. Skip it and do not spend the hosts.
- **The matrix.** The best profile, OFF and ON, on seeds 0 and 2.
  - OFF is your exact control.
  - Accept only if tasks rise on both seeds.
  - Report completions by final-window band.
  - Report unfinished-task work beside the task counts.

## Smallest subsequent test, kept separate

- **Candidate diversity is not the limit.**
  - Field robots scan every free task.
  - The fallback loop re-scans unused tasks.
- **Two other limits are the likely ones.**
  - The stock of tasks that are short enough.
  - Slack: inside the feasible tier, HRRN favours old tasks whose LB is close to R. Those tasks then miss the horizon, because realized time exceeds LB.
- **The follow-up, if split (b) is a large share.**
  - Add a calibrated middle tier under its own selector value.
  - Let ρ be this run's running ratio of realized task duration to LB over completed tasks. It needs no tuned constant.
  - Rank pairs in three tiers:
    - `ρ·LB ≤ R` first;
    - then `LB ≤ R < ρ·LB`;
    - then provably useless.
  - It reuses the same comparator plumbing, exact-prefix check and receipts.
  - It is a different mechanism, and should be judged against the minimal-rule arm, not merged with it.
