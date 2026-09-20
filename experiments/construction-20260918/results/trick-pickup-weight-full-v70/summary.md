# TRICK: pickup emphasis with static lanes and matching64

All eight full cases pass source/binary, complete-entry, simulator, memory, physical-binding and fixed-work checks. All four weight1 controls exactly reproduce V65. Frozen source [5a8a51a](https://github.com/fywu85/lorr/commit/5a8a51a38a8355811d49595f777572a819f9d10e). Explicit `--trick WAREHOUSE`, shared5s development; no strict1s certification.

| Short preference | Seed | Weight1 | Weight5 | Difference | Final1,000 change | Empty-work change |
|---|---:|---:|---:|---:|---:|---:|
| OFF | 0 | 147,328 | 147,457 | +129 | +70 | -173545 |
| ON | 0 | 142,922 | 146,873 | +3951 | +949 | -1138443 |
| OFF | 2 | 147,204 | 147,495 | +291 | +176 | -177039 |
| ON | 2 | 142,961 | 146,820 | +3859 | +902 | -1122254 |

With ordinary fairness, weight5 improves paired mean0.14260%, both late windows and both outstanding-age p90 values. Empty steps per completion fall from65.653/65.741 to64.419/64.411. This is a useful small component gain; its highest score147,495 remains below the overall147,502 record.

With short preference, weight5 recovers2.73189%, reduces empty work by over1.12million robot steps on each seed, and reaches146,873/146,820. This remains below ordinary fairness and exhibits a lower late completion rate; keep short preference OFF. The result does not reproduce NMS scheduling or isolate a fraction of its lead.

All40,000 entries completed. Maximum entry 1.013373009s, peakRSS 11,942,227,968bytes. A later combination may test weight5 with the successful score/matching policy; additivity is not assumed.

[Comparison](comparison.json) · [Verification](verification.json).
