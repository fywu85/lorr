I ran nothing. This is a read of the delta you sent.

## Semantics

- `pure_potential_cost` matches the excerpt on every point I can check:
  - The end-state potential is used.
  - A free ±90° minimum applies on one trailing wait, and a free 180° on two.
  - The last-hit override is applied after the minimum.
  - Service uses the raw `-slot`.
  - The scale is `*50` and the tie is the raw `-op`.
  - An idle robot's cost is `op`.
- The slot indexing agrees with NMS, because `path[d]` there is the state after action d.
- Bit-unpacking of the bands agrees between the generator and the C++ code.
- The generator's asserts show that the dump is exactly the direction mask plus the band bit on all 154,344 free directed values.
- I found no correctness failure in the visible code.

## Verify before trusting the full runs

1. **The parallel table path is untested under native weights.**
   - Production admits tables through `prefetch`, from both the batch path and the plan path.
   - The native regression checks only the serial `table()` compute.
   - The integrated profiles set no table batch, so they do not exercise `prefetch` either.
   - `init` sizes the buckets from `max(turn, base) = 20`; only `set_forward_costs` raises the maximum to 200 or 201.
   - If any prefetch scratch sizes its buckets before that call, or caches the size, Dial silently produces wrong distances. I cannot see that code.
   - Add a test that builds native tables through `prefetch` plus admission, at 1 and 4 threads, and compares them with the independent heap.
   - The old 4/16 profile had the same exposure, going from 5 to 17 buckets. The serial-versus-parallel decision tests would not catch a defect that is common to both.

2. **Width of the stored choice cost.**
   - The score is int64 at the point of computation.
   - If `TemporalChoice::cost` is a 32-bit int, a `kFar`-based fallback distance (about 2^20) scaled by 20 and then by 50 is about 1.05e9.
   - That fits, but with only about 2× headroom.
   - Confirm the field's type, or assert the bound at the fallback site.
   - Scheduler arithmetic is fine. `pair_for` is long long and clamped.

## Latent, currently masked by the guard

3. **`pickup_permutation_cycles` still rejects `unit_cost > 16`.**
   - Native passes `flow_cost_scale_ = 20`.
   - The first matching pass under native plus matching would therefore throw.
   - It is unreachable today only because native rejects matching.
   - Fix it, and add a gate test, before that guard is relaxed.
   - Your best profile contains matching, so a native win leads straight to this.

## Default exactness

4. **Default trajectories look exact; default timing is not.**
   - Trajectory-affecting defaults are unchanged:
     - The 16 cost limits are unchanged.
     - 17 buckets give the same modulus and the same pop order.
     - The cell bound reduces to the old `(inf-16)/64`.
   - The new `!cert` check and the cell-bound check in `TurnDistanceOracle::init` are inert on valid maps.
   - Timing differs on the default path.
     - The complete pickup Dial replaced a compile-time `% 17` with a runtime `% buckets_.size()`.
     - That puts a division in the hot loop.
     - My rough estimate is a few hundred thousand divisions per field, times 64 fields per call. That estimate is unmeasured.
   - Compare mean and maximum decision time on the no-flag 800 check against the previous binary, not only the fingerprint.
   - A wrap-around index removes the division for any bucket count and keeps the order identical: `b = idx + (cost - current); if (b >= B) b -= B;`.

## Provenance

5. **The receipt prints header constants, not a hash of the installed vector.**
   - The installed field combines two headers: direction masks from `warehouse_lanes.hpp` and bands from `warehouse_native.hpp`.
   - Their consistency is asserted only at generation time, and in the regression through FNV.
   - Add a runtime FNV-1a over the vector returned by `native_forward_costs` at init. Compare it to the constant, throw on mismatch, and print it. That is one pass over 280k bytes.
   - Cosmetic: the static-trick guard message still says "cost scale4".

6. **Meaning of `wide_fallback_tables`.**
   - It counts every admission that was stored wide, cumulatively.
   - It includes rebuilds after eviction or a metric change.
   - It resets at `init`. Your fixture going from 1 to 2 shows this behaviour.
   - It is not a resident count and not a count of distinct goals. Label the log field accordingly.
   - The table budget is unaffected, because the LRU already charges wide bytes. RSS still needs measuring.

## Fallback and idle behaviour

These are not failures. They are part of the compound.

7. **Missing-table robots price turns differently.**
   - They score with the spatial-plus-turn estimate times 20, so a turn is worth 20 to them.
   - For robots with a native table a turn is worth 1.
   - Rankings within one robot stay self-consistent.
   - In the joint objective, a fallback robot's rotation outweighs a table robot's by twentyfold.
   - Log the share of temporally scored robots that use the fallback, by tick.
   - The share should be large only during the startup build-up, and lanes also won most of their gain during startup.

8. **The idle tie flips.**
   - Returning `op` for idle robots prefers op 0.
   - If legacy idle scoring preferred higher ops, this is a behaviour change. It follows NMS faithfully, but record it.

## Missing tests, by value

- Op 0 is excluded from the independent macro replay; the loop starts at `op=1`.
  - Op 0 is every robot's baseline choice and the score is measured against it.
  - Add it, including the `first_action` seed semantics.
- There is no closed-loop native episode.
  - The integration test plans one robot for one step.
  - Add tens of robots for 100 or more ticks, with:
    - independent collision validation;
    - service events, since last-hit scoring interacts with one errand per tick;
    - wait-seed rotation under turn 1, where heading potentials differ by 1–2 units.
  - Assert that no learned publications occur and that fixed work holds.
- The prefetch-path oracle test from item 1.
- A unit-20 gate test for the matching cycle function, from item 3.
- A scheduler check under native:
  - Multiplying all weights and the turn cost by 20 leaves assignments identical.
  - Setting the turn cost to 1 changes a hand-counted pickup choice.
  - This proves that `pickup_scale`, the age term and the chain term all scale together.

## Isolation notes for reading the result

- The native arm against V68 changes, all at once:
  - the contraflow ratio;
  - the turn price in the potential;
  - the tie and service scalars;
  - candidate costs in the PIBT pass, where a wait costs 20 and a turn 1;
  - fallback scaling;
  - idle preference;
  - scheduler pickup pricing against the ×20 spatial chain.
- As you say, this is a compound fidelity test, not a cheap-turn effect.
- Fidelity of the raw tie also depends on CGAR's operation enumeration matching NMS's `get_operations()` order. I cannot see either list, so treat that as unverified.
- Nothing in the score or the retarget bounds supports a liveness claim. That claim still rests on the unchanged certificate, primary and recovery layer.
