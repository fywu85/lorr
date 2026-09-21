# RANDOM action-price comparison

Declared 2026-09-21 before outcomes. These profiles require `--trick RANDOM-01`
or `--trick RANDOM-03`. The selected full trajectories spend 24.23% and 40.65%
of robot actions turning. That observation is descriptive; it does not establish
that turns are avoidable. The window currently charges turn20 and wait40.

Test a 2x2 change: turn20/wait40 (control), turn40/wait40, turn20/wait20,
and turn40/wait20. All guidance, CGAR protections, scheduler, window work,
and seed settings are otherwise copied from the selected profile. RANDOM-01 uses
uniform guidance and refreshed keep10; RANDOM-03 uses its KK reference field,
no retained history and delay selection3. Earlier turn40 factors used different
field/history settings, so those outcomes cannot answer this interaction.
RANDOM-02 has already had nearby price factors rejected and is not repeated.

Full runs: RANDOM-01 seed4, 600 steps, control693; RANDOM-03 seed2, 800 steps,
control1982. Four bound physical cores per case, strict1000ms complete entry,
32,000,000,000-byte process limit; node-local log spool. Frozen qualified source
311d04800c369f227407b63641c1c4f0a7102186 and chain-paid-build-v1 are used with all
new paid-chain flags absent. Controls must repeat their full selected trajectories.
No success is claimed before verification. Promote only full valid runs; any
positive result will receive a separate matched-seed comparison.

Full results: RANDOM01 control693; turn40=647; wait20=690; both677.
RANDOM03 control1982; turn40=1799; wait20=1806; both1575. All eight valid,
controls repeat whole selected trajectories, no promotion. Keep current prices.
