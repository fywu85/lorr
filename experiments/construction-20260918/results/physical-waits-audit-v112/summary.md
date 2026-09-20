# Saved best-run physical-wait audit

Checked 2026-09-20T20:24:05.317433+00:00. GRID8900658. Read-only analysis; no new solver score.

Both full5000-step,10000-robot trajectories pass independent collision, ownership and service-event replay. All100million robot actions are checked. Nine hand fixtures pass. Raw hashes, phase totals and wait totals exactly match the earlier verified V99 artifacts.

| Observation | Seed0 | Seed2 |
|---|---:|---:|
| All waits | 1,503,671 | 1,512,799 |
| At current goal | 265 | 289 |
| Obstacle ahead | 9 | 5 |
| Free forward cell | 125,832 | 125,660 |
| Free and Manhattan-closer | 123,663 | 123,452 |
| Forward cell vacated this step | 95,542 | 95,587 |
| Occupied forward cell | 1,377,565 | 1,386,845 |
| Incoming moving blocker | 851,169 | 857,234 |
| Stationary waiting blocker | 265,848 | 268,406 |
| Rotating blocker | 260,548 | 261,205 |
| Completed-task waiter / unfinished-task blocker | 15,942 | 15,583 |

The four primary wait categories (current goal, obstacle, free, occupied) partition waits. Other rows are subsets and must not be added. The occupied forward cell accounts for91.61%/91.67% of waits; an incoming moving robot accounts for56.61%/56.67%. A rotating blocker accounts for17.33%/17.27%. These are observations under the actual joint actions, not avoidable-delay estimates.

In the last100 physical steps, completed-task waiters blocked by eventually unfinished tasks occur6730/6536times. Across the whole run there are15942/15583 such observations (0.0319%/0.0312% of all actions). Eventual completion is known only in this offline analysis and must never become a live input. These counts neither predict a gain nor bound the effect of changed priorities.

A free forward cell is only one-step feasibility under all other recorded actions. Five-step reservations, protected CGAR moves, route preferences and subsequent traffic can still require the wait. Manhattan closeness is not the native guidance metric. At-goal waits can serve a waypoint; another stationary action could also serve it.

Best scores remain155120/155056 with source27be6e312fdd79ad310583d33eeb2a4781d75b97. [Machine-readable audit](accounting.json), [fixtures](fixtures.json), [protocol and launcher failures](../../physical_waits/README.md).
