# PILOT RANDOM-03 and RANDOM-04: ten-percent campaign

Started 2026-09-21 05:17 UTC at the user's request. Throughput must be at least
10% above matched local NMS on both instances, with runtime reliably below one
second. This follows the completed RANDOM-05 4,000-task milestone; that record,
its four-core counterpart, and all held-out inputs remain preserved.

| Instance | Current selected best | Matched NMS | Minimum +10% | Full steps |
|---|---:|---:|---:|---:|
| RANDOM-03 | 2,551 | 2,359 | 2,595 | 800 |
| RANDOM-04 | 2,565 | 2,580 | 2,838 | 1,000 |

The comparison uses 16 physical EPYC9354 cores / 32 SMT workers, a 32 decimal GB
process limit, 30-second initialization, and strict 1-second entry deadlines.
GRID jobs run in parallel on verified allocations. Hosts may be shared; CPU
binding and no-quota checks remain mandatory. Search has a fixed work budget.
Any overrun invalidates the attempt; no wall-clock-truncated portfolio is returned.

## Qualification before completion

- Reach both archived-input thresholds and independently replay all actions,
  assignments and task events. Record commits, timestamps, exact settings,
  throughput and completed/censored waits in the progress artifacts.
- Preserve seed-specific bests, but distinguish them from reproducibility.
  Repeat each selected archived configuration exactly, and test runtime across
  at least three planner seeds, retaining every attempt. Target a maximum below
  900 ms for headroom; every qualification run must remain below 1,000 ms.
  A configuration with qualification timeouts is not called robust.
- Freeze selected source, configuration, planner seed, comparison protocol and
  new input seeds in a commit before generation. Use two fresh task/start inputs
  per density, each with two unmodified NMS repetitions. Report the advantage
  over the stronger NMS repetition on each input and in aggregate; seek at least
  10% aggregate on each density. No substitution or tuning on these inputs.
- Keep general mechanisms separate from explicit `--trick RANDOM-03` and
  `--trick RANDOM-04` guidance/horizon choices. Per-instance presets are selected
  development settings, not a claim of one universally optimal configuration.

## First declared comparisons

Manifest: experiments/random34-tenpercent-first-full.json. All full horizons.
RANDOM-03 uses frozen source4b70a80/build81, window20/keep14/I2048/islands32,
blocker/equal repairs. Eight one-change trials test field contrast1.6 or3.2,
repair groups4 or16, unit rotation/wait costs, scheduler length weight0 or0.5,
and unguided matching. The horizon remains off; the planner guidance is an
explicit trick. These probe routing and scheduler alignment, not more seeds alone.

RANDOM-04 uses frozen sourcea2ff2b2/build80, depth16/K14400/plannerseed4. Three
exact optimization controls change cache capacity128 or512, or enable the cycle
mask. They should preserve complete trajectories; verify that before crediting
any runtime gain. A fourth case lowers fixed work toK11520 and enables sampled
policy profiling, to locate runtime costs with extra deadline headroom. It is a
separate throughput configuration. No failed original run is discarded.

In parallel, inspect whether a larger fixed LNS repair budget benefits RANDOM-03
without consuming the one-second margin, and profile the crowded policy before
changing its inner loop. The current implementation caps window iterations at
2,048 even though the selected RANDOM-03 run averages275 ms; that cap can be
raised explicitly, while default behavior and fixed-work semantics stay intact.

## Source86 work-budget trials

Sourcef60f254/build86 passes the full regression suite in22.59 seconds. The
next five full RANDOM-03 cases keep the selected field/scheduler unchanged and
test I2048 as an exact control, I3072, I4096, plus repair-group4 at I4096/I8192.
The preliminary group4/I2048 result is2427 (versus2439), but mean155/max231ms
versus275/372ms: a substantial speed/quality tradeoff to measure at larger work.
These are explicitly budgeted iterations, not a deadline-based search.

The first batch's group16 exceeds1s atstep111 and remains invalid. The unguided
matching trial was refused before solver launch on research40 (64 physical
cores visible where16 were requested), so it has no algorithmic result. That
original refusal is preserved; exclude research40 from subsequent submissions
until its binding is understood. No cause is inferred from the resource record.

## First results and next declared full batches

Source86 exactly reproduces the2439 control in all six trajectory/schedule/
event/task fields. I3072/I4096 with group8 give2452/2469; group4 atI4096/I8192
gives2466/2479. The new2479 record has mean520/max733ms and is5.09% above
NMS, still116 below the10% target. All five attempts independently replay.

Source88 (49f9724) passes its regression suite, including cooperative-round
mobility, worker scheduling and search-storage equivalence. Six full cases
compare exact control/storage reuse, sharing rounds2/4/8, and the public KK400
field under the stronger window budget. Earlier KK400 atI512/keep6 gave1797
versus1839 for the flow field; the optimizer interaction is the reason to retest.

RANDOM-04 cache128/cache512/cycle-mask all finish2565, with means826/849/853ms
and maxima931/937/920ms. Lowering work toK11520 gives2500, mean722/max869ms.
Neither larger caches nor the mask provides a compelling runtime gain. The
source87 cache-fusion control and fused variant both time out atK14400
(step1/84,1191/1083ms), so neither qualifies for robust timing. Their original
failures remain preserved; the lower-work fused case is still running.

The next eight RANDOM-04 cases use source80 andK8064/first4032 for runtime
headroom. Compare18/6/8/10 forecasts per candidate, an8-forecast depth12 case,
public NMS/KK guidance, and noise100. Complete branch groups are checked before
submission. All are explicit RANDOM-04 presets and use the full1000-step run.

## Cooperative and runtime results; next declared controls

Source88 sharing twice reaches2514 on RANDOM-03 (mean544/max714ms), versus
2479 for independent islands. Four/eight rounds give2483/2469; KK400 gives2429.
Exact reusable-storage control stays2479, mean509/max695ms. Source89 passes
regressions and introduces only the compact cache representation. The next six
R03 cases repeat2514 and test known-horizon triage scales0.45/0.75/1/1.25, plus
guided mix1 atscale1. This horizon assumption is explicitly a trick; the general
cooperative optimizer remains independent of the map/horizon.

At lower fixed workK8064, RANDOM-04 B18/B6/B8/B10 give2473/2355/2461/2501.
The B10 case mean506/max575ms provides useful headroom. Depth12 atB8 gives2405;
NMS/KK fields give2187/2163, noise100 gives2460. None improves the2565 record.
Next source89 cases repeat theB10 control, increase its work toK11520, then
separately test triage0.75/1, guided mix1, turncost1, dispersion0.4. TwoB18/K11520
controls test the compact layout and optional fused read against2500. All full
1000 steps, with fixed work and strict deadlines. The earlier fusedK11520 run
finishes2500 atmean686/max742ms; exact replay equivalence is checked separately.

## Routing diagnostic and declared lower-work routing ablation

The2565R04 trace uses2.05 completed loaded forward moves per hop lower bound,
versus1.67 in NMS's2580 trace. This is observational across different completed
task sets. Its200-step completion blocks are428/559/489/461/628 against
NMS638/520/451/474/497: most of the net gap comes from initialization, while
the horizon trick helps late. Full motion phase evidence is in
results/random34-motion-diagnosis.

Eight full source89 trials retain the lowerworkB10/K8064 budget for headroom.
They test directional contrast0.8/1.6/3.2, contrast1.6 with reference scale2.4,
plain-distance score blend0.25/0.5, turn-aware field construction0.5 without
the previous field's flip, and weaker betweenness0.25. These separate route
construction from rollout scoring; all use explicit --trick RANDOM-04.
No new held-out input is used. Compare to declared exact2501control.

Initial matching is very similar in the2565/NMS2580 traces: both assign271
two-stop tasks; internal hop sums26774/27306, approach697/624. NMS's first
200steps nevertheless completes638 versus428 with2421 versus2166waypoints.
This motivates a bounded R04-specific recheck of the existing progress-rank
weighting during startup, not a claim of causal attribution or shorter tasks.
Four source89 full runs atB10/K8064 test rankpower0.5/1/2 for200steps and
power1for400steps. The earlier all-run power0.25 test lost onR04; these are
distinct predeclared startup ablations. Matching and task locks stay unchanged.

Source90/ac5cbc9 passes regression23.26s. Six full R04 runtime controls test
its one-cache-line representation: B10/K8064, B10/K11520, B18/K11520 with
fused reads off/on, and B10/K11520 with OpenMP workers bound to the32 allocated
logical CPUs, then also passive waiting. These are exact-behavior controls,
not additional throughput policies; compare all six trace fields. Affinity stays
within the verified16-core/32-thread GRID allocation. Timing differences on
shared hosts require repeated qualification before a robust speedup claim.

RANDOM-03 triage scales0.45/0.75/1/1.25 give2529/2538/2551/2514; scale1
withguidedmix1 gives2545. The new2551 record is8.14% above NMS (44short of
target), mean520/max698ms. All six cases fully replay. The horizon rule is
an explicit trick. Source91/de0b763 passes regression24.46s for fixed-work
annealed repairs with a retained best complete incumbent. Eight full R03 cases
compare exact2551control, temperatures0.25/1/4, group3, group6 at6144repairs,
and plannerseeds0/3. Temperature defaults0; no timeout truncation is introduced.

The weaker-routing batch finishes2241–2483; none beats its2501 control.
Do not infer that reducing detours necessarily increases throughput: opposing
traffic and the cost field interact. Full-source90 follow-ups keepB10/K8064
for headroom and separately test eight forecasting/search settings (future
mutation0.1/0.5, risk-1/+0.5, elitefutureblend0.5, rootradius3, rootmutation0.1,
dispersion0). These mechanisms were unhelpful in earlier RANDOM-05 stages, so
this is a bounded density-specific recheck, not an untested general claim.

A separate eight-case explicit guidance trick comparison removes the inherited
R05 field flip, replaces its seed5 with0/1/2/3/4/6, or flips4edges atseed5.
This tests small direction changes around the generated layout at700robots.
No unseen task stream or hidden task information is used for selection.

Seven full R03 source90 cost ablations hold the2551 cooperative/horizon
recipe fixed: contrast2/2.2/2.6, turn1.5/2.5, wait1.5/3. Earlier broad contrast
1.6/3.2 and simultaneous unit turn/wait tests lost at lower work; these finer
one-change tests probe the stronger optimizer without silently combining
settings. All are explicit tricks, full800steps, with strict timing preserved.
