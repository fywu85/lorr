# Observing regional search peaks before changing retention

The confirmed reference uses two rounds of four parallel regional searches. Each
region performs25,000 attempts at initial temperature1000ppm, then returns its
final state only when that state improves on the initial one; otherwise it restores
the initial state. Many accepted moves and rollbacks do not establish that better
intermediate plans were discarded: accepted moves include ties and losses.

V48 adds optional read-only CGAR_TEMPORAL_REGION_PEAK_AUDIT, default0. It observes
the complete scalar score after every completed attempt, tracks a strict running
maximum above tolerance1e-6, and records initial, peak, final and returned scores.
It changes no selections, reservations, RNG, acceptance rule, work count or return
policy. A timeout still throws; incomplete audit records cannot be aggregated.

Every200steps, cumulative counters cover ALL regional batches since initialization,
including the preceding199steps. They report batches, attempts, peak updates, peak
attempt positions, sums of peak/final gains over each batch's initial score, and
positive peak-minus-returned score. lost_peaks counts any batch whose peak exceeds
its returned score; lost_improvements is the subset with a peak above initial but
final no better than initial. A peak can be discarded even when final still beats
initial, so both counts matter. Scalar differences are not throughput estimates.
No selection snapshots or best-plan retention are implemented.

Regression fixtures compare audited/ordinary searches and subsequent continuations
under the same RNG, use three physically valid choices with analytically known
scores to produce temporary gains followed by losses, and inject an explicit
mid-repair timeout. Existing72 regional cases at temperatures1000/100/0 additionally
compare audited/unaudited plans, search counters, round scores and future RNG.
The protected serial/parallel adapter is exercised with auditing enabled. Build
8899144 is complete: every CGAR regression passes. The transient-score fixture
observes actual discarded peaks, and all22 source/test hashes reconstruct from
3946d33 plus the archived patch. This synthetic evidence is not a warehouse result.

The planned200-step screen has temperature1000 with auditOFF/ON and temperature0
with auditON. Both1000 trajectories must reproduce the confirmed regional prefix;
all cases must meet complete-entry deadlines and RSS limits with isolated physical
cores. After it passes, full seeds0/2 compare audited1000 versus0. Audited1000 must
reproduce the confirmed full trajectories. Temperature0 is an existing configuration,
not a new search implementation: it still consumes the existing random draw for
worsening proposals and counts equal final scores as reverted. Only full outcomes
can establish a policy gain; any useful candidate requires the remaining four seeds.

[Screen profiles](regional-peak-screen-variants.json),
[full profiles](regional-peak-full-variants.json),
[Fable review and local qualifications](fable-flow-session/turn14/).

Screen8899145 is complete and valid for all three profiles. Both1000ppm trajectories
exactly reproduce the regional reference; maximum959.70ms with auditOFF and960.32ms
with auditON. The0ppm candidate reaches953.31ms. PeakRSS5.303GB, four disjoint
physical cores each, no CPU quota. Across all1,600 batches/40million attempts in each
audited prefix, lost_peaks and discarded_gain are zero. The0ppm cumulative scalar
gain is larger, but this is not a throughput acceptance or causal savings estimate.
[Screen verification](results/regional-peak-screen-v48/equivalence.json).

Full8899147/analysis8899148 now compares audited1000ppm versus0ppm on seeds0/2:
four simultaneous full runs, four physical cores each,16cores/64GiB aggregate,32GB
processRSS cap. The1000ppm controls must match confirmed regional full hashes.
No best-selection retention was added; its warehouse value remains unestablished.

All22 frozen V48 source/test hashes match exact commit [8ba2484](https://github.com/fywu85/lorr/commit/8ba2484dd5f1b9bfc719f5f1e8f4533d7543a7bd).
