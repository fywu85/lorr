# Rejected motion experiments

These mechanisms are absent from the active planner. Their full-horizon results,
including every regression, remain in the parent report. Both patches apply to
an isolated checkout of commit `83fc931`; they include the diagnostic and original
orientation-guidance support used by the experiments. Do not apply both patches.

- `portfolio-from-83fc931.patch`: four complete movement proposals, perturbed
  ordinary priorities and a one-step progress score. Production source matches
  the portfolio run manifest; the patch also includes the stronger subsequent
  regression fixtures recorded under `validation-portfolio-protection`.
- `rotation-from-83fc931.patch`: additionally reserve turning robots' current
  cells during PIBT and let parents try another move immediately. This made
  planning excessively restrictive in the tested implementation.

Build the selected isolated checkout using the repository's normal instructions,
then use the corresponding `*-variants.json` with `tools/benchmark_matrix.py`.
The active planner intentionally does not recognize `CGAR_PROPOSALS` or
`CGAR_ROTATION_RESERVATIONS`.
