# KK guidance and unopened-task rematching diagnosis

Prepared 2026-09-20. This is an explanatory competitor experiment following the
[bottleneck review](../bottleneck_review/REPORT.md), not a CGAR improvement or an
equal-resource SoTA claim. The CGAR frontier remains 144,510 single / 143,941.2
six-seed mean tasks. Production CGAR and the archived KittyKnight sources are unchanged.

## Intervention and allocation

The four arms cross supplied versus all-one weights with unopened-task rematching
enabled versus disabled. A fifth run repeats the supplied/rematch control. Every
arm uses the same input, binary, 5,000-step horizon, 10,000 robots and native
1,000ms entry limit. Each runs on a separate exclusive EPYC 9354 GRID host with
16 physical cores / 32 SMT threads, no CPU quota, and a 128GiB address-space
reservation. Actual process RSS is reported against the separate CGAR 32decimalGB
limit. A large reservation is not a claim that KK meets that limit.

Full array **8899243.1–5** started 2026-09-20 00:50:42UTC. Raw directory:
`runs/kk-guidance-rematch-full-v2-20260920`. Held analysis **8899246** replays every
completed trajectory with the existing four hand-checked travel-accounting
fixtures. A second held job **8899251** applies the validated task-pool replay
after accounting, preserving the original fields and adding cohort/age snapshots. It retains failures, all-empty work, unfinished loaded work, reassignment
counts and the observed difference between the two controls. Pending full results
must not be inferred from the screens below.

`LORR_KK_REMATCH_UNOPENED=0` preserves existing task assignments and limits matching
to free robots and unassigned tasks. Unset or `1` retains native pool eligibility.
Started tasks stay protected. The native early exit when no robot is free remains
in both modes. This toggle only changes the warehouse scheduler in a frozen
experimental copy; it is not a generic policy being installed in CGAR.

Uniform guidance uses KK's native loader: retain the `map_weights_path` conditional
array and set each branch's `value` to `""`. The decoded configuration differs only
in the weight path (and the native diagnostic `details` string). Both scheduling
and motion use the resulting shared weights, so this is a whole-policy intervention.
The supplied warehouse weight file is never imported into CGAR.

The patch also logs scheduler wall time, the LNS time limit and remaining time
before search, merged iteration records and search costs. Native KK subtracts
scheduling time from the planner allowance. Consequently rematching changes can
affect planner work as well as assignments. Iteration records are not all attempted
worker searches, and the entry timing excludes its final diagnostic print. Simulator
timeout counters are retained; these diagnostics do not certify CGAR's complete-entry
deadline contract. Two controls provide an observed difference, not a confidence interval.

## Validation and startup correction

Final build V2 completed 2026-09-20T00:49:23.626873UTC, binary SHA-256
`4e4d11880896ad741a0735683e5572dc2e2bb0dbbefd6dcec82505c47dadb70a`.
Only `scheduler.cpp`, `LNSSolver.cpp` and `Entry.cpp` differ from the frozen 419-file
archive. [Patch and complete manifests](evidence/build-v2/).

The compiled fixture links the actual KK scheduler, orientation heuristics and
TaskManager. Ten native/patched/mode/cost executions cover five cases each:
held/free/started assignments, all-free matching, the original no-free-agent exit,
an empty disabled pool, and a real two-call task lifecycle. Uniform and synthetic
nonuniform directional costs give the hand-computed assignments. Native and enabled
outputs agree; disabling rematching retains holders while assigning free robots.
Checks include unique assignments, exact owner metadata, unchanged task progress,
simulator re-synchronization, and started-task protection after pickup. Invalid
toggle values fail explicitly. The separate native configuration-loader fixture
confirms only the weight path changes and rejects the old malformed scalar shape.
[All fixture results](evidence/build-v2/fixture-results.json).

The initial uniform screens failed before planning, exit139, because **our runner**
incorrectly replaced the conditional array with a scalar empty string. The loader
rejects that shape; the original asynchronous initialization path does not report
the exception properly before using the uninitialized planner. The native-loader
fixture reproduces the configuration rejection. Correcting the array values fixes
startup. Both failed attempts and their original configurations remain in
[initial-screen](evidence/initial-screen/); they are not algorithm failures or scores.

| Correctly configured screen | Horizon | Planner / schedule errors / timeouts | Peak process RSS |
|---|---:|---:|---:|
| Supplied, rematch | 200 | 0 / 0 / 0 | 30.658 GB |
| Supplied, keep | 200 | 0 / 0 / 0 | 30.640 GB |
| Uniform, rematch, corrected | 200 | 0 / 0 / 0 | 30.640 GB |
| Uniform, keep, corrected | 200 | 0 / 0 / 0 | 30.626 GB |

[Screen summaries](evidence/screens.json) preserve task counts for completeness;
200 steps are a feasibility check, not a quality ranking. Screens use V1. V2 keeps
the same eligibility patch and adds timing observations and stronger fixtures.

The proposed test with more free robots than unassigned tasks falls outside this
input's invariant: TaskManager replenishes 15,000 ongoing tasks for 10,000 robots,
so with A assigned robots, unassigned tasks exceed free robots by 5,000. Native KK
requires enough tasks. This experiment preserves that behavior rather than silently
changing the competitor to handle a different task regime.

## Reproduction and interpretation

Use `build.py --output runs/NEW_BUILD`, then
`run.py --build runs/NEW_BUILD --output runs/NEW_RUN --steps 5000`.
`analyze.py --raw runs/NEW_RUN` freezes the analyzer and submits a held GRID job.
The current analyzer's destination is the named V2 results directory; use a new
destination for a new campaign. Build/run scripts save source, support, binary,
configuration and input hashes. The recorded Git hash is the base commit; the
patch and manifests identify the exact diagnostic source built on top of it.

Fable turn20 reviewed the original patch and fixture, without running tools.
[Local assessment](../fable-flow-session/turn20/assessment.md) records the resulting
fixture additions, budget instrumentation and qualifications. A KK treatment
effect will motivate a CGAR hypothesis; it cannot establish a CGAR gain without
paired complete runs and six-seed confirmation.
