I ran nothing, and everything below comes from reading the supplied diffs.

## 1. Concrete bugs

I found no correctness defect and no flag-isolation defect in the supplied changes.

**What I checked:**

- **Costs respect the oracle's contract.**
  - Free cells cost 4 or 16 and walls carry an unused 4.
  - Every cost stays inside the range from base 4 to 16.
  - With turn cost 4, Dial needs 17 buckets.
  - Wide tables remain the fallback.
  - The adapter still charges the paid toll in `forward_surcharge`.
  - Paid-progress units are therefore unchanged.
- **Direction semantics are verified against NMS itself.**
  - The generator checks NMS's own forward offsets against 0 east, 1 south, 2 west, 3 north.
  - Both systems attach the forward cost to the source cell and heading.
- **Activation has one path.**
  - It is the driver's `--trick` option.
  - The driver validates the name and then the exact occupancy.
  - Only then does it set `trick_instance`.
  - `Cgar::initialize` validates again.
- **With no flag, the arithmetic is unchanged.**
  - Every modified condition reduces to the original expression.
- **The learned field cannot overwrite the static one.**
  - Learned initialisation is gated off, and so is `observe`.
  - The static field is installed once, straight after `turn_oracle_.init`.

**One observation that is not a bug.** `static_trick_metric_` is derived from any non-empty instance name. That is fine for a single trick. It becomes a latent coupling once a second trick exists.

**The scaling failure is the turn-09 exposure, now reached.**

- Work per scheduling call is proportional to (free robots) × (8,192 pops).
- The generic path escapes this only because its pickup metric activates at step 128.
- By then almost every robot is busy.

**The planner side is a separate, still unscreened risk.**

- Under the trick, every oriented table is a weighted Dial build from step 0.
- Generic runs use the uniform BFS branch during the cold ramp.
- Steps 0 to about 80 are the heaviest of a run.
- Your 2M arm already timed out at step 36.
- The screen should report the maximum entry time over that window specifically.

## 2. The startup correction

V54 is an acceptable minimal fix.

- At `curr_timestep == 0` the scheduler runs the exact generic path.
  - The BFS branch, the scale, the cursor and the arithmetic are all the same.
  - The initial assignment should therefore match the no-flag run.
  - Your planned equality test checks this.
- I see no reason from the source to defer to step 129.

**The alternative I would prefer costs about the same.**

- Gate on the cause, not on the clock index.
- Use the static pickup metric for a whole scheduling call only when `robots.size() <= N`.
  - N is a fixed constant.
  - The gate lives under the trick flag only, so generic hashes are untouched.
- Otherwise run the whole call on the generic basis.

**What the count gate gives you:**

- It is identical to V54 at step 0.
- It stays one-basis per call.
  - A per-robot cap would mix weighted and BFS pairs inside one sorted list.
- It bounds scheduler work explicitly: at most N × 2 × 8,192 pops plus the 64 fields.
- It also covers any later mass-free tick.
  - A timestep test cannot do that.

**If you keep the timestep form, note two things.**

- The existing fixture asserts zero warm-up calls at the first schedule.
  - That assertion must invert for step 0.
  - It must hold again from the next call.
- The rule silently assumes the first scheduler call is at timestep 0.

## 3. Missing tests that would catch real coupled failures

**Silent non-activation or mislabelling, end to end.**

- The riskiest failure is a run labelled as a trick run in which the planner never received the flag.
- Make the runner's validity rule two-sided:
  - A trick-track run is valid only if the planner log contains the `[CGAR_TRICK]` receipt with the expected field hash.
  - A generic-track run is invalid if that line appears at all.
- Derive the JSON track from what the planner received.
  - Do not take it from the wrapper's label.
- Your negative CLI fixtures do not prove that the flag reaches `Cgar::initialize` through the real `BaseSystem` path.

**A production-scale cold-start smoke test as a standing gate.**

- Run the first few timesteps on the real 10,000-robot instance, for both tracks.
- Run it before any 200-step screen.
- Run it whenever the pickup-metric predicate or the table-build path changes.
- No one-robot fixture could have caught the step-0 timeout.

**Scheduler and planner metric coupling under the static provider.**

- On a small directed static field, install the field through the trick path.
- At any tick after the first, check three quotes against each other:
  - the bounded pickup search;
  - the complete pickup field;
  - the planner's reverse-table potential.
- They must agree for the same robot state and the same first stop.
- At the startup call the scheduler must be on the generic basis.
  - This documents the intended mismatch.
- This extends your existing basis-equality regression to the static provider.
  - It does not mirror the predicate.

**A window summary of the cold ramp, reported for any trick run.**

- Report the maximum entry time over steps 0–100.
- Report the counts of fallback-metric robots and of exact-metric robots over the same window.
- This is a readout, not a unit test.
  - Early Dial builds combined with fallback scoring are the next most likely failure.

I retain your corrections:

- The 8-step chain gap is not a bound and carries no guaranteed gain.
- Improvement in the guidance potential is not physical work.
- A 1/10 forward ratio with turn cost 1 does not reproduce NMS's 20/200 with turn cost 1.
- Compact tables fall back to wide storage without any shortest-distance assumption.
- Holders-first rematching is a separate intervention on scheduling order.
