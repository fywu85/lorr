# Conditional pair bounds and late free-pool witnesses

Exact spatial BFS plus one service tick for repeated locations, first admissions
from recorded step4000. This bound is conditional on keeping the assigned holder.
All input hashes match the prior verified audit; all completed FINAL-holder bounds
pass. Three independent fixtures cover exact equality, unrevealed and already-
assigned alternatives, alongside the existing motion/assignment fixtures.

| Seed | Late completed | Initial pair impossible but completed after retarget | Unfinished, initial pair impossible | Unfinished, LB-feasible | Task-disjoint feasible-alternative witnesses |
|---|---:|---:|---:|---:|---:|
|0|21363|1|9285|669|4024|
|2|21290|1|9232|737|3980|

Witnesses are selected chronologically by admission step, robot and task ID. An
alternative must already be revealed, never previously assigned, and not assigned
by the actual schedule in the same batch. Choose minimum exact bound then taskID;
mark BOTH actual and alternative task IDs to avoid reuse. This is a conservative
sample of distinct witnesses on the unchanged pool, not a counterfactual rollout,
complete opportunity count, causal task gain or upper bound. Cache-miss Manhattan
in production can be weaker than these exact spatial bounds.

The first audit failed because it treated an initially impossible pair as globally
impossible despite subsequent retargeting; that failure is preserved inV86. The
corrected check distinguishes such rescues. The production rule only ranks new
pairs, leaves matching intact and never deletes a task. The measured sample is
large enough to justify the predeclared two-seed full horizon test.
