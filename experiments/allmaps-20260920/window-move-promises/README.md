# Motion commitments with the CGAR rolling window

Declared before benchmarking. The crowded temporal-only commitment experiment
improved RANDOM04 seed0 from2059 to2237; replication is pending. This separate
composition tests the same one-action occupied-cell constraint on the window
planner used for RANDOM01/02/03. It remains the CGAR protected seed plus complete
fixed-work repair, not a replacement planner or an anytime timeout fallback.

`CGAR_WINDOW_MOVE_PROMISES=1` requires an enabled rolling window and
`CGAR_TEMPORAL_MOVE_PROMISES=1/2`. A forward promise fixes its next cell. A wait
promise fixes its current cell but leaves heading free. Every bounded A* path,
retained-history candidate and final joint solution must respect it. Existing
primary/support first actions, recovery witnesses, pockets and parked robots keep
their stronger protections. Next-step promises use the selected window's second
occupied cells and actual emitted first headings. Default-off behavior is unchanged;
without the explicit composition flag the old incompatible combination still fails.

Independent layered action search checks constrained optima, plus explicit forward
and idle-cell fixtures, free idle rotation and rejection of incompatible history.
Production episodes compare complete serial/parallel actions, repeated task
services and protected CGAR recovery across both promise modes and refreshed
retained history. Full GRID regression and source qualification must precede runs.

Declared full comparisons: RANDOM01 seed4/control693, RANDOM02 seed2/control1215,
RANDOM03 seed2/control1982. Each has control, forward-only promises, forward/wait
promises, with all existing window work, field and scheduler settings unchanged.
Four physical cores per case, strict1000ms complete entry,32decimalGB and node-local
logs. Profiles retain explicit `--trick RANDOM-01/02/03`. No throughput claim yet.

All three full comparisons are now verified: RANDOM01 control693/mode1 687/
mode2 688; RANDOM02 1215/1197/1172; RANDOM03 1982/1848/1942. Every control
repeats its full selected trajectory and all entries pass strict1s. The temporal
commitment gain does not transfer to these selected rolling-window profiles.
Keep the previous sparse profiles; first-results.json records the negative result.
