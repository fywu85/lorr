# Local assessment of Fable turn20

Completed 2026-09-20T00:38:21.718270UTC in persistent session
`1ebb1075-3538-49d1-93d1-a00c94fa256a`, Fable5.1/max, no tools or permission denials.
USD82.33881575 is cumulative session cost. Production source deltas:0. The prompt
supplied the V1 experiment patch and original fixture, not the subsequent V2 additions.

## Incorporated checks

Fable correctly identified that the original fixture did not verify post-call
`agent_assigned` metadata or a real simulator lifecycle. Native matching does mutate
that metadata. Inspection confirms TaskManager refreshes `env->task_pool` and
`curr_task_schedule` from authoritative simulator state before the next entry.
V2 checks exact owner values and uses actual TaskManager assignment, pickup,
completion, replenishment and synchronization across two scheduler calls. Started
tasks remain protected and both proposed schedules pass simulator validation.

The fixture now runs with both all-one and hand-counted nonuniform directional
costs. This checks eligibility with heterogeneous costs; it does not apply the
literal warehouse weight file to the synthetic corridor. Native, default-enabled
and explicit-enabled outputs agree in all five cases for both cost settings.
The disabled option retains held unopened tasks and correctly assigns free robots.

Fable's planner-budget concern is confirmed by `kk/src/MAPFPlanner.cpp:47`:
the wrapper subtracts time elapsed since `env->plan_start_time`, including scheduling.
V2 records scheduler milliseconds, LNS limit and remaining time before search,
and the existing merged-iteration observations. These make the confound visible;
they do not make the comparison equal-work. Uniform weights also affect both
scheduler and planner costs. Interpret the full factorial as whole-policy effects.

The initial malformed uniform configuration was found locally after this prompt.
The runner now retains the native conditional array and empties its branch values.
The actual native-loader fixture confirms the corrected pair differs only in the
weight path and rejects the old scalar shape. Both startup failures are archived,
and corrected uniform screens completed without errors or timeouts.

## Qualifications and deferred suggestions

- The requested free-robots-greater-than-unassigned-tasks case is outside the
  supplied input's invariant. Native TaskManager maintains 15,000 ongoing tasks
  for 10,000 robots, giving a 5,000-task surplus regardless of assigned count.
  Native matching requires enough tasks. Do not rewrite that branch and call the
  resulting experiment a simple rematching ablation.
- Fable estimates approximately30 new free robots per tick. That is not a
  measurement here. The original no-free-robot exit is retained; sampled pool logs
  are emitted only when matching runs.
- A duplicate control gives an observed repeat difference, not a statistical noise
  bound. A contrast smaller than that difference is weak evidence, but a larger
  one is not automatically significant. More repeats may be required.
- Longer chains among unfinished tasks can result from horizon censoring even
  without deliberate stranding. Pool-age and chain distributions are useful
  descriptive checks, not proof that all8.025 steps of the completed-chain gap
  can be recovered. An intervention can also alter the revealed task prefix.
- The predicted keep-arm empty-travel increase is a testable part of Fable's
  proposed mechanism. Its absence would weaken that explanation; it would not
  logically disprove every additive-assignment/rematching interaction in CGAR.
- The suggested compute-full-matching-then-discard control does not guarantee
  equal planner work, and native matching mutates owner metadata. If later used,
  isolate that state and verify real costs and search effort. It is not implemented.

No new throughput gain is established by this review, the fixtures, or the screens.
