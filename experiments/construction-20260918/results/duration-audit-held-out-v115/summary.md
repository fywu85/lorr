# Evaluation duration: observed prefix-direction errors

predeclared_new_cases; checked 2026-09-20T21:27:02.256558+00:00.

A prefix favors the candidate when its completed-task count is greater than the paired control. The full5000-step result supplies the eventual direction. Ties abstain. This is a retrospective diagnostic; no statistical reliability guarantee.

## whole_run

Pairs: 4. Earliest sampled prefix after which all remaining sampled directions agree with the final result: 5000 steps. This observed value is not a validated stopping rule.

| Prefix steps | Correct direction | Tied /no signal | Eventual winners rejected | Eventual losers favored |
|---:|---:|---:|---:|---:|
| 500 | 2/4 | 0 | 1 | 1 |
| 800 | 3/4 | 0 | 1 | 0 |
| 1000 | 2/4 | 0 | 1 | 1 |
| 1500 | 3/4 | 0 | 1 | 0 |
| 2000 | 4/4 | 0 | 0 | 0 |
| 2500 | 3/4 | 0 | 1 | 0 |
| 3000 | 4/4 | 0 | 0 | 0 |
| 3500 | 4/4 | 0 | 0 | 0 |
| 4000 | 4/4 | 0 | 0 | 0 |
| 4250 | 4/4 | 0 | 0 | 0 |
| 4500 | 4/4 | 0 | 0 | 0 |
| 4750 | 4/4 | 0 | 0 | 0 |
| 4900 | 4/4 | 0 | 0 | 0 |
| 4975 | 3/4 | 0 | 0 | 1 |
| 5000 | 4/4 | 0 | 0 | 0 |

## cutoff

Pairs: 2. Earliest sampled prefix after which all remaining sampled directions agree with the final result: 5000 steps. This observed value is not a validated stopping rule.

| Prefix steps | Correct direction | Tied /no signal | Eventual winners rejected | Eventual losers favored |
|---:|---:|---:|---:|---:|
| 500 | 0/2 | 2 | 0 | 0 |
| 800 | 0/2 | 2 | 0 | 0 |
| 1000 | 0/2 | 2 | 0 | 0 |
| 1500 | 0/2 | 2 | 0 | 0 |
| 2000 | 0/2 | 2 | 0 | 0 |
| 2500 | 0/2 | 2 | 0 | 0 |
| 3000 | 0/2 | 2 | 0 | 0 |
| 3500 | 0/2 | 2 | 0 | 0 |
| 4000 | 0/2 | 2 | 0 | 0 |
| 4250 | 0/2 | 2 | 0 | 0 |
| 4500 | 1/2 | 0 | 1 | 0 |
| 4750 | 1/2 | 0 | 1 | 0 |
| 4900 | 0/2 | 0 | 1 | 1 |
| 4975 | 1/2 | 0 | 1 | 0 |
| 5000 | 2/2 | 0 | 0 | 0 |

## all

Pairs: 6. Earliest sampled prefix after which all remaining sampled directions agree with the final result: 5000 steps. This observed value is not a validated stopping rule.

| Prefix steps | Correct direction | Tied /no signal | Eventual winners rejected | Eventual losers favored |
|---:|---:|---:|---:|---:|
| 500 | 2/6 | 2 | 1 | 1 |
| 800 | 3/6 | 2 | 1 | 0 |
| 1000 | 2/6 | 2 | 1 | 1 |
| 1500 | 3/6 | 2 | 1 | 0 |
| 2000 | 4/6 | 2 | 0 | 0 |
| 2500 | 3/6 | 2 | 1 | 0 |
| 3000 | 4/6 | 2 | 0 | 0 |
| 3500 | 4/6 | 2 | 0 | 0 |
| 4000 | 4/6 | 2 | 0 | 0 |
| 4250 | 4/6 | 2 | 0 | 0 |
| 4500 | 5/6 | 0 | 1 | 0 |
| 4750 | 5/6 | 0 | 1 | 0 |
| 4900 | 4/6 | 0 | 1 | 1 |
| 4975 | 4/6 | 0 | 1 | 1 |
| 5000 | 6/6 | 0 | 0 | 0 |

## Limits

Observed directions only, not confidence intervals or reliable stopping guarantees.
Pairs share fixed instances, seeds, controls and selected development history; they are not independent samples.
Prefixes come from full runs with their original configured horizon; this does not measure changing the horizon to the prefix length.
A 5000-step cutoff policy may first act late; identical early scores provide no evidence of no gain.
The two held-out directories were declared while their full runs were still pending. They are new solver cases, not new generated inputs.
All running full cases finish regardless of this audit; no prefix-only promotion or termination.

[Exact paired curves and input hashes](analysis.json).
