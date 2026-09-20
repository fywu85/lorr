# Independent RANDOM-05 combined solver

A new implementation guided by the colleague's development log supplied by the
user. Their code and tuned traffic field are unavailable. This campaign uses the
archived 800-robot RANDOM-05 input, so its absolute scores are not directly
comparable to the colleague's private synthetic instances.

[Throughput history](../RANDOM05_PROGRESS.md) records each verified best with a
UTC timestamp, source commit, settings, and linked benchmark evidence. Published
scores, local NMS references, single-seed bests, and replicated results are
identified separately.

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
`R05_DEPTH`. `R05_THREADS` controls parallel workers. A deadline overrun raises an
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
