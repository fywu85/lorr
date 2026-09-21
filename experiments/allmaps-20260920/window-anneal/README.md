# Fixed-work annealed CGAR window repair

General, default-off transfer from immutable PILOT source
`de0b7637fe16a83e824c9c8bb826382249dde933:random05/src/window.cpp`, lines327–365.
The unrelated random05 tree remains untouched. CGAR retains its complete seed,
protected first actions, task-stage oracle and own neighborhood builder.

`CGAR_WINDOW_TEMPERATURE=0` preserves greedy acceptance and its random stream.
Positive integer values are initial objective-cost units, capped at65536. Complete
legal group repairs may be accepted with probability exp(-extra_cost/temperature);
temperature cools linearly by declared iteration count. Lexicographic progress
regressions at equal primary cost may also be explored. Partial group failures
always roll back. Current cost and the best complete snapshot are tracked exactly
with integer deltas. Only paths changed since the last best are copied on updates.
All committed and retained-best costs, protected paths and collisions are checked
before a completed island returns. All iterations must finish: a deadline is a
failure even after a useful incumbent was found. No early best-so-far fallback.

First declared full-horizon comparisons: selected RANDOM01seed4, RANDOM02seed2,
RANDOM03seed0 profiles, temperature0/20/80/320, unchanged selected attempt budgets.
Four physical cores per case, strict1000ms entries,32decimalGB. Delay sampling
remains off in this initial factor so annealing has a separate measured effect.
All map-specific field and dispatch settings stay under `--trick RANDOM-0N`.
Replicate any gain and report timeout failures without accepted partial scores.

All first annealing arms lose, including RANDOM03with history off. A single
follow-up tests the interaction used by the PILOT reference: on the new delay3
RANDOM03profile(seed2), compare keep0vs14/refreshedtail and temperature0vs40
in a2x2matrix. Controls must reproduce1,982; unchanged fixed work and strict1s.
This is a new interaction hypothesis, not a claim that annealing already helped.
