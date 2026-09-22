# Local-refinement comparison: partial results

Fourteen of eighteen frozen full runs pass independent replay and strict checks.
Four selected RANDOM-05 cases remain pending. All completed throughput changes lose.

| Profile | No local proposals | Five proposals | Fifteen proposals |
|---|---:|---:|---:|
| General RANDOM-01 | 647 | 642 | — |
| General RANDOM-02 | 1106 | 1090 | — |
| General RANDOM-03 | 1634 | 1616 | — |
| General RANDOM-04 | 1616 | 1564 | — |
| General RANDOM-05 | 2226 | 2160 | — |
| Selected RANDOM-04 | 2782 | 2756 | 2750 |

The RANDOM-04 five-proposal serial control also scores2756 and matches all six
full trace fields of parallel evaluation. This verifies that pair's decisions;
it does not turn the throughput loss into an improvement. Timing for both runs
is preserved in [the partial exact proof](random04-parallel-equivalence-partial.json).

Sparse general reactive controls are distinct from stronger windowed records;
the current selected739/1408/2646/2782/4302 remain unchanged. The full batch
and all nine declared exact comparisons still need completion. [Partial audit](audit.json).
