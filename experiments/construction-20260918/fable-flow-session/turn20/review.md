I did not run anything. From what is visible the patch logic is sound. I cannot see the matching code below the changed lines, so everything downstream rests on your fixture.

**What holds:**
- In mode 0 both pools shrink consistently. Tasks enter only when `agent_assigned == -1`, and agents only when their schedule entry is −1.
- Held pairs survive through the copied `proposed_schedule`.
- The static toggle is parsed once and an invalid value throws.
- The no-new-free-agent early exit runs before any pool construction. It is identical in both modes.
- Scenario 3 exercises the empty-pool path in mode 0.
- With roughly 30 completions per tick the skip will almost never fire in the real run. "Not necessarily every tick" is therefore effectively every tick. The `[kk-rematch-diag]` line will show this, because it prints only on ticks where matching runs.

**Fixture gaps that could give false confidence:**

1. **`agent_assigned` is the one field your before/after comparison leaves out.** Mode 0's eligibility depends on exactly that field.
   - Find out whether the native function writes it.
   - Either assert it is unchanged, or assert its post-call value in both modes.
   - Separately confirm the real harness re-syncs `env->task_pool` from the simulator every tick. If it does not, mode 0's pool membership would come from state the scheduler wrote itself.

2. **No scenario has more free agents than unassigned tasks in mode 0.** This is the `agents > tasks` branch.
   - Add two free robots, one unassigned task, and one held unopened task that is cheaper for a free robot.
   - Expected result:
     - one free robot gets the unassigned task;
     - the other stays −1;
     - the held task is not taken.
   - This is the unmatched-held-task path you asked about. It is where an unseen downstream assumption that every unopened task is in the pool would show up.

3. **There is no lifecycle across ticks.**
   - A holder opens its task between two calls, with `idx_next_loc` going from 0 to 1.
   - Assert it stays excluded, and that a newly free robot sees only unassigned tasks.

4. **Run the fixture once with the supplied non-uniform weights loaded.** The eligibility logic should be independent of the weights. One run confirms the two treatments do not interact at the source level.

**Risks to the factorial interpretation:**

- **Both treatments move clock-selected planner work.**
  - Mode 0 shrinks the matching problem from every unopened holder plus the free robots against thousands of tasks down to a few dozen agents.
  - The scheduler gets much cheaper, and I would expect the LNS to get more time from that.
  - Your `[kk-lns-diag]` records are therefore essential.
  - Also log the scheduler's wall time per call and the time limit actually handed to the LNS.
  - If merged iteration records differ materially between the rematch and keep arms, the contrast mixes assignment policy with planner budget.
  - A work-equalised variant would compute the full native matching, discard it, and then match free robots only. It is a larger patch, so I would build it only if the iteration gap turns out to be material.
- **Uniform weights act through the scheduler's costs and the LNS heuristic at once.** Read that arm as "weights removed everywhere", not as "motion guidance removed".
- **Noise.** Runs are clock-limited and nondeterministic, and each cell is a single seed. The duplicate supplied-weights rematch control gives you the noise scale. Treat any contrast smaller than the gap between that control and its original as no evidence.
- **Scope.** The keep arm is an additive-cost dispatcher without rematching under KK's planner. A KK contrast can support "rematching matters for such a dispatcher". It cannot establish what it would do inside CGAR. That still needs its own paired CGAR runs.

**Observations to check before reading a KK contrast as support for the CGAR interaction.**

- **Leftover tasks, from the historic KK run (no new run needed).**
  - Take the chain-length distribution of tasks still unserved at the end, and of tasks that waited longest in the pool.
  - Compare it with the completed tasks.
  - My hypothesis needs the leftovers to be strongly biased toward long chains.
  - If they are not, the 8.0-step cohort gap does not come from stranding. The interaction story then fails whatever the contrast shows.
- **Per-arm accounting with your exact accounting identity:**
  - completed-chain mean;
  - all-empty time per task;
  - first-assignment and last-assignment pickup;
  - reassignment count.
  - The hypothesis predicts both arms keep short chains, with the keep arm paying for them in empty time.
  - It is falsified if the keep arm's empty time is no higher, or if its chains lengthen.
- **The clogging trajectory, sampled over time in both arms.**
  - Track the mean chain cost of the unassigned pool and the mean pickup cost of new assignments.
  - Without rematching I expect the pool to fill with long chains and the pickup cost of new assignments to rise.
  - If the two arms look alike over time, rematching is not doing what I claimed.

Your corrections are retained:
- The weight file's provenance is unproven.
- The HRRN-off empty increase was 17.0% and 17.6%.
- A static cheaper pickup is not a closed-loop bound.
- Conflicts visible 6–10 steps ahead do not prove the horizon binds.
