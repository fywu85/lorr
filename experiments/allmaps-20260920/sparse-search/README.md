# General search transfer across RANDOM01/02/03

Alongside the field tricks, test the same four generic profiles unchanged on
all three instances: selected control; remaining-chain ranks with unequal
linear weights; eight priority-noise50 proposals; and both. The portfolio uses
500k global candidates per proposal rather than the control4M for one proposal.
This keeps the nominal aggregate cap4M while exposing parallelism and diversity;
actual work can differ. Regional work and scheduler remain unchanged. These
are the existing general mechanisms useful on dense RANDOM and GAME, with no
instance lookup, static field, horizon rule or --trick.

Full600/600/800steps, seed0, strict1s,32decimalGB,four physical cores per case,
four concurrent profiles per instance. All previous controls must be reproduced.
No claim of universal improvement until the three full comparisons are measured.

## Measured first pass and declared follow-up

General control / chain ranks / portfolio8 / both:
RANDOM01: 611 / 607 / 613 / 603; RANDOM02: 1084 / 1080 / 1083 / 1075;
RANDOM03: 1484 / 1613 / 1439 / 1543. All full runs pass1s; all control
trajectories reproduce exactly. No universal improvement.

The next explicit trick comparisons test NMS-field control versus pickup4,
pickup8, and portfolio8 on RANDOM01/02. On RANDOM03, compare the generic1613
chain-rank control with KK/NMS fields and KK+pickup4. The lanes-off control
under --trick RANDOM-03 must reproduce the generic trajectory. Each keeps
forced-oldest admission, no known horizon and the same fixed search budget.
One full seed0 first; replicate useful winners before claiming robustness.
