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

## Full results and a distinct diversification check

RANDOM03 peak retention:1902->1898, leave off. RANDOM04 at1000ppm:1947->1589;
at5000ppm:1924->1999. Both disabled controls reproduce exact prior trajectories.
Thus score maximization alone is insufficient; the temperature interaction needs
replication. All cases complete below1s. Seed2/4 factorial is running.

Next compare8workers x500k(control),16x250k,32x125k,32x500k on the selected
RANDOM04/05 profiles. The first three have the same nominal4Mglobal candidate
cap sum; complete construction and final attempt overshoot mean actual work need
not match. Regional work, scoring and scheduler stay fixed within each map. All
workers use the unchanged CGAR kernel and independent construction-priority
perturbations, with four actual execution threads. This is seed diversification,
not the standalone solver's depth8 multi-future look-ahead. Use full horizons,
strict1000ms, and measured work/latency. No assumptions about throughput gains.

## Motion diagnosis of the temperature interaction

The complete movement audit shows why a better local score cannot be promoted
without measuring tasks. These counts cover actions after pickup.

| Temperature / retention | Tasks | Forward | Rotation | Wait | Forward moves away from goal | Expired commitments |
|---|---:|---:|---:|---:|---:|---:|
| 1000 / 0 | 1947 | 37.7% | 36.0% | 26.2% | 29.6% | 392 |
| 1000 / 1 | 1589 | 34.5% | 37.1% | 28.4% | 32.5% | 962 |
| 5000 / 0 | 1924 | 37.4% | 36.8% | 25.9% | 29.7% | 417 |
| 5000 / 1 | 1999 | 37.9% | 36.6% | 25.5% | 29.1% | 206 |

At the colder setting, retention is associated with more turning, waiting and
backtracking, plus more expired commitments. At the warmer setting, it improves
the measured run. These are outcome associations, not an isolated causal test.
Primary/recovery/commitment/pocket contexts overlap; their direct action counts
do not measure downstream blocking. No safeguard is removed on this evidence.

The next structural candidate remains a task-chain-conditioned orientation
potential or a coherent multi-step policy. It must evaluate known errands across
arrival headings and simulate actual service order. Adding a spatial tail constant
or the already-failed one-next-errand surrogate does not implement that idea.
[Exact counters](motion-diagnosis.json).
