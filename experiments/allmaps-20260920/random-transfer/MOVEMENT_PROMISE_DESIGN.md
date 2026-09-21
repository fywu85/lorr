# Next bounded general transfer: preserve a movement decision after turning

Status: design only, not implemented or benchmarked. Motivated by the read-only
[actual-action audit](rotation-audit-v1/audit.json) and the reference's movement
pipeline, not a claimed causal explanation of the whole throughput gap.

In the2574-task R05 run, there are583740 turns and453960 forwards. Excluding
adjacent task/goal/assignment changes conservatively leaves105055 immediate
inverse-turn pairs (18.22% of turn-predecessor transitions). Only35.25% of those
turns are immediately followed by forward motion. Generic R04 has12.77% immediate
reversals. The stronger R05 field still completes more tasks with fewer forward
moves than the generic control; raw motion count is not itself the objective.

A narrow first prototype can reuse the existing verified TemporalWarmStart
history, without implementing the reference's whole two-step pipeline:

1. For an ordinary robot whose previous selected path began with a planned turn,
   propose the next action from that complete path's shifted suffix. Require
   exact next-step position/heading, unchanged goal and a currently legal suffix.
2. Keep CGAR primary/recovery/pocket/witness paths authoritative. Reuse the
   existing monotone collision-reset closure before retaining any promise.
   Robots with no eligible promise reset to the ordinary compatible seed.
3. Replace each retained robot's seed with its selected compatible suffix and
   restrict its current candidate set to that promised first action. Retain
   all five-slot collision checks and fixed search work. Do not constrain an
   incompatible promise or output a partially finished portfolio on timeout.
4. Skip the adapter's ordinary-seed auto-rotation for these replaced seeds;
   otherwise a promised wait/forward could accidentally become a rotation.
   Preserve the real operation ID and remember the actual resulting heading.
5. Disable ordinary warm/mixed starts in the first prototype to isolate the
   constraint. Default OFF must preserve complete historical trajectories.

Meaningful tests: a completely occupied four-robot rotation cycle, a protected
seed that resets the dependent promise cycle, goal/state mismatch, no history,
wrong timestep, deadline failure, and serial/parallel action equivalence. Report
retained/reset counts. Evaluate full matched controls at1s on both maps.

This is a restricted one-step promise after a planned turn. It is not the full
reference pipeline, multi-future evaluation, or a new liveness proof. If it loses,
retain the negative result before designing a more faithful rollout transfer.
