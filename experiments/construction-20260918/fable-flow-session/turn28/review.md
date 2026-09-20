I ran nothing. Points 1 and 4 are findings from reading the draft. Point 2 is reasoning from your measurements, not an estimate of gain.

## 1. Bugs and misleading points in the draft

- **Mode 3 is not structurally pure.**
  - The first loop writes `priced_chain = oriented_chain` for every covered task in every nonzero mode.
  - Mode 3 then relies on the second loop to overwrite that price.
  - I found no reachable state that leaks an oriented price. A native chain of 0 implies an oriented chain of 0, and a zero denominator implies no task was covered.
  - The placebo still depends on those two coincidences. Compute the oriented value in all modes, then assign the price by mode in one place.
- **An invalid location throws instead of falling back.**
  - `oracle.value` calls `index_.at(cell)`.
  - An out-of-range `from` cell raises `std::out_of_range`, and that kills the entry.
  - Bounds-check both stops inside `resident_chain_price` and return the fallback with its own reason code.
- **No guard against a static trick.**
  - Under `--trick`, the pickup metric is active from tick 1.
  - Chain pricing would therefore switch on in that track.
  - Reject `static_trick_metric_` to keep the two apart.
  - For this first run I would reject `temporal_remaining_flow_` as well.
- **The snapshot ratio is noisy straight after each publication.**
  - A table flush leaves only a handful of whole chains covered.
  - One task with many turns can then move κ by tens of percent.
  - Both mode 2 and mode 3 apply that κ to nearly every task for some tens of ticks, ten times per run.
  - The two modes share κ in any one state, so κ does not enter their contrast directly. Both can still drift away from the baseline for a reason unrelated to pricing.
  - An alternative with no threshold is to pool the numerator and denominator over every call since the last publication, and reset both at each publication. At minimum, log κ and the covered count on every call.
- **A wording point.** "Lower bound" is correct for the relaxed sum against the true oriented chain, and the sum is at least the scaled BFS chain. The production contrast also carries a BFS-versus-Manhattan correction that your offline audit never measured. You already record this. The logged basis shares are what would size it.

Minor items:

- `chain_price_changed` counts task observations, one per call, not distinct tasks. Label it that way.
- The combined `missing` counter hides the split you planned:
  - no table;
  - unreachable stop;
  - invalid location.

## 2. Reasonable next step, or go straight to rematching?

It is reasonable as a cheap background test, not as the main assignment experiment. Four measurements drive that view:

- **The predictive signal is real but small.**
  - Every fold improves in your audit.
  - The MAE gain is about half a step on chains of roughly 240.
- **Under HRRN the pool turns over.**
  - Mean age is about 100 ticks.
  - No task at your checkpoints is older than 1,000.
  - Chain price can therefore only reorder near-ties among tasks that will all be served anyway.
- **The 8-step cohort gap does not come from price accuracy.**
  - Your pool replays show the competitors strand long tasks.
  - HRRN's age term prevents that however accurate the price is.
- **The pricing helper is not wasted if rematching follows.** Any additive scheduler arm should price chains in this same metric.

The one assignment mechanism with paired evidence is KK's rematching ablation. In its keep arm, empty work rose by 23.1 steps per completion while chains stayed short. That supports keeping the additive-with-retargeting 2×2 as the main experiment. Whether chain pricing moves throughput at all is unknown from this data.

## 3. Smallest informative arms

**Step 1: a shadow mode with no dedicated host.**

- Compute the mode-2 and mode-3 prices and all diagnostics on every call, but keep native pricing in `pair_for`.
- Peeks mutate nothing, so trajectories must stay hash-identical. The shadow mode can therefore ride on control runs you already need.
- Log these:
  - whole-chain coverage, bucketed by ticks since the last publication;
  - fallback reason codes;
  - native basis shares, all-table against Manhattan-containing, for covered and uncovered tasks;
  - κ and the covered count on each call;
  - shadow rank changes, as below.
- For the rank changes, take each robot's retained candidates, at most 16, before conflict resolution.
  - Count how often the first choice differs under mode 2 and under mode 3.
  - Record the chosen task's oriented excess against the pool mean.
- Recording the basis needs a side flag in `task_chain_cost`. The flag must change no value and no cache admission.

**Step 2: policy arms, only if the shadow passes.**

- The shadow must show adequate coverage and a non-trivial share of changed first choices.
- Then run mode 2 and mode 3 on seeds 0 and 2, against the existing exact controls. That is four runs.
- Skip mode 1. Its bias always favours uncovered tasks, because oriented cost is never below scaled native cost. If steady-state coverage turns out near complete, mode 1 equals mode 2 outside the flush windows anyway.
- Read the results through your replay identity:
  - completed chain, unit and oriented;
  - loaded time;
  - empty work;
  - age tails;
  - final 1,000 steps.
- Go to six seeds only if mode 2 beats both the control and mode 3 on both seeds.

## 4. Two fixtures worth adding

**Publication boundary with no side effects.**

- Drive a real production sequence across a publication:
  - tick p, covered under the old tables;
  - tick p+1, after the flush, with zero coverage;
  - then partial coverage, with κ coming from a single covered task.
- Assert these:
  - assignments in all three modes are bitwise native at zero coverage;
  - imputed prices match the hand-computed ceiling;
  - a task's price after its table is rebuilt under the new metric differs from its price before the flush, which proves there is no stale value;
  - up to the first divergence in assignments, turn-oracle residency, LRU order and `oriented_builds` are identical to mode 0.

**Placebo purity and unit consistency under a tolled field.**

- Build a state where a covered task's oriented chain differs from κ times its native chain. Its loaded leg runs against a published toll.
- Assert these:
  - mode 0 breaks the tie on task id;
  - mode 2 flips to the other task;
  - mode 3 prices every nonzero native chain at the κ ceiling and never reads the oriented value;
  - κ is identical between mode 2 and mode 3;
  - flow cost scales 1 and 4 give the same assignment;
  - the fair-admission robot, assignment uniqueness and started tasks are unchanged.

Your saturation, repeated-stop, disconnected-cell and invalid-index cases fit inside these two fixtures. They do not need separate lifecycles.
