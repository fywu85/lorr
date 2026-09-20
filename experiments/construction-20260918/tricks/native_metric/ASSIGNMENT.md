# TRICK: native field with matching and pickup weighting

For each native field (without/with the+1bands), compare three arms: matchingOFF
with pickupweight1; matching64 with weight1; matching64 with weight5. All other
settings are fixed to the native remaining-potential policy. Fair admissions are
ON, short preference OFF. Every process requires `--trick WAREHOUSE`.

Six200-step startup cases must validate four exact prior control hashes: both
V73 native-OFF trajectories and both V75 matched/weight1 trajectories. The two
weight5 profiles add feasibility checks only. Full seeds0/2 then use twelve
simultaneous cases, four reserved physical cores each (48cores total), a shared5s
deadline and32GB process RSS. The four full native-OFF controls must reproduce V73.

Analyze with `native_metric/analyze.py --matching-study --pickup-study`. Source is
the validated V77 optimized binary c6d9755. Full results report the matching
increment and conditional weight5 increment separately. There is no weight5/OFF
arm, so no factorial interaction or standalone weight5 native effect is inferred.
All complete-entry validity, source, resource, field fingerprint and prescribed
work checks remain. Failed cases have no partial score. Native matching has
independent gain-scale, protected-cycle and closed-loop coverage. This experiment
is a measured composition of prior gains, not a claim of NMS policy equivalence.
