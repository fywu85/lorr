# Independent RANDOM-05 combined solver

A new implementation guided by the colleague's development log supplied by the
user. Their code and tuned traffic field are unavailable. This campaign uses the
archived 800-robot RANDOM-05 input, so its absolute scores are not directly
comparable to the colleague's private synthetic instances.

[Throughput history](../RANDOM05_PROGRESS.md) records each verified best with a
UTC timestamp, source commit, settings, and linked benchmark evidence. Published
scores, local NMS references, single-seed bests, and replicated results are
identified separately. Exact frontier cases are saved in `best.json`,
`best-four-cores.json`, and `best-32-workers.json`. The history audit is run with
`python3 random05/tools/audit_progress.py`; it requires the frozen local builds
and traces. It also generates [order-latency history](WAITING_PROGRESS.md),
including unfinished-order ages. Throughput remains the primary objective.

The solver combines exact oriented task-chain costs, reassignment of unopened
tasks, a two-step PIBT pipeline, idle pre-rotation, and parallel look-ahead over
persistent randomized priorities. Experiments cover greedy or exact assignment,
static traffic fields, local search, cycle coordination, and pocket evacuation.
Started tasks remain assigned to their original robot.

Map-tuned guidance, capped priority aging, and known-horizon abandonment require
`--trick RANDOM-05`. Known-horizon abandonment suppresses a robot's planning goal;
it does not illegally unassign its started task. These throughput-oriented choices
are explicit experiments and carry no fairness claim.

Development uses a fixed number of complete futures (`R05_K`) of depth
`R05_DEPTH`. An optional `R05_FIRST_K` declares a smaller fixed count for
timestep 0 to leave room for startup work. `R05_THREADS` controls parallel workers. A deadline overrun raises an
error; a partial portfolio is not silently returned. Actual and promised spatial
transitions are collision-certified, and the simulator independently validates
complete 2,000-step runs. Regression tests cover chained costs, task locks,
collision rejection, dense turnover, deterministic worker counts, and eviction.

Build and test on GRID:

```sh
python3 random05/tools/grid.py submit --kind build --output runs/random05/my-build
```

For a benchmark, copy a case JSON from `experiments/`, set its binary to the
completed build, then submit:

```sh
python3 random05/tools/grid.py submit --kind benchmark \
  --output runs/random05/my-benchmark --cases path/to/cases.json
```

The runner freezes source/binaries, hashes inputs and guidance files, reserves
physical cores through GRID, verifies binding, and places concurrent cases on
disjoint cores. Each solver process has a 32 GB address-space cap. `results/`
contains compact evidence; `runs/` contains ignored binaries and full traces.
Experimental settings stay off unless explicitly enabled; use the configuration
in the frontier's evidence to reproduce it rather than assuming defaults are best.

For matched-hardware validation, add `--hosts 'research33*|research39*'` and
`--cpu-model 'AMD EPYC 9354 32-Core Processor'` to benchmark submission. The
runner selects queue instances on those known hosts and verifies the CPU model
before starting. Exploratory runs may use other CPU models; record that
explicitly and validate promising results on the reference hardware.

For mixed-duration batches, use `python3 random05/tools/split_grid.py submit`
with the same output, cases, hosts, and CPU-model arguments. Each case gets an
independent GRID allocation. Run `split_grid.py collect --output <batch>` to
assemble completed summaries; each child directory retains its own provenance.


Fresh task/start validation is documented separately in
[FRESH_VALIDATION.md](FRESH_VALIDATION.md), with frozen settings and two NMS
repetitions per input. Audit the completed comparison with:

```sh
python3 random05/tools/audit_fresh.py \
  --batch random05/results/fresh-validation-split-full-v31 \
  --output random05/results/fresh-validation-v1/audit.json
```

This checks the predeclared manifest at commit22e7cd1, candidate/reference binary
hashes, generated input hashes, matched physical cores/CPU model, complete valid
runs, latency and peak RAM. It needs the original generated inputs (recreatable
with the documented generator). Do not tune on that validation set.


The latest completed fresh-input comparison is [validation V3](FRESH_VALIDATION_V3.md):
a frozen four-core configuration beats the stronger of two NMS repetitions by
26.38% and24.54% on two newly generated task/start inputs,25.46% combined.
All six runs pass the full2,000-step, strict1s and32GB checks. These inputs remain
excluded from tuning. The original protocol is pinned at81bdfbd; later selected
records must not replace its candidate.

```sh
python3 random05/tools/audit_fresh.py \
  --batch random05/results/fresh-validation-v3-split-full-v52 \
  --output random05/results/fresh-validation-v3/audit.json \
  --protocol-commit 81bdfbd \
  --protocol-json random05/experiments/fresh-validation-v3-protocol.json
```
