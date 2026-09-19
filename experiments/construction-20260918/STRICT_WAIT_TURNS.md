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
The complete independent analysis validates all eight cases and all four exact
legacy-control hashes. All maxima are below0.785seconds and RSS below11.920GB.

| Policy | Seed0 | Seed2 | Two-seed mean |
|---|---:|---:|---:|
| Frozen legacy | 122,896 | 70,171 | 96,533.5 |
| Frozen strict | 122,076 | 121,534 | 121,805 |
| Refresh512 legacy | 134,511 | 134,519 | **134,515** |
| Refresh512 strict | **135,177** | 133,672 | 134,424.5 |

Strict mode avoids the frozen seed2 collapse, but has mixed effects with refresh:
+0.495% on seed0 and -0.630% on seed2. The mean is0.067% below ordinary refresh.
The new single-run peak135177 is retained in the progress log, without promoting
strict mode as an additive improvement.

Rotation attribution is now measured. In the collapsed frozen seed2 final1,000
steps,2,025,221 rotations come from planned operations,623,966 from wait seeds,
and2,075 from protected actions. Only16,421 are tied wait-seed rotations (0.62%
of all turns). Strict mode records zero ties, but its recovery cannot be explained
as merely subtracting the counted tie actions: the whole traffic trajectory changes.
Healthy refresh legacy has only9/8 tied seed turns in the final1,000 on seeds0/2.
This supports testing plan consistency and traffic behavior; it does not uniquely
identify the original collapse's cause.

[Complete results](results/strict-wait-turn-full-v32/),
[exact controls](results/strict-wait-turn-full-v32/equivalence.json),
[rotation attribution](results/strict-wait-turn-full-v32/rotation-attribution.json).

[Build](build-provenance/v32/validation.json),
[screen](results/strict-wait-turn-screen-v32/),
[full configuration](strict-wait-turn-full-variants.json).
