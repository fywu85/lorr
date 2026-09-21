# Delay-directed window repair

General default-off transfer from the immutable PILOT source
`de0b7637fe16a83e824c9c8bb826382249dde933:random05/src/window.cpp`, lines272–279.
That source compares three extra random agents on half the neighborhoods, using
current planned cost minus the initial remaining-chain potential. This transfer
keeps CGAR's existing neighborhood builder, complete seed and protected actions.
The separate random05 workspace is read-only and untouched.

`CGAR_WINDOW_DELAY_SAMPLES=0` preserves the old RNG and path choices. Positive
values (1–16) keep the initial sweep and half the later attempts uniform; the other
half select the greatest extra cost among the ordinary root and that many draws.
Subtracting the initial oriented chain potential avoids simply preferring long
tasks. Cached costs update only after a whole repair group is accepted and are
reconciled against all committed paths before returning each island. Fixed attempt
counts, bounded node search, tie objective and explicit timeout failures remain.

Declared first comparisons: selected full-horizon profiles for RANDOM01(seed4),
RANDOM02(seed2), RANDOM03(seed0), each with samples0/1/3/7, unchanged work.
Strict1000ms whole-entry limit, four disjoint bound physical cores/case,32decimalGB.
Existing instance configurations require `--trick RANDOM-0N`; no silent map dispatch.
Promote only complete verified runs and replicate any throughput gain. More work
alone lost on RANDOM01 and timed out at16384attempts/island.
