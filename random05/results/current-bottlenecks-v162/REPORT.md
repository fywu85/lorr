# Current RANDOM replay and runtime bottlenecks

Selected full archived combined runs; references are matched max(NMS, Kitty Knight).
All source/input/replay/resource audits are linked below. This is observational diagnosis.

| Instance | Tasks / matched max | Completed loaded mean steps | Completed loaded wait / turn share | Extra cells vs hop bound | Initial orders unfinished |
|---|---:|---:|---:|---:|---:|
| RANDOM-01 | 735 / 692 (+6.21%) | 68.1 | 1.66% / 21.02% | +3.17% | 9 |
| RANDOM-02 | 1408 / 1256 (+12.10%) | 70.7 | 3.58% / 22.20% | +6.86% | 17 |
| RANDOM-03 | 2621 / 2359 (+11.11%) | 101.8 | 5.31% / 26.36% | +40.11% | 28 |
| RANDOM-04 | 2782 / 2649 (+5.02%) | 171.5 | 27.73% / 27.10% | +75.59% | 175 |
| RANDOM-05 | 4254 / 3172 (+34.11%) | 282.7 | 36.57% / 28.72% | +110.63% | 147 |

| Instance | Mean / max entry ms | Mean logical CPUs | Peak RSS MB |
|---|---:|---:|---:|
| RANDOM-01 | 73.470 / 111.230 | 30.27 | 386.793 |
| RANDOM-02 | 317.533 / 492.792 | 30.18 | 368.419 |
| RANDOM-03 | 502.134 / 745.646 | 30.82 | 463.966 |
| RANDOM-04 | 478.686 / 739.553 | 30.56 | 698.966 |
| RANDOM-05 | 579.781 / 874.301 | 29.63 | 490.107 |

RANDOM-01/02 already take relatively direct routes. Their remaining opportunities
should be tested in assignment/task completion and rotation coordination; raw
cell-distance shortening has less room than on the crowded cases. RANDOM-03
trades substantial detouring for movement at half occupancy. RANDOM-04/05 have
much higher loaded waiting and detouring, so coordinated motion and guidance
remain plausible targets. These comparisons do not prove which intervention
would recover any particular fraction of the observed work.

The failure-first extra repair order lost on every tested profile. Retain that
negative result rather than spending additional compute on it. RANDOM-05 flow80
is a small selected improvement and is undergoing paired planner-seed checks.
For a subsequent runtime experiment, a group rejection bound could avoid finishing
repairs whose best possible complete cost cannot beat the incumbent. Such a bound
needs admissibility and floating-point reasoning, exact rollback, and compatibility
with annealing/component salvage/multiple repair orders before implementation.
No new solver source or measured speedup is claimed by this diagnostic.

Caveats:
- Selected trick-enabled full development runs; configurations and planner seeds differ by density.
- Loaded waiting and detours are observations, not causally recoverable work. Detours compare with cell-hop distance and omit necessary rotation costs.
- Completed-work metrics exclude unfinished tasks; initial unfinished counts expose censoring.
- CPU usage is cumulative user+system time divided by wall time, measured in logical CPU equivalents, not physical cores. Sampled phase wall times include contention and are not isolated CPU attribution.
- Phase samples cover the first5steps and each100thstep, not every step; their average is not an estimate of the exact all-step phase distribution.

Evidence:
- [RANDOM-01 source/run audit](../../results/random12-anneal-split-full-v162/audit.json) and [full result](../../results/random12-anneal-split-full-v162/trick-random-01-anneal-temp0p25/summary.json).
- [RANDOM-02 source/run audit](../../results/random12-resume-cutoff-split-full-v132/audit.json) and [full result](../../results/random12-resume-cutoff-split-full-v132/trick-random-02-resume-cutoff-0p875/summary.json).
- [RANDOM-03 source/run audit](../../results/random12345-physical-deadline-split-full-v162/audit.json) and [full result](../../results/random12345-physical-deadline-split-full-v162/trick-random-03-physical-deadline1/summary.json).
- [RANDOM-04 source/run audit](../../results/random45-progress-triage-split-full-v144/audit.json) and [full result](../../results/random45-progress-triage-split-full-v144/trick-random-04-progress-triage-mixp25-span32/summary.json).
- [RANDOM-05 source/run audit](../../results/random345-flow-iterations-split-full-v162/audit.json) and [full result](../../results/random345-flow-iterations-split-full-v162/trick-random-05-flow-iterations80/summary.json).

[Machine-readable accounting](audit.json).
