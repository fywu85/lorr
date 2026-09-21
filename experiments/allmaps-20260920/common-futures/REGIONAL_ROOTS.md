# Completed regional plans as future candidates

Declared 2026-09-21T09:41:54.971619+00:00. Source697f423e; full regression and source/binary
qualification required before submission. RANDOM04/05full1000/2000steps, selected
seeds0/10, four bound physical cores, strict1000ms,32decimalGB and explicit tricks.

RANDOM04compares its selected global-pool/H20profile against regional pools at
H20/H15/H10. RANDOM05compares selected no-future control against regional pools
atH10/H15/H20. Four roots and two common continuations in each future treatment.
Distinct completed regional checkpoints are considered newest first, then global
starts fill remaining slots. Final incumbent always root0; all original regional
search and future work must finish. This adds no extra regional search or RNG
draws. Hypothesis: these candidates can compete more effectively than early global
starts against the final heavily repaired plan. Measure actual changed actions.

The initial1c8c6db5build failed the new integration regression with SIGSEGV. A
selected replay owner was scoped too narrowly and was destroyed before action
emission. Source697f423extends its lifetime through the whole planning call.
No benchmark used the failed build. Preserve the failure record and rerun the
whole suite before qualification; a partial regression pass is insufficient.
