# Complete pickup fields and wider candidate ranking

V42 adapts NMS's broad task comparison to CGAR's fixed-work entry contract.
`CGAR_PICKUP_FULL_ROBOTS` selects0–64 free robots in the existing rotating order;
default0 disables the feature. `CGAR_PICKUP_FULL_THREADS` accepts1–32, default4.
It requires pickup flow and activates only after the first complete published
traffic field. Other robots continue through the existing bounded candidate
search; every eligible free robot still reaches the assignment/fallback pass.

For each selected robot, a complete forward shortest-path calculation prices all
reachable pickups from its observed cell and heading. Positive integer edge costs
1–16 use17circular buckets, with worker scratch reused between fields and steps.
Each cell receives the minimum over its terminal headings. A fixed robot quota
bounds the number of complete graph traversals. The entry must finish that work
within its deadline or fail explicitly. All builders join before field-to-robot
slots are published, including exceptions from allocation, computation or thread
creation through the existing preparation helper.

Selected robots scan all currently unassigned eligible tasks and retain the best
existing-size shortlist (up to16; up to8when replenishing) by the unchanged
pickup/chain/HRRN objective. This changes candidate discovery: the old local
search first found nearby endpoints and then ranked them. The new policy can
choose a cheaper whole task outside those nearest endpoints. Global pair limits,
unique assignments, oldest-task admission and started-task protection remain.
Complete fields also supply finite fallback/fair estimates for their selected
robots. No task forecasts, map identity or map-specific routing templates are used.

As in v41, scheduling reads the last complete publication, before the current
plan's observation/publication. Chain estimates remain spatial approximations.
The field uses physical free cells, restricted to the core in capacity mode;
a start outside that core is not selected. Cached certificate-domain estimates
can differ around pockets. Unreachable field values fall through to the former
fallback estimate. These are static distance estimates, not predictions of
congestion waits or complete task service time.

## Validation

Build8898839 passed all regressions. A final test-only extension checks two free
robots with a quota of one: one gets a complete field, the other still receives
a distinct assignment, and simulator metadata remains unchanged. Build8898842
(`runs/cgar-pickup-full-build-v42-r1-20260919`) passes the full suite with that case.
Production source is identical between those builds. No C++ build is active.

The independent repeated-relaxation oracle compares32256goal costs against both
the bounded heap search and the complete bucket search, across directed costs,
obstacles/disconnection, all headings and scales1/2/4/8. Complete fields must settle
every reachable state. Additional tests compare twelve complete fields with one
versus four workers, inject failure inside a worker, verify other workers joined,
reuse scratch after a failed search, reject invalid costs/configuration, preserve
cold behavior/fair admission/started tasks, and choose a better task beyond the
nearest16endpoints. Synthetic scheduler tests ingest a prescribed physically valid
observation trace; full simulator runs validate actual offered actions separately.

[Build provenance](build-provenance/v42/) independently reconstructs all22 requested
source/test hashes from its recorded base plus patch. Fields are published only
on successful batch completion; the kernel's output buffer itself can be partial
when it throws and must not be read as a completed field.

Screen8898846 passes all five profiles: disabled,16serial,16parallel,32parallel,
64parallel. The maximum full entry is0.892714s and peakRSS5.224GB. Disabled
reproduces the v41graded/pickup-flow prefix;16serial and parallel prefixes exactly
match. These200-step runs establish feasibility and equivalence, not throughput
ranking or full-horizon memory usage.
[Screen](results/pickup-full-screen-v42/).

Full8898851 with analysis8898853 compares0/16/32/64fields, four build threads,
seeds0/2,5000steps. Eight cases run concurrently on disjoint four-core allocations,
128GiB aggregate reservation and32decimalGB individual limit. Configurations:
[pickup-full-variants.json](pickup-full-variants.json).
The original control was v41's graded pickup-flow policy, then under evaluation.
Its separate six-seed confirmation subsequently passed, establishing the preceding
reference; the first two full-field comparisons still use their matched controls.

## Full results and confirmation

All eight full tests completed with valid decisions, maximum0.886371s and peak
RSS11.937GB. Both disabled controls exactly reproduce the v41graded/pickup-ON
full trajectories. Exact tested source:[4872d04](https://github.com/fywu85/lorr/commit/4872d048dd8155927074ac2a4d27a55756266fd5).

| Fields | Seed0 | Seed2 | Mean vs disabled | Final1000 differences |
|---|---:|---:|---:|---|
|0|138963|139677|reference|reference|
|16|139697|141725|+0.998%|+70/+378|
|32|70043|142735|−23.637%|−23883/+613|
|64|141829|142988|+2.217%|+401/+670|

Fields16/64 also lower outstanding agep90 on both seeds. Fields64 reduces total
empty robot-steps7.60%/7.84%, averages285.1–287.1ms per entry and1.466–1.473CPU
cores, reaches11.898GB RSS and takes24.98–25.17minutes. The32-field seed0 failure
is a policy collapse, ending with5433tasks in its final1000steps; decisions remain
valid. The response is not monotone in quota, and64's improvement contradicts
a blanket claim that global HRRN discovery must hurt. No causal explanation yet.
[Complete comparison](results/pickup-full-full-v42/comparison.json).

Job 8898882/analysis 8898883 completed the remaining seeds 1/3/4/5: twelve
simultaneous cases on 48 reserved physical cores, four per process, 192 GiB
aggregate reservation and a 32 decimal GB per-process cap. Together with the
first pair, all 18 runs pass and all six disabled trajectories exactly reproduce
the preceding pickup-flow reference. All 22 source/test hashes match 4872d04.

| Fields | Seeds 0/1/2/3/4/5 | Mean | Change vs disabled |
|---|---|---:|---:|
|0|138963 /140002 /139677 /138671 /139241 /139773|139387.8|reference|
|16|139697 /141925 /141725 /140734 /140928 /141878|141147.8|+1.263%|
|64|141829 /143325 /142988 /141802 /141988 /142917|142474.8|+2.215%|

Both candidates improve every full total, final 1,000-step window and outstanding
age p90. Fields64 also beats fields16 in every full total and becomes the reference.
Its empty robot-steps fall 7.43–7.84%, final-window gains are 355–670 tasks, and
age p90 falls 5–14 steps versus disabled. The rejected32 policy remains separate
evidence; the quota response is not monotone.
[Six-seed evidence](results/pickup-full-six-seed-v42.json).

The promoted policy averages 272.4–317.4 ms per complete decision, p99
557.3–602.0 ms and maximum 874.2 ms. CPU averages 1.425–1.494 cores out of four,
peak RSS is 11.934 GB, and full wall time is 23.93–27.70 minutes. All 30,000
candidate entries meet one second.
[Full quantiles and resources](results/full64-reference-latency-six-seed-v42.json).

The CPU-affinity guard omission found by Fable is fixed in v43; v42 benchmarks
explicitly allocate four cores to four threads, so their allocation is valid.
The cost-based shortlist follow-up loses and is rejected; retain the original
age-aware key. See [cost-key results](PICKUP_COST_KEY.md).
