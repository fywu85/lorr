# PILOT RANDOM-03 and RANDOM-04: ten-percent campaign

Started 2026-09-21 05:17 UTC at the user's request. Throughput must be at least
10% above matched local NMS on both instances, with runtime reliably below one
second. This follows the completed RANDOM-05 4,000-task milestone; that record,
its four-core counterpart, and all held-out inputs remain preserved.

| Instance | Current selected best | Matched NMS | Minimum +10% | Full steps |
|---|---:|---:|---:|---:|
| RANDOM-03 | 2,514 | 2,359 | 2,595 | 800 |
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
