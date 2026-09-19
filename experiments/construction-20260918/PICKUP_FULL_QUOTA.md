# Larger complete pickup-field quota with parallel construction

This tests the existing complete pickup-field quota at64,128 and256, each with16
field-construction threads on16 reserved physical cores. Motion search remains one
complete4M worker with four preparation threads; all other confirmed graded-flow
and age-aware assignment settings remain unchanged. Surcharge, warm/mixed search,
regional repair and the post-service audit stay disabled. The initial screen used frozen V45, exactc339e8b. That build accepts quotas only
through64, so larger quotas need a separate source change before they can be tested.

The earlier0/16/32/64 quota experiments were nonmonotone:32 collapsed on one seed,
and64 improved all six totals over16 and0. These new larger quotas are hypotheses,
not an extrapolated gain. Their16-thread64 control must exactly reproduce the
confirmed four-thread64 trajectory. This is pickup-field construction parallelism;
it does not add global planning workers or change the orientation-table build quota.

The200-step screen checks exact control trajectory, physical CPU binding, actual
field-construction quota/thread counters, all entry deadlines and process RSS below
32decimalGB. It is not a quality ranking. Feasible profiles proceed to matched full
5,000-step seeds0/2; any useful candidate needs the remaining four seeds. Keep complete
fixed work and explicit timeout failure. Record total tasks, final1,000 tasks,
outstanding ages, empty and loaded actions, CPU, latency and RAM.
[Configuration](pickup-full-quota-variants.json).

Screen8899044 completed with one valid case and two explicit initialization failures.
Quota64 with16 field threads exactly reproduces the confirmed200-step trajectory;
max892.02ms. Quotas128/256 exit125 before planning with the existing0–64 validation
limit. These are unsupported-configuration rejections, not timeout, memory or quality
results. The initial submission mistakenly assumed a larger supported range. Preserve
this failed screen; raise the generic bound with quota/assignment regression coverage
and rebuild before rerunning the proposed experiment.
[Screen evidence](results/pickup-full-quota-screen-v45/).

V46 build8899056 now raises only the generic validated quota ceiling from64 to256.
Default values and the scheduling algorithm are unchanged. All CGAR regressions
pass, including actual257-robot fixtures at64/128/256 fields with serial/four-thread
construction, unique nearest assignments, an unselected fallback robot and intact
simulator metadata. Quota257 remains explicitly invalid. The63-line source/test patch
reconstructs all22 frozen hashes fromc339e8b. [Build](build-provenance/v46/).
Screen8899059 reruns all three proposed quotas on16 physical cores per process;
no full-run quality claim yet.

Corrected screen8899059 is complete: all three quotas accepted and valid, maximum
887.55ms, peakRSS5.229GB, disjoint16-core bindings and no CPU quota. Every complete
200-step trajectory is identical to the64-field reference. This does not demonstrate
that64 binds or that larger quotas help. [Assertions](results/pickup-full-quota-screen-v46/equivalence.json).

Full seed0 jobs8899064/8899065 and seed2 jobs8899066/8899067 now compare all three
quotas. Each matrix runs three concurrent16-core cases (48cores/48GiB aggregate),
with the same32decimalGB per-process cap. Full controls must exactly reproduce the
reference before interpreting any effect or equivalence.
