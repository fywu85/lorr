I ran nothing. Tags: [M] your measurement, [S] source fact as reported, [O] my opinion.

## 1. What the pool replay changes, and how to read the factorial

**What the replay settles [M].**
- KK strands long tasks before pickup, and keeps doing so.
  - Its unassigned pool averages 528 chain steps against 234 for completed tasks.
  - 1,642 unpicked tasks are 4,000 or more ticks old.
- CGAR's pool turns over: mean age is about 100 and no task is older than 1,000 at any checkpoint.
- Back-of-envelope from your numbers [O]: if tasks in service sit near the population mean, the revealed population averages about 245 for both systems.
  - KK completes tasks about 4–5% below that. CGAR completes about 1% below.
  - Selection, not a different task stream, plausibly produces the 8-step cohort gap.
- None of this quantifies what CGAR could recover [O].

**What it sharpens [O].**
- KK's held-unopened tasks are themselves long and old: chain 428, age 1,762.
- KK's 53-step first pickups against 7-step last pickups point the same way.
- Together they suggest rematching works as placeholder holding plus late binding. Robots drift toward long tasks they rarely pick up, and are diverted to fresh cheap tasks that appear nearby.

**How I would read the supplied-weights rematch arm against the keep arm.**
- Compare components of your accounting identity, not totals.
- The keep arm's cheaper scheduler gives the LNS more time. Check the iteration records before trusting any contrast.
- **Supports the hypothesis:** the keep arm shows more empty time per task, chains stay short, the pool still clogs, and throughput falls beyond the noise of the repeated control. Rematching is then what pays for stranding.
- **Weakens it:** the keep arm's empty time and chains match the rematch arm's. Additive cost alone would suffice under KK's planner. CGAR's 17% empty penalty would then be specific to CGAR, for example its truncated candidate discovery or its pickup weighting. The CGAR experiment should then target that, not rematching.
- **My model is wrong:** the keep arm's chains lengthen and the pool does not clog. An additive dispatcher should strand long tasks even without rematching.

**How I would read uniform against supplied weights.**
- Read the loaded-motion components separately from empty time and chain, because the weights also change scheduler costs.
- If waits, turns and detours rise sharply, KK's motion edge leans on a weight file of unproven provenance, and CGAR's motion lever is online field quality.
- If they barely move, planner structure matters more.

## 2. One bounded assignment design, if assignment is selected

**Factor 1: additive ranking.** This is the existing HRRN-off switch, with the oldest-task admission kept.

**Factor 2: task-centered late binding.** It runs every tick, before ordinary scheduling.

- **Roots.**
  - Unassigned tasks revealed within the last few ticks, held in a ring capped at 256.
  - Roots are never taken from another holder.
- **Candidate takers.**
  - Run a bounded BFS from the root's first stop (2,048 nodes or fewer) over the occupancy index.
  - Collect up to 16 robots that are either free or eligible unopened holders.
  - Eligibility is your existing predicate: not the primary or pending primary, not in recovery, not holding a fair-admitted task, and past cooldown.
- **Pricing.**
  - Pickup cost is one lookup in the root first stop's resident reverse oriented table.
    - If that table is absent, skip the root. There are no builds.
    - A retargeted robot therefore always has exact guidance.
  - The holder's remaining pickup cost comes from its current goal's table.
  - Chain cost is U times the native chain, on the same basis as ordinary pairing.
  - Richer chain pricing stays a separate factor.
- **Accept rule.**
  - Rank pairs greedily one-to-one by total additive cost.
  - A free robot takes a root unconditionally.
  - A holder takes a root only if its own total falls by at least max(4U, 10%) of that total.
  - Each holder exchange releases one task and takes one, so no robot is left without a task.
  - Released tasks re-enter the pool on the next tick.
- **Released-task externality.**
  - The released task is by construction the costlier one, usually a stale long task.
  - Under additive ranking it returns to the stranded pool, which is the intended cost.
  - Under HRRN the ordinary dispatcher would hand it straight back by age. That is why the rematch-only arm is expected to be ambiguous.
- **Progress.**
  - Each exchange strictly lowers the taker's remaining total, so exchanges per robot are finite.
  - Started tasks, the primary and recovery robots are untouched, and nothing is deleted.
- **Work per tick.**
  - At most 256 BFS runs of 2,048 nodes each, 4,096 table lookups and one sort.
  - No clock dependence. Measure it in the 200-step screen.

**How it differs from the failed pool pass [S/O].** It has fresh-task roots and not robot-centered scans of mostly stale tasks. It uses total cost and not a pickup-only guard. It prices in the heading- and toll-aware metric. It runs every tick without the cap of 32 exchanges per 10 ticks, and it has no permanent once-retargeted lock.

**Comparison arms.**
- Baseline, additive only, rematch only, and both, all on the current reference.
- Re-run additive-only on the current reference and do not import the old loss.

**Readouts.**
- Completed chain mean.
- All-empty time per task.
- First and last pickup.
- Exchanges per tick.
- Pool chain and age at your checkpoints.
- Waits and turns per task.
- Final-1000 stability.
- Six seeds before promotion, with a collapse watch given the pickup-weight-5 precedent.

**Fairness.**
- Retained:
  - validator legality;
  - started-task immutability;
  - primary and ticket liveness for robots;
  - the oldest-task admission cadence.
- Expected to worsen, by design:
  - the age p90 and maximum of unpicked tasks;
  - the number of tasks older than 1,000;
  - held-unopened age.
- KK shows the scale: a mean pool age of about 2,700.

## 3. One discriminating experiment, if motion is selected

**The measurement [M].** Per frame, roughly 810 to 830 robots are stationary.
- About 325 to 330 have a physically improving first-forward candidate blocked by one movable owner.
- About 140 have one blocked by two movable owners.

The one-owner case is already inside the production neighborhood, and it survives 4M global attempts plus 200,000 regional attempts. The open question is why.

**Experiment: an offline exhaustive joint counterfactual on dumped frames.**
- Use your frozen-decision harness. It is read-only and needs no production change.
- For a sample of one-owner and two-owner roots, fix everyone else's reservations.
- Search exhaustively, with pruning, over the root plus its owners, three robots at most.
- Look for complete replacements that raise both the scalar total and the physical total.
- Classify each root:
  - **(i) No improving replacement exists.** The wait is correct under the 5-step objective.
  - **(ii) One exists, and the production single-chain DFS rules can reach it.**
    - It was missed through coverage, the random skip, or the abort on the first rejected chain.
  - **(iii) One exists only through simultaneous multi-owner moves.**

**Decision rule [O].**
- If (i) dominates, more search structure is futile. This covers MD-PIBT, group LNS and regions.
  - The motion gap then lies in the objective, the horizon or guidance.
  - Choose between a 10-slot port and online field quality using the uniform-weights arm.
- If (ii) is material, fix the specific rule responsible.
- If (iii) is material and carries real physical gain, a true multi-owner mechanism finally has a demonstrated target.
  - The old transaction pass never showed one.

A heuristic blocker count cannot make this separation. The exhaustive replay can, within its robot cap, and that cap should be stated in the result.
