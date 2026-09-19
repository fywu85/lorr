I read the diffs and ran nothing.

Two of my hypotheses are now closed:
- Known-next continuation lost on both seeds.
- Your peak tracker found 3 discarded peaks in about 18,000 batches, so retaining the best interim state has almost nothing to recover.

## 1. Equivalence check: it holds exactly

In `TemporalGeometry::cost`, every step on the goal overwrites `d = -t*U`, so the last hit wins.
- The terminal distance and its trailing-wait virtual turns are discarded for every candidate that hits the goal.
- `turns_to_goal` is frozen at the first hit, and `forward_surcharge` breaks at the first move onto the goal.

Setting `d = -4U` for any hit therefore gives exactly `native - (4 - t_last)·U·S`, with extras and the op term unchanged.

The specific cases you listed:
- **Op 0:**
  - `first_action` is W for non-pinned robots.
  - It reaches the goal only when the robot is already standing on it.
  - In that case `t_last = 4`, so the seed is unchanged.
- **Own-cell start:**
  - Candidates that stay on the goal first hit at t=0.
  - Candidates that move forward first never hit.
  - A forward-first departure therefore gets no service credit, which matches TaskManager.
- **Turns and directed tolls:** they are charged through the first hit only, identically in both forms.
- **Final virtual turns:** they are irrelevant for hit candidates in both forms.
- **Repeated service cells:** the rule "the index is the last in `locations`" excludes them. A task ending in the same cell twice is ineligible while on the first of the two.
- **Fallback metric:**
  - The formula needs no table, and native already uses `-t*U` in the same U.
  - Fallback robots can therefore be eligible, or excluded for purity.
  - Whichever you choose, test it.

One point in the policy's favour: TaskManager completes the task on the first end-of-tick location match, whatever the robot does afterwards. Native's penalty for leaving the goal after service has no counterpart in the simulator. The neutral tail is therefore closer to the simulator than native is.

## 2. Coherence, correctness and liveness

The policy is coherent and bounded. I see no safety or liveness issue.
- Only scores change. All five reservations stay complete and are validated as now.
- The pinned primary and the recovery robots never enter the branch.
- Fixed work, RNG use and first-action validation are untouched.
- The incentive to arrive is not weakened. Any hit scores at least native's credit, so the arrival cliff stays the same or grows.

What remains is a quality hypothesis.

## 3. Failure modes and intentional optimism

**1. The op term becomes the only differentiator after service.**
- Among continuations with the same prefix, a higher op wins, so the robot drives straight on.
- Post-service tolls are uncharged, so this holds even against the learned flow.
- The phantom reservation changes from a static hold into a sweep across cells ahead, sized by the op-term scale and not by anything physical.
- In an arrive-at-t=0 example at U=4 and S=50:
  - Native preferred holding by about 520 score units.
  - The neutral tail prefers straight-on by about 280.
- If distance scale 513 wins, post-service choice becomes nearly arbitrary. Settle that experiment first, so the two effects are not confounded.

**2. Truthfulness is unknown in both directions.**
- A finishing robot usually rotates for one or two ticks.
- A neutral tail can under-predict that hold, so followers plan to enter the goal cell and get blocked for a tick.

**3. The receding horizon limits the effect.**
- As with known-next scoring, the finishing robot's own first action never changes.
- Any effect comes only through other robots' plans.

**4. The precedent and the size of the class are both unfavourable.**
- Known-next scoring made the same kind of change, more truthfully, and lost 0.35% with waits up.
- The finishing-only class is about 23 robots per frame, roughly 0.2% of the fleet.
- The expected effect sits inside two-seed noise. Detecting it would need six seeds.

## 4. Fixtures, if you run it

**Independent oracle:**
- Enumerate every start, heading, goal, op and toll field, with U in {1, 4, 8} and T = U plus {0, 1, 2}.
- Replay the actions independently to get the first hit, the last hit, and the extras up to the first hit.
- Assert that the proposed score equals `(-4U + extras to the first hit)·S - op·U`.
- Assert that it also equals `native - (4 - t_last)·U·S`.
- Assert that candidates which never hit the goal are bitwise native.
- Assert that candidates which hold through slot 4 are unchanged.
- Include own-cell starts.

**Joint fixtures:**
- Take your two probe examples with robot A finishing.
  - The exhaustive joint minimum should make both first actions forward.
  - The joint plan should be collision-free.
- Add a control in which fixed robots block every continuation of A. The native plan should come out unchanged.

**Adapter fixtures:**

| Case | Expected |
|---|---|
| Last-errand robot | The policy is active. |
| Non-last errand | Bitwise native. |
| Guided robot | Bitwise native. |
| Missing task or goal mismatch | Bitwise native. |
| Pinned primary on its last errand | Native, with the primary's action unchanged. |
| Fallback robot | Per your eligibility choice. |
| Option OFF | Full hashes exact. |
| Serial against 4 preparation threads | Equal. |
| 80-tick lifecycle run with services | No stale state, since the option holds none. |
| Wait-seed projection | Unchanged. |

**Mechanism readouts:**
- Count selected hit candidates that leave the goal and those that hold it.
- Record post-service audit buckets before and after.
- Count first actions that differ from native.

## 5. Is it worth a full pair now? I would not prioritise it

The known-next loss is modest evidence against "release the hold" as a direction, and the class is tiny. Run it later as a cheap rider at most.

**Does task-chain pricing miss known costs?**
- Yes. It prices each leg at unit BFS distance.
- It omits necessary turns, tolls, and the coupling between arrival heading and the first loaded leg.
- All three are available from resident oriented tables through `peek`. That is about 4 lookups per leg for roughly 5,000 free tasks per call, well under a millisecond.
- I still expect little from it:
  - It only reorders service. Any benefit arrives through censoring at the horizon and through the HRRN denominator.
  - The earlier spatial refinement was inert.
  - Cost-side changes have been fragile.
  - The cache is cold after every publication.
- I rank it below the mechanism in the next block.

**The one mechanism I would test next, after your ppm and parts comparisons: move fixed work from the serial global pass to the parallel regional rounds.**

Evidence:
- The reference averages about 410 ms per entry while using only 1.73–1.77 of 4 cores.
- The single global worker runs serially, so it dominates wall time.
- Regional attempts run 4-wide in parallel.
- In your six-seed runs, cooperative regional repair converted: +1.03%, with all six seeds up.
- Competitive global workers did not: +0.19% at four workers and +0.45% at sixteen, both mixed.

The bounded experiment:
- Cut the global candidate limit to 3M or 2M.
- Spend the freed time on rounds 3–4, or on larger regional steps, at equal or lower maximum entry time.
- Work stays fixed by counters.
- Arms to compare:
  - the reference;
  - 3M global with four regional rounds;
  - 2M global with four regional rounds.

Two risks:
- The four-workers-at-1M collapse shows the global pass needs enough repair after construction, so do not go below about 2M.
- Building `choice_regions` costs a serial O(n × choices × 5) per round. Screen the ticks around each publication, because your maximum is already 962–967 ms.

**A source fact for reading the 4-versus-8 region result.**
- On a 140×500 map, 4 parts resolve to 1 row part and 4 column parts.
  - Partition phases 2 and 3 then duplicate phases 0 and 1.
  - Two rounds therefore already cover every partition.
- Eight parts resolve to 2 by 4.
  - All four phases then differ.
  - Two rounds use only phases 0 and 1.
  - Four rounds are therefore structurally meaningful only in the eight-region arm.
- This is a fact about partition coverage and says nothing about quality.

**Status:**
- No bug found.
- The goal-hold credit and the restriction to the last errand are intentional approximations.
- Everything in section 5 is a hypothesis until the full paired runs and six seeds are in.
