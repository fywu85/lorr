I read the V44 diffs and ran nothing.

## V44 source review and what the results tell us

**Source review.** I found no real inconsistency. I checked these points:
- `guidance_turn_cost_` reaches the reverse oracle, the PIBT turn-then-forward macro, `robot_turn_cost` for oriented robots, and both pickup searches.
- Fallback robots keep unit turns by design.
- `extra = T - unit` charges prefix turns and terminal-wait rotations consistently.
- The mixed start gives `initial` only to worker 0.
- `warm_started` holds distinct `char` slots, so the concurrent writes are safe.
- The strict `>` keeps worker 0 on exact ties.

**What the negatives show.**
- Both of my turn-11 hypotheses failed.
- The surcharge traded 9% fewer turns for 11–13% more waits. In this planner, turns largely substitute for waits under blocking, so turns are the wrong target.
- Plan continuity hurt. Cold restarts beat warm starts, and more independent searches helped on seeds 0 and 2 (16 beat 4, and 4 beat 1).
- Your replay also puts empty travel near KK's level now.
  - CGAR spends about 352.5 cycle steps per task, of which 277.4 are loaded.
  - KK spends about 326.8, of which 252.5 are loaded.
  - The remaining gap is therefore loaded: chain mix plus conflicts.

## Change 1 (existing code, hypothesis): cooperative regional repair on the current reference

**Code basis.**
- The adapter keeps exactly one worker's plan and discards the rest.
- Best-of-N cannot combine an improvement one worker found in one place with one another worker found elsewhere.
- `repair_temporal_regions` does combine them: it repairs one plan in parallel by region, with crossing robots frozen and each region kept or reverted.

**Evidence, from your own runs, not re-run by me.**
- In the no-flow era, two-round regional repair gained 2.7% on seed 0 within 1 s on four cores, when four-plan portfolios gained 0.55%.
- Regions were then switched off for clean traffic comparisons and were never combined with refresh, graded tolls or complete pickup fields.
- Best-of-16 now gains 0.88% on two seeds. Best-of-4 gains 0.19% over six seeds, with one seed that declines late.

**Fixture.**
- Hand-build choices as in the transaction regression: two separated two-robot conflicts, with the base plan resolving the left one well and the right one badly.
- Assert that the regional pass fixes the right conflict and leaves the left selections untouched.
- Add a robot whose path crosses the region border and whose frozen path forces a revert.
- Assert exact restoration and a complete collision-free result.

**Validation.**
- All arms use fixed work and no clock dependence.
- At 4 cores, compare the one-worker reference against the same run plus regions (4 parts, 2 rounds, your fixed steps).
- At 16 cores, compare 16 cold workers against one worker plus 16 regions.
- Run the 200-step deadline screen first, with publication ticks included. Then run full seeds 0 and 2. Then run six seeds, including seed 5, which broke four workers.
- Read these together, and do not judge on score alone:
  - task totals and final-1000 windows;
  - age p90;
  - kept and reverted region counts;
  - loaded waits, turns and extra forward moves per task from the replay.

**What comes next.**
- If cooperative repair converts on the full runs, the next structural step is recombining complete worker plans. Your reversible reservation table already has the machinery.
- If it does not convert, stop spending on search structure.

## Change 2 (hypothesis, audit-gated): next-errand continuation in temporal scoring

**Source facts.**
- `check_finished_tasks` advances one errand when a robot's location matches at the end of a tick, regardless of heading. The next errand is visible in `task.locations`.
- `TemporalGeometry::cost` sets `d = -t*unit` at the last step on the goal, so holding the goal through the horizon scores best. `forward_surcharge` also stops at the goal.
- The arriving robot's own first action never changes under continuation. The policy acts only through the truthfulness of other robots' reservations. It can also change the arrival heading when alternative approaches exist inside the window.

**Audit specification.** It is a const pass with a stride coprime to 512, using `peek` only.
1. For each robot j, find `t_a(j)`, the first step at which its selected path is on its goal.
2. Mark every cell slot and edge slot that j reserves after `t_a(j)` as a phantom slot. Tag each phantom slot by whether j's next errand is known or j is finishing its task.
3. For each non-fixed robot with a goal, examine every candidate whose physical gain over its selected choice is at least one unit. Classify the easiest such candidate, in this order:
   - free;
   - blocked only by phantom slots;
   - blocked by a real slot or a fixed owner.
4. Report the robots whose easiest class is "phantom-only", split three ways:
   - by the selected first action (wait, rotation or forward);
   - by whether the blocked candidate starts with a forward move;
   - by phantom tag (next errand known, or finishing).
   Also report the summed physical gain. This is a bound on plan-score distortion, not on throughput.
5. For arriving robots with a cached next-errand table, report `D_next(goal, arrival heading) - min over headings of D_next(goal, heading)`.

**What the audit distinguishes.**
- If phantom-only robots are negligible, drop this direction.
- If they are mostly tagged "next errand known", implement continuation scoring.
- If they are mostly finishing robots, continuation cannot help without forecasts. The only candidate left is a neutral tail, which gives the same arrival credit to every post-arrival continuation. That is a separate hypothesis.
- A frequently nonzero heading statistic means the arriving robot benefits too.

**Scoring, when the next errand is known and its table is cached.**
- Keep the native arrival credit and add paid progress on the next leg:
  `d = -4*unit - [D_next(goal, arrival heading) - D_next(end state) - extra*turns after arrival - surcharges after arrival]`.
- The terminal-wait discount applies to `D_next`.
- `forward_surcharge` continues past the goal.
- Otherwise the native rule applies unchanged.
- With the option off (the default), behaviour is identical to today.

**Fixture.**
- Use a 3×9 open grid.
- Robot A is at (r1, c2) facing east, with errands at (r1, c3) and then (r1, c8).
- Robot B is at (r1, c1) facing east, with its goal at (r2, c6).
- Under the native rule, A holds c3, so B's best plan is turn, forward, turn, forward, forward, and its first action is a rotation.
- Under continuation, A drives on, so B can go forward five times and its first action is forward.
- In both cases the joint plan must be collision-free and A's first action must be forward.
- I derived this by hand, so please confirm it.

**Validation.**
- Start with a run that enables only the audit.
- If the audit supports it, run seeds 0 and 2 against exact controls, then six seeds.
- Read reversals, waits and turns per task from the replay alongside the totals.

## Idea 2: flow-aware chain legs

It is coherent as an alignment step, but I would not rank it in the top two. It only reorders which tasks are served. Its benefit comes through which tasks are left unserved at the horizon, and the earlier spatial refinement was inert.

**Your concern about cache-dependent sampling is real.**
- After each publication, about half the tables are missing for a while.
- Legs priced from the fallback are toll-free and turn-free, so those tasks look cheaper and get favoured.

**If you build it anyway:**
- Recompute per scheduling call with `peek` and no caching across calls. Publication handling is then automatic, and there are no LRU or admission effects.
- Minimise over the initial heading for each leg.
- Keep the chain in scaled units inside `pair_for` without multiplying by the scale a second time. Leave the unit-based statistics unchanged.
- Remove the mean bias by multiplying each uncached leg by a per-call ratio: the sum of oriented costs divided by the sum of scaled unit costs over the cached legs, accumulated in task-id order.
- Validate by checking that ranking changes are not concentrated in the window after a cache flush.

## Status

- **Demonstrated bugs:** none.
- **Intentional approximations:** holding the goal after arrival, the unit-turn fallback, scalar worker selection, and unit chain costs.
- **Hypotheses:** both changes above. Neither is supported until the full runs and the six-seed results are in.
