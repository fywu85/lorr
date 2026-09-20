I ran nothing. Tags: [M] is your measurement, [S] is a source fact as you reported it, [O] is my opinion.

## What the NMS replay changes

[S] NMS's active kernel is essentially CGAR's:
- five slots;
- 129 operations;
- single-owner displacement;
- annealed repair;
- power 1.

[M] Its loaded overhead is 16.8 steps per task. CGAR's is 31.0.

[O] That weakens my turn-19 emphasis on a longer horizon or SIPP-style LNS, because a five-slot one-owner kernel is evidently sufficient for low overhead. The motion gap is therefore mostly outside the kernel. The candidates are:
- guidance;
- assignment;
- search budget;
- heuristic coverage.

[S] Both competitors use map-specific guidance: NMS's hard-coded lanes and KK's weight file. Neither can be imported, and the KK uniform-weights arm is still pending, so none of this is proven.

## Points 1, 2 and 5

I agree on all three:
- There is no disabled multi-owner code to credit and no power rule to copy.
- The 5×pickup factor should not be copied.
- The full-matrix heuristic memory should not be copied.
- The idle-step correction touches only the NMS diagnostic accounting.

## Point 4: the root-plus-owners snapshot probe

[O] It is a useful counterfactual, and it differs from the failed two-owner pass.
- That pass reported only acceptances and throughput under fixed work limits.
- The probe reports whether a strictly improving complete replacement exists at all, in both scalar and physical terms, inside a tiny group with every other robot's reservations fixed.

**How to read each group size.**
- **Root plus one owner.**
  - Production can reach this case in principle.
  - A witness points to one of:
    - attempt coverage;
    - the random skip;
    - the rule that aborts on the first rejected chain, so the root's second-best candidate is never tried.
  - It does not point to the owner's choice rule. The first free candidate is the owner's best feasible choice for a given root move.
- **Root plus two owners.**
  - Production rejects these at the `-2` return.
  - A witness shows a multi-owner opportunity exists at that snapshot.

**Caveats that must stay attached to any result.**
- The group is smaller than production's chain neighbourhood.
- A null says only that this group cannot improve with everyone else fixed.
- A witness shows existence. It does not show reachability or throughput value.
- Roots are sampled in robot-index order from a rotating start. That is fine, but report how many roots were captured per class.

**Two risks in the capture itself.**
1. **Payload size inside the timed entry.**
   - Up to 129 choices per robot are printed, each with cells, edges and owner arrays, for up to 64 groups per frame.
   - That is megabytes of JSON written inside the one-second entry, against a reference maximum of 928 ms.
   - A capture frame that lands on a heavy tick would fail the run explicitly.
   - Keep the per-class count small, or drop the fields that (start, heading, operation) already determine: cells, edges, heading, depth and validity.
   - Only the owner arrays are essential.
2. **The 200-step identity screen.**
   - It must include at least one capture frame adjacent to a publication tick.

## Point 3: a bounded streaming form of task-centered rematching

Under additive cost, the endpoint-group factorization applies.
- For a first stop with a resident reverse oriented table, each robot's cost is a single lookup.
- The cheapest pair in the group is its shortest-chain task with its cheapest robot.

**Streaming pass, run before ordinary scheduling:**
- **Roots.** Tasks revealed this tick, plus a short ring of recent still-free tasks, capped at 64.
- **Scan.**
  - For each root whose first-stop table is resident, scan the free robots and the eligible unopened holders.
  - No BFS, forward fields or table builds are needed.
- **Work.** About (roots × eligible robots) lookups plus one lookup per holder for its current remaining cost. Each is probably a cache miss, so measure it and do not assume a figure.
- **Accept rule.**
  - Free robots may always take a root.
  - Holders take one only if their own total improves by a margin.
  - Rank the resulting pairs greedily, one-to-one.
  - Each acceptance releases exactly one task and takes one.
- **Finite retargeting by explicit caps.**
  - Cap the releases per task and the retargets per robot per empty episode, and keep the cooldown.
  - Costs vary over time, so I make no liveness claim from cost decrease.
- **Protections.** Primary, pending-primary, recovery, fair-admission and started-task rules stay as they are.
- **Arms.**
  - Baseline, additive only, rematch only, and both.
  - Richer chain pricing stays a separate factor.

**Coverage diagnostic to run first (read-only, default-off, hash-neutral).** Log three things per newly revealed task:
- whether its first-stop table is resident;
- the best eligible holder's improvement margin, and the best free robot's total;
- after an offline join, the pickup cost of the robot that eventually served the task, against the best holder's cost at reveal.

**How to read it:**
- If residency for fresh first stops is low, the streaming design cannot act without new table work.
- If holders rarely beat the eventual assignee by the margin, drop the rematch factor before building it.

## The at-most-two next experiments

Finishing the in-flight probe is a measurement, not an experiment. It should not by itself fund a planner rewrite.

**1. The assignment 2×2 above, gated by the coverage diagnostic.**
- [M] Both competitors strand long tasks:
  - NMS's unassigned pool averages 507 cells per chain;
  - the mean age of those tasks is 2,703 steps;
  - NMS's completed chains average 234.8.
- [S] Both reopen every unopened task.
- [M] Empty work is 62 to 66 steps per task in all three systems.
- [O] Additive cost combined with late binding is the one assignment combination CGAR has not tested. Under it, the waiting-time tails will worsen by design.

**2. A guidance-coherence measurement, and only then a change.**
- [O] CGAR's per-edge learned field tolerates neither stronger tolls nor margin 0.
- [O] NMS's coherent, complete lanes tolerate a 10:1 cost ratio with the same kernel as CGAR's.
- **Hypothesis:** spatial incoherence along corridors limits how strong CGAR's field can usefully be.
- **Measurement, from the raw trajectories you already replay for all three systems:**
  - Take each maximal straight chain of degree-2 cells.
  - Compute the traffic-weighted share of crossings in the minority direction.
  - For CGAR, also compute the disagreement in toll sign within each chain, from the reconstructed published fields.
- **Reading:**
  - If CGAR's minority-direction share is about equal to NMS's and KK's, drop the idea.
  - If it is materially higher, test one change. Pool the observed counts over each chain and publish one graded toll decision per chain. Non-chain cells are unchanged, and margin and strength stay at the reference values.
- **Constraint question.**
  - This pools an existing estimator over topologically equivalent edges.
  - Geometry prescribes no direction, parity or map identity.
  - You should still judge it against your constraint on lane rules before building.
  - I expect a nonzero collapse risk, given margin 0.

**Order.**
- Run both read-only measurements now.
- Let the KK factorial decide which full experiment goes first:
  - If KK's motion overhead rises sharply under uniform weights, guidance goes first.
  - If KK's keep arm pays in empty time while chains stay short, assignment goes first.

## What would change my mind

- **Assignment.** A coverage diagnostic showing few holder improvements removes the rematch factor.
- **Guidance.** Equal minority-direction shares across the three systems removes the coherence hypothesis.
  - Attention then shifts to search budget and coverage: NMS's 32 clock-limited workers and its fully precomputed heuristics.
  - [M] CGAR's fixed-work worker tests there were mixed.
- **Probe.** A witness-rich result, especially in the two-owner groups, would reopen search structure despite the NMS evidence.
