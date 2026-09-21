# Full CITY follow-ups after8213

CITY-01: reproduce8213, then individually test pickup weight8, matching every
step instead of10, and removing forced-oldest admission (HRRN already0). This
separates three specific scheduling changes. Started/primary/recovery protection,
task rematching budget and cooldown remain unchanged.

CITY-02: reproduce adapted-field14851, transfer the CITY01 direct4/HRRN0/match64
bundle, test adapted short preference, and combine dispatch with short preference.
Unlike CITY01, the short-only case removes both HRRN and forced-oldest admission.
Full3000steps, seed0, strict1000ms,32decimalGB and four physical cores per case;
four cases in parallel per instance. Both matrices use frozen85c3f0f, binary
726eedf7. This source already reproduced the previousCITY01field control; demand
the previousCITY02control identity too. All profiles explicitly --trick their
instance. No promise, squared rank, known horizon or new motion candidate.
Throughput first; report waiting tails. No accepted partial scores.

Completed: CITY01 control8213, pickup8=8378, cadence1=8242, oldest-OFF=8227.
CITY02 control14851, dispatch15797, short-only14459, dispatch+short15604.
All eight valid strict1s and whole controls unchanged. Retain forced-oldest
admission. CITY01 next checks selected pickup8 on planner seeds2/4/6 (same
archived input, not independent task streams). CITY02 tests pickup8, pickup12,
and pickup8+cadence1 against15797, full seed0 comparisons.

The first follow-ups are complete: CITY01 pickup8 seeds2/4/6 score
8367/8374/8386 (seed0=8378); CITY02 control pickup4=15797, pickup8=16123,
pickup12=16159, pickup8/cadence1=16107. All pass1s, whole CITY02 control
unchanged. Source inspection caps pickup weight at16. Before submission, declare
CITY01 pickup8 control versus10/12/16; CITY02 pickup12 control versus16,
16+short preference (oldest admissions OFF), and12+pickup-neighborhood grouping (same64group quota).
Full3000steps, seed0, explicit trick, no known horizon. Source85c3f0f and the same binary retained.

CITY01 pickup12 and16 both reach8423 at seed0, above published8420 by3tasks.
Replicate both settings on plannerseeds2/4/6, six fully parallel cases with
four distinct physical cores each. This measures stability and may set a new
individual maximum; it is not a matched local NMS comparison.
