# RANDOM-05 results

**Current selected archived best:4,175tasks** (+31.62% versus matched NMS32),
source[027df4d9](https://github.com/fywu85/lorr/commit/027df4d9), plannerseed0.
The explicit680active-order cap leaves all800robots movable. All2000steps pass
strict timing/resource checks and independent replay. Repetition and fresh-input
validation of this new configuration are pending.
[Admission-transfer audit](results/random05-resume-transfer-split-full-v132/audit.json).

The **4,000-task milestone is independently verified**:4,011 on the full archived
RANDOM-05 combined benchmark, reproduced exactly. On two untouched task/start
inputs, the frozen candidate scores3,912/4,047 and beats the stronger NMS repeat
by **26.63% in aggregate**. It improves on the immediately previous configuration
by0.442%, positive on both inputs. All eight original validation runs pass strict
timing, resource and independent trajectory/event audits.
[Completed frozen validation](FRESH_VALIDATION_V5.md),
[final milestone audit](results/completion-audit-4000/audit.json).

## Selected development records

All runs use800 robots,2,000 steps, strict1s entry limits and a32GB process guard.
NMS comparisons use the same archived input and matched EPYC9354 allocations.

| Allocation | Our best | NMS reference | Gain | Mean / max entry time | Peak RSS |
|---|---:|---:|---:|---:|---:|
| Four physical cores / four workers | 3,770 | 2,914 | +29.4% | 788 / 845ms | 485MB |
| 16 physical cores / 32 workers | 4,175 | 3,172 | +31.6% | 559 / 872ms | 492MB |

For the preceding4,011 configuration, raising the explicit cutoff's directional mix from0.75 to1 adds21tasks on the
selected archived planner seed0. The archived repeat is exact. Frozen V5 results
are3912/4047 versus3901/4023 for the previous3990configuration, and stronger
NMS3095/3190. Candidate means573/560ms, maxima624/644ms, RSS below560MB.
All predeclared validation criteria pass. These are new starts/tasks on the same
map; they do not establish transfer to unseen layouts.

AtK16320/mix0.5, move bias3 has a1.61% higher aggregate across eight paired
development planner seeds (five positive, including the selection seed). More
search is not monotonic: K24480/mix0.5 gives3,872. The windowed planner is off
in the record; startup weighting and finalist rescoring remain off.

These are selected single-seed maxima. Exact configurations and executable hashes
are in [best-four-cores.json](best-four-cores.json) and
[best-32-workers.json](best-32-workers.json). The four-core source is
[5f81613](https://github.com/fywu85/lorr/commit/5f81613); the32-worker source is
[027df4d9](https://github.com/fywu85/lorr/commit/027df4d9). Both use declared map-specific
guidance and known-horizon triage enabled by `--trick RANDOM-05`.

## Earlier four-core frozen independent-input comparison

Protocol[a7bad0c](https://github.com/fywu85/lorr/commit/a7bad0c) precedes generation.
Candidate settings and planner seed3 stayed fixed through all six original runs.

| Task/start seed | Frozen candidate | NMS repeats | Gain over stronger repeat |
|---|---:|---|---:|
| 50007 | 3,680 | 2,907 / 2,870 | +26.59% |
| 50008 | 3,641 | 2,930 / 2,918 | +24.27% |

Aggregate:7,321 versus5,837, **+25.42%**. All six original attempts pass full-run,
source/binary/input, four-core, deadline and memory checks. Independent replay
also verifies movements, collisions, locked assignments and every waypoint event.
Candidate means are796/791ms, maxima859/852ms, with RSS below499MB.
[Frozen protocol and evidence](FRESH_VALIDATION_V4.md).

Earlier frozenV3 measured+25.46% on different inputs with an older configuration.
This corroborates the broad performance level; it is not a paired estimate of
improvement fromV3 toV4. All input seeds50001–50010 remain excluded from tuning.

## What depends on the horizon trick

| Allocation | Cutoff off | Cutoff on | Gain from cutoff | Off versus NMS |
|---|---:|---:|---:|---:|
| Four cores | 3,503 | 3,770 | +7.6% | +20.2% |
| 32 workers | 3,632 | 3,852 | +6.1% | +14.5% |

These ablations use the earlier3,770/3,852 configurations with triage scale1.5.
Each pair differs only in `R05_HORIZON`; binary and input hashes match, as do CPU
model and allocation. All four full runs pass the strict deadline. The guidance
trick remains enabled with the cutoff off.
[Paired-setting audit](results/staged-no-horizon-split-full-v65/horizon-ablation.json).

## Method and retained evidence

The independent implementation uses oriented task-chain costs, guided matching
of unopened tasks, a two-step PIBT pipeline and randomized look-ahead. Staged
search evaluates a few futures for every proposed priority vector, then spends
the remaining futures on promising survivors. Every final candidate is fully
evaluated. Exact caches and shared prefixes reduce evaluation cost.

A deadline violation fails the run; no partial portfolio is returned. The
high-budget seed2 initially exceeded1s at step1850. Its declared strict repeat
and5s diagnostic later produced identical3,705-task trajectories, both with
maxima below565ms. The original failure remains in the evidence, and no cause
for the transient spike is asserted. Setup-loop fusion was also rejected after
full controls showed exact outputs but slower runtime; the faster source was
restored. Larger work budgets do not reliably improve throughput.

[Timestamped frontier records](../RANDOM05_PROGRESS.md),
[full frontier audit](results/progress-audit.json),
[previous goal audit](results/completion-audit.json).

## Secondary waiting metrics

The current development records' longest completed orders take1,941/1,964 steps,
versus NMS1,997/1,976. All solvers still leave some initial orders unfinished at
step2,000, so eventual maximum latency is unknown and at least2,000. These are
censored statistics, not a fairness guarantee; throughput selected the runs.
[Earlier comparison](results/task-waiting-frontiers-20260920T1612/REPORT.md),
[current fresh-input waiting metrics](results/fresh-validation-v5/WAITING.md),
[earlier four-core waiting metrics](results/fresh-validation-v4/WAITING.md),
[history](WAITING_PROGRESS.md).

## Frozen transfer to other instances

On RANDOM-01..04, the unchanged RANDOM-05 recipe scores592/1122/2171/2456
against shipped NMS649/1228/2359/2580. Disabling guidance and known-horizon
triage gives647/1079/1582/1558. These are single archived-input runs across
densities on the same layout, not retuned configurations or unseen layouts.
The pipeline's five large-map distance tables need95–189GB before other
allocations; they cannot fit the32GB limit. NMS's new large-map attempts hit
the30s preprocessing deadline. [Complete evaluation](GENERALIZATION.md).
