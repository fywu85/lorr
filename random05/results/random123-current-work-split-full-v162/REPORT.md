# Current-profile search-work comparison

Completed 2026-09-22 01:52 UTC. Increasing fixed search work does not improve any selected profile. All eight attempts are retained: seven strict-valid full runs pass independent replay; RANDOM-02 I4096 fails atstep1 after1029.213ms and has no score. The three unchanged controls match all six complete trace fields.

Source162 (`4fb9498e`), identical archived inputs, planner seeds and selected explicit trick profiles. Larger budgets preserve the original first-step repair count, increasing only later work. All cases retain32workers/16physicalcores, full600/600/800-step horizons,1s entries and32GB guard.

| Instance | Iterations | Tasks | Max entry ms | Result |
|---|---:|---:|---:|---|
| RANDOM-01 | 512 | 739 | 104.655 | Full replay passes |
| RANDOM-01 | 1024 | 732 | 172.872 | Full replay passes |
| RANDOM-01 | 2048 | 731 | 367.766 | Full replay passes |
| RANDOM-02 | 2048 | 1408 | 535.540 | Full replay passes |
| RANDOM-02 | 3072 | 1402 | 725.549 | Full replay passes |
| RANDOM-02 | 4096 | — | 1029.213 | Failed; no partial score |
| RANDOM-03 | 6144 | 2646 | 705.598 | Full replay passes |
| RANDOM-03 | 8192 | 2619 | 806.147 | Full replay passes |

Keep selected739/1408/2646 and their existing work budgets. More work is not monotonically better when the optimizer changes future decisions and the task schedule. This experiment measures the final throughput effect, not a claim about optimization quality of an individual window.

[Full audit](audit.json), [exact controls](control-equivalence.json), [preserved failure](failures.json).
