I ran nothing. Tags: [M] is your measurement, [S] is a source fact as you reported it, [O] is my opinion.

## Inferences in the synopsis I would challenge

**1. "KK's reassignment is not a throughput explanation."**
- [O] This misses what reassignment enables.
- [M] Rematching does not lower KK's empty time.
- [O] It may be what lets an additive-cost dispatcher select short chains without the empty penalty CGAR paid:
  - [M] CGAR with HRRN off shortened chains to 235.3 but raised empty travel 17–18%.
  - [S] Both competitors pair additive cost with rematching of every unopened task on every tick.
  - [O] A fresh cheap task then goes to the nearest of about 2,000 en-route robots, not the nearest of about 28 free ones.
- [M] KK's completed chains are 3.3% shorter than CGAR's.
  - [O] That is about what stranding the roughly 5,000 longest tasks in the pool would produce. This is my turn-05 estimate, never tested.
- [M] "Removing age regresses" was measured without rematching.
- [M] "Pool exchange is weak" was measured under HRRN, at 32 exchanges per 10 ticks.
- [O] That exchange test also ran with a pickup-only guard. I recommended that guard, and it forbids exactly this chain-mix gain. Treat it as a constraint I introduced, not as evidence against the mechanism.

**2. "KK's targeted LNS explains its lower motion overhead" is unproven.** [S] KK also differs from CGAR in:
- a planning window of 10 with 3 executed steps;
- SIPP single-agent replanning;
- a LaCAM2 initializer;
- a warehouse-specific weight file, which you did not count as a confound.

[O] That file is an offline-optimized guidance graph.
- [M] CGAR's online field was worth +23% to CGAR.
- [O] An offline-tuned field could plausibly explain much of KK's lower waits and turns.
- [O] CGAR cannot copy the file, so whatever share of the gap it explains is not addressable by LNS work.

**3. The motion gap is not a cohort artifact.** [M] Per 100 chain cells:

| | CGAR | KK |
|---|---|---|
| Turns | 4.0 | 2.4 |
| Waits | 5.6 | 3.6 |
| Detours | 3.2 | 2.0 |

[O] Shorter chains do not explain these differences.

## The best three experiments, in order

**1. Two KK ablation runs.** This is pure diagnosis and decides how the 20.5-step gap splits.
- **(a) KK with uniform weights instead of its file.**
  - Mechanism [O]: map-tuned guidance suppresses conflicts.
  - Falsified if KK's waits, turns and detours per task stay near 8.4, 5.7 and 4.6.
- **(b) KK with rematching of assigned-unopened tasks disabled.**
  - Mechanism [O]: rematching lets additive cost select short chains at no empty cost.
  - Falsified if the completed chain stays near 234 and empty time does not rise.
- **Readout:** your existing per-task identity table on each KK trajectory.
- **Caveat:** these are not equal-resource scores. They are one-seed diagnostics only, and I expect them to say more than any CGAR run now.

**2. CGAR scheduler restructured to what both competitors share:** additive cost plus bounded per-tick rematching of unopened assignments, with chain-mix gains allowed.
- **Pricing.**
  - Robot-to-first-stop cost comes from resident reverse oriented tables, one lookup per pair.
  - Chain cost comes from your drafted oriented chain with ratio imputation.
  - This folds experiment A in here. Under HRRN I expect sub-percent from A, so it should not take the first full benchmark.
- **Discovery.**
  - For each newly revealed task, run a bounded BFS over the occupancy index from its first stop. This makes the search task-centered.
- **Exchange rule.**
  - An unopened holder is eligible as the taker if switching to the new task strictly lowers its own total.
  - Any task it releases returns to the pool.
  - Your exchange pass's exclusion and cooldown rules carry over: primary, pending primary, fair, recovery and cooldown.
- **Minimum discriminating measurement:**
  - the completed chain falls toward 234–236;
  - all-empty time does not rise by as much as the chain saving;
  - there is no late collapse;
  - promotion requires six seeds.
- **Falsified if** the empty increase eats the chain saving, as with HRRN off, or if pickup-weight-5 style collapses recur.
- **Pre-screen:**
  - [M] Your HRRN-off runs already exist.
  - Replay those trajectories using current information only.
  - At each assignment, check whether an eligible robot other than the chosen one (a free robot, or an unopened holder other than the primary) had a lower oriented pickup cost to that task.
  - Count greedily, consuming each robot once.
  - If that recoverable empty travel cannot cover the 17% increase, drop the experiment without building it.

**3. One motion experiment, chosen by result 1(a) and a refreshed audit.**
- **If KK keeps its motion advantage without the weight file:**
  - The cause is its planner structure.
  - Refresh the multi-owner audit on the current reference first, including the physical partition.
  - Add an offline predictability statistic. For each executed wait, find how many ticks earlier the conflicting pair's guidance-shortest paths already met in space-time.
    - If most conflicts were visible 6–10 ticks ahead, the 5-slot horizon is the binding limit.
    - Only a 10-slot port tests that. No cheaper proxy exists in your representation.
  - If the audit shows a material population of improvements blocked by multiple movable owners, run group-destroy (stage 1 of your B) once. Compare it with the same fixed work spent on extra regional attempts.
- **If KK loses its advantage without the file:**
  - Put the effort into the quality of the online field (your E).
  - At the existing publication points, add a delay-informed or load-informed term.
  - Gate it on shadow evidence first.
  - The persistent-route guides that failed do not rule this out. Their mechanism was route-to-go potentials, not edge weights.

## Is conflict-connected LNS different from regional repair?

[O] At the same 5 slots, the same 129 operations and the same scalar score, mostly no. It would add two things:
- **Simultaneous multi-robot removal.**
  - [M] The one- and two-owner transaction pass approximated this. Its six-seed mean was −0.29%.
- **Delay-targeted roots.**
  - [S] That same pass already restricted its roots to stationary robots with physically improving candidates.

[M] Deeper or differently structured search at this objective has mostly lost:
- 8 regions;
- 4 regional rounds;
- zero temperature;
- 16 workers, which was mixed.

[M] Only 4 regions with 2 rounds converted.

The meaningful parts of KK's LNS are the 10-step window with 3 executed steps and optimal single-agent space-time replanning. Stage 1 cannot test those parts.
- A null stage-1 result would not reject WPPL-style planning.
- A positive result would probably be small.

Hence its demotion to a conditional third.

## Where I think the addressable gap is [O]

- **Chain cohort, about 8 of the 20.5 steps.**
  - This part is addressable.
  - The mechanism is known and both competitors use it.
  - It is limited only by the congestion-stability risk CGAR has already shown.
- **Motion, about 12.3 steps.**
  - An unknown share of it is KK's map-tuned guidance.
  - It is addressable through online field quality or through a longer horizon, never by copying the weight file.
  - I do not know the split. Experiment 1 exists to find it.

**What would change my mind:**
- If ablation (b) leaves KK's chain and empty time unchanged, my stranding hypothesis is wrong and experiment 2 falls to a pricing rider.
- If ablation (a) leaves KK's motion unchanged, guidance is not the driver and the horizon port becomes the main line.
- If the pre-screen shows too little recoverable empty travel in the HRRN-off runs, drop experiment 2 before building it.

## Limits

- [M] The per-task accounting identity is exact as an identity: totals of 347.36 and 326.84 robot steps per task give a ratio of 1.0628. No proof establishes what causes any component of the gap.
- [O] Every mechanism above is a hypothesis.
- The ablations are single-seed diagnostics of a competitor run under different resource limits. They bear only on how to allocate effort and never on a CGAR score claim.
