# Local assessment of Fable turn23

Completed 2026-09-20T02:24:34.507510UTC in the persistent Fable5.1/max session.
No new source payloads, tools, tests or permission denials. The USD86.2046215
reported by the CLI is cumulative session cost, not this turn's cost.

The review incorporates the full KK factorial and the user's new authorization:
map-specific work must be labeled TRICK and activated only by
`--trick <map-instance-name>`. Generic and trick frontiers remain separate.

The first bounded trick will test NMS-derived lane directions with CGAR's
existing units (forward4/16, turn4). Extract directions by executing the native
NMS guidance constructor, verify orientation and every cell, then embed the
result separately from generic code. Require an explicit WAREHOUSE flag and
exact occupancy match. No learned-field publication may replace the static
field. Pickup quotes use the same metric from initialization. A generic run
without the flag must retain its exact reference trajectory.

This changes guidance contents, cold-start availability, publication/cache
behavior and scheduler quotes together. It is an initial whole-policy screen,
not a causal estimate of lane contents. Cache-flush and planner-only separation
arms follow only if the initial candidate merits further investigation. The
native NMS cost ratio, cheap turns, coordinate bands and remaining-potential
objective are not reproduced by this adaptation.

Corrections and cautions:

- The observed chain-cohort difference is neither a bound on CGAR's possible
  improvement nor a guaranteed 2% prize. Both subsequent congestion and the
  outstanding-task pool can change.
- Aggregate guidance-potential improvement is not physical work improvement.
  The early group probe is a tiny stratified sample, and its full run is pending.
- A holders-only pre-pass does not allocate free robots directly, but consumes
  tasks ordinary scheduling might choose. It is a joint scheduling-order
  intervention and must be labeled as such. First measure the proposed
  fresh-task opportunity; do not treat this as a clean rematching-only arm.
- Uniform unit scaling preserves behavior only when every relevant cost is
  scaled consistently. Forward1/10 with turn1 does not reproduce native NMS's
  forward20/200 with turn1. It would be another adapted ratio experiment.
- Do not assume all distances fit16bits. Retain the tested lossless wide-table
  fallback and measure memory; the field's largest edge alone is not a bound
  on shortest-path distance.
- Turning on the field at step0 can make cold-cache work exceed1s. Keep explicit
  timeout failure, screen startup, and require full runs before any promotion.

No new CGAR throughput gain has been measured.
