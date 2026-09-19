I read the supplied diffs and ran nothing. I found no reproducible correctness bug in `CompletePickupSearch::run`, in the parallel field construction, or in the top-k retention.

My turn-09 claim that cached oriented probes are rare was wrong, given your 96.5% figure. The churn hypothesis is rejected by your reconstruction.

**Why the search and its integration hold:**
- **Bucket discipline.**
  - Every push costs between `current + 1` and `current + 16`.
  - `current` advances only over empty buckets.
  - Any entry, stale or live, is therefore popped exactly when `current` equals its cost. Seventeen buckets are sufficient.
- **Stale entries.** They fail either the settled test or `distance_[state] != current`.
- **Integer bound.** The guard `cells <= (infinity - 16) / 64` keeps every sum below `infinity`.
- **Cell minima.** The first settled heading of a cell gives that cell's minimum.
- **Parallel build.**
  - Workers read only immutable state.
  - Each owns its scratch and writes disjoint field slots.
  - `run_temporal_preparation` joins on every path.
  - Field slots are published only after the join and a deadline check.
- **Top-k heap.**
  - With `better` as the comparator, the heap front is the worst retained pair.
  - Replacing it and then sorting is equivalent to a full sort and truncate, because `better` is a strict total order.
- **Uniqueness.** It still comes from `robot_used` and `task_used`.

## Findings

**1. `candidates()` full-field branch: the retention key changes the dispatch rule under HRRN. This is a semantic issue, not a bug, and probably not intended.**
- **What differs between the branches:**
  - The bounded searches keep the first `limit` tasks in nondecreasing pickup cost. Pair ranking runs on those afterwards.
  - The full scan keeps the top `limit` by `better`, and `better` compares HRRN score first.
- **Minimal case:**
  - Task A has age 0, pickup cost 2 and chain 100. Its score is 1.0.
  - Task B has age 500, pickup cost 200 and chain 100. Its score is 1 + 500/300 = 2.67.
  - The bounded policy never shortlists B unless it is among the 16 nearest pickups. The full scan keeps B and ranks it first.
- **Consequence.** With a quota of 32 or 64, which is at or above the typical number of free robots per step, most assignments become global age-weighted dispatch. That is a larger change than extended reach.
- **Regression gap.** The shortlist regression runs with HRRN=0, where `better` is ordered by cost. It does not exercise the production ordering.
- **Overlap (hypothesis).**
  - Field robots can retain largely the same old tasks.
  - Robots that lose every retained pair in the sorted pass are assigned in the second loop, in robot order.
  - A counter for field-robot assignments made in that loop would size this.
- **If the intent is discovery:**
  - Retain by total cost, pickup weight × pickup distance plus scale × chain, with task id as the tie-break.
  - Let the unchanged Pair ranking act on the retained set.
  - That preserves the existing two-stage structure.

**2. `initialize()`: `pickup_full_threads_` is not covered by the CPU-affinity check.**
- The supplied diff parses the option after the `required_threads` block and does not change that block.
- `CGAR_PICKUP_FULL_THREADS=32` on four reserved cores would oversubscribe silently, unlike your other thread options.
- Results stay deterministic, but decision-time margins do not.
- Parse the option earlier and include it in the check.
- This finding is conditional on that block being unchanged in the full source.

**3. `estimate()`: there is now a third basis in cross-robot comparisons. This is an approximation.**
- **The three bases:**
  - Field robots are priced on the physical free graph.
  - Other robots are priced from certificate-domain oriented tables.
  - Robots with neither fall back to the scaled unit estimate.
- **Expected impact:**
  - In fair admission this mixes bases across robots for a single task.
  - I expect low impact at 96.5% cached probes.
  - Field and table values differ only through pocket shortcuts.
- **Check:** one counter for fair admissions decided between robots on different bases would bound it.

## One bounded diagnostic and experiment

**Diagnostic on the running fields=16 arm, within one run on one weighted basis:**
- Split assignments by path: field, bounded, or second loop.
- Report three values at assignment time for each path:
  - normalized pickup cost;
  - chain cost;
  - chosen-task age.
- Report realized empty robot-steps per assignment for each path from the travel audit.
- **Falsifier for finding 1:** if the field path's mean pickup cost is within about 10% of the bounded path's, the locality concern is refuted. The arms can then be read as reach-only.

**Falsifiable reading of the full matrix, with no causal claim:**
- If quotas 32 and 64 show higher empty robot-steps per task and lower age p90 than fields=0, that is the signature of global HRRN dispatch.
- In that case, do not read the throughput sign as a verdict on complete fields.
- Rerun quota 32 on seeds 0 and 2 with cost-key retention.
- Success is empty robot-steps per task at least 1% below the fields=0 control, with throughput at least 0.3% higher.
- Otherwise reject complete fields.
