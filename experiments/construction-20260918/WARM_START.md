# Reusing a complete prior plan

Build v19 adds optional `CGAR_TEMPORAL_WARM_START=1`, disabled by default. It
removes the already executed first action from each prior five-action plan and
appends a wait. All 129 operations have a matching suffix in the existing pool;
there is no new map-dependent operation or route template.

Reuse requires consecutive simulator steps, identical expected positions and
orientations, an unchanged goal, and a currently admissible candidate. Primary,
recovery and other protected paths retain their ordinary baseline seed. When a
seed conflicts with a retained suffix, that robot resets to its seed too. This
monotone closure visits each reset robot once. Original suffixes are mutually
compatible, as are the baseline seeds; closure removes cross-set conflicts. The
complete constructor independently validates the resulting reservations.

The usual global construction and all prescribed repair attempts follow. Every
worker joins before selection, final five-step collision validation remains
mandatory, and a deadline overrun fails the whole decision. No search result is
selected by elapsed time. History stores only a fully validated selected plan and
is cleared during initialization; later state mismatch rejects reuse.

The complete regression suite passes. Independent fixtures verify all 129
operation suffixes, reuse of a three-robot convoy, a two-robot cascading reset
from a protected seed, changed-goal and filtered-candidate resets, skipped steps
and state/orientation mismatch rejection, propagated timeout, and unchanged
history after failure. Production episodes check primary/pocket/capacity safety,
9,600 matching regional actions with and without frozen learned flow, and 1,920
matching serial/parallel portfolio actions. Counters prove reuse is nonzero.
The archived patch reconstructs every tested source hash.

All four 200-step deadline screens pass. Maximum entry times are 0.506 seconds
for the unchanged 50k control and 0.488/0.485/0.454 for warm starts with
50k/25k/10k fixed repair attempts. The disabled option preserves every prior
trajectory field. At step 200 the 50k warm profile retains 9,434 suffixes, after
263 initial resets and 303 cascading collision resets. These numbers establish
feasibility and real reuse, not full throughput gains.
[Screen evidence](results/warm-start-screen-v19/),
[default equivalence](results/warm-start-default-equivalence.json).

Full job **8898475** and analysis **8898476** follow the current guidance matrix.
It runs two single-core instances in parallel within 24 GiB total memory. The
unchanged control also checks full-horizon equivalence after the v18 oracle
optimization. The pending temperature matrix follows these jobs; no running
experiment was interrupted. Full 5,000-step throughput, memory, late deadlines
and multiple-seed confirmation remain outstanding.
