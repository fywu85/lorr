# Work remaining under the minimal horizon rule

Both saved ON runs are verified against their complete V86 trajectories and V89 motion-input hashes. Independent handoff/boundary accounting passes, covering100million robot-steps.

| Seed | Baseline unfinished-task work | H=5000 unfinished-task work | Change | ON empty / loaded unfinished work |
|---|---:|---:|---:|---|
|0|1997613|1632309|-365304|557171 /1075138|
|2|1998338|1655252|-343086|569917 /1085335|

ON post-last-completion work is1629213/1653236; it differs from unfinished-task work because task handoffs can finish on another robot. For first admissions at recorded step4600 or later,4603/13707 and4555/13643 finish (33.58%/33.39%), versus baseline3746/12528 and3731/12517 (~30%). These are changing cohorts and trajectories, not a causal partition or recoverable-work estimate.
