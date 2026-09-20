I ran nothing. Tags: [M] your measurement, [S] source fact as you reported it, [O] my opinion.

## What the KK 2×2 supports

**Rematching [M].**
- With supplied weights, the keep arm holds loaded motion and chain flat at 235.0.
- Its empty work rises from 65.7 to 88.9 steps per completion, and throughput falls 6.8%.
- I predicted exactly that signature in turn 21. It is now a paired result, but only inside KK's policy.

**Transfer to CGAR [O].**
- CGAR's empty work is already 64.3 under HRRN, so the prize is bounded.
- It is the chain cohort, about 8 steps per task.
- That is roughly +2% if empty work stays flat.
- It will not close the whole gap.

**Uniform weights.**
- [M] KK collapses to about 24k without its weight file.
- This is a whole-policy intervention, as you say.
- [O] It shows KK's policy depends on its field.
- [O] It says nothing clean about whether CGAR's learned field lacks coherence.
- [O] It raises the value of a guidance trick far more than it raises generic coherence work.
- Generic coherence work stays gated on the corridor minority-direction measurement.

**Early snapshot probe [M].**
- No root-plus-one-owner group was feasible.
- No root-plus-two-owner group improved both scalar and physical cost.
- [O] This is consistent with putting multi-owner search below the other work, pending the full capture.

## The one generic change: additive ranking × unopened-holder retargeting, with a changed order

**Why ordinary-first cannot capture the mechanism [O].**
- Reveals per tick, completions per tick and newly free robots per tick are all about 29.
- Under additive ranking, fresh tasks are the cheapest chains in a clogged pool.
- The free robots therefore take them at reveal, from any distance.
- [M] KK's keep arm is this regime: a median of 31 agents against 5,031 tasks, with empty work of 88.9.
- A pass restricted to retargeting, run after ordinary scheduling, would then find almost no fresh free tasks. I predict a near-null result for it.
- KK's rematching works because assigned, unopened tasks stay open to competing robots.

**Minimal bounded equivalent, keeping your point that the pass must allocate no free robot.**
- Run a holders-only pre-pass over fresh roots.
  - Roots are tasks revealed this tick plus a short ring of recent still-free tasks, capped at 64.
  - Skip any root whose first-stop reverse oriented table is not resident.
- Scan the eligible holders with one table lookup each.
- Let a holder take the root only when both conditions hold:
  - its own total falls by more than the margin, where the total is the new pickup cost plus the chain cost, compared with its remaining pickup cost plus its current chain cost;
  - its total beats the best free robot's total for that root by more than the margin.
- The second condition costs one extra scan over about 30 free robots.
- Ordinary scheduling then runs unchanged on what remains. Released tasks return to the pool on the next tick.

**Rules retained.**
- Explicit finite caps: releases per task and retargets per robot per empty episode.
- The 20-step cooldown.
- Protection for the primary, the pending primary, recovery robots and fair-admission holders.
- Started tasks are never changed.
- Costs vary over time, so I make no liveness claim from cost decrease.
- The holder's merit subtracts its old remaining cost, as you note.
- The pickup eventually made is not a causal bound.

**Arms and readouts.**
- Arms:
  - baseline;
  - additive only, re-run on the current reference;
  - retarget only, under HRRN;
  - both.
- Richer chain pricing stays a separate factor.
- Under HRRN the fresh tasks stay free at reveal, so the order matters only in the additive arms.
- Readouts from your exact accounting identity:
  - completed chain;
  - all-empty work;
  - first and last pickup;
  - exchanges per tick;
  - pool chain and age tails;
  - final-1000 stability;
  - waits and turns.
- Use six seeds before any promotion.

**Premise check first (read-only).**
- Under additive ranking, count fresh tasks still free after ordinary scheduling.
- Count roots where an eligible holder beats every free robot by the margin.
- If the first count is large, or the second is tiny, my ordering argument is wrong. In that case do not build the pre-pass.

## First isolated trick: NMS lane directions at CGAR's bounded costs

**Why this factorization first [O].**
- NMS's kernel is essentially CGAR's, so its lanes were designed for this kind of planner.
- A binary static field fits the existing oracle:
  - costs of 4 with the lane and 16 against it;
  - flow scale 4 with the base unit, turn cost, op-preference term and all reference units unchanged;
  - 17 Dial buckets;
  - worst-case distances far below 65,535.
- No oracle refactor is needed.
- It is "NMS-derived directions", not the NMS policy.

**Terms that change.**
- The forward edge cost enters:
  - the reverse potentials;
  - the PIBT macro;
  - the paid toll in `forward_surcharge`;
  - scheduler pickup quotes.
- In CGAR every slot still earns one unit of paid progress.

**Differences from NMS that the first arm leaves out.**
- NMS scores the weighted potential alone, with no paid forward toll.
  - A move against a lane that lies on the weighted-optimal path drops the potential tenfold.
  - That gives the robot priority in conflicts.
- [S] NMS's turn costs 1/20 of a forward step. CGAR's turn is time-true.

**Factors for later.**
- The 10:1 ratio, as costs 1 and 10 at scale 1.
  - Your neutral-scaling regression makes the unit change behaviour-neutral.
  - Do this in place of any 20/200 refactor.
- The +1 bands.
  - They are 5% in NMS, but the nearest CGAR step is 25% at scale 4.
  - They are not representable at scale 1.
- KK's float field goes last. It needs quantizing, its provenance is unknown, and it was tuned for a different planner.

**Minimal implementation.**
- **Field generation.**
  - Generate the field by running NMS's own `GraphGuidance` offline.
  - Dump the weight for every cell and direction, and hash the dump.
  - Do not re-derive the mask and `set_grid` by hand.
  - NMS's direction indexing must be verified against CGAR's convention: 0 east, 1 south, 2 west, 3 north.
  - Keep the generated table under an isolated trick directory.
- **Installation.**
  - Call `set_forward_costs(static)` once, straight after `turn_oracle_.init`.
  - Gate FlowGuidance so it never publishes. It must never overwrite the static field, so there is never a cache flush.
  - Replace the `publications() > 0` tests with a "metric active" predicate, so pickup flow works from step 0.
  - Allow scale 4 with the static field as the provider.
- **Activation.**
  - Only `--trick WAREHOUSE`, parsed by the driver, can activate it.
  - Verify rows, columns, free-cell count and a map hash. On any mismatch or unknown name, fail clearly.
  - Any experiment knob is an error unless the flag is present.
  - Log TRICK.
- **Deadline risk.**
  - Tables are weighted Dial builds from step 0. That is during the cold ramp, where your 2M variant timed out at step 36.
  - Screen the first 200 steps explicitly.

**Arms.**
- **G.** The generic reference, with no flag.
- **T1.** Static lanes used everywhere. Scheduler pickup pricing shares the field, and that confound is stated openly.
- **T1 with flushes.**
  - Use the same field with forced `clear_tables()` at the generic publication steps. The cache-only machinery already exists.
  - A static trick also removes ten flush dips and gives guidance before step 128.
  - This arm separates the lane content from being static.
- **T2, only if T1 gains.**
  - Lanes for the planner only.
  - Pickup searches use uniform forward costs.
  - The cached-table estimate branch is disabled in the scheduler.
  - The residual side effect on fair and fallback estimates is stated.
- Keep assignment native throughout.
- Keep separate frontiers for generic and trick runs.
- Treat collapse risk as nonzero. The margin-0 runs showed that broad tolling can tip CGAR.

**Tests.**
- With no flag, the full trajectory hash is identical to the reference.
- The flag receipt and the mismatch failure both work.
- The field conforms to the NMS dump, including walls and edges.
- Oracle distances match an independent shortest-path computation on the static field.
- The paid-progress identity holds.
- There are zero publications and zero cache resets. The flush arm has exactly the scheduled clears.
- Scheduler units are correct.

## Claims to avoid

- KK's rematch effect shows the mechanism inside KK. It does not give a CGAR gain, and the expected CGAR prize is about the chain cohort.
- The uniform-weights collapse is not a causal estimate of guidance.
- A lane-trick result says nothing about generic coherence unless the separation arms are run.
