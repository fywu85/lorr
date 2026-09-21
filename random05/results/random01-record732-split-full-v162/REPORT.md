# RANDOM-01 physical-deadline gate qualification

Audited 2026-09-21T23:47:22.548932+00:00. All nine full600-step cases pass independent motion/task/waiting replay and strict1-second/32GB checks. Source `4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad`, audited build162. The gate is explicitly a known-horizon trick.

| Planner seed | Gate off | Gate on | Difference |
|---|---:|---:|---:|
| 0 | 723 | 729 | +6 |
| 1 | 727 | 733 | +6 |
| 2 | 723 | 729 | +6 |
| 3 | 723 | 727 | +4 |
| 4 | 729 | 732 | +3 |

Total3650 versus3625 (**+0.690%**); five gains, no losses. Seed4 is the original selecting pair; its unchanged729 control comes from the preceding all-five batch. Gate-on732 repeats all six full trace fields exactly (`control-equivalence.json`).

New selected **733** is **+5.92% versus matched max(NMS,KK)=692**. It finished 2026-09-21T23:35:12.917204+00:00; mean/max entry 70.908/119.027ms, peakRSS 384.614MB. An exact733 repeat is separately frozen and queued.

These are development-input planner seeds, not fresh task streams. Fresh01V1 still qualifies the earlier729 configuration only. Full settings and all outcomes are retained; throughput is primary, waiting-time distributions remain in audit.json.
