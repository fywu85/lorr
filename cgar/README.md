# Start-Kit

## Join the competition

Log in to the [competition website](http://www.leagueofrobotrunners.org/) with a GitHub account, and we will automatically create a private GitHub submission repo for you.
The repo will be the place where you submit codes. In the `My Submission` page, you can click "My Repo" to open your GitHub submission repo page.

## Clone your submission repo

Clone your submission repo to your local machine. The repo contains starter codes to help you prepare your submission.

```
$ git clone git@github.com:your_submission_repo_address
$ cd your_submission_repo
```

## Compile the start-kit

### Dependencies

- [cmake >= 3.16](https://cmake.org/)
- [libboost >= 1.49.0](https://www.boost.org/)
- Python3 >= 3.11 and [pybind11](https://pybind11.readthedocs.io/en/stable/) >=2.10.1 are recommanded for python interface user.

Install dependencies on Ubuntu or Debian Linux:
```shell
sudo apt-get update
sudo apt-get install build-essential libboost-all-dev python3-dev python3-pybind11 
```

[Homebrew](https://brew.sh/) is recomanded for installing dependencies on Mac OS.

### Compiling

Using `compile.sh`:
```shell
./compile.sh
```

Using cmake: 
```shell
mkdir build
cmake -B build ./ -DCMAKE_BUILD_TYPE=Release
make -C build -j
```

## Run the start kit

Running the start-kit using commands: 
```shell
./build/lifelong --inputFile the_input_file_name -o output_file_location
```

for example:
```shell
./build/lifelong --inputFile ./example_problems/random.domain/random_32_32_20_100.json -o test.json
```

more info on help:
```shell
./build/lifelong --help
```

## Windows users
If you are a Windows user, the most straightforward method to utilize our start-kits is by employing the WSL (Windows Subsystem for Linux) subsystem. Follow these steps:
1. Install WSL, please refer to [https://learn.microsoft.com/en-us/windows/wsl/install](https://learn.microsoft.com/en-us/windows/wsl/install)
2. Open a shell in WSL and execute the following commands to install the necessary tools (CMake, GCC, Boost, pip, Pybind11):
```shell
sudo apt-get update
sudo apt-get install cmake g++ libboost-all-dev python3-dev python3-pip
pip install pybind11-global numpy
```
3. Employ the commands provided above to compile the start-kit.

While it's technically possible to use our start-kit with Cygwin, Mingw, and MSVC, doing so would be more complex compared to using WSL. You would likely need to configure the environment yourself.

If you are a docker user, another choice is to develop and test your python implementation under a docker environment. You can the re-create the evaluation environment locally on your machine. For more details, check out the [Test in Docker](./Prepare_Your_Submission.md#test-in-docker) section.

## Upgrade Your Start-Kit

If your private start-kit copy repo was created before a start-kit upgrade, you could run the script `./upgrade_start_kit.sh` to upgrade your start-kit to the latest version.

You can check `version.txt` to know the current version of your start-kit.

The `upgrade_start_kit.sh` will check which file is marked as an upgrade needed and pull those files from the start-kit. It will pull and stage the files, but not commit them. This allows you to review the changes before committing them. 

For files stated as unmodifiable in [Parepare_Your_Planner.md](./Prepare_Your_Submission.md), you always commit their changes.

⚠️ But please be aware that, the start-kit v2.1.0 introduces requested API changes on `task_pool`. This requires minor revision to your implementation to adapt to the new API.  
This change also impacts the implementation of function `update_goal_locations` in `src/Entry.cpp`, therefore, the upgrade script will pull the new version of `src/Entry.cpp` and may overwrite your changes. You could compare the difference using `git diff` and decide whether to revert some modifications or partially accept changes on this file. 

The upgrade script will not touch most of the participants' implementation file.
How every the example implementation in `python/pyMAPFPlanner.py`,`python/pyTaskScheduler.py`, `inc/MAPFPlanner.h`, `inc/TaskScheduler.h`, `src/MAPFPlanner.cpp`, `src/TaskScheduler.cpp`, `default_planner/planner.cpp` and `default_planner/scheduler.cpp` are updated with with new API and additional documentaion. You may want to view changes on these files. 

## Input output description

Please refer to the [Input_Output_Format.md](./Input_Output_Format.md).

## Prepare Your Planner

Please refer to the [Prepare_Your_Submission.md](./Prepare_Your_Submission.md).

## Debug and Visualise Your Planner
We provide a visualisation tool written in Python: [https://github.com/MAPF-Competition/PlanViz](https://github.com/MAPF-Competition/PlanViz).
It is able to visualise the output of the start-kit program and help participants debug the implementations. 

Please refer to the project website for more information. Also the document [Debug_and_Visualise_Your_Planner](./Debug_and_Visualise_Your_Planner.md) which provides helpful hints for interpreting and diagnosing planner output.

## Submission Instruction

Please refer to the [Submission_Instruction.md](./Submission_Instruction.md).




### Optional regional peak retention

`CGAR_TEMPORAL_REGION_KEEP_PEAK=1` retains the highest-scoring complete plan visited
inside each regional repair batch. It is off by default and requires enabled
regions. The search still finishes its prescribed attempts/candidate work; later
attempts continue from the current search state. Only afterward may a strictly
better retained plan replace the final state. A timeout remains an error, including
when a better plan was already visited. Equal-score final plans retain the existing
acceptance behavior. This mechanism does not inspect map identities or task horizons.

The configuration receipt and `[cgar-regional-keep-peak]` counters record activation
and actual restored batches. `CGAR_TEMPORAL_REGION_PEAK_AUDIT=1` adds read-only
observations of initial, peak, final and returned scores. Higher local scores do
not establish a throughput gain; keep this option experimental until full A/B runs.


### Experimental complete-chain potential

`CGAR_TEMPORAL_CHAIN_MODE=1` scores temporal choices against the complete remaining
revealed task chain, preserving all four arrival headings. Mode 2 changes only the
remaining-work priority; mode 3 enables both. Modes 2/3 require
`CGAR_TEMPORAL_ORDER=2`. Default 0 preserves the previous choices and random streams.
This is a general CGAR guidance mechanism; map-specific fields still require the
corresponding `--trick INSTANCE` flag. No unrevealed task or known run horizon is used.

The score uses the actual end heading and advances at most one errand after each
simulated action, including turns and waits. Starting on an errand does not service
it without an action. Costs are exact for the declared static weighted guidance
graph and ordered services, not predictions of actual congested travel time.
CGAR reservations, primary paths, task ownership and recovery remain authoritative.
A missing task record uses its visible single goal; unreachable guidance falls back
to the existing score and is counted. Learned/dynamic fields, guide routes, the old
partial next-errand extension and neutral-tail scoring are incompatible.

Preprocessing builds **complete** heading tables using `64 * free_cells^2` bytes:
42,928,704 bytes for the 819-cell random map. `CGAR_TEMPORAL_CHAIN_MB` limits this
table allocation (default 512 MiB, at most 8192); total process RSS is separately
enforced. Large maps may exceed this budget and are rejected before allocation.
`CGAR_TEMPORAL_CHAIN_THREADS` selects 1–32 preprocessing threads (default 1).
Preparation/search thread counts remain separately controlled. No partial table
is exposed, and preprocessing or planning timeouts fail explicitly.

`[cgar-chain-config]` records complete preprocessing, storage and service semantics;
`[cgar-chain]` reports prepared robot steps, fallback and scored/service/completed
choices. Independent regression compares against a heap shortest-path search on
the full action/service product graph and checks serial/parallel production actions.
The option remains experimental pending full throughput comparisons.


### Experimental CGAR-seeded rolling window

`CGAR_WINDOW=20` enables a general fixed-work rolling-window repair layer. Default
0 preserves all earlier decisions and random streams. It starts from the complete
CGAR five-action plan (including actual wait-seed rotations) and appends waits.
Primary, recovery, parked and supporting paths remain immutable; ordinary paths
obey the same core, witness and protected-intent restrictions. Protected tails
are conservative forecasts, not predictions of future primary/recovery decisions.

Each independent island repairs small robot groups using time-space A* with a task
stage in the state. Its objective is paid action costs plus exact remaining-chain
potential; idle waits after completing the revealed chain cost zero. Services
occur after actions, at most one per tick. A group is committed only if every
replacement is complete, collision-free and no more expensive in aggregate.
Node-cap exhaustion rolls back the entire group. All declared attempts and all
islands finish; a deadline raises an error even after a better plan was visited.
The selected island returns a complete plan and executes its first action.

`CGAR_WINDOW_KEEP` retains up to 6 actions from the previous suffix by default.
Retention checks observed cell/heading, task identity, one-step service advancement,
current domains and new protected reservations. Conflicting histories reset
monotonically to the validated fresh seed. Fresh and retained seeds compete on the
same objective. Keep must be smaller than the configured horizon (6–32).

Work defaults: `CGAR_WINDOW_ITERS=128` attempts **per island**,
`CGAR_WINDOW_NODES=2048` expanded states **per single-robot search**,
`CGAR_WINDOW_GROUP=4`, `CGAR_WINDOW_WORKERS=4`, `CGAR_WINDOW_THREADS=4`.
Thread count changes execution allocation without changing island seeds or results.
`CGAR_WINDOW_WAIT_COST=0` uses the ordinary forward base; a positive override
uses that integer wait cost. Turn/forward costs follow the static guidance graph.
The existing `CGAR_TEMPORAL_CHAIN_MB/THREADS` bound and build the complete oracle.

The first implementation requires static guidance and disables five-step chain
scoring, legacy temporal history/promises, guide routes and partial next-errand
scoring while the new layer is enabled. Instance fields still require
`--trick INSTANCE`. No run horizon or unrevealed task is used. Existing temporal
score/rotation diagnostics describe the seed search; `[cgar-window-config]` and
`[cgar-window]` identify the final objective, complete work, history and changed
first actions. Movement diagnostics account for the actions actually executed.

This is a new implementation informed by PILOT's rolling-window mechanism, not
an imported alternative planner. Its production default remains off. Independent
layered action search, protected paths, atomic rollback, task-history invalidation,
deadline propagation and serial/parallel production decisions are regression-tested.
Throughput promotion requires full strict one-second benchmark evidence.

`CGAR_WINDOW_SEED_ROLLOUT=1` optionally extends the first five CGAR actions with
complete joint temporal-PIBT chunks before window repair. It reserves all robots
in each projected chunk, advances revealed task stages, and leaves protected tails
fixed. This avoids initializing the remaining window as a wall of stationary
robots. The original wait-extended seed remains available if its whole-window
objective is lower. All chunks finish before selection; deadline failure propagates.
The first five CGAR seed actions remain unchanged by this extension. Default 0.

Window randomness has an independent seeded stream. A node-capped no-op overlay
with history disabled therefore preserves the complete original CGAR trajectory,
not just its immediate first action. This identity is tested in production.

`CGAR_WINDOW_PROGRESS_TIES=1` favors lower remaining-chain cost among equal
paid-plus-remaining group objectives, seed choices and completed islands. Within
A*, equal primary estimates prefer lower heuristic cost before deeper time.
This is a secondary progress preference; it never accepts a worse primary group
objective. Default0 retains the previous comparisons. The primary A* objective
remains checked against independent action search, with both tie modes and
serial/parallel production coverage. Receipts report all three remaining-cost
values alongside their total costs. No future completion-time guarantee is implied.

`CGAR_WINDOW_PROTECTED_PREFIX=1` retains the exact physical first action of the
CGAR primary and its supporting robots while allowing compatible later forecasts
to move. Active transactions, witness cells, parked robots and capacity bootstrap
paths stay frozen for the entire window. Pocket forecasts use the robot's existing
CGAR pocket permissions; other robots still cannot enter its protected intent.
The adapter preserves the primary's real commitment and proposed destination,
including turn actions whose committed destination differs from the occupied cell.
No actual CGAR protected action is changed. Future paths remain forecasts and are
revalidated at the next step; this is not a new liveness proof. Default0 keeps the
original fully frozen tails. Regression exercises changed forecasts with unchanged
first actions, pocket recovery, capacity mode and serial/parallel production.


Optional field comparisons (all require the matching `--trick INSTANCE`):

- `CGAR_TRICK_RANDOM_REFERENCE=3` selects the existing integer adaptation of
  PILOT's flow field on any of RANDOM-01 through RANDOM-05. This is exactly the
  field previously available as dense reference 0; no new field is fitted.
  Explicit references default to turn price 20, overridable as before. Field
  geometry and configuration selection are tricks; the CGAR planner is retained.
- `CGAR_TRICK_LANE_COST=4|8|12|16` changes the opposing-edge price in the adapted
  CITY-01, CITY-02 and GAME lane fields. Preferred edges and turns cost 4. The
  default 16 preserves earlier behavior; 4 gives uniform forward costs. Native
  metrics, lanes-off mode and other instances reject this selector. Each field
  has a distinct recorded SHA-256. This does not change physical action duration.

Neither option is a claimed improvement until its complete benchmarks pass.


## Optional common-continuation selection

`CGAR_FUTURE_ROOTS=4` compares the current complete repaired CGAR proposal with
other completed global starts. Defaults are horizon15, branches4, threads4 and
priority noise50 (`CGAR_FUTURE_HORIZON/BRANCHES/THREADS/NOISE`). Each root gets the
same declared continuation orders and random seeds. Complete joint temporal-PIBT
chunks project only already revealed task errands; no future assignments are
invented. The aggregate objective is paid action cost plus exact remaining chain
cost. Only the first action of a validated original CGAR proposal is executed.
The incumbent is always included and wins exact ties. Protected primary, recovery
and support paths stay fixed. This is a general, map-independent mechanism; any
selected guidance field or instance profile still requires `--trick INSTANCE`.

Roots must not exceed `CGAR_TEMPORAL_WORKERS` (maximum32); horizon must be
10/15/20/25/30, branches1..16, threads1..32 and noise0..1000000. Complete chain
resources use `CGAR_TEMPORAL_CHAIN_MB/THREADS`. Static temporal orientation
guidance is required; rolling-window repair, learned changing fields, guide
routes, next-errand scoring, neutral tails and legacy temporal history/promises
are incompatible. Chain scoring and persistent priority portfolios are allowed.
The selected root's originating priority vector is remembered.

All declared root/branch pairs finish before selection, including with one root.
Any error or deadline aborts the whole step; there is no partial-result success.
An independent RNG and non-mutating distance-table peeks preserve the old complete
trajectory when disabled or configured with one root. The forecast is a heuristic:
it holds current protected paths stationary after their five-action prefix and
does not predict future CGAR primary/recovery decisions. A lower forecast cost is
not itself evidence of greater competition throughput. Full benchmarks decide.


`CGAR_WINDOW_HISTORY_ROLLOUT=1` optionally completes the joint forecast after the
retained prefix before comparing it with the fresh seed. It requires an enabled
window, seed rollout and `5 <= KEEP < WINDOW`. Histories must first pass the same
pose/task/service/permission/protection checks and conflict resets. The kept
prefix and fixed paths remain unchanged. Complete temporal-PIBT chunks extend the
joint candidate from `KEEP` to the horizon; the repaired whole-window objective
must still beat or tie the fresh seed before history is accepted. If no compatible
history survives, no extra forecast is required. The declared maximum is
`ceil((WINDOW-KEEP)/5)` complete chunks. Diagnostics count the actual chunks.
A separate random stream leaves existing fresh seeds and repair seeds unchanged.
Any deadline failure propagates before a new persistent history is published.
This is a general default-off experiment; it has no map lookup or throughput
claim until full comparisons finish.


`CGAR_PICKUP_STARTUP=1` also uses the current orientation/forward metric for
initial timestep-zero dispatch. The default0 keeps the earlier spatial mass
assignment. Complete-field robot quotas, search caps, candidate limits and
failure semantics are unchanged; every declared field must finish. This general
option requires pickup flow and adds no map lookup. Before a learned field has
been published, its current metric is the uniform oriented metric. Later warmup
policy is unchanged. Static instance fields still require their explicit trick
gate. This can be expensive for large initial fleets and is never auto-enabled
based on elapsed time. Full benchmarks determine whether better initial pairings
repay the extra work; no throughput gain is assumed.


`CGAR_FUTURE_REGIONAL_ROOTS=1` fills the common-future pool with distinct completed
regional-round plans, newest first, before falling back to global starts. It
requires enabled futures and regions. Root zero remains the final repaired
incumbent. Capturing a round copies its validated selection vector without
additional search or RNG draws; reconstruction validates all joint reservations.
Every prescribed regional round and future branch still completes before action
selection. A deadline after a captured round remains a failed step. Diagnostics
count how many regional candidates actually enter the pool. The default0 retains
the original global-start pool. This addresses a candidate-quality hypothesis:
unrepaired global roots may be too weak to compete with a heavily repaired root.
It does not assume a measured throughput improvement or alter the CGAR protections.
