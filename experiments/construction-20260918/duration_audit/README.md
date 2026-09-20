# How short can a Warehouse evaluation be?

The current evidence does not support a short prefix for ranking small solver
changes. This audit reuses completed full runs; it launches no new simulations.
Thirteen historical comparisons give40paired candidate/control effects:24from
changes active across the run and16from configured-cutoff policies. All metrics
are bound to valid full verification and exact trajectory hashes.

For the24whole-run pairs,800steps predicts only12final directions correctly,
falsely rejects5eventual winners and favors7eventual losers. Even4000steps misses
3eventual winners. All directions are stable only from the4975-step sample in
this selected history. That is an observation, not a reliability guarantee.

The16cutoff-policy pairs all have identical candidate/control task counts through
4000steps. At4975steps,4eventual losers still appear better. In the combined set,
only the full5000-step sample gets all40directions right. Never interpret an early
tie as evidence that a late policy cannot help.

Practical choice: retain800-step checks for validity, activation, resource use and
exact-default compatibility. Use full5000steps to rank these small Warehouse gains.
This does not prove that all other algorithms/instances require this duration.
No statistical confidence interval is appropriate: the pairs share one fixed
input, seeds, controls and selected development history. Changing the configured
horizon to a shorter value would be a different experiment, not a saved prefix.

The script declares two additional cases before their full results arrive:
`native-turn-full-v113` and `match-horizon-seeds13-v114`. Apply exactly the same
checkpoints to them with `--held-out` after verification. This checks new solver
cases, not new generated inputs. All currently running full cases finish regardless
of their prefixes; no early stopping or prefix-only promotion is introduced.

[Observed directions and exact curves](../results/duration-audit-v115/summary.md).
