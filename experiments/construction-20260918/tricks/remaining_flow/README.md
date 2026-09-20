# TRICK: remaining-potential score with static Warehouse lanes

The native NMS macro score uses remaining distance, terminal-wait orientation
choices and goal-service rewards without paying for traversed contraflow extras.
This experiment tests that structural difference inside CGAR. It does not copy
NMS's numeric cost metric, cheap turns, bands, scheduling or search budget.

Activation requires `--trick WAREHOUSE` and `CGAR_TRICK_REMAINING_FLOW=1`.
Lanes must be ON, short preference and matching OFF. Generic remaining-flow keeps
its original separate selector; using that generic selector under TRICK fails.
Guide, next-errand, paid-progress audits and branching remain incompatible.
No environment setting alone activates a trick.

The static field stays forward4/opposed16/turn4 with the same field hash. Only the
paid-forward-extra term in macro scoring is removed. Ordinary fairness, scheduler
parameters, cache algorithms and fixed search work stay fixed; later trajectories,
cache contents, memory and completed cohorts can of course diverge.

Source [0f4183f](https://github.com/fywu85/lorr/commit/0f4183fcdafcaab391ace632f70a0c513caba14b),
[exact build binding](../../build-provenance/v68/exact-source-commit.json).
All regressions pass, including101,079 independently replayed static-field macros,
386changed rankings,43,725zero-surcharge macros, integrated activation, incompatible
component rejection and unchanged generic scoring tests. Benchmark receipts reject
wrong components, absent CLI activation and wrong field hashes; legacy receipts
normalize the absent new component to OFF only.

The [200-step screen](../../results/trick-potential-screen-v68/comparison.json)
is valid; the disabled arm exactly reproduces3,709tasks and the previous full
trajectory prefix. Enabled3,805 is not a full-run gain. Shared5s development,
maximum1.011857s in the control, RSS6.024GB. Full0/2 comparison is running under
8899413/8899414, with the complete5,000-step horizon and exact controls.

`analyze.py` independently freezes and checks source/test hashes, binary, actual
physical bindings, complete entries, RSS, simulator validity, exact component/field
and score receipts, absence of matching, and prescribed global/regional work.
Full reports compare totals, final1,000steps, ages, empty work, loaded turns/waits.
Aggregate movement changes are not task-cohort-matched causal savings.
