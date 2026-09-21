# PILOT RANDOM-03 and RANDOM-04: ten-percent campaign

**Resumed 2026-09-21 15:03 UTC.** The user requested further work on all five
RANDOM instances. The earlier stopping checkpoint remains in the history;
this RANDOM-03/04 qualification is a retained milestone within the broader work.

Started 2026-09-21 05:17 UTC at the user's request. Throughput must be at least
10% above matched local NMS on both instances, with runtime reliably below one
second. This follows the completed RANDOM-05 4,000-task milestone; that record,
its four-core counterpart, and all held-out inputs remain preserved.

| Instance | Current selected best | Matched NMS | Minimum +10% | Full steps |
|---|---:|---:|---:|---:|
| RANDOM-03 | 2,606 | 2,359 | 2,595 | 800 |
| RANDOM-04 | 2,782 | 2,580 | 2,838 | 1,000 |

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

## September 21, 07:23 UTC: complete window merging reaches 2,580

Source99/db2d9bc5, group6/I6144 with two sharing rounds, reaches2580 (+9.37%
against2359). Mean535/max736ms, full800 steps, independent replay and order
accounting passed. It is15 tasks short of2595. Group4 merging gives2562;
annealing and temporal mixing do not add to it. All original outcomes remain.
Next comparisons replicate seeds0/1/3/4 and groups5/8. Source101 tests explicit
task-priority weights on this merged group6 recipe, with an exact control and
the validated four-way heap. No newly generated task/start stream is used.

## September 21, 07:28 UTC: weighted-window initialization rejection

The six nonzero-rank source101 cases were rejected by the older window/search
compatibility guard before planning. They have no throughput score and remain
original failed attempts. The direct-Config algorithm regressions passed, but
did not exercise environment parsing. The follow-up removes the obsolete rank
exclusion and adds a parser test for acceptance with the matching trick flag,
rejection without it, and continued rejection of unsupported future matching.
The full suite and full-horizon cases will rerun before evaluating the idea.

## September 21, 07:29 UTC: closed-loop forecast transfer

R04 component refinements give2550–2618 versus2641; priority caps, early fill
and softer field confidence also fail to improve it. The next eight source96
full cases compare a half-work control with visible-task replacement inside
rollouts, then six closed-loop forecasts that reschedule/replan using only the
current visible task pool. Configurations specify every root, future, inner
work count and nested worker count, all within32 workers. No hidden future
tasks are introduced. Source95 already fixed shared-cache construction in
parallel forecast shadows; these runs explicitly use private caches.

## September 21, 07:39 UTC: explicit guidance expansion and allocation refusal

Source103/eb43ba4a adds whole-field direction reversal behind the trick gate.
A regression verifies that every directed edge swaps prices with its reverse,
while topology and turn costs remain identical. Fourteen full cases compare
reversal onR03/R04, six additional R04 layout seeds, and four local edge flips.
All use full work and archived inputs; no field is promoted from a shorter run.

The original visible-task rollout was refused before solver launch onresearch45:
64 physical cores were visible where16 were requested. Preserve that refusal,
exclude45 from future submissions, and rerun the identical frozen case elsewhere.
No throughput or timing conclusion can be drawn from an unlaunched solver.
Reduced-worker R03 genuinely timed out at timestep0 (1156ms);32-worker controls
pass. The complete weighted-window rerun is valid but loses2466–2571 versus2580.

## September 21, 07:45 UTC: spend measured runtime headroom on fixed work

The four-way heap reproduces2580 atmean471/max672ms. Four full source100 IPO
cases now compare that recipe, I8192, I8192 with four sharing rounds, and
I10240 with a declared eight-step4096 startup budget. These use complete fixed
repair counts; a deadline overrun still fails the run. This specifically tests
whether extra work benefits component merging, after the old independent
optimizer showed weak scaling. No claim of a gain precedes the full results.

## September 21, 07:54 UTC: RANDOM-03 crosses the archived ten-percent target

Triage scale0.875 on the merged group6 recipe reaches2602 versus NMS2359
(+10.30%), above the2595 threshold. Source102/02bccaa7, full800 steps,
mean507/max710ms,434MB RSS. Finished07:48:41.498854 UTC; independent full
replay and order accounting passed. This is an archived seed-specific crossing.
The overall goal remains active: fresh validation and R04>=2838 remain open.

Freeze the first crossing in random03-fresh-validation-v1-protocol.json before
generating task/start seeds50011/50012. Compare candidate2602, previous2580
with identical source, and two unmodified NMS repetitions on each input.
Archived exact repetition and seeds0/3 are also declared. Other development
results cannot change this frozen configuration or its validation seeds.

## September 21, 08:16 UTC: RANDOM-03 qualification and next R04 work

The frozen2602 recipe repeats exactly in all six trace fields; planner seeds0/3
score2548/2566, with maxima655/674ms, versus661ms for the exact repeat.
Fresh50011/50012 score2599/2557 versus stronger NMS2327/2343 (+11.69%/+9.13%,
+10.41% aggregate). All eight original fresh runs pass full replay, source,
resource and timing checks. Both fresh candidate maxima are below701ms. See
RANDOM03_FRESH_VALIDATION_V1.md. Keep these inputs excluded from tuning.
The overall goal remains ACTIVE because R04 still needs197 tasks to reach2838.

R04 current2641 recipe has five planner seeds0/1/3/4/5 with scores2526/2601/
2596/2641/2628; all maxima<498ms. Profiling is observational and preserves the
2641 trajectory exactly. R03 extra merged-window work gives2579/2558/2556,
so the additional budget did not improve the2580 control. Full evidence remains.

Source104/1436f231 aligns shared rankings to64bytes and optionally delays raw
cost-row resolution. Four full controls test shared/private and eager/lazy
lookup; expect exact2641 actions. Source105/aa370473 optionally prices current
destination demand from already opened visible tasks, with coefficients1/4/16/64
and a zero control. The first nine job submissions incorrectly required the
abbreviated CPU model string; all were refused before solver launch. Preserve
those audit records; identical cases run in the cpufix batches with the exact
EPYC9354 model string. No host defect or solver failure is inferred.

Source106/75723ed8 passes full regression. At identical complete rollout work,
eight R04 cases compare control, restart periods0/2/8/16 (default4), and minimum
elite decision differences1%/3%/10%. Diversity compares planned next moves,
retains the incumbent and excludes incomplete candidates. A semantic fixture
checks selection; dense simulations check collisions, task locks, checkpoint
replay and worker independence. These general options default to old behavior;
per-instance experiments remain explicitly flagged. No fresh R04 input exists.

## September 21, 08:38 UTC: RANDOM-04 reaches 2,661; broad checks stay negative

Guided cutoff mix0.75 instead of0.5 reaches2661 on the previous2641 profile,
sourcefa98fd1d/build96, plannerseed4. Completed08:24:36.112951UTC; mean430.374ms,
max465.929ms. Full independent replay passed. This is+3.14% over2580, still
177tasks short of2838; the new profile needs repetition and seed qualification.
The other six cutoff refinements score2493–2640. All original runs are valid.

Source104's four cache controls preserve2641 in every trace field; timings
vary and do not establish a compelling alignment/lazy-row gain. Destination
prices1/4/16/64 score2540/2515/2543/2566 versus2641. Source106 restart/diversity
variants2447–2605 and source107 feasible-edge/wait proposals2531–2615 also
lose. Both default controls preserve2641 exactly in all six trace fields.

More complete work with the shared cache and IPO gives2615/2592/2615 at
K11520/13824/16128 versus2641 atK8064. These full runs meet1s, but additional
compute does not help throughput. The existing three-step operation backend
scores1733–1936 on five valid R04 transfers; K256 times out atstep74 after
18562.600ms. Preserve that original failure without attributing it to a host.
This backend is not a competitive replacement here under the tested settings.

The declared layout-search protocol tests32 new fields101–132 atK1024 over
all1000steps, then verifies the topfour atK8064 on plannerseeds4and0 alongside
inherited controls. All screens lose the inherited2361 control. Per the frozen
selection rule, fields109/111/105/115 (2324/2307/2286/2285) proceed anyway;
low-work ordering does not establish full-work superiority. This is explicitly
map-specific layout selection; held-out inputs are untouched.

Source108/d879feda passes regression for optional horizon-aware matching. It
adds a soft cost for an unopened visible chain's estimated work beyond the
declared end, preserves opened-task locks, and requires the explicit trick flag.
Five full trials compare weights0/.25/1/4/16 on the2661 recipe. This tests
whether matching can avoid assignments that planning immediately triages.

## September 21, 09:00 UTC: qualify runtime; test execution and potential changes

The2661 configuration has four complete runs with maxima below490ms. Its
source108, source109 and repeated source96 controls preserve the same score;
full trace-equivalence proofs are recorded separately. Additional plannerseeds
3/5 are running; the selectedseed4 score is not a multiple-seed average.

Horizon matching loses2573–2602. Bounded startup remaining-work priorities
(source109/7714e8ca) lose2522–2630; all seven runs are valid. Loop thresholds,
dispersion and priority-noise refinements lose2496–2601. The declared full-work
layoutfinalists109/111/105/115 give2526/2490,2481/2490,2494/2567,2433/2489 on
plannerseeds4/0 versus inherited2661/2542. The inherited field remains selected.
These failures argue against relying on more compute or further small sweeps.

Source111/17f978a4 instead tests optional immediate legal forward chains at the
first forecast step. Some roots retain the ordinary pipeline; others add only
already-facing moves into available cells or legal cycles. Later forecast steps
return to the ordinary pipeline. Existing promises remain preserved and every
selected actual/pending move is independently certified. Fixed evaluation counts
remain unchanged. Periods1/2/4/8 and two gain thresholds are running alongside
an exact control. This differs from always enabling the earlier early-fill rule.
Regression covers checkpoint replay, worker/cache invariance, common-prefix and
screened continuations, and independent rescoring. A missing rescoring mode was
caught in local review of source110 before benchmarks and fixed in111.

Source112 tests a second mechanism: blend oriented lane cost-to-go with physical
action cost-to-go before constructing chained task potentials, while retaining
lane edge prices. This can change both local move ranking and matching, unlike
an earlier score-only blend. The resulting potential is approximate; windowed
A* is explicitly excluded. Tests verify the any-heading cache, immutable lane
prices, task turnover, checkpoint replay, cache/worker invariance and trick gate.
Build/regression must pass before any full performance claim.

Fable's new6022-byte results-only summary was prepared but NOT SENT. Automatic
approval review rejected transmission as non-public research requiring exact
payload/destination approval, despite general consultation authorization. The
user question is pending; local research continues. This is separate from the
previous CLI request that failed usage credits.

## September 21, 09:21 UTC: completed repeats and physical-cost controls

The original 2,661-task profile now has four planner seeds (0/3/4/5):
2,542/2,576/2,661/2,605. Original-source qualification and repetition peak below
490 ms. Equivalent controls from sources108/109/111 preserve all six trajectory,
schedule, event and task fields. The later source113 control also repeats
exactly, but peaks at755.061 ms (mean540.262); that slower observation is
retained in the timing table. No host cause is inferred. All remain under1s.

The immediate-root experiment111 loses: periods1/2/4/8 give2538/2569/2578/2543;
half-root gain2 gives2611. The physical-distance mixture113 also loses:
fractions.0625/.125/.25/.5/1 give2561/2510/2370/2112/1576 versus2661. Every run
finishes all1000steps and passes independent replay. Source112's const-pointer
compile failure is archived;113 fixes construction order before benchmarking.

Source114/93746116 tests a coherent alternative: blend actual oriented edge
prices toward physicalcost2, then solve exact distances. Regression checks the
one-step Bellman relation, rotations, walls, checkpoint replay and worker/cache
invariance. Six full trials are running with the same fractions and budget.
Neither cost experiment changes the selected record or held-out inputs.

## September 21, 09:31 UTC: route dependencies and explicit admission

The coherent edge-cost test114 also loses: fractions.0625/.125/.25/.5/1 yield
2548/2482/2450/2263/1878. All six full1000step runs are independently replayed;
the2661 control is identical in all six trace fields, mean445.546/max483.893ms.
Keep both physical-potential options off.

Source115/92a96cfa passes dense/checkpoint/cache/worker regression and tests
priority mutations among robots connected by preferred-route occupancy. Eight
full comparisons retain the original K8064/first4032 fixed work. Source116
separately implements a declared task-admission trick, preserving opened work
and allowing every robot to yield. Dummy Hungarian columns select the requested
number of real pairs; optional startup expiry returns to full admission. Its
regression build is pending. Neither feature has a throughput claim.

## September 21, 09:52 UTC: admission reaches 2,698; correct horizon checks

The explicit560-task admission cap reaches **2,698**, sourceaa171530/build116,
finished09:40:28.766893UTC. Mean448.029/max654.058ms; full1000steps and independent
replay pass. That is+4.57% against matchedNMS2580 and140shortof2838. All700robots
remain movable, openedordersstayprotected, and unopenedwork canwaitlonger.
Longestcompletedorder962steps;181initialordersremainunfinished (132unopened),
witholdestunfinishedcensored1000. Seeds0/3/5andnearbyadmission/triagechecksarequeued.

Caps660/620give2641/2686; startupcapslose2563–2594. Cap480scores2631 over1000steps,
but the originalrunner incorrectly rejected it: reportedmakespan976counts
assigned-worksteps, not executedsteps. All700pathsand1000computesamples confirm
thefullhorizon, and independentreplaypasses. Originalfailedsummaryiskept;
cap480-horizon-correction.json records the correctedinterpretation. Newrunner
and audithelperscheckactualhorizonsandentrycounts, preservingmakespanasa metric.
Sixregressionsrejecttruncated/unequaltraces andmissingentrywork; all10existing
general/trickfrontiers keeptheirverifiedhorizons. Noresultsorinputsareedited.

All route-dependencymutations115lose2423–2597. CouplingNMS-inspired task-length
prioritywithscoreweightsalso loses2457–2614; allfulltracesare independently
replayed andallcontrolsrepeat2661exactly. Sources117/118 nowtest exact32byte
scoredrankings andoptional8byte order-onlyrankings. Fullregressionpassed; six
full2661/2698controlsare running before any speedclaim or runtimepromotion.

## September 21, 10:33 UTC: admission reaches 2,741; timing improvements are exact

The cap560/triage1 profile reaches2738 and improves every tested planner seed:
0/3/4/5 give2718/2689/2738/2735, versus2542/2576/2661/2605 without admission.
Its original-source runs, including exact repetition, peak below671ms.
Lowering the cutoff scale to0.875 gives the new **2741** record at10:14:35.787464UTC,
sourceaa171530/build116, mean450.058/max649.058ms. Full action/schedule/event replay
passes; the archived10%target remains97tasks away. Longest completed order997steps;
178initial orders remain unfinished,134unopened. Throughput gains carry this fairness cost.

Source118's 8byte order cache and119's dummy-prefix matching optimization preserve
all six full trace fields in nine comparisons. The2698 profile's RAM falls from
1.73GB to about0.57GB; fast matching gives mean427/max619ms on its full control.
This is measured evidence, not a universal speedup. The2738 controls on119/120
also preserve all six fields, with mean432–455/max626–684ms. Runtime rows retain
original builds, repetitions, seed checks and the older755ms observation.

Active-only travel calibration120 gives2733/2737/2714/2659 versus2738; keep it off.
Matching length weights0/.5/1/2 give2297/2691/2712/2693. Correct predictive matching
gives2715. The first predictive manifest used an unrecognized environment key;
its2738 run is preserved and classified as an extra control, with a separate
corrected experiment. New pre-submission validation rejects unknown R05 keys
against each frozen binary's source (three regression checks pass).

The next eight full trials keep the2741 admission profile and test its interaction
with dispersion, turn/wait costs and exact chained matching. They include a full
runtime-control repetition. Cost-sensitive admission is the next separate mechanism:
leave expensive unopened pairings unassigned instead of always filling the cap.
No fresh RANDOM-04 inputs have been generated or used for tuning.

## September 21, 10:42 UTC: reduced dispersion reaches 2,762

On the capped profile, dispersion0.4 gives **2762**, source487a35da/build119,
mean/max in the runtime ledger (maximum629.872ms). Every action and task event
replays. Dispersion0/1.6 give2721/2603; turn0.4/1, wait1 and chained matching
all lose (2717/2688/2693/2726). The runtime control preserves2741 in all six
fields. Three planner seeds and nearby cap/dispersion choices are running.
The current archived target remains76tasks away; no fresh R04 input exists.

The [admission diagnosis](results/random04-admission-diagnosis/REPORT.md) locates
an early gain despite fewer forward moves, but NMS still completes more orders
in the first200steps. All initial matchings select271two-stop tasks. New full
trials therefore test guidance costs under reduced demand and short-task reward
variants, rather than attributing the gap solely to task selection.

Source236a04c6/build121 passes33.44s regression for optional priced idle slots.
Cheaper mandatory dummy columns preserve the existing hard cap; extra idle slots
at the declared price make expensive unopened assignments optional. Semantic
fixtures cover expensive/cheap pairs, opened locks, scarcity, zero tasks, cap
limits, exact/greedy matching, checkpoint/cache/worker invariance and trick gates.
Seven full tests include price-off control, prices12/16/20/24/32, and uncapped24.

Sourceb752b6e8/build122 tests a smooth completion preference in the rollout score:
replace remaining cost d by tau*d/(tau+d), normalized to mean initial slope1.
Priorities, full rollout work and legal execution are unchanged. This is an
explicit short-task preference trick, default off; it still needs regression
and full-run evidence. No improvement is claimed from the new objective.

## September 21, 11:06 UTC: 2,776 tasks and a startup admission hypothesis

The selected contrast2.2, dispersion0.4, cap560, cutoff0.875 profile reaches
**2776**, source487a35da/build119, mean434.778/max626.736ms. Seeds0/3/4/5 give
2730/2711/2776/2776, all maxima below630ms. This selected best is62tasks short
of2838. Contrast2.4 gives2725/2751/2762/2772, so the2.2 change improves the
best observed score but not the four-seed aggregate. Do not call it a general win.
The contrast2.4 exact seed4 repetition peaks729.537ms; retain that slower observation.

All ordinary reward trials lose: completion bonuses2/8 give2721/2716, rank
powers0.25/0.5 give2729/2714, startup rank1 gives2725, startup remaining-work
priority2 gives2738. Nonlinear utility122 (scales25/100/400/1600) gives
2617/2692/2718/2758; default control2762 is exact in all six fields. Keep all
these optional reward changes off. Cutoff scales0.625/0.75/1/1.125 and mixes0.5/1
also lose2750–2764. Immediate legal forward candidates on periods1/2/4/8 lose
2732/2720/2724/2726. More fixed work atK11520/K13824 or B12K11520 gives
2771/2762/2771; increasing work again does not improve throughput.

One intended B8/K8192 trial was rejected by native configuration validation
before simulation: K and first-stepK must fill complete screening groups of
four generations times14 evaluations. Preserve the original exit125 and its
configuration note. A separate corrected B8/K8064 run gives2710; submission
validation now catches this specific class of invalid budgets before allocation.
Four manifest checks pass. This was a configuration error, not a deadline failure.

Fixed admission price12/16/20/24/32 gives874/1587/2627/2740/2741 on the previous
2741 profile. Priceoff and nonbinding32 preserve the full control trace exactly.
The low prices eventually stop admitting useful work; leave them off for now.
However price12 completes289/669 tasks by steps100/200 versus222/570 for its
control and309/638 for NMS. Price16 completes1206 bystep400 versus1121.
These early observations motivate a bounded startup policy, not a claim that
phase-specific gains can simply be added.

Sourced51bdbf2/build123 passes34.81s regression. R05_ADMISSION_PRICE_STEPS
expires the optional price at an explicit simulation-step boundary while the
560-task cap remains. Opened orders stay protected. Tests cover the boundary
for exact and greedy matching, cap preservation, dense mobility, checkpoint and
worker equivalence. Eight full trials test price12 through100/150/200/250 on
the original profile, price12 through150/200 on the2776 profile, price16 through
300 on2776, and a priceoff2776 control. All are strict1s/full1000steps. No new
RANDOM-04 validation inputs have been generated.

## September 21, 11:20 UTC: startup gains disappear; test late admission accounting

Every temporary-price run loses: on the2741 profile, price12 until100/150/200/250
scores2690/2661/2630/2547; on2776, price12 until150/200 scores2710/2666, and
price16 until300 scores2703. The2776 control is exact in all six trace fields.
All four original-profile variants preserve the constant-price12 prefix through
the declared expiry, and first differ in actions on expiry+1. The early boost
was real but does not establish a full-run gain; retain the negative evidence.

Sourcecaedcce7/build124 passes33.72s regression for fractional admission credit
on previously deferred opened orders. At step900, the2776 control has528opened
orders and only32new admission slots, while171assigned goals are deferred by the
planner. Counting deferred opened work against the cap may prevent idle robots
from serving additional finishable orders. The option discounts a declared
fraction of previous-step suppressed task IDs from admission accounting. It never
releases opened schedules. It is an estimate with a one-step delay, not a strict
bound on the current number of active planner goals. Task IDs, not robot flags,
prevent stale credits; the metadata survives checkpoints and shadow forecasts.

Eight full cases compare credit0/0.25/0.5/1, combinations with horizon-aware
matching and optional idle price32, and their controls. The price/horizon pair
can leave an infeasible unopened assignment empty instead of forcibly filling it.
All are explicit tricks with unchanged strict1s/32GB/full1000-step rules.
Two additional full controls run the existing R03=2602 and R05=4011 recipes on
build124 to check that common-engine changes preserve those frozen records.

## September 21, 11:32 UTC: credit loses; refine admission pairing

All eight source124 runs finish and independently replay. Credit fractions
0/0.25/0.5/1 give2776/2766/2757/2743. Combining full credit with horizon
weights4/16 and idle price32 gives2767/2762; without the price it gives2693.
Keep credit off. Horizonweight16+price32 alone reaches2777, mean469.540/max
779.602ms: a one-task selected record, not a substantial improvement. It is
61tasks short of2838. The unchanged default control will be compared across
all six raw fields. The original2776binary repetition is also queued.

Next source124 tests retain the2776 profile and refine matching around its
R05-derived length coefficient0.25:0.125/0.1875/0.3125/0.375. The new admission
cap changes the cost of choosing more work, so the inherited coefficient may
no longer be appropriate. Two separate trials penalize already-opened
destination demand at0.5/2; no future task stream is available. A seventh case
repeats2777 exactly for timing and reproducibility. These are full1000-step
archived-input tests with explicit RANDOM-04 tricks, fixed work and strict1s.

## September 21, 11:45 UTC: exact controls and local waypoint guidance

The2777 repetition is exact in all six raw fields, mean442.631/max770.041ms.
The original2776binary also repeats all six fields, mean428.880/max622.264ms.
Source124 reproduces R03=2602 and R05=4011 exactly in all six fields, with
maxima671.719/572.148ms and independent full replay. These preserve the
existing recipes; they do not replace their original frontier source commits.

Matching refinements all lose: length weights0.125/0.1875/0.3125/0.375 give
2759/2732/2733/2705; destination demand0.5/2 gives2727/2760. Keep the prior
pairing objective. No fresh R04 input has been generated.

The [goal approach diagnostic](results/random04-admission-diagnosis/goal-approach.json)
compares the same completed task IDs. At geometric distance1/2 from the next
waypoint, PILOT takes1452/1707 forward moves away, versus679/697 for NMS.
Their distance1 toward moves both equal5257, as expected for the common completed
waypoints. At these distances PILOT also waits5462/5441 times versus3925/2998.
Assignments and traffic histories differ; this is a hypothesis lead, not a causal
estimate of recoverable tasks. Distant travel still needs traffic coordination.

Source4dea3bc0/build125 tests goal-local lane prices. Within a declared hop radius,
forward edge prices blend toward the uniform physical price2, linearly tapering
to the existing lane field at the boundary. The current waypoint selects the
field. Oriented backward shortest paths, chained task costs, move ranking and
shared caches all use the same positive edge prices. Outside the radius the
field is unchanged. This is an explicit trick, default off; unsupported window
and operation modes are rejected. No extra future task data is used.

Regression passes34.04s, including all-target Bellman checks, exact cache/worker
equivalence, checkpoint restore, dense legal movement and public trick gates.
Seven full1000-step cases compare the2776default control and radii2/4 with
mixes0.25/0.5/1. Binary SHA41cd19ae90ba487163b96f5cfa215086a5ccd852dc0bb80772cb796e330f5726.
All fixed work must finish under1s; no performance improvement is claimed yet.

## September 21, 12:05 UTC: timing qualification and a rejected local-field change

The2777 profile completes plannerseeds0/3/4/5 at2726/2742/2777/2771. Original,
repeat and all seed checks peak below789ms. Every full action/schedule/task
trace independently replays; runtime has measured headroom on these inputs.
Throughput still falls61tasks short of2838, so the goal remains active.

Goal-local radius2 atmixes0.25/0.5/1 gives2753/2725/2692; radius4 gives
2719/2713/2665. All full1000-step runs are valid; maxima592–742ms. The disabled
control preserves2776 exactly in all six fields. Keep local guidance off.
The [follow-up motion audit](results/random04-admission-diagnosis/goal-local-outcome.json)
does not show fewer near-goal away moves on the common completed orders: at
distance1, control/r2mix0.5/r4mix1 give1391/1417/1396 versus648forNMS; at
distance2 they give1622/1592/1765 versus668. The common task set differs from
the preceding two-solver diagnostic. These observations do not identify which
move was forced by another robot.

Source1c9c8b95 adds a read-only trace diagnostic, with the planner unchanged.
It reconstructs the directed, oriented cost of each loaded forward action and
compares it with uncongested and kinematically eligible alternatives, including
waiting. A nonminimum move can arise from collision handling, loop completion
or a suppressed goal; it is not automatically an error. First700-step counts
limit late-horizon confounding. Exact static costs and legal action/event replay
are checked on the frozen trace. This will separate route-cost hypotheses from
collision-policy hypotheses before the next throughput change.

## September 21, 12:14 UTC: most nearby detours are not static-cost minima

The [read-only cost diagnostic](results/random04-goal-score-diagnosis-v126/diagnostic.json)
replays the2776trace. In its first700steps, loaded robots make2781moves away
from a waypoint1–2cells away. Only102are minimum static-score moves;203are
minimum only after imposing the previous pipeline heading restriction. The
remaining2476(89.03%) have a cheaper kinematically eligible alternative before
accounting for other robots. Collision handling, loop completion or suppressed
goals can explain such choices; the classification does not prove wasted work.
Independent Python loaded-forward and away counts agree exactly with the C++
diagnostic. [Accounting and input hashes](results/random04-goal-score-diagnosis-v126/accounting-check.json).

Source9ee387da/build127 caches static candidate scores while charging displacement
loss dynamically. Without the two new optional pricing rules it must preserve
the original push-cost policy exactly, including tied directional order. Shared
caches retain scores when push pricing is enabled; the compact order-only cache
continues to serve the ordinary policy. One optional rule exempts robots without
a current goal; the other excludes an impossible immediate swap when estimating
the displaced robot's cheapest exit. Both default off. Legality remains enforced
by the original joint planner and simulator.

Regression passes34.69s. Legacy push prices0.25/1/2 preserve dense trajectories
with caches and workers; corrected pricing also passes checkpoint/worker/cache
checks. A direct fixture checks that an opened order can push an idle robot when
idle-goal loss is exempted, without releasing the opened assignment.

Eight full1000-step cases compare unchanged2776control, cached legacy prices
0.1/0.25/0.5, idle-free prices0.25/0.5, and idle-free/swap-excluding prices
0.1/0.25. Separate full K1024/first512 slow/fast controls test exact production
trace equivalence; they are not compared to the higher-work throughput record.
All use strict1s, fixed completed work and the verified32-worker allocation.
No new performance claim yet. Build SHA52d77c5532d9bbed5729204a85873aa1ce7ea86df11fb4ba5749aa3bc7e6ca6f.

## September 21, 12:21 UTC: cached push pricing is exact; reduce its hot-loop work

At fixed K1024/first512 and pushprice0.25, the uncached and cached implementations
both finish2559tasks and preserve all six action/schedule/event/task fields.
Independent full replay passes. Observed mean time drops166.743to139.332ms,
while maxima are531.368/550.214ms; this pair alone is not a universal speedup
or a new throughput record. The larger K8064 experiments remain running.
Their sampled steps take roughly0.8seconds, so extra runtime headroom matters.

Source5ccd3828/build128 removes a redundant directional comparison sort by
indexing the five unique move directions. It also scans only eligible exits
when masks are available and avoids computing unused second-best exits.
Costs, tie order and work budgets stay the same. Regression passes34.62s.
A full2559control and a K8064push0.25case compare exact behavior and timing.
Build SHAc2f0e296b4226fa3533add85894c022df8fd4768357489ebb61ab2e01a84d4ac.

Four separate full source119 trials revisit small routing proposals after
admission changed active demand: biases0.5/1/2 on one eighth of robots, and
bias1ononequarter. These alter which alternatives the rollout policy can
propose; its scoring stays unbiased. Earlier larger routing proposals lost
on the uncapped profile, so these are a bounded interaction check, not an
assumed transfer. All keep the2776profile, strict1s and full1000steps.

## September 21, 12:35 UTC: displacement pricing loses; protect near arrivals directly

The eight source127 runs are independently audited. Exact default control2776
matches all six archived trajectory/schedule/event/task fields. Cached legacy
push prices0.1/0.25/0.5 score2749/2762/2705; idle-free0.25/0.5 score2744/2743;
idle-free plus swap-exclusion0.1/0.25 score2686/2618. All finish1000steps, but
priced runs reach887–954ms peaks and lose throughput. Leave pricing off.
The four source119 small routing proposals also lose:2771/2743/2731/2757.

Source128's low-work control reproduces2559 in all six fields, mean127.287ms,
max541.035ms. Its K8064 full-work attempt fails the real deadline at step1:
1027.906ms, exit124. This is not an allocation refusal; no score is promoted
and no host cause is inferred. Preserve the original failed artifacts.
Four K6144/first3072 pricing runs test a lower fixed workload; still pending.

Source2fe1f366/build129 adds optional R05_ARRIVAL_PRIORITY (default0, requires
explicit trick). A robot gets a bounded priority bonus when its next waypoint
is its current projected cell or an adjacent cell reachable under the pipeline
heading constraint. Aging, dead-end precedence, costs, search work and legal
joint-move checks stay intact. The hypothesis follows the observed near-goal
forced detours; it is not yet an improvement claim. Direct heading/arrival
fixtures, trick rejection and dense checkpoint/cache/worker regressions pass
in34.53seconds. Five full strict cases compare0/25/50/100/200 bonuses against
the2776 profile, with displacement pricing off. Binary SHA
47b27dfa400c9675ba0bdbad1e8a58c00622c080d308095baa2f9b4e1c7143e4.

## September 21, 12:40 UTC: lower-work pricing closed; search interaction check

All four lower-work K6144/first3072 push-price cases finish and independently
replay:2736/2695/2723/2654. They do not improve2777; pricing stays off.
The separate source119 search interaction batch keeps the2776 admission
profile and tests depth12/20 or priority noise100/150/300/400. These knobs
were chosen before reducing active demand, so the new six full strict runs
check whether their optimum shifted. No held-out input is used.

At 12:41 UTC, four additional full source119 comparisons were declared: turn
cost0.5/0.75 or waitcost0.25/0.75, each on the2776 profile. Earlier coarse
checks used turn0.4/1 and wait1; these bracket the current0.6/0.5 settings
after admission changed the active traffic. All are explicit RANDOM-04
configuration tricks, fixed work, strict1s, and archived input only.

At 12:43 UTC, three source119 lower-work controls were declared: K4096/5120/6144
with half-sized first-step work, all remaining2776 settings unchanged. This
checks the throughput/runtime tradeoff after admission and guidance changes.
Each completes its full declared work and1000-step horizon or fails; no
deadline-based partial return is introduced.

At 12:50 UTC, four additional planner seeds1/2/6/7 were declared for the
unchanged2777 source124 configuration. This is explicit best-seed selection
on the archived input, not a new algorithm or independent input validation.
Previous seeds0/3/4/5 remain in the comparison; no task/start seeds are changed.
All runs retain the full1000steps and strict1s/32GB resource checks.

## September 21, 12:53 UTC: optional first-decision arrival proposals

Source129's five full runs are independently audited: exact2776 control,
bonuses25/50/100/200 score2764/2734/2745/2732. Across2482 common completed
tasks, bonus200 reduces distance1–2 away moves2944->2045, but raises loaded
steps beyond distance8 from265064to273456. Total loaded steps rise406317to
408117. This is observational redistribution, not a causal recovered-work
estimate. The mechanism changes its intended behavior but loses throughput.

Source2c309720/build130 adds R05_ARRIVAL_ROOT_PERIOD. A positive period offers
the arrival bonus only at the first decision for a declared fraction of roots;
all continuation steps use ordinary priorities. The global forecast score
therefore chooses whether to use that proposal. Period0 preserves source129's
always-on behavior; default bonus0 preserves the selected solver. Fixed work,
incumbent anchors, prefix sharing, screening, local refinement and finalist
rescoring carry the root choice consistently. Component/replanning combinations
are rejected. The explicit trick gate remains mandatory.

Regression passes36.71s, including dense task turnover, checkpoint restore,
worker/cache equivalence, shared/unshared futures, screening and rescoring.
Six strict full cases compare2776/2764 controls, bonus25 at period2, and bonus200
at periods1/2/4. Binary SHA32368f385bce8ff0a3c4f300b45d8f4e3549819383df7018eb4e0ccf0ef15373.

## September 21, 13:01 UTC: a bounded coordinated-move proposal

Source75048eed/build131 adds the general R05_JOINT_PROPOSALS option (default0,
maximum2). It matches robots jointly to legal next cells on the exact promised
occupancy, minimizes aggregate remaining cost, and cancels head-on two-cycles
by making both endpoints wait. Other paths and longer cycles retain their
collision-free destinations. Two declared cost models use zero or half the
lane edge price. Idle robots prefer waiting unless displacement helps the
joint assignment; all existing kinematic restrictions remain enforced.

Each proposal is scored through every ordinary continuation from its fixed
first decision and competes with the completed PIBT search result. This is a
bounded additional candidate, not a replacement rollout policy. Total work
is the original K plus10 or20 complete futures and1 or2 assignments; no elapsed
time truncates it. Unsupported root-operator combinations are rejected.
The colleague's negative joint-assignment toy result remains a caution, not
evidence for this additional-candidate design. No throughput claim yet.

Regression passes37.88s: explicit swap cancellation, a legal four-robot cycle,
a push chain into a hole, promised-move rotation constraints, dense turnover,
checkpoint replay, caches/workers, shared futures, screening and turn-reversal
accounting. Four full strict cases compare2776 control,1/2 proposals, and2
proposals at K6144. All selected map/horizon settings remain behind RANDOM-04's
trick flag; the move-proposal mechanism itself uses no map-specific knowledge.
Binary SHA8f7160fa2aecaa129316388db7619d4446357d664c85aa5dac6793bada77a10c.

All source119 admission-search, kinematic-cost and lower-work batches are now
independently replayed and closed. Depth12/20 gives2756/2753; noise100/150/300/400
gives2707/2738/2724/2723; turn0.5/0.75 gives2739/2734; wait0.25/0.75 gives2754/2685.
K4096/5120/6144 gives2661/2703/2757 at mean239/291/342ms and max517/593/687ms.
The lower-work tradeoff is recorded, but throughput selects the existing profile.

Source124's newly declared seeds1/2/6/7 give2757/2766/2753/2753. All independently
replay; the maximum is790.204ms. The2777 configuration now has eight planner
seeds plus its exact repetition, all below791ms. Earlier below789ms statements
referred to the original four-seed qualification; the new slower observation
is retained. No fresh task/start inputs were generated or tuned.

## September 21, 13:10 UTC: bounded rematching before swap cancellation

Source027df4d9/build132 adds R05_JOINT_REPAIR_ROUNDS (default0, max8). Each round
forbids one lower-regret directed edge of every remaining head-on swap, then
runs another complete matching. Each robot's own wait stays available, so the
matching remains feasible. Every declared round completes; final unmatched
swap conflicts still become paired waits. This changes only optional candidate
generation, never the entry deadline or ordinary policy. Regression38.30s
includes the blocked two-robot corridor and dense replay/worker/cache checks.
Four full strict cases compare two-proposal control with1/2/4 repair rounds.
Binary SHA175466cf2a3ee9bcb2e62905f1fffa8173dc66a2bab9c4d71f11914f22fe4801.

Source130 arrival-root cases are closed and independently audited: exact2776
and2764 controls; bonus25/period2 gives2727, bonus200/period1/2/4 gives2699/2728/2740.
All lose. Both controls preserve all six trajectory/schedule/event/task fields.
Keep arrival protection disabled. Source131's global joint proposals remain
under full-run evaluation; sampled steps usually reject them and report many
canceled two-cycles, motivating the rematching comparison without claiming a win.

## September 21, 13:13 UTC: approved Fable request returned a credit error

The user explicitly approved the prepared6022-byte results-only summary. Its
original SHA was verified, and Claude Code resumed the existing Fable5.1/max
session27a4316e-b79d-46cf-86b4-41b0f558938a with tools disabled and the prepared
12USD cap. The provider returned an out-of-usage-credits error and no review.
The earlier automatic approval rejection is resolved. The exact payload and
original response remain in ignored runs/random05/fable-random04-diagnosis-02;
they are not published or rerouted. Continue local work and do not retry the
unchanged credit error. No new external feedback is claimed.

## September 21, 13:24 UTC: closed joint proposals and user-requested checkpoint

All build131 and build132 full runs are independently audited. Build131 gives
2776 control,2775 with one proposal,2773 with two, and2721 at lowerK6144.
The control preserves all six archived2776 trace fields. Build132 gives
2773/2776/2776/2776 for0/1/2/4 repair rounds, with maximum entry times
678.807/682.767/686.688/699.938ms. Zero repairs reproduces build131's2773
exactly. The other three runs equal the old2776 score but differ in all six
trace fields; equal throughput does not establish equivalent behavior.
No record is promoted. Joint proposals and rematching remain default-off.
[Joint proposal audit](results/random04-joint-moves-split-full-v131/audit.json),
[repair audit](results/random04-joint-repair-split-full-v132/audit.json),
[trajectory comparisons](results/random04-joint-repair-split-full-v132/control-equivalence.json).

The user requested a comparison against max(NMS, KK). The dashboard now includes
the frozen published team scores separately from matched local NMS. Selected
PILOT726/1376/2602/2777/4011 versus published688/1260/2334/2547/3050 gives
+5.52%/+9.21%/+11.48%/+9.03%/+31.51%. KK supplies the first two references;
NMS supplies the last three. There is no matched local KK evaluation yet, and
the original matched-NMS qualification thresholds2595/2838 are unchanged.

At the user's stop request, all PILOT simulation jobs have finished and all
current result audits are closed. No new experiment or source133 was started.
Current records remain726/1376/2602/2777/4011. RANDOM-04 still needs61tasks to
reach2838; no fresh RANDOM-04 input exists. Preserve the eight-seed timing
qualification, all strict deadline failures and held-out inputs50001–50012.
Logs and small evidence artifacts are committed and pushed as the stopping
checkpoint. Resume only at the user's request.

## September 21, 15:10 UTC: local components from a joint proposal

Sourcebddcfda7/build133 adds R05_JOINT_GROUPS, default0 and bounded at128.
Each joint donor is decomposed with the ordinary best plan into independent
motion components. A hybrid substitutes one whole component, ranked by static
remaining-cost decrease, then scores every ordinary continuation. The final
forecast objective stays unchanged; the static ranking only chooses proposals.
If fewer components exist than declared, unchanged anchors fill the workload.
All declared futures complete: baseK + proposals*(1+groups)*branches.

Full regression passes39.19s, including positive component budgets, checkpoint
replay, worker/cache equivalence, shared futures, screening, and reverse-turn
accounting. Unsupported root operators remain rejected. This uses no map-specific
knowledge; selected guidance/horizon/admission still require the explicit flag.
Six full RANDOM-04 cases compare2777 control,1x16 repaired components,2x16/32
unrepaired components,2x32 repaired components, and a legacy2773 control.
No performance improvement is claimed before complete-run replay and timing.
Binary SHAe92518b8a8e42298687451e4579bacf76a46c1c5f80aebdffecad4e07bb8fe10.

### 2026-09-21 15:45 UTC — resumed campaign checkpoint

The broader all-five objective is active. Source133 component proposals close
without a record:2760/2777/2723 versus2777; the equal score is a different trace.
The first-step1075.667ms failure remains recorded; its smaller fixed startup-work
variant completes2739. Source135 idle alignment currently loses2545/2756 onR03/04,
while controls reproduce2602/2777 in all six fields. Source132 alternate R03 fields
and weaker contrast all lose; uniform guidance fails its first entry at1313.366ms.
PersistentR03caps340/360/380 score2467/2536/2546 and do not transfer the R05gain.

Active full tests retain the selected first guidance flip while adding1–4 further
flips on both densities. R04also tests caps540/550/570/580 on its current2777
combination. These are explicitly flagged tricks; no held-out input is used.

### 2026-09-21 16:39 UTC — negative closures and independent-edge search

Source136progress ties giveR03=2556versus exact2602; source137local goal guidance
radii1/2/4 and mixes0.25/0.5 give2543/2564,2573/2561,2558/2563, control2602exact.
R04largerK10240/12288 gives2751/2754; rootmutation0.05/0.15/0.2/0.3 gives
2703/2690/2679/2634. All are fully audited; no new record. Nonlinear matching138
also loses at both densities; its full audit is being finalized.

Source139/fd1a724d passes44.82s regression and preserves selected guidance flips
while adding one independent edge mutation. Controls plus extraseeds0–3 per
density are active. The earlier nested flips all included the same second edge;
this experiment tests different second edges without undoing the selected first.
R04still61tasks below2838; freshR04inputs50013/50014remain ungenerated.
