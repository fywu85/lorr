# End-of-run work in the verified native matching profile

Read-only V78 best-profile replay, pickupweight5 with ordinary fairness, seeds0/2.
Both input hashes equal the separately verified V82 motion audit. Independent
handoff/boundary fixture and all100M robot-step phase totals pass. No solver rerun.

| Seed | Completed | Unfinished-task work | Empty part | Loaded part | Post-last-completion work | Tasks first admitted >=4600 that finish |
|---|---:|---:|---:|---:|---:|---:|
| 0 | 152383 | 1997613 (3.9952%) | 635503 | 1362110 | 1986804 | 3746/12528 (29.90%) |
| 2 | 152048 | 1998338 (3.9967%) | 635199 | 1363139 | 1988417 | 3731/12517 (29.81%) |

Every robot completes at least one task and there are zero idle steps. Post-last-
completion work differs from unfinished-task work because of unopened retargeting;
4902/4157 of those tail steps contribute to tasks completed by another robot.
Late cohorts count first admission only, not repeated assignment events.

This is evidence to investigate an explicitly configured-horizon admission TRICK.
It is not an estimate of recoverable work or a guaranteed task-count gain. Long
started tasks remain protected. The existing environment does not expose the
horizon, so any experiment must disclose its configured-horizon assumption and
remain behind --trick WAREHOUSE. No horizon policy has yet been implemented.
