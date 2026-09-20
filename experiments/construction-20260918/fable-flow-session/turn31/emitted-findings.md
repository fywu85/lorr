I ran nothing. The numbers below are arithmetic on your verified table, and the code points come from the excerpt in this message.

## What the 2×2 says

- **The short-task effect is stable and additive.**
  - Without lanes it is −7,677 and −9,527 tasks per seed.
  - With lanes it is −8,674 and −8,632.
  - The interaction is −51 tasks.
- **Per completed task**, from `empty_steps/tasks` and the completed loaded mean:

| Arm | Empty | Loaded | Sum |
|---|---:|---:|---:|
| control | 64.2 | 272.1 | 336.3 |
| short | 91.7 | 267.2 | 358.9 |
| lanes | 67.2 | 264.2 | 331.4 |
| lanes + short | 94.9 | 258.2 | 353.1 |

- **The accounting roughly closes.** The sums predict −6.3% and −6.2%; you observed −5.96% and −5.90%.
- **Short saves little loaded work and costs a lot of empty work.**
  - It saves 4.9 loaded steps without lanes and 6.0 with lanes.
  - It costs 27.5 and 27.7 empty steps.
  - Chains fall 7.6 steps in both pairs.
  - Loaded steps per chain step rise from 1.125 to 1.140 without lanes, and from 1.091 to 1.101 with lanes. That gives back 1.6 to 2.7 of the 7.6 steps. More empty robots in traffic is my hypothesis for this rise.
- **What a retargeting layer must do to tie its matched arm:** remove 22.6 empty steps per task without lanes, or 21.7 with lanes. That is 82% or 78% of the empty work that short added.
- **Two accounting scenarios, neither a bound, both TRICK-only:**
  - Empty work fully restored gives about +1.5% or +1.8% over the matched arm. It gives about +2.6% if loaded overhead per chain step also reverts.
  - KK gained 7.3% from keep to rematch. Applying that ratio to your short arms gives +0.9% or +1.0%. KK differs in solver and chain metric, so this is indicative only.
- **Your short arms reproduce the competitors' pool but not their empty work.**
  - Mean unpicked age is 2,687 and 2,713, against 2,707 for KK and 2,703 for NMS.
  - Empty work is 91.7 and 94.9, against 65.7 and 62.0.
  - This is consistent with stranding being reproduced and broad retargeting being the missing half. It is not an attribution.

## The existing passes

1. **Cycling is prevented by the one-time rule alone.**
   - Both passes insert both task ids into `reassigned_tasks_`.
   - `unopened_candidates` skips any robot whose proposed task is marked.
   - The exchange also drops marked tasks from `available`.
   - Records clear only when the task is opened or gone.
   - The cooldown is nearly redundant. It can bind only if a retargeted robot finishes its whole task within 20 steps.

2. **The same rule caps coverage.**
   - Each retarget event spends the single token of two tasks, and each task brings one token.
   - Events therefore run at no more than about half the assignment rate. For the exchange this assumes released tasks are re-assigned, which is my inference.
   - Recovery per task is then at most half the mean realized saving per event.
   - Tying would need 43–45 realized steps per event, with every task taking part.
   - Legs are near 92 steps and the acceptance margin is 10%, so I read that as out of reach. Your logs can check it.
   - As written, the exchange is capped lower still. `interval` 10 with `exchange_limit` 32 allows 16,000 exchanges per run, so tying would need about 190 steps per exchange.

3. **The exchange is the wrong primitive under short.**
   - Its comment says ordinary scheduling has already assigned tasks in this call. So it sees only the residual pool, which under short is the stranded stock.
   - `existing_only` excludes this tick's new long legs.
   - It changes the served cohort through `pool_chain_delta`, which confounds your cohort measure.
   - The +0.186% came from a pool dense with acceptable tasks.

4. **The swap pass is cleaner but pairwise.**
   - Chain terms cancel, so it is cohort-neutral and cannot add stranding.
   - It admits this tick's new assignments.
   - It cannot express the cascade that a per-tick global greedy produces. In a cascade, robot F takes task a from H1, H1 takes b from H2, and H2 takes the fresh task.

5. **The cost basis is unit distance, not empty time.**
   - Both passes use unit BFS, and the swap pass falls back to Manhattan.
   - `pool_pickup_saving` and `reassign_saving` are unit distances.
   - Under static lanes an opposing edge costs four times the base cost, so unit distance is a poor proxy there.
   - Task-centered oriented costs are free only for held pickups, because their tables are resident as current goals. A fresh task needs a table build.
   - A robot-centered complete field prices every pickup, held and fresh, on one heading-aware basis in the active metric.

6. **Every retarget resets planner state.** It passes through `sync_agents`, which assigns a new ticket and sets `best = kInf` and `stall = 0`. A finite budget keeps those resets finite.

## Recommendation

**1. Stop this line as a throughput lever.**
- Do not run either existing pass under short.
- I withdraw my turn-30 follow-up. As written it cannot tie by the arithmetic above, and it looks at the wrong pool.
- Leave the trick guard rejecting `CGAR_REASSIGN` and `CGAR_REASSIGN_POOL`.
- Bank the 2×2 as the measurement of short-task preference without retargeting.

**2. If you want closure on retargeting × short, fund one intervention, shadow first: holders as bidders.**

- **What it does.**
  - Before ordinary candidate generation, take a fixed quota of holders from `unopened_candidates(proposed, true)`. Use its own rotating cursor.
  - Give those holders complete pickup fields. These come on top of the existing 64 free-robot fields and do not replace them.
  - For each holder, find the best free task under the arm's own `better`.
  - Accept only if, inside that one field, both hold:
    - the new pickup undercuts the old by the existing margin, max(4 steps, 10%), in the field's units;
    - the new cost does not exceed the old.
- **Shadow stage.**
  - Execute nothing. Log per tick:
    - the would-accept count;
    - the summed saving;
    - eligible, quota and protection counts.
  - The run must reproduce your verified lanes+short seed-0 trajectory exactly. Fields are transient and read only `forward_cost`. Verify that; do not assume it.
  - This stage is one run.
  - Go active only if accepts per assignment times the mean saving clears 21.7 steps per task. Convert the saving with your measured ratio of realized empty steps to `pickup_estimate`.
- **Active stage, if reached.**
  - Release the old task into the same call's free set.
  - Replace the one-time set with a per-task count and a bud

get of 3. A budget of 1 reproduces today's rule, so current regressions stay meaningful.
  - By the token arithmetic, 3 is the smallest budget at which tying needs about 15 realized steps per event instead of 43 or more.
  - Clear commitments on retarget, as the existing passes do.

- **Guards.**
  - Add a third selector read in `tricks::options`: 0 off, 1 shadow, 2 active.
  - Print it in `[CGAR_TRICK_COMPONENTS]` together with quota and budget, and have the benchmark expect it.
  - It is the only path. `CGAR_REASSIGN` and `CGAR_REASSIGN_POOL` stay rejected under `--trick`.

- **Protections, unchanged from `unopened_candidates`.**
  - Started tasks, and a robot standing on its pickup cell.
  - The primary and the next-oldest ticket.
  - Recovery robots and transaction cells.
  - Parked, locked and non-core robots, and robots with an outstanding commitment.
  - Fair tasks when short is 0.
  - No task is deleted. Released tasks keep their metadata.

- **Fixed work.**
  - Quota 256 holders per tick under the 5 s development limit. About 2,600 robots are empty at any time in the short arms (`empty_steps/5000`), so each holder gets a look roughly every ten ticks.
  - That is four more 64-field batches. I have no timing for it, so measure it.
  - No partial results. An overrun fails the run, as now.
  - Fields are transient, so RSS should not move. Confirm it.

- **Arms.**
  - lanes+short+retarget and lanes+retarget, on seeds 0 and 2.
  - Compare against your existing lanes+short and lanes rows. That is four runs.

- **Acceptance.**
  - Outcome: tasks above lanes-only on both seeds.
  - Mechanism, from the replay and not from logged savings:
    - realized empty steps per completed task;
    - the ratio of realized empty-step change to summed logged saving;
    - retargets per assignment, and budget exhaustion counts;
    - completed chain and loaded means;
    - unpicked age tails;
    - maximum decision time.

- **Strongest reason it fails.**
  - Each fresh task is visible to only the quota, about a tenth of holders per tick, and there is no within-tick cascade. KK and NMS re-match every unopened holder every tick.
  - Second, estimated savings may not survive as realized time. A retarget clears a commitment, can add turns, and resets ticket and stall.

- **What the acceptance rule does not give you.**
  - It lowers one robot's estimated cost at one instant.
  - It guarantees neither less realized empty time nor more throughput.
  - Liveness rests on the unchanged protections and the finite budget, not on the cost decrease.
