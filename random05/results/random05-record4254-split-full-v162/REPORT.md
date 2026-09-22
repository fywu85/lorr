# RANDOM-05 flow80 qualification

All seven new full2000step runs pass independent replay and strict resource/deadline checks.
The4254repeat matches all six complete trace fields. Source4fb9498e/build162.

| Planner seed | Flow20 baseline | Flow80 candidate | Difference |
|---|---:|---:|---:|
| 0 | 4242 | 4254 | +12 |
| 1 | 4028 | 4189 | +161 |
| 2 | 4183 | 4252 | +69 |
| 3 | 4128 | 4253 | +125 |

Totals: 16948 versus16581, **+2.2134%**, positive on all four seeds.
The candidate mean is4237.0. Versus the earlier4197profile four-seed total16687,
the same candidate total is+1.5641%. This is development-input planner-seed evidence;
no fresh-input or unseen-layout qualification follows. Selectingseed0is included.

Largest entry across these seven runs: 919.594ms; peak RSS494.387MB.

[Full audit](audit.json), [exact repeat](control-equivalence.json), [paired evidence](paired-seeds.json).
