# Explicit squared rank weights from NMS

NMS `Solution/Planner/PIBT/pibts.cpp:710` squares the linear priority-rank
weight on GAME and RANDOM-04/05. The frozen reference SHA256 is
dc3bf5a5d2c6af7881aa82f2a01f25da8f651739e71995c82ca926874b4d7357.
CGAR now exposes this through `CGAR_TRICK_RANK_SQUARED=1`, requiring an explicit
matching `--trick` name. OFF preserves ordinary weights and consumes no RNG.
The common objective is used by all global workers, regional and branch repair;
protected primary/recovery paths and candidate geometry are unchanged.
This is an experimental throughput preference, with no starvation guarantee.

Declared full seed0 tests after the regression build passes:

* RANDOM-04: selected generic1503 with lanesOFF and its identical matching
  selector expressed as `CGAR_TRICK_UNOPENED_MATCH`. Compare linear vs squared.
  Whole control trajectory must match1503 before attributing an effect.
* RANDOM-05: selected field/matching profile, linear2574 seed0 vs squared.
  Selected cross-seed maximum2608 remains the record to beat.
* GAME: adapted-field equal-weight10080 control; linear current-goal ranks;
  squared current-goal ranks; squared remaining-chain ranks. Linear vs squared
  isolates the exponent; the final case separately changes ordering.

All1000/2000/5000steps, strict1000ms,32decimalGB, four bound physical cores per
case on shared EPYC9354. Matrices run cases concurrently on disjoint cores.
The separate RANDOM-05 solver is untouched. No known horizon or short-task
selector is added. Complete runs and activation/resource/waiting evidence only.
