I ran nothing. This is a read of the patch against the constructor and `build` source you attached.

## Verdict

- The patch is semantically equivalent.
- I found no counterexample, including the case where construction revisits an ancestor.
- The state is also identical at the only point inside `build` that can throw. Equivalence therefore does not depend on timeouts being discarded.

## Key invariant

1. **Reserved robots.** Every robot whose reservations are present has `selected_` naming exactly its reserved path.
   - This holds in both versions.
   - A frame suspends only at the recursive call. That call comes after `selected_[r] = k; add(r)`.
2. **The unreserved robot.** At most one robot is unreserved at any moment: the subject of the top-of-stack `build` frame.
   - Its caller removed its reservations.
   - It can therefore never be the `other` that `blocker` returns.
3. **When the subject's entry is read.** The subject's `selected_` entry is read only by three things:
   - its own `old` capture, taken at frame entry before any write;
   - `choice(r)` inside `add` and `remove`, which the patch still precedes with the store;
   - the restore on exit.
4. **What the patch removes.** The writes it removes are the ones on the `continue` paths:
   - `-2`;
   - a fixed blocker;
   - a visited blocker;
   - the two random rejections;
   - the depth gate.
   - No statement reads the value those writes stored. The patch changes only a value that is never read.

## Ancestor revisit in construction

The trace is r0 adds k0, r1's candidate is blocked by r0, and the inner `build(r0)` runs.

- At the check `selected_[other] != 0`, `other` is r0 and r0 is reserved with k0.
  - Both versions read k0.
  - The short-circuited RNG draw is consumed identically.
- The inner frame captures `old = k0`.
- On return 0 or 2 it restores k0 in both versions. The caller's `add(r0)` therefore reserves k0 again, and the outer `remove(r0)` later releases the same path.
- On return 1 nothing in the suspended frames touches `selected_`.
- The budget-exhausted `return 2` at the top of the frame happens before any write in either version.

## Rollback and score

- Every `add` and `remove` pair sees the same `k`.
- `reserve` therefore performs the same floating-point operations in the same order, and `score_` is bit-identical.
- Candidate order, `++stats.candidates`, `counter`, the `visited_` handling and the `consider()` draw are all untouched.

## Exceptions

- The only throw site inside `build` is `check()` at the top of a frame.
- At that point:
  - every suspended frame sits at its recursive call with `selected_[r] = k` already stored, in both versions;
  - the throwing frame has written nothing.
- The whole `selected_` vector is therefore identical at the throw, not only on successful runs.
- I see no difference even for timeout diagnostics.
  - This assumes nothing else inside `build` can throw.
  - I see no allocation and no bounds-checked access there.

## Constructor and aliasing

- `blocker(*choice(r).path)` runs after the index check and the `valid` check, and it yields the same path object as before.
- The reference points into paths owned outside the search.
- `add` and `remove` never modify a path, so there is no aliasing hazard.
- Exception order and messages are unchanged.

## One thing to confirm in source

- The patch shows two call sites of `blocker(int)`.
- Some other caller might use the set-then-query idiom, where it writes `selected_[r]` and then calls `blocker(r)`. An audit, snapshot or transaction helper could do this.
- Any such caller must now pass the candidate path explicitly.
  - The compiler will flag it if the old overload is removed.
  - It will not flag it if both overloads are kept.

## Minimum meaningful production checks

- **Exact fingerprints.** Compare against your frozen controls:
  - the generic 800-step prefix;
  - the native startup prefixes that include matching and the horizon.
- **Work counters.** Compare the `TemporalStats` counters as well as actions:
  - `recursive_calls`;
  - `candidates`;
  - `budget_exhausted`;
  - `repairs`;
  - `max_depth`.
- **Ancestor-revisit coverage.**
  - Your component harness already checks plan, score bits and work checksums.
  - Make its ancestor-revisit coverage non-vacuous.
  - Either add a tiny deterministic construction fixture where the displaced robot's candidate is blocked by its displacer.
  - Or add a test-only counter of "blocker is an ancestor" events, and show it is nonzero in the randomized construction runs.
- **Scope of the timing result.** The 0.7–1.6% is component CPU evidence only. It says nothing about full-run time.
