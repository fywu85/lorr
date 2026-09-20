# Evaluation duration: observed prefix-direction errors

retrospective_development_cases; checked 2026-09-20T20:54:41.154208+00:00.

A prefix favors the candidate when its completed-task count is greater than the paired control. The full5000-step result supplies the eventual direction. Ties abstain. This is a retrospective diagnostic; no statistical reliability guarantee.

## whole_run

Pairs: 24. Earliest sampled prefix after which all remaining sampled directions agree with the final result: 4975 steps. This observed value is not a validated stopping rule.

| Prefix steps | Correct direction | Tied /no signal | Eventual winners rejected | Eventual losers favored |
|---:|---:|---:|---:|---:|
| 500 | 13/24 | 0 | 7 | 4 |
| 800 | 12/24 | 0 | 5 | 7 |
| 1000 | 11/24 | 0 | 8 | 5 |
| 1500 | 16/24 | 0 | 5 | 3 |
| 2000 | 16/24 | 0 | 5 | 3 |
| 2500 | 18/24 | 2 | 3 | 1 |
| 3000 | 18/24 | 0 | 4 | 2 |
| 3500 | 21/24 | 0 | 3 | 0 |
| 4000 | 21/24 | 0 | 3 | 0 |
| 4250 | 21/24 | 0 | 3 | 0 |
| 4500 | 20/24 | 0 | 2 | 2 |
| 4750 | 21/24 | 0 | 2 | 1 |
| 4900 | 21/24 | 0 | 2 | 1 |
| 4975 | 24/24 | 0 | 0 | 0 |
| 5000 | 24/24 | 0 | 0 | 0 |

## cutoff

Pairs: 16. Earliest sampled prefix after which all remaining sampled directions agree with the final result: 5000 steps. This observed value is not a validated stopping rule.

| Prefix steps | Correct direction | Tied /no signal | Eventual winners rejected | Eventual losers favored |
|---:|---:|---:|---:|---:|
| 500 | 0/16 | 16 | 0 | 0 |
| 800 | 0/16 | 16 | 0 | 0 |
| 1000 | 0/16 | 16 | 0 | 0 |
| 1500 | 0/16 | 16 | 0 | 0 |
| 2000 | 0/16 | 16 | 0 | 0 |
| 2500 | 0/16 | 16 | 0 | 0 |
| 3000 | 0/16 | 16 | 0 | 0 |
| 3500 | 0/16 | 16 | 0 | 0 |
| 4000 | 0/16 | 16 | 0 | 0 |
| 4250 | 5/16 | 9 | 1 | 1 |
| 4500 | 11/16 | 0 | 3 | 2 |
| 4750 | 9/16 | 0 | 2 | 5 |
| 4900 | 6/16 | 1 | 5 | 4 |
| 4975 | 12/16 | 0 | 0 | 4 |
| 5000 | 16/16 | 0 | 0 | 0 |

## all

Pairs: 40. Earliest sampled prefix after which all remaining sampled directions agree with the final result: 5000 steps. This observed value is not a validated stopping rule.

| Prefix steps | Correct direction | Tied /no signal | Eventual winners rejected | Eventual losers favored |
|---:|---:|---:|---:|---:|
| 500 | 13/40 | 16 | 7 | 4 |
| 800 | 12/40 | 16 | 5 | 7 |
| 1000 | 11/40 | 16 | 8 | 5 |
| 1500 | 16/40 | 16 | 5 | 3 |
| 2000 | 16/40 | 16 | 5 | 3 |
| 2500 | 18/40 | 18 | 3 | 1 |
| 3000 | 18/40 | 16 | 4 | 2 |
| 3500 | 21/40 | 16 | 3 | 0 |
| 4000 | 21/40 | 16 | 3 | 0 |
| 4250 | 26/40 | 9 | 4 | 1 |
| 4500 | 31/40 | 0 | 5 | 4 |
| 4750 | 30/40 | 0 | 4 | 6 |
| 4900 | 27/40 | 1 | 7 | 5 |
| 4975 | 36/40 | 0 | 0 | 4 |
| 5000 | 40/40 | 0 | 0 | 0 |

## Limits

Observed directions only, not confidence intervals or reliable stopping guarantees.
Pairs share fixed instances, seeds, controls and selected development history; they are not independent samples.
Prefixes come from full runs with their original configured horizon; this does not measure changing the horizon to the prefix length.
A 5000-step cutoff policy may first act late; identical early scores provide no evidence of no gain.
The two held-out directories were declared while their full runs were still pending. They are new solver cases, not new generated inputs.
All running full cases finish regardless of this audit; no prefix-only promotion or termination.

[Exact paired curves and input hashes](analysis.json).
