# Current Warehouse work decomposition

The refreshed replay covers both verified cutoff-guard seeds (155173/155090), all100million robot actions. It establishes no new score. Exact raw hashes, production phase/action counters and independent unfinished-work totals agree; all five hand-counted accounting fixtures pass.

The earlier large loaded-motion deficit has mostly closed. Current CGAR averages only3.00extra forward cells,5.65turns and7.27waits per completed task. Together those loaded overheads total15.93steps, compared with16.75in the saved NMS diagnostic. CGAR completes spatial chains averaging7.67cells longer in these different completed cohorts. The comparison is descriptive: the saved NMS run had one timeout and exceeded32GB, so it is not a qualifying competitor score.

| Component, robot steps per completion | CGAR pooled seeds0/2 | NMS saved diagnostic | CGAR minus NMS |
|---|---:|---:|---:|
| Shortest spatial chain of completed tasks | 242.465 | 234.794 | +7.671 |
| Extra forward distance on completed loaded tasks | 3.004 | 3.032 | -0.028 |
| Turns on completed loaded tasks | 5.655 | 5.802 | -0.148 |
| Waits on completed loaded tasks | 7.271 | 7.920 | -0.649 |
| All empty travel, turns and waits | 60.104 | 62.021 | -1.917 |
| Loaded work on unfinished tasks | 3.809 | 8.672 | -4.863 |
| Unassigned work | 0.000 | 0.379 | -0.379 |

All empty work is18.65%of the action budget; loaded detours, turns and waits on completed tasks together are4.94%. Necessary turns, congestion-avoiding waits and useful detours are included. These figures are accounting partitions, not independently recoverable savings or a throughput ceiling. The shortest completed-chain component alone is75.23%of actions.

For further work, keep assignment quality and early distance coverage in scope, and measure specific motion opportunities before increasing search work. The queued prewarm comparison tests early coverage without changing initial dispatch. Turn-price trials test whether fewer rotations help overall traffic; fewer turns alone has already proved insufficient on one seed. Upper-quantile trials test the final cutoff tradeoff without removing fairness protections.

The initial-versus-final-holder pickup distances are different snapshots with different task holders and timing; their difference is not an isolated causal reassignment saving. Future task completion is used only to label this finished replay, never as planner input.

[Exact accounting and full trajectory hashes](accounting.json), [seed0 decomposition](trick_matchguard1-s0-r0.json), [seed2 decomposition](trick_matchguard1-s2-r0.json), [frozen analysis provenance](provenance.json).
