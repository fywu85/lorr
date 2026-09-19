## Recommendation: B

**Facts.**
- Interim audit, physical partition, per sample of about 962 stationary roots: 5 free, 361 one-owner, 175 two-owner, 47 three-plus.
- Four full 4M workers gained 0.2% at 2.2× the CPU.
- Empty travel is 20.6% of robot-steps: 76.6 per task against KK's 65.7.
- About 2,000 robots are en route at any time, and about 27 tasks are revealed per step.
- The harness source permits releasing unopened tasks and keeps `t_revealed`.

**Inference.**
- The one-owner class is already inside the existing search neighborhood. It is twice the two-owner class, yet it survives into the final plan.
- Whatever blocks it applies more strongly to two owners. The cause is either that the displaced owner loses at least what the root gains, or that the chain fails.
- A's ceiling is 1.75% of robot-steps (175 of 10,000), and only at full conversion with zero owner loss.

**Prediction.**
- I expect A to gain at most 0.5%.
- I expect B to gain 1.5–3%.
- A edits the certified core. B sits behind a validator.

**Before building A**, add a static pair check on the one-owner class. For each case, compute root gain, plus the owner's best non-conflicting alternative, minus the owner's current gain, in physical units.
- If most results are ≤ 0, the objective is declining those moves correctly, and A is dead.
- If many are > 0, fix conversion inside the existing neighborhood first.

**If A is ever built**, it needs:
- an explicit undo journal, because a successful first sub-chain leaves no rollback record in `build()`;
- `score_` restored from a snapshot, not by inverse arithmetic;
- the root and both owners marked visited for the whole transaction, because `build()` clears `visited_` on failure;
- one `consider()` call, made on the complete assignment.

## Most serious flaw in sketch B

**The acceptance rule is not system-aligned.**
- Comparing d_rem + chain_old against d′ + chain′ lets a shorter chain pay for a longer pickup.
- System-wide the chain terms cancel, because someone still serves the released task.
- Such exchanges only shift the task mix, which is the job of the HRRN matrix.
- They also discard the travel already spent approaching the old pickup, and they charge a fresh full approach to whoever inherits the released task.

**Fix.**
- Require a pickup saving on its own: d′ + max(4, 10%·d_rem) ≤ d_rem.
- Keep the total only as a non-worsening guard: d′ + chain′ ≤ d_rem + chain_old.
- Remaining pickup distance then strictly falls at every exchange. With one release per task there is no livelock.
- The mix confound you flagged disappears by construction.

**Regression (scheduler-level, on a line graph).**
- R sits at x=0 holding unopened task X, with pickup at x=+10 and chain 100.
- Free task Y has its pickup at x=−12 and chain 60.
- S at x=−40 is finishing an opened task and becomes free 5 steps later.
- Under the sketch rule, R's total falls from 110 to 72, so R exchanges X for Y. S then inherits X at distance 50, and the two tasks cost 72 + 150 = 222.
- Without the exchange, the two tasks cost 110 + (28 + 60) = 198.
- Assert that total pickup-plus-chain over both tasks does not increase with the pass enabled. The sketch fails. The fixed rule passes, because 12 > 10 and no exchange occurs.
- Also assert:
  - `proposed` passes `validate_task_assignment`;
  - no task is both released and re-assigned in one call;
  - the released task keeps `t_revealed`.

**Second flaw: eligibility bias.**
- "BFS must reach both pickups" under a 2048-node bound excludes robots far from their pickup, which are the largest savings.
- Take d_rem from the cached spatial table of the current pickup using `peek`, and skip the robot if the table is absent.
- Run the robot-centered BFS over core cells only, truncated at depth d_rem − margin.
- Source fact, in the certified configuration:
  - For a core goal the oracle BFS traverses core cells only, and unit BFS is symmetric. Both numbers are therefore on one basis.
  - The unrestricted `candidates()` BFS over all free cells is not on that basis.

**Bounded implementation plan.**
- Share one eligibility predicate with `reassign_unopened`.
- Run after regular scheduling and the swap pass.
- Define availability as the free set, minus tasks in `proposed`, minus this call's releases.
- Use a rotating cursor with at most 256 sources and at most 16 candidates each.
- Accept only candidates whose chain legs all have complete cached distances.
- Match greedily one-to-one by pickup saving.
- Cap accepted exchanges per call at about 32.
  - Each exchange creates demand for a new oriented table, against the 32-per-step build budget.
  - Watch `fallback_metric` for the effect.
- Add counters for exchanges, ΣΔd, ΣΔchain and skip reasons.
- Leave the feature off by default.

## Offline screen: current information only, no double counting

- Replay a saved run causally.
- At step t, consider pairs of an eligible en-route robot r and a task Y that is free at t and not yet virtually consumed, where the pair satisfies the fixed rule.
- Match greedily one-to-one by saving.
- On each match, credit d_rem − d′ exactly once. Then:
  - retire Y for the rest of its real free lifetime;
  - close r's current assignment episode;
  - mark r's old task as released.
- Do not propagate any effect into the future.
- Report credited steps per completed task, exchanges per 1000 steps, and ΣΔchain as a check.
- I predict at least 5 credited steps per task. If it is under 3, drop B.

## Interpretive notes

- **Inference.** Legacy cache-only seed 2 still collapses late (102,988, with a last window of 10,650). Periodic perturbation did not avoid the collapse, so that instability looks robust and not one unlucky path.
- **Refuted prediction.** Scales 2 and 4 both lost. That refutes my earlier suggestion that smaller tolls would help under refresh, and full-step conventions work better.
- **Fact.** The free-candidate sweep opportunity is about 5 roots per sample, so my sweep idea is dead.
- **HRRN matrix.** Let it set the dispatch mix. Evaluate B under whichever setting wins, and read B's gain from empty steps per task, not from chain means.
