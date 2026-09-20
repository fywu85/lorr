# TRICK: remaining-potential score with bounded matching

The two full single-component policies improve lanes-only on seeds0/2:
remaining-potential147,422/147,502; matching64 147,328/147,204.
This tests their combination against both controls, without assuming additivity.

Sourcec696d5f relaxes only the explicit static-lane interaction guard. Generic
remaining-flow with matching still fails; short preference stays OFF. The full
regression suite passes, including actual beneficial cycles under both scoring
modes, primary protection, unchanged simulator metadata, one-retarget protection
and a valid post-swap plan. Existing score/distance and matching-optimum tests pass.

Three arms use identical4/16/4fields and prescribed work: score-only, matching-only,
and both. First200steps verify both exact prior control prefixes; then all5,000steps
on seeds0/2, four reserved physical cores per case, shared5s/32GB. `--trick WAREHOUSE`
is required. Full controls must match V68/V65 respectively. No generic claim.
