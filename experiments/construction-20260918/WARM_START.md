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

## Full 5,000-step result, seed 0

Jobs **8898475/8898476** completed on two distinct reserved physical cores of
EPYC 9354 host research38, with no CPU quota and a 24 GiB total reservation.
All four runs passed all 5,000 entry samples, with zero planner/scheduler errors
or timeouts and process RSS below 32 decimal GB. Compact evidence:
[full-v19](results/warm-start-full-v19/).

| Profile | Tasks | Final 1,000 | Max entry, seconds | Wall, seconds | Peak RSS, bytes | Task age p90 |
|---|---:|---:|---:|---:|---:|---:|
| equal_compact | 107,457 | 22,002 | 0.543615 | 978.770 | 11,915,051,008 | 883 |
| warm_50000 | 108,492 | 22,137 | 0.514700 | 983.412 | 12,592,222,208 | 860 |
| warm_25000 | 104,548 | 21,370 | 0.477913 | 755.857 | 13,303,881,728 | 902 |
| warm_10000 | 94,510 | 19,371 | 0.452127 | 630.206 | 11,810,168,832 | 1003 |

Warm 50k improves this seed by 1,035 tasks (+0.96%) and keeps a small final-window
gain. It reduces loaded turns from 3,831,662 to 3,250,260, but loaded waits rise
from 2,848,136 to 2,997,693 and farther moves barely change (3,335,096 to
3,331,477). This is not a replacement for better routing. It has no meaningful
wall-time saving at the same work count in this paired matrix.

Reducing warm repair to 25k/10k gives 104,548/94,510: faster, but below the 50k
control, with worse final-window rates and outstanding-task ages. Do not promote
them as throughput improvements. Earlier cold 10k used different robot weights,
so it is not a one-variable comparison. No six-seed warm-start confirmation.

The disabled-warm control exactly matches the earlier full compact trajectory
(`actualPaths`, `actualSchedule`, `events`, `tasks`), SHA-256
`3a288626a78a96f0b8e41f4088746f6568f2e92b34b381afaff9f1bb1070348e`.
This also verifies the v18 static reverse-neighbor optimization over the full
horizon. The isolated oracle speedup remains a component measurement; different
full matrices do not establish a whole-planner speedup.
