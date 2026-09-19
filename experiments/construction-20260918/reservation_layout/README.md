# Reservation layout probe

A synthetic component profile using the exact current kernel found most sampled
search time inside blocker checks. This does not measure warehouse cache misses.
Fable turn16 proposed colocating each cell's five vertex slots and its two canonical
edges' five slots in one64-byte record. The existing representation uses separate
20-byte-per-cell and40-byte-per-cell arrays. The candidate increases table storage
from60 to64bytes per grid cell, while potentially avoiding separate edge cache lines.

This experiment changes only storage addressing. It preserves all stored owners,
vertex-before-edge scan order, reservation updates, scores, RNG draws and work limits.
The patch is isolated from production. Test30 alternating paired timings for each
of three synthetic modes: global cold, regional filtered, and warm unequal-weight.
All selections, score bits, search counters and independently checked full five-slot
reservations must match. Include construction/setup/destruction in timing. A useful
component result still requires production regressions, exact trajectory checks and
warehouse timing before adoption. No throughput gain is claimed.

A separate equal-owner-first patch tests one hot branch without the layout change.
The blocker aggregation returns immediately when the slot equals the already seen
owner (including both empty). Otherwise a negative slot is ignored, a distinct
second owner rejects, and the first occupied owner is retained. This is exactly the
same three-way result, with different branch order. It touches no audit classifications
or reservation scan order. It is measured separately before considering combination.

The layout-only probe8899187 completed180 observations (30pairs x3modes). Every
checksum and work counter matches. Median time ratios candidate/baseline are
1.0073 global,0.9976regional,1.0060warm: no useful speedup demonstrated. The layout
patch stays outside production. [Results](../results/reservation-layout-probe-v49/).

The equal-owner-first probe8899188 also preserves all checksums/counters but shows
no useful speedup: median ratios0.9994global/1.0052regional/1.0006warm. Neither patch
is adopted. [Branch comparison](../results/equal-owner-first-probe-v49/).
