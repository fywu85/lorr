# Continuation screening diagnostic

These are observations from three complete, valid2,000-step runs. The diagnostic samples every100steps, fourgenerations per sample. Each trace exactly matches its pre-diagnostic counterpart; no extra futures or random draws were introduced.

| Full-run tasks | Full futures/root | Sampled generations | Two-future top-quarter winner survival | Top-eight recall | Mean lost full score |
|---:|---:|---:|---:|---:|---:|
| 3743 | 8 | 80 | 100.0% | 99.1% | 0.000 |
| 3741 | 16 | 80 | 100.0% | 97.2% | 0.000 |
| 3709 | 8 | 80 | 100.0% | 97.0% | 0.000 |

Keep the first anchor within the retention quota. Ranking uses the measured score and the solver's configured equal-score tie rule. Partial candidates are never treated as fully evaluated finalists.

This supports testing staged allocation, but does not establish a throughput gain or a statistical guarantee. Samples share one development input and evolving trajectories. The new search generates different candidate populations; discarded elite candidates can alter later generations.

The original3709 diagnostic attempt exceeded its strict deadline at the first step while printing individual scores to unbuffered stderr. The buffered diagnostic is a separate corrected build and completes at160.9ms mean,234.7ms maximum on32workers. Preserve the failed attempt.

[Machine-readable audit](audit.json), [full-trajectory equivalence](equivalence.json).
