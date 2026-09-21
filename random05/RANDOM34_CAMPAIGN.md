# PILOT RANDOM-03 and RANDOM-04: ten-percent campaign

Started 2026-09-21 05:17 UTC at the user's request. Throughput must be at least
10% above matched local NMS on both instances, with runtime reliably below one
second. This follows the completed RANDOM-05 4,000-task milestone; that record,
its four-core counterpart, and all held-out inputs remain preserved.

| Instance | Current selected best | Matched NMS | Minimum +10% | Full steps |
|---|---:|---:|---:|---:|
| RANDOM-03 | 2,552 | 2,359 | 2,595 | 800 |
| RANDOM-04 | 2,641 | 2,580 | 2,838 | 1,000 |

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

## Smaller root mutations improve RANDOM-04

At B10/K8064, rootmutation0.1 reaches2634 (mean495/max532ms), versus2501
for0.3. This is a133-task paired development gain at unchanged declared work;
independent replay precedes promotion. Other forecasting controls give
2380–2497. These are per-instance settings of a general search mechanism.

Ten source90 follow-ups test mutation0.02/0.05/0.15/0.2, generations8/16
atK8192/first4096 (work counts divisible in every generation), K11520 with
B10/B18, and plannerseeds0/3. Source92/b4b4abe passes its regression suite.
Four full source92 controls compare exact2634 with shared immutable rankings
at1024/2048MiB, and2048MiB without the private cache. Search budgets unchanged;
compare full traces before crediting runtime savings. All strict1s/32GB.

Source91annealing reaches2552 attemperature4, onlyone task over2551, so this
is a selected record rather than evidence of a substantial generic gain.
Temperatures0.25/1 give2499/2519; groups3/6 give2481/2550. The2551profile
on plannerseeds0/3 gives2522/2516; all8strict/replayed. Finer R03cost changes
alllose2478–2533. Next eight source91 cases keep temperature0 and probe the
cooperative planner's window: H16/keep10or12, H24/keep16or18, H28/keep20,
H32/keep22, H20/keep12or16. Use8192repairs except6144forH28/H32 to retain
time margin. These are full800step evaluations; no partial-prefix score.

Source92 exactly reproduces2634 in all six trace fields for all four controls.
Shared1024/2048MiB givesmean449/447ms versus489ms, maxima480/485 versus540ms,
withRSS1.57/1.56GB. Withoutprivatecache mean441/max593ms,RSS1.48GB.
These are observed single full runs on shared allocations; no universal timing
guarantee. The1024/2048cases give useful headroom within32GB.

Source94/0ca9413c passes regression. Nine full R03 controls test exact2551
with fastgroups off/on, then fixed first4096, work12288/16384, H24 keep16/18,
H28keep20 andH32keep22. The two largest windows use6144regular/2048first;
all others use8192regular/4096first unless stated. Higherwork is declared,
not time-adaptive; any overrun remains an unsuccessful attempt.

Themutation0.1follow-up yields2448/2535/2529/2482 at0.02/0.05/0.15/0.2;
G8/G16 give2622/2632, B18/K11520=2603, seed0/3=2584/2530. B10/K11520
fails atstep264=1071ms. The selected2634 is not a replicated10% gain.

Eleven source92 full cases use the exact2048MiB-cache2634 profile and change
one setting: E/P1,4,16; noise100/400; discounted progress0.95/0.98; all-run
remaining-task rankpower1/2; depth12/20. The rank preferences are explicit
tricks and may increase waiting for long tasks. NMS's local source squares
remaining-task rank on both RANDOM04/05, but its different search means this
is only a hypothesis, not an expected transferable gain. All other controls
are general mechanisms evaluated in the flagged R04profile.

## Broader explicit guidance screen at bounded work

The inherited field15 was selected on RANDOM-05. Small flips and broad cost
softening have not closed R03/04 gaps. Declare nine full-horizon cases per
density: the inherited control, plus fieldseeds4/7/11/19/23/29/37/43 with no
inherited flip. R03 uses source94, I4096/two rounds/fastgroups/horizon800; R04
uses source92, B10/K4096/first2048/mutation0.1/sharedcache2048MiB. Lower work
keeps the layout screen inexpensive; every score still covers the full800 or
1000 steps. Validate promising fields at the larger selected work budget before
claiming an improvement over that recipe. All are explicit --trick instances;
held-out task/start inputs remain unused.

Source96/fa98fd1d passes regression25.06s. Nine full R03 cases test exact
2551/2552 controls after A* state/row reuse, then an8-call startup work schedule
andlarger windows/work. H20uses8192/12288/16384regular with4096startup; H24
keep16/18 uses8192/4096; H28keep20uses6144/2048. H32keep22 uses4096/1024
and16startupcalls. Everydeclared count completes; all original source94
timeouts stay retained. This does not treat a rejected partial run as a score.

## September 21, 06:57 UTC: layout screen and next full comparisons

At I4096, RANDOM-03 field23 scores2517 against2483 for the inherited field.
The other seven new layouts score2318–2481. Next six full source96 cases test
field23 at I8192, temperature4, triage0.75, two single-edge flips, and H22/I6144.
These remain explicit guidance tricks selected only on the archived input.

All eight new RANDOM-04 fields lose at K4096:2114–2439 versus2492. The next
nine full source96 cases retain the best2634 recipe and the exact2048MiB cache.
They test a control, movement-proposal biases1/3, bias3 on half of candidates,
prospective waits, intent mode1, half waypoint-age retention, zero loop
threshold and larger loops. These are bounded transfers from earlier R05
mechanisms, not assumed improvements. Every case uses the full1000steps,
fixed work, verified allocation and strict1s. No held-out input is generated.

Source94 control and fast groups reproduce all six fields of2551 exactly.
Runtime observations are consolidated in [the timing report](results/random34-runtime/REPORT.md).

## September 21, 07:04 UTC: scheduler and forecast alignment

Nine declared full source96 RANDOM-04 trials keep the2634 planner budget and
shared ranking cache. Compare matching on committed next positions, oriented
whole-chain assignment, keep bonuses0/2, length weights0/0.5, aging inside
forecasts, and terminal completion rewards2/8. The ordinary policy ignores
new assignments after forecast completions, which may undervalue finishing;
these are tests of that hypothesis. No unpublished future tasks are consulted.
The new pending-position and chain-cost settings already exist as general
options; the density-specific selected combinations remain explicit tricks.

## September 21, 07:12 UTC: safe plan composition and traffic-policy probes

Source99/db2d9bc5 adds optional merging of complete LNS windows. Cross-parent
vertex and reverse-edge conflicts link agents into indivisible components.
Only cost-improving complete components are copied; the resulting whole plan
is validated before use. Tests enumerate every mix in late-vertex and edge-swap
fixtures and check dense mobility, worker determinism and checkpoint replay.
Six full R03 cases compare control, merging at group4/group6, annealing, mixed
temporal neighborhoods, and half the work. Source98 temporal tests remain
separate; neither mechanism is claimed to improve throughput yet.

The2634 R04 trace has completed-waypoint intervals with p95=182, p99=234 and
maximum322 steps. Eight full source96 cases test priority caps64/128/256, two
early-fill thresholds, soft guidance confidence0.5/1 and construction penalty2.4.
Priority caps and guidance choices are explicitly marked tricks; completed
and censored order waits are retained by the audits. No new held-out data.

Source97's four-way heap repeats2551/2552 exactly in all six fields. Mean
runtime494->459ms and489->471ms, while maxima917->759 and683->780ms.
These single pairs support a mean-runtime improvement, not a uniform peak-time
claim. Every listed run is complete and strict-valid.

## September 21, 07:15 UTC: RANDOM-04 reaches 2,641

Source96/fa98fd1d, keep bonus0, scores2641 (+2.36% versus local NMS2580),
mean441/max498ms and1.55GB RSS. Full replay and order accounting passed.
This is seven tasks above2634 and197 below the current2838 target; it is a
single-seed best, not replicated ten-percent superiority. All other scheduler
alignments and forecast completion rewards lose2391–2589.

The next six source96 cases compare the2641 control against the existing
motion-component optimizer at32/64/128 trials, four rounds, or minimum group4.
Each hybrid commits whole collision-linked groups and evaluates all declared
future branches. Earlier dense R05 results do not establish transfer toR04.
Temporal blocker scanning on R03 gives2523–2535 versus2551; the controls and
unsuccessful alternatives remain audited. Full window composition is running.
