# Regional search acceptance and discarded improvements

The CGAR temporal kernel preserves NMS's final-versus-initial repair rule: it may
visit an intermediate plan with a better local score than the final returned
plan. The existing peak audit observes this without changing RNG or selections.
Before implementing peak retention, measure its opportunity on the dense maps.

First compare regional temperatures1000ppm(control),0,100,5000 on the1801-task
RANDOM04 profile (KK forward field, uniform turn6, four rounds). Enable read-only
peak diagnostics in every arm. The audit control must reproduce the earlier
whole trajectory. This tests a general search parameter on an explicit map-trick
base, not a claim that a larger local score necessarily improves full throughput.
All four cases use full1000steps, seed0, strict1s, four bound physical cores and
32decimalGB. Keep losses. Use discarded-gain counters to decide whether any
new incumbent-retention implementation is warranted.

## Measured result and next general experiment

Full1000-step temperature1000/0/100/5000 results:1801/1768/1636/1885. The read-only audit control has the exact earlier whole-trajectory SHA256. At1000ppm, returned plans discard0.794% of the observed peak gain; at5000ppm,6.573%. Greedy acceptance does not improve throughput.

Implement a default-off regional best-plan retention option. Record improvements only at complete attempt boundaries; finish the identical prescribed attempts/candidate work before choosing the retained plan. Exceptions remain failures. Check an analytic three-score fixture, unchanged disabled trajectories, serial/parallel reservations and primary protection, then run a full controlled matrix. A higher local score is not assumed to imply higher task throughput.
