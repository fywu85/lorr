I ran nothing. This is a read of the 4e1a8fe delta plus your verified audits.

## Source findings

**What holds up.**
- The bound is `max(1, spatial) + chain + repeats > H − now`. It is sound:
  - `value` falls back to Manhattan when the table has no entry;
  - the chain cache can only weaken the bound;
  - equality counts as feasible, which matches your trace check.
- Tiering comes first and the ordinary order applies within a tier, so the comparator is still a strict weak order.
- The fallback loop and its sampled improvements use the tiered comparator.
- `fair_admission` and the bucket sort keep the ordinary comparator.
- After H the flag is never set.
- The prefix argument is sound.
  - The flag is read only inside comparators.
  - If no comparison records a changed decision, every sort and heap executes exactly as in the control.

**Concrete items.**

1. **The assessment runs for the whole run, not only late.**
   - `horizon_active` is true from tick 0.
   - Every `pair_for` call therefore pays for a hash `peek`, a table read and two counter increments from the start. That includes every full-field scan.
   - The effect is on timing only. Watch maximum decision time across all 5,000 ticks, since the profile already peaks near 1.05 s.
   - An exact gate with no tuning is available for the next build.
     - Skip the assessment in any call where `R ≥ max(chain + repeats over the pool) + D`.
     - D is an upper bound on any BFS distance. Twice the eccentricity from a single init-time BFS gives one.
     - With a Manhattan-only pickup bound, D is simply `rows + cols`.

2. **Rank-change detection assumes the baseline is `ordinary_better`.**
   - With `CGAR_PICKUP_FULL_COST_KEY=1` the retention baseline is cost order.
   - Tier flips inside the heap would then be compared against the wrong baseline.
   - The logged first-change tick would stop being a safe bound in that mode.
   - Production has cost-key off. Either reject cost-key together with the horizon, or compare against the active retention baseline.

3. **Receipt coverage.**
   - The horizon prints its own line and does not appear in `[CGAR_TRICK_COMPONENTS]`.
   - The immutable verifier should require `[CGAR_TRICK_HORIZON]` exactly when the configured value is nonzero.
   - It should also stamp the result as known-horizon.
   - H=0 with the variable present prints nothing, so the case manifest is the only record of that setting.

4. **Test gaps.**
   - No fixture has BFS greater than Manhattan changing the tier.
     - A pickup behind a shelf would be impossible with a resident table and feasible without one.
     - In the straight-line fixtures both bases give 11, so neither a wrong `value` argument order nor an unused table would be caught.
   - No fixture drives the empty-shortlist fallback path with its sampled candidates under an active horizon.

5. **Parsing.** Leading zeros are accepted, so "05000" parses as 5000. This is harmless, but it is not strictly canonical.

## The audit and the follow-up

- **The OFF/ON matrix was warranted, as I read your audit.**
  - About 9.3k late non-finishing admissions were provably impossible for their holder.
  - About 0.7k were slack failures.
  - About 4.0k are task-disjoint alternative witnesses.
- **The baseline slack count will understate slack under the policy.**
  - Redirected admissions are chosen by HRRN within the feasible tier, not by slack.
  - Many will therefore sit close to the bound.
  - The ON run's own replay gives the relevant numbers for late admissions from the feasible tier: how many finished, and `realized − LB` plotted against LB.
  - Decide the follow-up from that, not from the OFF audit.

- **A cumulative ratio is the more error-prone calibration.**
  - Your own accounting shows overhead that looks closer to additive per task than proportional to distance: loaded turns about 5.7, loaded waits about 7.5, plus empty-leg overhead.
  - ρ·LB would therefore under-predict precisely the short tasks that decide the endgame.
  - Even a correct mean puts a boundary task near even odds.
  - Excluding retargeted tasks is right.
  - If you calibrate at all, fit `realized ≈ a·LB + b` online from sufficient statistics. It stays untuned and deterministic.
  - Check its late-window calibration read-only on the ON run before it ranks anything.

- **A simpler follow-up needs no estimator.**
  - Within the feasible tier only, order pairs by ascending LB, then by the ordinary comparator.
  - Switch this on at the first tick where any pair is provably impossible. That trigger is self-defined and needs no window constant.
  - It reuses the quantity you already compute, and it is monotone in slack.
  - For counting completions by a common deadline, shortest-first is the natural order.
  - Your V80 data is supportive: short preference improved every initial 1,000-step window. It is not clean evidence, because that window includes the startup transient.

- **Conjecture and caveat.**
  - A window of under about 1,000 ticks should avoid the later empty-travel loss that sank short preference.
  - Against that, ordering by ascending LB spends the shortest tasks early and may starve robots that free up later.
  - Treat it as a distinct, separately labelled mechanism.
  - Judge it against the minimal-rule arm and do not merge it with that arm.
