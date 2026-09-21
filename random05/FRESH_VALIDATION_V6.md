# RANDOM-05 frozen fresh-input validation V6

**The frozen 4,175 admission profile is +31.99% above NMS and +5.25% above its preceding 4,011 configuration in aggregate.** Both individual inputs improve against both references.

Protocol [1502afa3](https://github.com/fywu85/lorr/commit/1502afa3412d06e3f27d388653a4c89008fac4c8) was committed at15:56:30UTC, before generation at15:58:30.350635UTC. Candidate and baseline both use [027df4d9](https://github.com/fywu85/lorr/commit/027df4d9), build132. Only active-order cap680 and its exact fast-admission implementation differ from the uncapped4,011 recipe. All800robots remain movable; opened orders remain locked.

| Fresh input | Candidate | Previous | NMS repeat1 | NMS repeat2 | Gain over stronger NMS | Candidate mean/max ms |
|---|---:|---:|---:|---:|---:|---:|
| 50015 | 4,182 | 4,001 | 3,154 | 3,155 | +32.55% | 558.4/774.0 |
| 50016 | 4,177 | 3,941 | 3,178 | 3,149 | +31.43% | 556.5/798.8 |

All eight original full2,000-step runs pass strict1,000ms entry limits,30s preprocessing,32 decimalGB process limits and independent movement/collision/assignment/task-event replay. Allocations are16physical EPYC9354cores/32SMTworkers with verified affinity and no CPU quota. Completed and censored order waiting remains a secondary metric.

The archived candidate repeats exactly and plannerseeds0/1/2/3 score4175/4151/4171/4120. This fresh test freezes plannerseed0. Task/startseeds50015/50016 remain permanently excluded from tuning, as do50001–50012. Seeds50013/50014 remain reserved forR04.

**This validates the 4,175 profile, not the later 4,197 cutoff refinement.** Fresh4,182/4,177 scores belong to different inputs and do not replace the archived-input record. This is replication across task/start streams on one layout, not unseen-map generalization or the colleague's private instances.

[Protocol](experiments/fresh-validation-v6-protocol.json), [frozen cases](experiments/fresh-validation-v6-full.json), [source/resource/timing audit](results/fresh-validation-v6/audit.json), [independent replay](results/fresh-validation-v6/replay-audit.json), [order waiting](results/fresh-validation-v6/WAITING.md).
