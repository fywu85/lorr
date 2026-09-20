I ran nothing. Everything here is conditional on the replay that is still queued.

## 1. Replay audit

**Counting and orientation.** I see no defect in these parts:
- Edge direction normalization is consistent per edge and per chain.
- Group edges are appended in order along the line.
- The `surcharge` formula matches the rule you described: margin 25, graded strength 4, minimum 8.
- You assert the reconstructed field against the unchanged FlowGuidance.
- `observe(t)` runs before the actions at t are processed, so publication timing matches production.
- Resetting `previous` on a neutral edge means sign flips are counted only between directly adjacent directional edges. Confirm that is what you intend.

**Four measurement problems, all in how the numbers would be read:**

1. **Temporal aggregation.**
   - Minority shares are summed over the whole run, over everything after step 1,000, or over the final 1,000 steps.
   - CGAR's field can reverse at a publication. A static competitor field never does.
   - A chain that is cleanly one-way before and after a reversal therefore reports a large minority share with no simultaneous contraflow.
   - The final-1,000 window spans two publications.
   - Compute the shares per publication epoch for every system, with the same fixed windows for KK and NMS. Then average them weighted by traffic.

2. **Minority crossings are not avoidable conflict.**
   - Your `limits` field already says as much.
   - A robot that services a cell inside a chain and leaves the way it came adds minority crossings that no convention can remove without a detour.
   - Neither the per-edge share nor the pooled share separates this from contraflow.
   - Add one tick-level exposure statistic:
     - Take the robots inside a chain or on its end edges.
     - Sum over ticks the smaller of the count heading along the positive axis direction and the count heading along the negative.
     - Divide by the total chain occupancy.
   - Also count face-to-face adjacencies, meaning two adjacent robots with opposing headings along the axis.
   - This costs one group lookup per robot per tick and needs no new inputs.

3. **The chains are too small for the question.**
   - Every chain has 2–3 interior cells.
   - Through-traffic crosses all 3–4 edges of a chain equally.
   - Per-edge tolls inside a chain are therefore probably already consistent. That is a prediction, not a result.
   - The chain statistic cannot show whether directions are coherent along a line through junctions. That is the scale of the structure you would compare against.
   - Add a second grouping to the same program: maximal collinear free runs.
     - Horizontal edges are grouped by row run, vertical edges by column run.
     - Runs split only at obstacles.
   - Report the same sign statistics, the same gap statistics and the same per-epoch minority statistics for these runs.

4. **Cross-system comparison.**
   - Normalize by chain traversals or by occupancy, not by raw crossings.
   - Cohorts and volumes differ, and the NMS trace contains a timeout.

## 2. The pooled operator

**The evidence rule.**
- Requiring the pooled total to be at least minimum_samples times the number of edges is sound. It keeps the mean per-edge evidence instead of relaxing it.
- Pooled dominance is a traffic-weighted average. It leans toward through-traffic, which is the right bias.

**Expected effect.**
- With chains of 3–4 edges the operator can change little. It can only fix mixed signs or gaps inside a chain.
- Your replay's counterfactual counts will size this before any run:
  - changed directed edges;
  - new and removed penalties;
  - historical crossings on changed directions.
- If those counts are small, skip the operator.

**The one measurement if gaps are many and mixed signs are few: prospective agreement.**
- Take each neutral edge inside a chain that is otherwise directional.
  - Record the pooled sign at publication p, using only counts up to p.
  - Record the realized majority direction on that same edge during the following epoch, judged with the same margin.
- Compare the agreement rate on gap edges with the same rate on edges that were already directional. Weight both by traffic.
  - If gap edges agree at about the baseline rate, completing them is predictive and useful.
  - If the realized traffic is balanced or opposite, the pooled toll would be imposing a direction the traffic does not support.
- As a secondary breakdown, split gaps by cause.
  - Gaps from too few samples are the plausible beneficiaries.
  - Gaps from a balanced margin are the risk.

## 3. If chains are already coherent

**Hypothesis.**
- An edge or a 3–4-edge chain is too small a unit for learning a direction.
- Local preferences can then be coherent without forming a usable one-way circulation across junctions.
- Example: adjacent parallel lines all prefer the same direction. Legs are then forced either to pay tolls or to take long detours.

**Measurement, offline and current-information only, using your reconstructed fields.**
- For each completed task leg, under the field live when it started, compute:
  - its unit length;
  - its shortest toll-free length, using only untolled directed edges;
  - the tolls paid on its weighted-shortest path.
- Report:
  - the share of legs with a toll-free path at unit length;
  - the distribution of toll-free detour length;
  - the share of legs with no toll-free path.
- Add the sign-flip rate along collinear runs from item 3 of the replay audit.
- Decision:
  - If detours are small and lines are consistent, circulation is not the limit. Drop this direction.
  - If many legs need tolls or large detours, the hypothesis is supported.

**One conditional operator: coarse-to-fine pooling with the unchanged evidence rule and margin.**
- At each publication, for each edge, take the coarsest group that passes both tests.
  - The pooled total is at least the minimum times the number of edges in the group.
  - The pooled dominance exceeds margin 25.
- Try the collinear run first, then the degree-2 chain, then the single edge.
- Apply that group's graded toll.
- This contains your chain operator as its middle level.
- Properties:
  - Directions remain learned.
  - Groupings are computed once from obstacle geometry.
  - The rule is equivariant under rotation and reflection.
  - Ties stay neutral and every edge stays usable.
  - Work is linear.
- Alternation between neighbouring lines is not imposed. It could emerge only through the existing refresh feedback, because traffic displaced from one line polarizes the next.

**Risks.**
- A long line with heterogeneous demand fails the margin and falls back to a finer level. That is why the fallback exists.
- Broader coherent tolling can tip CGAR over, as the margin-0 runs showed.
- Whether a collinear run counts as a generic estimator unit or as a lane rule is your call under the generic constraint.

**Gating.**
- Run the line-level pooled counterfactual in the replay first. If it would remove more penalized traffic than it adds, do not build it.
- The early TRICK screen is not evidence for this mechanism.
- Its maximum entry times of 995 ms and 988 ms suggest the full TRICK runs may fail on the deadline before they produce any result.
