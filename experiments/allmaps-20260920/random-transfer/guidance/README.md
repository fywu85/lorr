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
