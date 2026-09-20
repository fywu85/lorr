# Local assessment of Fable turn19

Completed 2026-09-20T00:14:17.860547UTC in the same session, Fable5.1/max,
no tools/tests or permission denials. USD81.30132875 is cumulative session cost.
The prompt supplied verified current accounting, active competitor source facts,
primary literature summaries and negative experiments. Production source deltas:0.

## Accepted changes to priority

Competitor ablations should precede a substantial CGAR planner rewrite. Compare
KK with supplied versus uniform weights, and with unopened-task rematching
enabled versus disabled, preferably factorially. Re-run unchanged controls under
the same allocation: KK is clock-limited and nondeterministic, so the historic
152981 alone is not a sufficient contemporaneous control. Retain the existing
score as the target. These are explanatory competitor runs, not admissible CGAR
scores; their resource and timeout outcomes must remain explicit.

The prior pool pass is not a decisive test of additive assignment combined with
broad rematching. Inspection confirms it permitted only pickup improvements,
ran32 exchanges per10ticks, and used the older HRRN policy. A shorter-chain task
could not pay for a longer pickup. This supports a distinct hypothesis, not a
presumption of success. Preserve task/primary fairness and bound released-task
externalities if a new pass is built. Price basis and rematching should be
separate arms before combining them; Fable's proposed combined implementation
would otherwise change discovery, objective, pricing and pool mobility at once.

At five slots, simply releasing a group and calling the same single-owner
constructor may mostly retest the failed extra transaction pass. A new solver
must state what new joint replacements it can find, or test a longer complete
reservation horizon. Refresh blocker and region-boundary opportunities first.

## Corrections and limits

- Supplied map weights were already explicitly included as a confound in the
  prompt. Their exact offline optimization provenance was not established by
  inspecting the filename. We can ablate the supplied nonuniform weights without
  claiming how they were trained.
- Uniform weights change KK's shared heuristics for BOTH scheduling and motion,
  and may change how much search fits its clock budget. The intervention cannot
  assign a causal fraction of the20.526-step gap to motion guidance alone.
- If KK retains its advantage with uniform weights, that does not prove planner
  structure is the cause. Assignment mix, search work, time selection and other
  differences remain. If it loses the advantage, online CGAR guidance is a
  promising alternative, not a proven replacement for the supplied weights.
- Per-chain-cell normalization controls only chain length, not route geometry,
  congestion exposure, number of legs or completion censoring. Fable's assertion
  that the motion gap is not a cohort artifact is too strong. Matched-task or
  same-state/fixed-goal comparisons are needed for a stronger claim.
- The235.265/235.270 chains and~17.0/17.6% extra empty steps belong to the old
  HRRN-off refreshed-flow runs (FLOW_REFRESH_DISPATCH.md). The17.46–17.95% figures
  describe a later cost-based shortlist experiment that still used HRRN. Do not
  merge those policies or treat either as a current-reference intervention.
- The claimed~2000 versus~28 available robots and the~5000 longest tasks left
  in the pool are estimates from earlier discussion, not measurements in this
  audit. The8.025-step cohort difference is not guaranteed recoverable.
- A cheaper alternative pickup in a historical snapshot is only opportunity.
  It does not include the released task's later pickup, congestion changes or
  competing assignment effects. Failure to exceed the old17% static gap is not
  a rigorous rejection bound for a changed closed-loop policy.
- Conflicts visible6–10steps ahead do not prove a five-slot horizon is binding.
  Prediction errors and available avoidance actions matter. Compare complete
  five/ten-step joint alternatives on the SAME states and goals if this becomes
  the selected experiment. Earliest predicted overlap alone is insufficient.
- The chain gap cannot be declared addressable merely because competitors use
  rematching. Age protection, task-pool evolution and longer-term starvation
  remain material. The SoTA goal is still unmet.

The consolidated report therefore promotes controlled diagnosis first, retains
a scheduler/rematching factorial as the next policy hypothesis, and makes the
motion architecture choice conditional on fresh evidence. It does not repeat
the regional parameter sweeps or promise a particular gain.
