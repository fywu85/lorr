I ran nothing; this is a read of the patch. `better`, `retain_better`, `oldest_task()` and the counter reset inside `fair_admission` are outside the excerpt, so the points that depend on them are marked.

## Activation and isolation

I found no activation bug in the C++ gating.

- The selectors are read once, before `hrrn_` is parsed.
- The driver, `initialize` and the benchmark all reject a selector without `--trick`.
- Map identity is validated in all four arms.
- Flag-only behaviour stays lanes 1, short 0.
- `static_trick_metric_` now means lanes only. Its uses that I know of are lane-specific:
  - the static costs;
  - the learned-flow gate;
  - `pickup_metric` from tick 1.
- The remaining-flow and chain-pricing guards correctly widen to any trick.
- A free acceptance check: within one lanes arm, short 0 and short 1 must give identical assignments at t=0.
  - Every task has age 0 at t=0, so the HRRN score is exactly 1.0 in both arms.
  - The forced admission cannot fire until `regular_admissions_` reaches `n_` = 10,000, which happens only on the last initial assignment.

## Findings

1. **The "short" factor is almost entirely `CGAR_HRRN=0`.**
   - Forced-oldest admission fires once per 10,000 regular admissions. If the counter resets on each firing, that is about 15 tasks in a 5,000-step run. The `fair=` field in any reference log confirms the count.
   - The main mechanism therefore stays reachable in the generic track through the environment alone, by setting `CGAR_HRRN=0`.
     - Your earlier HRRN=0 runs were booked as generic.
     - Either keep `CGAR_HRRN=0` as a generic ablation, or reject `CGAR_HRRN=0` without `--trick`.
     - As it stands, the labelling rule has a hole.
   - The arm also bundles variants you have already measured.
     - HRRN=0 gave 131,199 and 130,947. Completed chains fell to 235.27, and empty travel rose 17.0% and 17.6%.
     - If `retain_better` defers to `better`, as the V42 description says, `hrrn_=false` also turns complete-field retention into cost-only retention. That is the cost-key variant that lost 2.248%.
     - Both results come from older baselines without lanes. They are not a prediction for this trial, but they say which table to read: chain against empty work.

2. **The benchmark can mislabel a run.**
   - `expected_components` is built only when a selector appears in `environment`.
   - Suppose the child process inherits the shell environment and the shell carries `CGAR_TRICK_SHORT_TASKS=1`.
     - A `--trick` run then executes lanes plus short.
     - Its manifest records no components.
     - The receipt check still passes on the lane hash alone.
   - When `--trick` is set, always build and verify the components line, using the defaults for unset selectors.
   - Refuse selectors that arrive from the ambient environment.
   - I cannot see how the child environment is assembled.

3. **Equality of the 0/0 arm with generic needs a matched reference.**
   - Compare it against generic from the same binary, at the same 5 s limit, on the same host class.
   - If generic at 1 s differs from generic at 5 s, that shows clock dependence. It does not show trick contamination.
   - No regression asserts the equality.
     - Any later code keyed on `trick_instance` instead of the component flags would silently break the control.
     - A short closed-loop comparison on the warehouse fixture would catch that.

4. **The interaction term carries a pricing-basis confound.**
   - Chain is priced as the spatial chain times 4 in every arm. Pickup is priced in the active oriented metric.
   - That metric is 4/16 under static lanes, and at most 8 under learned flow at strength 4.
   - Under cost-only ranking, the two lanes arms therefore trade pickup against chain at different rates.
   - A short-by-lanes interaction may come from units and not from traffic.
   - `pickup_estimate` and `chain_estimate` at assignment already exist. Report them per arm.

5. **The fallback pass keeps one age pathway.**
   - The fallback pass still seeds `oldest_task()` as a candidate.
   - Under cost-only `better` it has no priority, so "may wait indefinitely" still holds.
   - Report `fallback=` so the pathway is known to be rare.

## Test gaps

- **The changed guards are not tested where they changed.**
  - The old rejection cases use flag-only runs with lanes 1. Those cases pass under both the old and the new expressions.
  - Add remaining-flow and chain-pricing rejection with `CGAR_TRICK_LANES=0`.
  - Add one case for the new general guard, for example lanes 0 with `CGAR_REASSIGN_POOL=1`.
- **The lanes-0 arms have no positive assertions.**
  - Assert that the learned flow publishes.
  - Assert that an oriented table equals the generic one.
  - Assert that `pickup_metric` is false at tick 1.
  - For lanes 1 with short 1, assert that the static field matches the lanes-only field.
- **The fixture cannot make retention bind.** It has one robot and two tasks, so the top-16 retention never applies.
- **Two smaller points.**
  - An unknown trick name now fails inside the new `options` try-block.
    - Check that the message and exit code still match what existing tooling expects.
  - The receipt prints `learned_publications=enabled` even when `CGAR_FLOW_STRENGTH=0`.

## Measurement

- Report chain saving per 1,000-step window.
- Report the summed chain of the free pool at each window end.
- The sum of revealed chain equals completed chain plus open chain. That is an identity, not an estimate. Any completed-chain saving must therefore appear as stranded stock in the pool.
- Your checkpoints can then answer an empirical question: does the saving persist once the free pool is mostly stranded tasks, while any empty-work cost continues?

## One follow-up if the trial does not improve

- **When to run it.** Run it if short 1 shows the HRRN=0 signature, meaning completed chain down and empty work up by more.
- **What to run.** Short-task preference combined with bounded late binding.
  - Use the existing `exchange_unopened_with_pool`, enabled by a third selector gated by `--trick`.
  - The new guard currently rejects it.
  - It is age-free and work-bounded.
    - It ranks by pickup saving.
    - It accepts a swap only when `new_total <= old_pickup + old_chain`.
  - It already has regression coverage.
  - Your measured +0.186% under HRRN is unrelated to this use.
- **Why this one.** The evidence is your KK keep-arm.
  - Under cost-only matching without retargeting, empty work went from 65.7 to 88.9.
  - Chains stayed at 235.0.
- **What it is not.** It is one bounded step toward that mechanism. It is not the per-tick full rematch that NMS and KK use.
- **What counts as success.** Empty work returns toward the control while the chain saving remains.
