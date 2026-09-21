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
