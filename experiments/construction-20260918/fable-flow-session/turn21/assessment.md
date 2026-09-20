# Local assessment of Fable turn21

Completed 2026-09-20T01:23:46.685504UTC in the same Fable5.1/max session,
no tools/tests or permission denials. Source deltas:0. USD83.4075485 is cumulative
session cost. The prompt supplied validated current blocker and task-pool audits;
the five full KK factorial runs were still pending. No policy gain was supplied.

## Accepted direction

The task-pool history strengthens the hypothesis that KK's shorter completed
chains accompany persistent deferral of long tasks. It is useful to test whether
rematching reduces the empty-travel cost of that task preference. The current
factorial must be interpreted through full action accounting and task-pool ages,
with scheduler/LNS budget changes visible. A repeated control is descriptive
evidence about variability, not a formal significance bound.

If assignment is selected, a task-centered retargeting pass is a distinct candidate:
consider recently revealed free tasks, find nearby eligible unopened holders,
compare pickup plus chain costs, and allow a shorter chain to compensate for a
longer pickup. This differs from the old robot-centered, pickup-improvement-only
pass. Use one complete published metric for both old/new pickup quotes; keep
richer chain pricing separate. Baseline, ordinary additive-only, retarget-only
and both remain the relevant four arms. The old additive loss is evidence of
risk, not a result for the current reference.

If motion is selected, search complete small-group alternatives on identical
decision states before implementing another broad repair mechanism. Distinguish
existence of an improving joint replacement from ability of the current constructor
to reach it. A witness can identify a concrete search limitation; a heuristic
blocker count cannot. A bounded probe must report truncation instead of calling
an unfinished enumeration exhaustive.

## Corrections to measurements and inference

- The measured CGAR statement concerns **unpicked** tasks at six checkpoints.
  It does not say no task of any kind exceeds1,000 steps; started tasks can be older.
- The revealed-chain means are available in the verified reports. KK's is248.543,
  not an estimated245. Avoid Fable's approximate4–5%/1% selection percentages.
  The task file is shared but performance-dependent reveal prefixes differ.
  Persistent deferral is supported; its causal share of the8.025-step cohort
  difference is not established.
- "Placeholder holding plus late binding" is a proposed mechanism. The old held
  tasks and first/last pickup delays support investigation, but do not demonstrate
  each robot follows that pattern or explain the whole throughput advantage.
- Uniform weights change pickup/chain costs, selected cohorts, routing and
  clock-selected work. A loaded-action contrast can guide priorities but cannot
  prove that the weight file alone, or alternatively planner structure, explains
  the motion gap. Its offline training provenance remains unknown.

## Required fixes to the proposed assignment design

Fable suggests removing the permanent retarget limit because each exchange lowers
the robot's current cost. **That is not a liveness proof.** The robot moves, goals
change and the guidance metric is republished, so the compared costs need not be
a common monotone potential across exchanges. Retain the existing finite-retarget
protection for a first test, or supply and test a separate finite credit/ticket
invariant before removing it. Cooldown alone is insufficient. Preserve started,
primary/pending-primary, recovery and fair-admission protection.

A pre-scheduling pass that unconditionally gives free robots fresh cheap tasks
also changes ordinary assignment in the supposed retarget-only arm. To isolate
that factor initially, let ordinary scheduling finish and retarget existing
eligible unopened holders using the remaining pool. Define availability from
the proposed schedule, not stale `agent_assigned` fields. Released tasks become
ordinary candidates on the next call; their original reveal/progress metadata
and fair-admission access remain intact.

The released task still needs service. A shorter taker cost does not account for
its later pickup or congestion. Report those costs through full empty work,
pool cohorts and age tails. Existing oldest-task admission is a progress mechanism,
not a tight waiting-time guarantee; throughput improvement can worsen those statistics.
Do not silently equate retaining admission cadence with preserving current age p90.

Resident reverse oriented tables must match the current published metric and
cover both old/new quotes. A cache hit does not guarantee that the next planning
phase, which may publish another metric, retains exact guidance for the new goal.
Count cache/basis coverage and skipped roots, and make no speculative table builds
under the proposed cache-only variant. Fresh tasks may be precisely those with
missing tables; a null result with little coverage would be weak evidence.

The proposed256×2,048 BFS work ceiling is a candidate bound, not a validated
runtime allocation. Old and new pickup costs require at least two lookups per
holder pair, besides chain quotes and bookkeeping. Calibrate a deterministic
quota with deadline screens, then require all full entries to meet1s. No
clock-based early return of a partial plan.

## Required fixes to the motion interpretation

There is no reusable full frozen-decision dump in this campaign yet. Existing
artifacts contain trajectories and aggregate candidate audits. A new read-only
snapshot/probe must preserve candidates, costs, powers, protected flags and all
relevant outside reservations; verify unchanged decisions before using it.

Failure to find a replacement for a root plus at most two owners establishes a
null result only for that group, candidate set, horizon and fixed outside plan.
It does **not** make MD-PIBT, group LNS or other regions futile: a larger group,
another owner set, intermediate cost increase or longer horizon can differ.
Likewise, existence of a joint witness does not by itself prove the production
DFS can reach it. Reachability needs faithful replay/instrumentation of its
ordering, power checks and rollback rules, not just counting owners.

These qualifications retain Fable's useful experiment proposals without turning
them into unproved guarantees. Selection of the next implementation still follows
the pending KK factorial; the current CGAR frontier remains unchanged.
