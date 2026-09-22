# Local-refinement comparison: partial results

Six of eighteen frozen full runs have completed and passed independent replay
and strict resource/deadline checks. Twelve crowded cases remain pending.

| General reactive profile | No local proposals | Five proposals |
|---|---:|---:|
| RANDOM-01 | 647 | 642 |
| RANDOM-02 | 1106 | 1090 |
| RANDOM-03 | 1634 | 1616 |

All three completed changes lose. These are reactive comparisons; the current
selected01/02/03 scores739/1408/2646 use windowed search and remain unchanged.
No runtime improvement is claimed until the declared serial/parallel controls
finish. The full independent audit and nine complete trace comparisons remain
required. [Partial audit](audit.json).
