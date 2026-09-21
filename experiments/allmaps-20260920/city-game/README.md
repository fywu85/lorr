# Explicit NMS guidance transfers for CITY and GAME

The user reaffirmed on2026-09-21UTC that instance-specific tricks should be
pursued alongside general improvements. These providers require
`--trick CITY-01`, `--trick CITY-02` or `--trick GAME`, exact archived occupancy
and the corresponding1500/3000/6500-robot fleet. No automatic map dispatch.

The unchanged archived NMS `GraphGuidance` constructor was executed for both
maps, then every forward value was checked against an independent parity
formula. Both active methods use alternating horizontal/vertical preferences:
forward20, opposed200, rotation1; their commented-out narrow-passage penalties
are not active and are not copied. Wall entries are unused and normalized20.
The existing4/16 adaptation is available as a separate comparison, with its
ordinary scored edge costs. [Generation proof](generation-v1/generation.json)
records source, map, binary, generated-header and field hashes. The two city
cases share the layout and field but have distinct fleet gates.

`variants.json` predeclares four full-horizon seed0 comparisons per instance:

1. Lanes OFF: exact selected generic4M-region control with the explicit CLI gate.
2. Adapted4/16 parity field with existing guidance/temporal score units.
3. Native20/200 field and NMS pure-potential score, rotation1.
4. Native field plus explicit short-task preference: disables HRRN and forced
   oldest admission. Started assignments and CGAR motion protections remain.

The native field/score transfer is a bundle; adapted versus native is not an
isolated edge-direction comparison. No after-turn promise is enabled. Its dense
prototype lost the current controls. There is no known-horizon rule here.

Full3000/3000/5000 steps, four bound physical cores per case, shared EPYC9354,
32decimalGB and enforced1000ms decisions. Each four-profile matrix requests16
cores. Simulator errors, timeouts and memory violations disqualify a run; an
unfinished run has no accepted throughput. Source/test and actual binary hashes
must match. Compare lanes-off trajectories to the archived selected generic
controls before crediting a field result. Report waiting tails separately.

Status: CGAR integration and the full regression suite passed. Source
596b609351f4653dcb404e264edb4f20de98d7eb, binary
41aea6783c66413dfc9df6970f18bae29c7ac431b592b1a8310b986a929ff758.
New checks independently verify1544008 field values,361564 oriented distances,
22000 generic/control actions and11000 native planning actions, plus explicit
map/fleet and selector gates. [Build checks](build-v2/checks.json).
The first build failed a new fixture assertion because timestep0 deliberately
uses spatial mass dispatch; the native pickup fixture now uses timestep10.
Production code did not change for that test correction. [Failure record](build-v1-failed/failure.json).

All twelve full cases are complete and verified (CITY-01 jobs8901193/94,
CITY-02 jobs8901195/96, GAME jobs8901197/98). Every generic control reproduces
its previous complete trajectory. Each winner retains HRRN and oldest admission.

| Instance | Control | Adapted field | Native field | Native + short | Selected maximum ms |
|---|---:|---:|---:|---:|---:|
| CITY-01 |7305|**7755**|6815|6669|764.89|
| CITY-02 |14068|**14851**|14185|14265|807.04|
| GAME |6519|**10080**|6784|6758|904.65|

All strict1s, source/binary above, seed0 on the archived stream. These three
adapted profiles become selected bests. Published NMS gaps remain7.90%,11.53%,
and56.69%; published scores are not matched local controls. GAME still has an
outstanding-task age p90 of5000steps. Selection JSONs retain exact configuration,
completion timestamp, whole-control hash and resource/waiting evidence.
