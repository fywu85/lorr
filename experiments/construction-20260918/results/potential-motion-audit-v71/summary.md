# TRICK: static remaining-potential movement audit

All four independently verified full V68 trajectories pass the saved-run replay, including 50 million action conservation per case, exact task/empty-work agreement, independent lane-phase-window fixtures and a wrong-field rejection. Source/input/binary hashes are retained in [accounting.json](accounting.json).

| Policy / seed | Tasks | Empty / task | Chain mean | Loaded overhead | Loaded detours | Loaded turns | Loaded waits | Against-lane forward share |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| trick_lanes_potential0-s0-r0 | 146,659 | 67.180 | 242.244 | 21.924 | 4.809 | 5.943 | 11.172 | 0.181% |
| trick_lanes_potential0-s2-r0 | 146,566 | 67.299 | 242.187 | 22.035 | 4.813 | 5.968 | 11.254 | 0.182% |
| trick_lanes_potential1-s0-r0 | 147,422 | 65.781 | 242.223 | 21.624 | 4.082 | 6.637 | 10.906 | 1.539% |
| trick_lanes_potential1-s2-r0 | 147,502 | 65.678 | 242.251 | 21.528 | 4.030 | 6.612 | 10.885 | 1.533% |

The positive full result accompanies almost unchanged completed chain length, lower empty travel and lower loaded overhead. Loaded detours and waits fall while turns rise. Against-lane moves rise from roughly0.18% to1.54% of forward moves. This is consistent with permitting useful local departures from static lane directions, but it is not a task-matched causal attribution.

Both matching and score-only policies reduce empty travel. Their gains may therefore overlap; the running combination comparison tests this directly against both exact single-component controls. No additive gain is assumed.

[Full comparison and new record](../trick-potential-full-v68/summary.md) · [Metrics](metrics.json).
