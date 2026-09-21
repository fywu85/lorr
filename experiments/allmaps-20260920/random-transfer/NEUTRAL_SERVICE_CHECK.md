# Dense native service-tail check, predeclared

Fixed source aa63d48, binary9fd0c126..., four bound physical cores per case,
full1000/2000-step seed0 episodes,1000ms and32decimalGB. Explicit named trick
activation; other agent source/build/jobs untouched. Existing fully tested
`CGAR_TRICK_NATIVE_NEUTRAL_TAIL=1` is the sole changed setting in each pair.

The native score normally gives a service credit based on the last horizon slot
that visits the current goal. This option gives the same terminal credit to any
actual goal hit in the complete physical footprint, so leaving a served goal
within the proposed window does not lose that credit. It does not simulate the
next task, implement the reference's whole-chain dynamic program, free later
reservations, or drop a current task. All protected CGAR paths remain.

RANDOM-04 uses the field +direct-pickup4 control1481; RANDOM-05 uses field +direct
pickup4 +matching64 control2574. Promises and short preference OFF. Compare full
control trajectories with the previous verified cases before accepting results.
This is a map-specific native scoring ablation, not a claimed general gain.
All four full runs are verified; leave selected profiles unchanged.

The option loses both pairs: RANDOM-04 1481->1398 (-5.60%); RANDOM-05
2574->2542 (-1.24%). All four runs passed strict1s and32decimalGB, with
maximum171ms. The two controls reproduce previous complete trajectories.
The new score actually changed669231 /1576648 served candidate scores in the
respective full runs; this was not a disabled/vacuous ablation. Keep it OFF.
[Activation, paired results and source proof](neutral-service-summary.json),
[RANDOM-04](../results/random04-neutral-service-full-v1/summary.md),
[RANDOM-05](../results/random05-neutral-service-full-v1/summary.md).
