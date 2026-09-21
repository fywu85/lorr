# Complete joint forecasts after retained prefixes

Declared 2026-09-21T09:17:03.345665+00:00. Build4154b482mustpassfullregression
and source/binary qualification before submission. Full archived600/600/800steps,
selected seeds4/2/0, four bound physical cores, strict1000ms and32decimalGB. Existing
instance guidance/scheduler tricks stay explicitly gated. Separate PILOT tree
and its jobs/builds are untouched.

The current window initializer truncates a compatible old path atKEEPand adds
waits, then compares it with a fully projected fresh seed. This can reject useful
history because its unfinished tail incurs avoidable delay. A default-off option
now completes the joint tail afterKEEPwith temporal-PIBTchunks. The kept prefix
and all fixed protected paths stay immutable. Pose/task/service/permission checks
and monotone conflict resets happen first; the entire joint candidate must still
beat or tie the fresh seed by the existing objective. No partial-result success.

RANDOM01/02compare their exact selected profile with refreshed keeps6/10/14.
RANDOM03compares its selected keep0profile, old keep14, refreshed keep14andkeep6.
The separate history RNG does not alter fresh projection or LNS worker seeds.
Counts report all actual complete history chunks, including rejected forecasts.
This is not a change to real primary/recovery actions or a motion pipeline.

RANDOM01keep10refresh improves all three tested seeds:685/681/693versus671/668/682,
+1.88%aggregate. Keep6/14on seed4score665/685versus682control. RANDOM02all refresh
variants lose to1215. RANDOM03oldkeep14=1849, refreshedkeep14=1933/keep6=1924,
allbelow selectedkeep0=1939. The retained tail issue is real for some profiles,
but the option is not enabled globally. Next declared RANDOM01seed4comparison:
current4096iterations/group4 versus8192/16384iterations and group8at4096.
All fixedcompletework, samefourcores/strict1s; failures remain failed.
