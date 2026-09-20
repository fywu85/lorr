I ran nothing; every point below comes from reading the diff. I could not see `estimate()`, `fair_admission()`, the complete-field scan, `ChainCostCache::Entry`, or the parse order above the new guard, so items that depend on them are marked.

## Modes 0 and 4: no trajectory-changing defect found

**Mode 0**
- The pair expression is the original uncapped one.
- `task_chain_cost` runs at the same point relative to the `at_cell` push.
- The only new executed code is a local bool and a larger `TaskCost` (16 to 36 bytes).
- Nothing touches the RNG, an oracle, or container order.
- Equivalence to 4524843 cannot be tested in-process. Your exact-control hash on the new binary is the only check.

**Mode 4**
- The only oracle calls added are `peek` and `value`.
- `pair_for`, shortlist retention, fair admission and the fallback pass all stay native.
- The shadow score uses the same floating-point evaluation order as `pair_for`, and the same first-minimum tie rule.
- Mode 4 has whole-run failure points the control lacks:
  - `chain_table_basis_.at(id)`;
  - the `chain_price_snapshot` deadline check;
  - the added wall time.
- The `.at` cannot throw while both maps are written and pruned under one rule, and they are.

**Overhead**
- Work is bounded by counts:
  - one peek and four reads per priced leg;
  - one clock read per task;
  - at most 32 price evaluations per robot, each with a 128-bit division.
- The cost in milliseconds is unmeasured. Time the quote-plus-snapshot block and the shadow block, and print the sum and the maximum.

**If control and shadow diverge**
- Look first at any path that depends on the clock but does not fail the run, for example `try_table` returning null at its deadline and falling back to Manhattan.
- I cannot confirm from the excerpt that such a path exists.

## Counter and logging problems that can undermine the shadow measurement

1. **Per-call data exists only under `diagnostics_`.**
   - `basis[]`, `nonzero`, `covered_nonzero` and `clipped_native` are locals that feed only the printf.
   - With diagnostics off, the basis split and the nonzero coverage are lost.
   - The cumulative `covered` also counts zero-length chains, which are "covered" with no table resident.

2. **Shadow counters are cumulative only.**
   - They print every 200 steps, and the last print is at t=4800.
   - First-choice changes therefore cannot be bucketed by ticks since publication. That is the question your unmeasured-κ-swing qualification leaves open.
   - The final 199 ticks never print.
   - The snapshot line precedes candidate generation, so it cannot carry these counters.
   - Add a per-call line after the robot loop. It should also give the number of robots with fewer than two candidates, which are currently excluded without being counted.

3. **`shadow_specific` is a symmetric difference.**
   - Let N, A and B be the native, mode-2 and mode-3 first choices.
   - The counter fires when A=N and B≠N, where mode 2 changed nothing.
   - `changed2`, `changed3` and `specific` give three equations for four unknown categories:
     - A only;
     - B only;
     - both, to the same task;
     - both, to different tasks.
   - One more counter (A≠N and B=N) identifies them all.
   - The fixture gives (1, 0, 1), which cannot tell the two readings apart.

4. **`assigned_covered` has no matching denominator.**
   - `stats_.assignments` includes every assignment made before the first publication, among them the initial 10,000.
   - Add a count of assignments made while `chain_metric` is true.

5. **A clipped native chain can enter κ.**
   - `clipped_native` counts clipped chains over all tasks, not over covered ones.
   - A covered task whose native chain is at least `kFar` adds `pickup_scale*kFar` to the denominator.
   - That would swamp the snapshot and pull κ toward zero, which corrupts `changed2` and `changed3`.
   - The `OutsideDomain` pre-check blocks the pocket-origin route into this case.
   - I cannot verify that the spatial and oriented oracle domains agree everywhere else.
   - Exclude chains at or above `kFar` from both sums, and count the covered-and-clipped cases.

One interpretive limit: the shadow reports each robot's first choice before conflict resolution, on the shortlist retained under native pricing. For the complete-field robots that shortlist is the native top 16. The fallback pass is not shadowed. The count is therefore a lower bound on shortlist effects, not a count of assignment changes.

## Confirm in source

- **`pair_for` routing (modes 1–3 only).**
  - `estimate()`, `fair_admission()`, the complete-field scan and any bucket ordering must all build their pairs through `pair_for`.
  - Any inline `pickup_scale*task.chain` would mix native and flow bases in one sorted list.
- **Guard ordering.**
  - `static_trick_metric_` and `temporal_remaining_flow_` must be assigned above the new guard.
  - The remaining-flow test case proves its own ordering. Nothing tests the trick case.
- **`entry.stop`.**
  - `all_table_derived` starts at `entry.stop + 1`.
  - It gives the right answer only if `estimate()` has already set `stop`. At this call site that holds, provided the unseen head of `task_chain_cost` routes to `estimate()` when refinement is on.

## Test gaps

1. **The closed loop can pass without exercising quotes.**
   - The vacuity check requires `shadow_queries>0`, but it never requires a covered nonzero chain.
   - With flushes every 16 ticks, zero coverage on every call would still pass.
   - Assert all three:
     - Covered is greater than 0;
     - MissingTable is greater than 0;
     - at least one call had a nonzero denominator.
   - Ideally also assert that the shadow counters are nonzero.
   - Compare more scalar stats between arms: pickup-flow and complete-field counters, `route_manhattan`, `table_reads`, `approximate_reads`, `assignments` and `fair_assignments`.
   - On a 63-cell grid with a 16 MB floor, nothing is ever evicted. Actions alone are therefore weak at detecting a cache perturbation.
   - The no-LRU-touch property is tested only at unit level. I accept that, because the production path adds no other oracle call.

2. **Basis flags are never asserted.**
   - `chain_table_basis_` and `all_table_derived` could be constant and every test would still pass.
   - Move the four basis counts into `Stats`.
   - Assert one table-derived chain and one Manhattan-containing chain, under both refine settings.

3. **The rejection list is incomplete.**
   - Missing cases:
     - `--trick`, which is the frontier-separation rule;
     - guide routes;
     - orientation guidance set to 0;
     - flow strength set to 0.
   - The `CGAR_PICKUP_FLOW=0` case still has `CGAR_PICKUP_FULL_ROBOTS=1` set.
   - If a complete-field guard fires first with its own message, that case fails for a fixture reason.

4. **The production path across a publication is still untested.**
   - The production fixture uses refresh interval 0, so no flush occurs in it.
   - Missing assertions:
     - after a flush, coverage is zero;
     - modes 1–3 are then bitwise native;
     - the quote after a rebuild differs from the quote before the flush.
   - Only the oracle unit test touches this.
   - The shadow benchmark does not need it; the policy arms do.

The R2 fixture change matches the production logic you describe. The hand costs do not depend on where the offered actions leave the robot, because both tasks share a pickup cell.
