# Read-only export of the standalone RANDOM-05 guidance field

This is preparation for a **trick**, not a CGAR throughput result. It exports the
map-tuned field15/one-flip-seed5 from frozen source233f5bf, with its average-flow,
power3, betweenness0.75, normalization and output-penalty2.4 settings. The
standalone working tree is untouched: compile copied Git blobs under a CGAR run
directory. Compare the full generated float field at1and4threads, then record
its integer adaptation atscale10with declared rounding error and wall values.
The turn cost0.6becomes6; normalized mean forward cost2becomes approximately20.

The native floats and their adaptation remain separate artifacts. Do not claim
integer equality with the original metric. Invalid-direction costs are recorded
but do not authorize moves into walls. Generation alone does not add CLI support;
any later use needs explicit `--trick RANDOM-04` / `--trick RANDOM-05`, exact map
occupancy and the corresponding700/800robot count, plus separate generic controls.


## First CGAR transfer comparison

`variants.json` declares three profiles on each archived input, planner seed0,
full 1000/2000 steps, four bound physical cores per process, shared EPYC9354,
enforced 1000 ms decisions and 32 decimal GB. All use the same eight-worker
500k-per-worker chain-rank search with fresh priority noise50. Regional budgets
and scheduling are unchanged. Compare lanes-off generic (expected old seed0
1306/2036) against a forward20/turn6 uniform control, then the integer field with
the same turn cost and pure-potential scoring. This isolates the nonuniform
field effect from the metric/scoring changes. The first difference is a bundle,
not a pure guidance effect. No known-horizon rule, short-task preference or
unopened matching is added. Fairness is reported as a secondary metric.

Use the corresponding `--trick RANDOM-04` / `--trick RANDOM-05` flag for all three
profiles, including the lanes-off control. The source remains CGAR; the standalone
RANDOM-05 tree, jobs and held-out evaluation streams are untouched. Any timeout
is a failed run, never a partial result. Seed0 and one archived input per instance
provide exploratory evidence, not a broad generalization claim.
