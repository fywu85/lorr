I ran nothing, and I see no correctness defect in the design as described. Four things could make the comparison between arms misleading. I think issue 1 is decisive.

**1. Mode 2 changes two things at once, so a constant-ratio placebo arm is needed.**
- Against OFF, mode 2 adds oriented information. It also raises every priced chain by about the ratio κ.
- That second change rebalances pickup against chain inside `W × pickup + chain`, and it shrinks the age term.
- Your pickup-weight runs showed the scheduler is sensitive to exactly that rebalancing.
- The placebo arm prices every task at `ceil(U × native chain × κ)`. It uses the κ that mode 2 computes on the same call and no per-task oriented values.
- Reading the result:
  - If the placebo is about equal to mode 2, the effect comes from the mean shift and not from the information.
  - If mode 2 beats the placebo, that supports the information.
- I would run OFF, mode 2 and the placebo as the primary three arms. Mode 1 is the optional fourth.

**2. Whether a task is covered depends on its destination, not on chance.**
- A task is covered only if every future destination already has a resident table. That happens when the cell is, or recently was, some robot's goal.
- Coverage therefore tracks how popular and how recently used the destination is.
- In mode 1, uncovered tasks always look cheaper, so mode 1 implicitly favours tasks going to unpopular destinations.
- A mode-1 gain could come from that artifact alone.
- If mode 1 beats mode 2, and mode 2 is about equal to OFF, the artifact explains the result and the pricing does not.
- Please report whole-chain coverage against ticks since the last publication. If steady-state coverage is near complete, modes 1 and 2 differ only in the windows after each flush. The three-arm contrast then has little power.

**3. The denominator of κ mixes two bases.**
- The native chain is BFS where a spatial table exists. It is Manhattan where the table budget ran out, and the result is then cached.
- A missing oriented table and a missing spatial table are plausibly correlated. I have not checked this.
- If they are, imputed tasks more often carry a Manhattan native chain. That chain is an under-estimate, so κ leaves a residual discount on exactly those tasks.
- The minimum diagnostic is the share of Manhattan-derived native legs among covered tasks and among imputed tasks.
- If the two shares differ materially, compute κ within each basis. The alternative is to treat the mode-2 result as still biased.

**4. A snapshot κ is noisy just after each publication.**
- With only a handful of covered tasks, one task with many turns can move κ by tens of percent.
- The imputed chains then shift against pickup cost for every task on that call.
- An alternative with no threshold is to pool the two sums over all calls since the last publication and reset them at each publication. That is deterministic and uses only current information.
- Whichever you choose, log the covered count each time imputation is used.

**Smaller points:**
- **Structurally unavailable tasks.**
  - If an earlier stop lies in a pocket outside the destination table's domain, the state is infinite and the task can never be covered.
  - The native chain handles that case through exit cells, so the two treat it differently.
  - Count these tasks separately from cache misses. They are imputed on every call.
- **Bounded local search.**
  - The `at_cell` bucket order uses `pair_for` with a zero distance.
  - Pricing therefore also changes which tasks survive truncation for robots without a complete field.
  - This is deterministic and is part of the treatment, but it means candidate discovery does not stay the same.
- **Fair admission.**
  - For a fixed task the chain term is common to all robots, and the score is monotone in pickup cost.
  - The chosen robot should therefore be identical in every arm.
  - That is worth asserting in a test.
- **Shadow counter.**
  - Count robots whose native first choice differs from their priced first choice, before conflict resolution.
  - Coverage counts alone cannot show how often the ranking actually moved.
- **Latency.**
  - The per-call cost scales with the number of free tasks times the remaining legs per task, and each peek is likely a cache-missing read.
  - Your maximum entry is already 962 ms. Measure the ticks around each publication with the option ON against OFF before the full runs.

**Expected size of the effect:**
- Your audit shows a real predictive signal: MSE is 5.5% lower and all six folds improve.
- The improvement is about half a step of MAE, on chains of roughly 240 steps.
- The age term dominates the score, and every task is eventually served.
- I therefore expect a sub-percent effect in either direction. Seeds 0 and 2 alone will not resolve it, which is why six seeds come before any conclusion.
- The audit's baseline is exact BFS. Production's native chain is partly Manhattan. The production contrast therefore contains a second, smaller correction that the audit never measured.

Nothing else in the plan looks likely to invalidate it. Recomputing on every call, never mutating the LRU, falling back for the whole task, 128-bit products and leaving the OFF arithmetic untouched all look right.
