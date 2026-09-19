# Strict improvement for wait-seed turns, v32

The temporal adapter can rotate an ordinary waiting robot without changing its
occupied cell. Its legacy tie rule prefers rotation when the new and current
headings have equal remaining cost. A blocked diagonal-goal fixture reproduces
an eight-step alternating rotation cycle. Optional
`CGAR_TEMPORAL_STRICT_WAIT_TURNS=1` keeps the current heading on such ties,
while retaining every strict orientation improvement and preserving protected
actions. It defaults to zero; invalid values and non-temporal use are rejected.

Diagnostics separate wait-seed rotations, tied wait-seed rotations, planned
rotations and protected rotations. This distinction prevents attributing every
observed turn to the postprocessing rule.

Build **8898635** passes all regressions. New independent fixtures reproduce
128 tied rotations under four rotations and four cost scales, prevent all 128
in strict mode, and retain 16 useful turns. Production fixtures validate 9,600
serial/four-worker actions with warm reuse, refreshed flow, protection and
independent conservation of all executed rotation categories. The archived
source patch reconstructs all requested hashes.

Screen **8898636** passes all four 200-step cases. The default flow control
exactly matches the old screen fingerprint. Strict mode records zero tied seed
rotations; its refresh0/512 prefixes match before the first publication. All
max entries are below 0.790 seconds. These screens are feasibility checks.

Full **8898637 / 8898638** compares refresh0/512 crossed with strict0/1 at
seeds2 then0 on exclusive research46, four physical cores per instance, two
instances, 24 GiB allocation, 5,000 steps and one-second complete decisions.
At the resumed checkpoint, seed2 simulator summaries are complete:

| Seed2 | Legacy ties | Strict improvement |
|---|---:|---:|
| Frozen flow | 70,171 | 121,534 |
| Refresh512 | 134,519 | 133,672 |

Those four summaries pass timings and RSS. Full independent trajectory analysis
and seed0 comparisons are still pending. The first seed suggests a large recovery
for frozen flow, but no extra throughput gain over periodic refresh. Avoid treating
one changed trajectory as proof of the original collapse's unique cause.

[Build](build-provenance/v32/validation.json),
[screen](results/strict-wait-turn-screen-v32/),
[full configuration](strict-wait-turn-full-variants.json).
