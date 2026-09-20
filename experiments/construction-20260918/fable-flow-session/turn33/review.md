I ran nothing. Everything below comes from reading the source in this message, plus your verified tables.

## 1. Correctness

**Real bug: fresh robots can defeat the next-primary protection.**
- With `include_fresh=true`, the first loop of `unopened_candidates` admits a robot whose `agent.task != proposed[i]`. It then compares that robot's `agent.ticket`.
- For a robot that finished a task last tick, `agents_[i]` still holds the ticket of its final delivery errand, because `sync_agents` only runs later, inside `plan`.
- That stale ticket can be older than every real holder's ticket. `oldest` then lands on a robot that can never be the next primary, because sync is about to give it the newest ticket.
- The true oldest holder loses its protection. It can be retargeted, which resets its ticket, `best` and `stall`.
- The one-move rule bounds the damage, but the protection the comment promises is not delivered.
- Fix: skip `agent.task != proposed[i]` in the `oldest` scan whatever `include_fresh` says.
- The old passes are unaffected. The default `false` reproduces their old predicate.

**The test gap that hides it.**
- No production fixture has a same-call assignment inside a matching pass.
- Add a fixture where a fresh robot's pickup table is already resident, for example through a shared pickup cell.
- Assert three things:
  - the fresh robot can move;
  - the true oldest holder stays protected;
  - a fair-admitted fresh task is excluded.

**What reads as sound.**
- Matrix orientation is correct: the row is the robot, and the column is the other holder's pickup table.
- `replacement` reads `proposed` before any write, and groups are disjoint. Accepting some cycles and keeping identity elsewhere therefore stays a bijection.
- Every non-trivial cycle in the optimum has a strictly positive distance saving. A zero-saving cycle loses to identity under the +1 moved term.
- The forbidden-edge cost exceeds any identity total, and int64 has room for it.
- All deadline checks precede mutation, except the last one, which can only fail the run.
- `committed = -1` is a no-op given the eligibility rule.
- Unmoved participants keep their token.
- A k-cycle resets k tickets at once, and sync reissues them in robot-ID order. Seniority inside the cycle is therefore scrambled. The one-move rule bounds this.

**Counter quirks.**
- `match_cursor_` advances by 128 although at most 4 anchors are consumed, so "128 anchors" overstates the rotation.
- `match_unreachable` mixes excluded robots with unreachable matrix entries.

**Your new fixture discriminates.**
- The old preselection leaves robot 1 alone in its group, so the old code fails it.
- It only covers crossings between robots that are near each other; see 2b.

## 2. Why the test is still too weak

**a. Exposure.**
- The cap is 128 participants per pass, with one pass per 10 ticks. That is 12.8 participant slots per tick.
- Against that there are about 29 assignments per tick and about 1,900 holders.
- Under 45% of tasks can ever enter a matrix, and many enter late in their leg. A null result would say nothing about rematching.
- Proposed change:
  - raise `group_limit` to a fixed 64, which allows at most 2,048 participants, 131k BFS nodes, 65k table reads and 64 solves of n ≤ 32 per pass;
  - keep interval 10;
  - advance the cursor by the anchors actually consumed.
- My guess is 15–25 ms on pass ticks. That is unmeasured, so time it and report the maximum decision time on pass ticks separately.
- Interval 1 costs ten times more for mostly redundant exposure, because positions change by one cell per tick. I would not start there.

**b. Grouping bounds the saving.** The bound is provable; its practical impact is unmeasured.
- Reindex the after-sum by task:
  - saving = Σⱼ [d(rⱼ, pⱼ) − d(r_{σ⁻¹(j)}, pⱼ)] ≤ Σⱼ d(rⱼ → r_{σ⁻¹(j)}).
- The inequality is the triangle inequality, and directed heading-aware shortest paths satisfy it.
- So each moved task saves at most the path cost from its old holder to its new holder.
- Groups of the 32 nearest robots cap that cost near the group diameter.
- The large opportunities, such as a robot passing the pickup of a far holder, need far-apart robots in one group.
- Your screen shows 26 weighted units per moved task, about 6.6 steps, which is consistent with a binding cap.
- If diagnostics show the cap binds, fill half of each group from a second index keyed on pickup cell. It uses the same BFS and the same caps.

**c. The available effect under HRRN is small.**
- Your accounting has CGAR empty work at 64.3 steps per task. KK is 65.7 and NMS is 62.0, and both rematch every tick.
- Ideal rematching under HRRN therefore has roughly two steps per task to find. That sits inside task-count seed noise.
- Judge this experiment on total empty steps and on empty steps per completed task.
- Your control seeds differ by only 0.06% there: 9,269,146 and 9,263,667.

**d. Passenger rows.**
- Min-sum matching moves a row for one unit of group gain, or even at a loss to that row itself.
- Each moved row pays a ticket reset and spends its token.
- Log, per accepted cycle, the rows whose own saving is not positive.

**e. Two known limits remain.**
- Resident attrition after each table flush still applies.
- So does the one-move-per-task budget.
- Report `missing` by ticks since publication, and report `budget_protected`.

**Diagnostics that separate coverage from a bad metric.**
- Coverage:
  - exposures per task-life before pickup, bucketed 0/1/2/3+;
  - leg age at first exposure;
  - the per-pass funnel from eligible to resident to participants;
  - the optimal group saving against the accepted saving.
- Metric:
  - at commit, log each moved task's estimated new leg, and at pickup log the realized steps;
  - do the same for unmoved participants, using their diagonal estimate;
  - compare realized/estimated between the two sets within the same run;
  - add the rate at which the first action after a move is a rotation, and the waits in the first few ticks after a move.
- Reading the results:
  - low exposure with matching ratios points to coverage;
  - healthy estimates, moved legs realizing worse than unmoved ones, and flat empty steps point to the metric or the reset cost.

## 3. If the corrected, full-exposure matching fails

Run the cross where your accounting shows something to recover: `--trick WAREHOUSE` with lanes 1, short tasks 1 and matching 1. Compare it against your verified lanes+short and lanes rows.

Why this cross:
- Short tasks left 27.7 extra empty steps per task in the lanes arm.
- Min-cost cycles are the within-holder cascade that pairwise swaps and the pool exchange could not express.
- The layer is cohort-neutral, so your cohort measures stay clean.
- The chain component of the gap to NMS is about 7 steps, and so far it has moved only under short-task preference.
- Under your new goal, a TRICK seed counts.

What it needs:
- **Selector.** Add an explicit third selector in `tricks::options`, printed in the components receipt. `CGAR_REASSIGN_MATCH` stays rejected under `--trick`.
- **Activation gate.** `flow_guidance_.publications()` is always 0 under static lanes, so the pass would never run. Use the `pickup_metric` predicate for activation.
- **Screen first.** Start with an 800-step screen:
  - Read the estimated saving per moved task and the realized empty steps per completed task.
  - Compare them against lanes+short at the same step.
- **Bar for tying.** Tying lanes needs about 21.7 of the 27.7 steps back. With one move per task, moved tasks must realize at least that on average, and more if not every task moves.
- **Reading the screen.**
  - Small estimates under short tasks mean grouping is the limit, as in 2b.
  - Large estimates with flat empty steps mean realization is the limit.

This is not NMS or KK rematching. A local cost decrease promises neither more throughput nor liveness.
