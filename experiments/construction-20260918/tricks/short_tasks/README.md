# TRICK: short-task preference × warehouse lanes

User-requested on 2026-09-20. The short-task component suppresses both the HRRN
waiting-age preference and the independent forced-oldest admission. The existing
pickup-plus-chain cost, candidate coverage, work quotas, started-task assignments
and motion protections stay intact. No task is deleted or explicitly excluded by
length. Long unpicked tasks may repeatedly lose to cheaper tasks.

All four factorial arms require `--trick WAREHOUSE`. Component selectors are
`CGAR_TRICK_LANES=0/1` and `CGAR_TRICK_SHORT_TASKS=0/1`; any selector without the
CLI flag is rejected, including value0. With neither selector, the existing flag
continues to mean lanes enabled and short-task preference disabled. The 0/0 arm
is labelled TRICK as an experimental control and must reproduce the generic
trajectory. No result from this study replaces the generic frontier.

| Arm | Lanes | Short-task preference |
|---|---:|---:|
| Control |0|0|
| Short tasks |0|1|
| Lanes |1|0|
| Both |1|1|

First validate 200-step startup, exact control/legacy-lane trajectories, CLI
rejection and source/binary/component receipts. Then run all four full 5,000-step
warehouse arms at matched seeds0/2, concurrently on four reserved/bound physical
cores each. Use the user-authorized shared-host 5-second development deadline,
fixed algorithm work, explicit timeout failures and 32,000,000,000-byte process RSS.
Full totals and paired effects determine further six-seed testing; startup totals
are not a quality ranking.

Report effects of short-task preference with lanes off and on, the corresponding
lane effects, and their interaction. Also replay unpicked ages≥1,000/4,000 at
sampled checkpoints, mean/p90 ages and pending chain lengths, completed cohort
lengths, all empty travel and loaded turns/waits. These interventions estimate
what the policy does inside CGAR. They cannot assign an exact fraction of KK's or
NMS's lead: their matching, guidance, route planning and resource usage differ.
The old HRRN-off test kept forced-oldest admission and is not this factorial.

The earlier inference that starvation is only map-specific is unnecessary: the
preference itself is generic. It is classified as TRICK here at the user's
explicit request because it trades fairness for the competition objective.

V58 source [f939b3b](https://github.com/fywu85/lorr/commit/f939b3b1ab44368e83617a9f97300f26c4063210) passed
all regressions and nine CLI rejection cases. The four startup runs are valid;
control and lanes-only hashes exactly match their historical references.
[Screen](../../results/trick-short-tasks-screen-v58/factorial-verification.json).
Full replacement8899316/analysis8899317 uses32 verified physical cores on research50.
The original8899310 launch received slots but no binding, so it was cancelled;
[receipts](allocation-rejected-v58/) preserve this infrastructure failure.
[Current allocation and request](full-v58-r2/). No full quality result exists yet.

The frozen full-run accounting source also passed an independent GRID fixture
job8899330 before the live runs finished: five original action-accounting fixtures,
ownership/pickup/completion/reveal boundary counts, and per-window conservation of
revealed = completed + outstanding whole-chain distance. [Receipt](accounting-fixture-v58/verified.json).
