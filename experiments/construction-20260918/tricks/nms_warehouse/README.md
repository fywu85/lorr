# TRICK: NMS-derived warehouse lane directions

This is an experimental map-specific policy. It requires **`--trick WAREHOUSE`**.
No flag means generic CGAR, even when the loaded map is the warehouse. No
environment variable enables a trick. Unknown names, another layout, an altered
warehouse obstacle, or the default/Python backend are rejected.

The field is extracted by executing the archived NMS `GraphGuidance` constructor
unchanged. The extractor verifies native E/S/W/N orientation semantics and all
280,000 forward costs, including wall cells. The generated asset embeds a lane
mask and the exact obstacle occupancy; runtime validation compares every cell,
dimensions and free-cell count. Its source, native dump, generated header and
field hashes are recorded in [asset-v2/generation.json](asset-v2/generation.json).
The first extractor rejected traversable `S`/`E` labels; that harness failure is
preserved in [asset-v1-failed](asset-v1-failed/).

This adaptation uses forward4 in the preferred direction and16 against it,
with CGAR turn4 and unchanged operation/progress units. It omits NMS's coordinate
band surcharge and cheap turns, and retains CGAR's paid forward toll. It is not
a reproduction of the complete NMS policy. The static metric is available from
initialization to the planner. V54 preserves generic pickup quotes for the mass
initial assignment at tick0, then uses static pickup quotes from tick1. Observed flow cannot
overwrite it or flush its tables.

The candidate needs temporal planning, orientation guidance, cost scale4 and
physical turn cost1, with no extra turn surcharge, alternative guide routes,
PIBT-reference backend or cache-only refresh. Use the generic reference profile
for all other settings. The benchmark wrapper propagates the flag to the actual
simulator and records it in metadata. Matrix trick profiles must have a
`trick_` prefix; a prefix alone cannot activate the feature.

Example after a verified build, using a JSON profile copied from
`../../warehouse-reference-variants.json` and named `trick_nms_lanes_4_16`:

```sh
python3 tools/benchmark_matrix.py --trick WAREHOUSE \
  --variants experiments/construction-20260918/tricks/nms_warehouse/variants.json \
  --instances WAREHOUSE --seeds 0 2 --parallel-suites 2 \
  --jobs-per-suite 1 --cpus-per-instance 4 --memory-gib-per-slot 4 \
  --output runs/trick-nms-warehouse-full-UNIQUE
```

The first comparison changes the lane contents, cold-start metric, cache
publication lifecycle and scheduler quotes together. It cannot isolate a causal
lane benefit. If promising, compare forced cache-flush and planner-only variants
as separate factors. Complete1s decisions,32-billion-byte process RSS, task and
motion validity, full5,000-step runs and all six seeds remain promotion criteria.
All commits/results stay labeled **TRICK**, with a separate frontier. The full results below are a two-seed experiment, not a six-seed promotion.

V53 status: all CGAR regressions and five negative CLI fixtures pass. The first
200-step trick screen fails explicitly at entry0 on seeds0/2 in
`pickup_flow_candidates` (1000.201/1000.189ms). No partial score is accepted.
[Failure evidence](../../results/trick-nms-warehouse-screen-v53/failure.json).
Next test: preserve the generic initial dispatch, then admit the same static
pickup metric from tick1. This is a fixed startup policy, not elapsed-time
selection or partial work.

V54 status: all regressions and both200-step TRICK screens pass at3,709/3,710tasks
versus generic2,888/2,857. Max complete entries994.717/987.554ms, peakRSS5.996/5.965GB.
These are startup-only results, not a full-run record. The benchmark now requires
a unique planner receipt with the expected field hash and matching result track;
any receipt invalidates a generic run. Full verification is next.

[Independent V54 startup verification](../../results/trick-nms-warehouse-screen-v54/trick-verification.json)
confirms all10,000initial task assignments exactly match the generic baseline in
each seed, both planner receipts/result labels are correct, and no learned field
publication occurs. Source is [abbe36b](https://github.com/fywu85/lorr/commit/abbe36b723bb43de2478c07bcb88621d5238a23a).
Full generic8899271 and TRICK8899272 are complete. Both generic trajectories
exactly reproduce144,510/144,107. TRICK reaches146,659/146,566 (+1.487%/+1.706%).
All20,000complete entries across four runs pass1s; TRICKmax984.533/980.945ms,
peakRSS11.914GB. Four physical cores per process, disjoint concurrent bindings,
exclusive EPYC9354 hosts and no CPU quota are verified. TRICKmean entries
311.7/307.7ms, averageCPU1.864/1.876cores, wall27.08/26.73minutes.

Final1000completions29,911/29,906 versus generic30,004/29,856 show no consistent
late-rate gain. Loaded turns/waits fall, while empty robot-steps increase;
these are differing task cohorts, not causal component savings. This is only
two seeds and remains unpromoted. Static lane contents, cold-start metric,
cache lifecycle and pickup quotes change together.
[Comparison](../../results/trick-nms-warehouse-full-v54/comparison.json),
[full checks](../../results/trick-nms-warehouse-full-v54/verification.json),
[generic isolation](../../results/generic-warehouse-full-v54/trick-verification.json).

Analysis8899275 first failed at its final archive copy because the old full
verifier expected `motion-analysis-submission.json`. Inputs and benchmark runs
were intact. Providing that metadata alias and rerunning only the exact frozen
analysis as8899279 completed verification. The failure and retry receipts are
preserved with the results; no benchmark was rerun or replaced. Generic analysis
8899274 passed. [Original submissions](full-v54-submissions.json).
