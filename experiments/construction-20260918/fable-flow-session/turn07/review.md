I found no concrete safety counterexample by reading the supplied source. I did not execute anything. Below I give why each safety invariant holds, the distance and work checks, one point I cannot verify, the fairness and throughput limits as I read them, and the missing regressions ranked by what they protect.

## Safety and correctness

- **No duplicate tasks.**
  - `assigned` is built from `proposed` after ordinary scheduling and after the swap pass, so freshly assigned IDs are unavailable.
  - `used_tasks` keeps the matching one-to-one.
  - A released ID never enters this call's snapshot.
  - Every task with `agent_assigned >= 0` sits in `curr_task_schedule`, and nothing earlier in the call drops one. A stale `agent_assigned` therefore cannot make an assigned task look free.
- **No lost tasks.**
  - The pass only overwrites `proposed[r]`.
  - The authoritative `set_task_assignment` resets the dropped task's `agent_assigned` to −1 and leaves `t_revealed` alone.
  - The pass writes no simulator metadata.
- **Started tasks are preserved.**
  - Eligibility requires `idx_next_loc == 0`, and it excludes a robot already standing on its pickup.
  - This matches the validator's only reassignment rule.
- **Protected robots.** The pass uses the same predicate as the swap pass, and that predicate excludes:
  - the primary and the pending primary;
  - robots in a transaction and robots on transaction cells;
  - parked, locked, or off-core robots;
  - robots with an unfinished commitment;
  - fair tasks.
  - On top of that, `existing_only` excludes same-call fresh assignments and robots swapped earlier in the same call. `fair_tasks_` is now recorded under either flag.
- **Releases are bounded.**
  - Both the outgoing and incoming task IDs enter `reassigned_tasks_`.
  - Both passes exclude such IDs as sources, and the pool pass also excludes them as candidates.
  - A task is therefore released at most once, and a robot cannot release the task it just took. Robot-level ping-pong is impossible.
  - The record set is bounded by the pool size because pruning removes opened or vanished tasks.
- **Determinism.** `task_pool` iteration order is hash-dependent, but `available` is sorted, the head/link lists ascend, and the pair ordering has a total tie-break.

## Distance domain and savings

- **Same graph for old and new pickups.**
  - For a core goal, `goal_pocket` is −1, so the oracle domain is core cells plus cells with `pocket == -1`.
  - The pool BFS uses exactly that domain.
  - Robot cells and new pickups are required to be core, and unit BFS is symmetric, so both distances are on one graph.
  - FIFO order gives exact distances to every reached candidate despite the node cap.
- **Pickup guard is exact.**
  - The margin is max(4, ⌈old/10⌉).
  - Nodes at the radius are examined but not expanded, so any accepted saving is at least the margin.
  - `old_pickup < margin` skips the source, so the radius cannot go negative.
- **Total guard permits equality.** That matches your "cannot worsen" rule.
- **Chain basis is consistent.**
  - Old and new chains use the same `peek`/`distance_from` path.
  - Any missing leg yields `kFar` and the comparison is skipped.
  - No tables are built and no LRU entry is promoted.
- **Throughput bias, not a bug.**
  - `considered` counts candidates rejected for missing chain tables or for the total guard.
  - The 16 slots can therefore be used up before a qualifying, farther task is reached.
  - This is the cache bias you already document.

## Deadline and fixed work

- **Work is bounded by counters only:** at most 256 sources, 2,048 nodes and 16 candidates per source, and 32 commits. No branch depends on elapsed time.
- **A mid-commit Timeout leaves `reassigned_tasks_` and `last_reassignment_` mutated** for exchanges that were never published. This is harmless only because a timeout fails the whole run. It should be stated as an assumption.

## One point I cannot verify

- **Pending-primary protection.** It uses the minimum ticket over unopened existing episodes. If `select_primary` skips that robot, for example because it is locked or otherwise unschedulable, the true next primary is unprotected for one call.
- **Consequence, if so.** Exchanging its task leaves the schedule valid. The effect is loss of ticket seniority, which is a fairness matter.
- **What I need.** I have not seen `select_primary`. Please check that its filter cannot disagree with `oldest`.

## Fairness, liveness and throughput limits, as I read them

- **Released tasks.** They rely on HRRN age and on roughly one fair admission per 10,000 admissions. The pass does not strengthen or weaken that.
- **Incoming tasks.** They are marked retargeted although they were never released, which also blocks later swaps of them. That bounds churn at some cost to optimality.
- **Pickup saving.** It is a static unit-distance figure at decision time. Heading, turn costs and the refreshed toll field are ignored, so a statically closer pickup can be farther in flow terms. A diagnostic counter of the oriented distance difference, recorded whenever both oriented tables happen to be cached, would size this without changing behaviour.

## Missing regressions, ranked

1. **Same-call fresh assignment.**
   - No regression in the supplied excerpt (or among those you list) has a free robot at the exchange call.
   - Add a free robot next to task 100's pickup so ordinary scheduling takes task 100 at timestep 10.
   - Assert that robot 1 does not exchange, that the IDs in `proposed` are unique, and that the real `validate_task_assignment` accepts the result.
   - A duplicate here would reject the entire schedule for that step.
2. **Authoritative round trip.**
   - Replace the hand-patched `agent_assigned` fields with `set_task_assignment(proposed)`.
   - Assert that the released task shows `agent_assigned == -1` with an unchanged reveal time.
   - Assert that it re-enters `free_tasks_` and that an ordinarily free robot receives it.
   - After the cooldown, assert that neither pass moves it again and that no pool source can take it.
3. **Both passes enabled.**
   - In one call, a swapped pair and an exchange must be disjoint in both robots and tasks.
   - Separately, run `CGAR_REASSIGN=1` alone and show it is unchanged by the `unopened_candidates` refactor. Your exact controls cover only the disabled path.
4. **Domain equality on a map with pockets.**
   - In regression builds, whenever a new pickup's table happens to be cached, assert that the BFS distance equals `distance_from`.
   - Add one fixture where a pocket shortcut must not be used, and one where unclassified floor (non-core, `pocket == -1`) must be used.
5. **Contention and caps.**
   - Two sources want the same task: the larger saving wins and the loser keeps its task.
   - Test the 32-exchange cap.
   - Test the guard boundaries: a saving of 3 is rejected and 4 is accepted at old pickup 40. At old pickup 41 the margin is 5.
6. **Stale primary.**
   - The primary finishes and `primary_` is not yet refreshed.
   - The oldest unopened episode must stay protected against an attractive alternative.
   - Add the locked-oldest variant once `select_primary`'s rule is confirmed.

On results, I have recorded the completed transactional and pickup-weight outcomes, including the pickup-5 seed-2 collapse. None of them is evidence of a robust gain.
