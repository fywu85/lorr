I ran nothing. This is a read of the two deltas plus your verified tables.

## 1. Correctness check

**Relative Dial index.** Correct by reading, with no defect found.
- `current_bucket` stays congruent to `current`, because the two are incremented together.
- Every offer is `current` plus an edge in [1, cost_limit], apart from the initial zero offer. So `current_bucket + delta` stays below `2*bucket_count`, and one subtraction is enough.
- Only the initial offer ever lands in the bucket currently being drained. Stale entries are still rejected by the `distance_[state] != current` test.
- The scan cannot alias, because every delta is at most `bucket_count - 1`.
- With 17 buckets the pop order is identical to `% 17`, which agrees with your equal distances, pops and states.
- Buckets are cleared at entry, so a timeout in the middle of a field leaves nothing behind for the next field.

**Native matching.**
- The `unit_limit` plumbing is correct and the default callers keep the limit of 16. I found no correctness or isolation defect.
- Verified from the source: under native, matrix entries price a turn at 1/20 of a forward step.
- Conjecture: this rarely matters, because the 10% rule screens most such cycles.
  - The acceptance rule (4 units and 10%) and `match_saving` therefore no longer approximate physical ticks.
  - A cycle that trades a few forward steps for many turns can now pass.
- This is part of the compound repricing and is not a bug. In reports, divide `match_saving` by the unit before comparing profiles.

## 2. The six-profile comparison

- Yes, run it. It is configuration only, and it composes three components that each improved both seeds on the legacy metric.
- Weight 5 under native has the same form as the NMS scheduler cost you showed, 5·pickup + chain in native units.
- The matching64 at weight 1 arm has the least value. It only attributes the increment between matching and weight, and it does not advance the best score.
  - If hosts are tight, run that arm on one seed.
  - The unmatched controls and the weight-5 arms are the essential ones.
- Report the usual replay rows per arm, and add `moved/assignment` and `budget_protected`.
- One addition informs mechanism (c) without a policy run: a read-only shadow pass at low cadence.
  - Every 100 ticks, group all resident holders, including the budget-protected ones.
  - Solve the permutation, log the accepted-cycle saving that the budget excluded, and commit nothing.
  - It uses peeks only, so the trajectories must stay exact.

## 3. Ranking two next mechanisms if about 2–3% remains

**First: (a) short preference on native + matching64 + weight 5.**

Evidence, all verified:
- The completed chain stays near 242.3 in every ordinary-fairness arm, against 234.8 in the NMS diagnostic.
- Empty work is already 63.0 against 62.0.
- Chain is therefore the largest remaining accounted component, and only task selection moves it.
- The short penalty shrank each time empty-travel handling improved:
  - about −6% for short alone;
  - about −3% with matching64;
  - about −0.4% with matching64 and weight 5 (V70: 146,873/146,820 against 147,457/147,495).

Conjecture: native pricing and the pure-potential score shift this further. They may not.

Design:
- It is a guard change only. Two guards currently reject short: the remaining-flow guard and the native guard. Relax both explicitly.
- The `CGAR_TRICK_SHORT_TASKS` selector and the components receipt already exist.
- Started-task, primary, recovery and finite-retarget protections stay unchanged.
- Regression additions:
  - the age test and the forced-oldest-independence test under native settings;
  - started long task not redirected;
  - one native beneficial cycle with short on.
- Arms: the best native field + matching64 + weight 5, with short 0 and 1, on seeds 0 and 2. That is two new runs against the top arm from step 2.
- Readouts:
  - tasks;
  - final-1000 rate;
  - completed chain;
  - empty work per task;
  - loaded overhead;
  - age tails;
  - moved per assignment.
- Reject short if it is below the matched ordinary-fairness arm on both seeds. Short then stays off under native as well.

**Second: (c) a second finite retarget per task, tested inside the short arm, and only if (a) has left a residual empty penalty after shortening chains.**

Evidence:
- Under ordinary fairness the empty gap to the NMS diagnostic is about one step per task, so a second move has little left to recover there.
- Under short the one-move budget visibly removed most holders in your earlier exposure sample.
- The shadow log from step 2 sizes the excluded saving before anything is built.

Design:
- Change the budget from 1 to 2. Keep the cooldown of 20 and keep every protection.
- The default of 1 stays exact.

Conjectural risk:
- Each retarget resets ticket, `best` and `stall` in `sync_agents`. A second move may therefore lengthen waits on empty legs.
- Read empty-leg waits, not logged savings.
- The finite budget supports no liveness claim.

**Not ranked: (b) multiworker global search.**
- Your only direct evidence is negative: four global 4M searches gave 143,312/144,336 against 144,510/144,107.
- Nothing measured says the native objective changes that.
- I would revisit it only if the native replay leaves loaded waits as the dominant gap after (a).
