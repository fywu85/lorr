# Active CGAR throughput targets

Updated 2026-09-21T09:06:12.707401+00:00. The user now requests **10% above max(NMS, KK)** for
RANDOM-01–05 and a stretch **5% above max(NMS, KK)** for the other five instances. Integer
targets round upward. Selected-seed maxima may qualify; replication and fairness
remain separately reported. These are published historical baselines, not matched
local competitor measurements.

| Instance | NMS | KK | Stronger baseline | Margin | New target | CGAR best |
|---|---:|---:|---:|---:|---:|---:|
| WAREHOUSE | 154,795 | 152,605 | 154,795 | 5% | 162,535 | 155,173 |
| SORTATION | 152,714 | 150,739 | 152,714 | 5% | 160,350 | 150,894 |
| CITY-01 | 8,420 | 8,445 | 8,445 | 5% | 8,868 | 8,427 |
| CITY-02 | 16,787 | 16,997 | 16,997 | 5% | 17,847 | 16,315 |
| GAME | 23,274 | 22,604 | 23,274 | 5% | 24,438 | 24,447 |
| RANDOM-01 | 639 | 688 | 688 | 10% | 757 | 668 |
| RANDOM-02 | 1,221 | 1,260 | 1,260 | 10% | 1,386 | 1,215 |
| RANDOM-03 | 2,334 | 2,084 | 2,334 | 10% | 2,568 | 1,939 |
| RANDOM-04 | 2,547 | 1,405 | 2,547 | 10% | 2,802 | 2,023 |
| RANDOM-05 | 3,050 | 2,085 | 3,050 | 10% | 3,355 | 3,065 |

GAME currently meets the new +5% target. KK sets the stronger reference on CITY-01,
CITY-02, RANDOM-01 and RANDOM-02. The remaining references come from NMS.

Source: [official 2024 Main leaderboard API](https://2024.leagueofrobotrunners.org/api/leader_board?track=0&comp_id=6739efc411647c60717ae548),
retrieved 2026-09-20T23:19:53.937073+00:00. NMS has published timeout labels for
WAREHOUSE, SORTATION and GAME; other NMS rows report none.
[Frozen values](published-targets.json), [active target records](active-targets.json).

PILOT is an independent solver. Its scores do not count as CGAR results. Read its
documented mechanisms and frozen source; do not edit its code, logs, builds, jobs
or held-out inputs. All transferred instance tricks require `--trick INSTANCE`.
