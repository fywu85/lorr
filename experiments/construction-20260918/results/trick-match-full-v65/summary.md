# TRICK: full Warehouse matching factorial

Exact source [5a8a51a](https://github.com/fywu85/lorr/commit/5a8a51a38a8355811d49595f777572a819f9d10e). All eight full cases pass independent source/binary, simulator, complete-entry, 32GB RSS, work-limit and physical-binding checks. Shared-host 5s development runs; no strict 1s certification. Four controls exactly reproduce the prior full trajectories.

| Configuration | Seed 0 | Seed 2 | Mean |
|---|---:|---:|---:|
| Lanes | 146,659 | 146,566 | 146612.5 |
| Lanes + matching64 | 147,328 | 147,204 | 147266.0 |
| Lanes + short preference | 137,985 | 137,934 | 137959.5 |
| Lanes + short preference + matching64 | 142,922 | 142,961 | 142941.5 |

Matching improves lanes-only by 669/638 tasks (+0.44573% paired mean), final-1000 by 179/121, and reduces empty work by 180,012/186,384 robot steps. Empty work per completion falls from 67.180/67.299 to 65.653/65.741. These are policy comparisons with endogenous completed cohorts, not a task-matched causal decomposition.

Matching recovers 4,937/5,027 tasks under short preference (+3.61120%) and cuts empty work by 1,329,074/1,371,770 steps, but those totals remain below lanes-only and lanes+matching. Keep short preference OFF for the best policy. The short-preferring arms still explicitly permit long-task deferral.

Best full run: **147,328**, seed 0, completed 2026-09-20T08:38:32.494327+00:00. Mean entry 315.781ms, maximum 1.011861262s, RSS 11,927,756,800 bytes, average CPU 1.858 of four reserved cores; wall 27.43 minutes. The target remains unmet: 7,467 tasks / 5.068% growth needed.

[Complete comparison](comparison.json) · [Verification](verification.json) · [Exact best configuration and trajectory hash](best-record.json).
