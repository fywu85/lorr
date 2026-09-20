# Persistent Fable turn40: assessment

Completed2026-09-20 11:51:48UTC, existing Fable5.1/max Claude Code session.
Design review, no independent execution. No tick-bound or comparator-placement
problem identified. The implemented V86 tier lives on each robot-task pair and
is used for shortlist retention, global ranking and fallback. Endpoint buckets
and forced-oldest comparator keep their prior ordering. All regressions pass.

The reviewer requested a saved-run impossibility/slack and alternative-stock
check before spending full runs. Corrected independent audit V86-r2 passes and
finds4024/3980 chronological task-disjoint feasible-alternative witnesses, enough
to justify the declared comparison. Its counts are neither complete nor upper
bounds; a small count would not by itself prove a policy cannot help, because
trajectories, revelation and later availability change. Likewise the user's
single-seed frontier criterion is distinct from the suggested both-seed acceptance.

A real counterexample qualified the word impossible: one completed late task per
seed had an initial holder that could not finish byH, then was retargeted and
finished on another robot. The initial audit's unconditional assertion failed;
that run and correction are retained. Every completed FINAL-holder bound passes.
Production only ranks conditional pair bounds; it never drops held tasks.

The review's relative tick wording is interpreted via actual saved events: the
last recorded completion timestamp isH=5000, after the action chosen at environment
tickH-1. Hence H-now remaining actions and equality feasible are correct. We do not
print H-1 as an event timestamp. The full verifier checks recorded terminal events.

The suggested chain/Manhattan short circuits are possible implementation work,
not correctness fixes; defer until late latency shows whether optimization is
needed. Matching can also move a feasible task to a holder with an infeasible
bound; this first trial deliberately leaves it untouched. Any effect should be
measured separately. A calibrated middle tier remains a distinct future mechanism,
not part of V86. No coefficient has been selected or promoted.
