# Fifth fresh-input validation: the 4,011 candidate

Status: all four candidate/baseline runs finished; NMS repetitions are still running.
No complete validation conclusion yet.

The first independently replayed archived run above 4,000 is frozen at **4,011**.
Protocol [e46f57b](https://github.com/fywu85/lorr/commit/e46f57b) was committed at
2026-09-21 03:56:06 UTC. Inputs were generated at 03:56:45 UTC, after that commit.
Source a2ff2b2 / build-v80, planner seed 0, first K7968 then K16320, B18,
screen 2 / keep 4, G4/E8/P8, move bias 3, triage scale 1.25 and directional mix 1
remain fixed. Guidance and the known horizon require `--trick RANDOM-05`.
The previous 3,990 configuration differs only in directional mix (0.75).

The new task/start seeds are 50009 and 50010. Each gets this candidate, the
3,990 baseline, and two unchanged NMS32 repetitions. All eight jobs use
16 physical EPYC9354 cores / 32 workers, 2,000 steps, strict 1-second entry
and 30-second preprocessing limits, and the 32 decimal GB process guard.
Actual affinity and CPU quota are checked; shared hosts are allowed.
Input generation uses the unchanged archived helper and its documented RNG
seed offsets. Hashes and generation metadata are retained. These are new
starts/tasks on the same map, not new geometry or the colleague's private data.

The criteria were fixed before generation:

- An archived repeat must reproduce all six action/schedule/event/task fields
  and at least 4,000 tasks, with every timing/resource check passing.
- On each fresh input the candidate must beat the stronger NMS repetition.
- Across both fresh inputs it must exceed the immediately previous 3,990
  configuration's task total. This measures the selected refinement, not only
  the already established advantage over NMS.
- Independently replay every successful run, preserve every failed attempt,
  and report latency, RAM, and completed/unfinished order waiting statistics.

All input seeds 50001–50010 are excluded from future tuning. The candidate cannot
be replaced by a later development record. A failure prevents declaring this
predeclared validation successful. The archived 4,000 threshold does not imply
4,000 on every different task stream. Two inputs provide limited replication;
they cannot establish broad generalization or a statistical guarantee.

[Exact protocol](experiments/fresh-validation-v5-protocol.json),
[frozen cases](experiments/fresh-validation-v5-full.json),
[input generation](results/fresh-validation-v5/generation.json).

Reproduce the frozen checks after all eight jobs complete:

```sh
python3 random05/tools/audit_fresh.py \
  --batch runs/random05/fresh-validation-v5-split-full \
  --output random05/results/fresh-validation-v5/audit.json \
  --protocol-commit e46f57b \
  --protocol-json random05/experiments/fresh-validation-v5-protocol.json
python3 random05/tools/audit_fresh_replay.py \
  --batch runs/random05/fresh-validation-v5-split-full \
  --output random05/results/fresh-validation-v5/replay-audit.json \
  --protocol-commit e46f57b \
  --protocol-json random05/experiments/fresh-validation-v5-protocol.json
```

The replay helper was checked against all six completed V4 cases before use.
Its result also includes release-to-completion and censored unfinished-order
statistics. The frozen acceptance criteria must be assessed separately; a
successful replay by itself does not establish a throughput improvement.

## Partial results, 2026-09-21 04:17 UTC

The archived repeat passes the full audit at **4,011**, exactly matching all
six action/schedule/event/task fields.
[Repeat audit](results/record4011-repeat-split-full-v80/audit.json),
[exact equality](results/record4011-repeat-split-full-v80/exact-equivalence.json).

| Fresh task/start seed | Frozen candidate | Previous 3,990 configuration | Difference | NMS repeats |
|---|---:|---:|---:|---|
| 50009 | 3,912 | 3,901 | +11 | Running |
| 50010 | 4,047 | 4,023 | +24 | Running |

The candidate leads the previous configuration **7,959 to 7,924 (+0.442%)**,
positive on both new inputs. These streams are distinct from the archived
benchmark; 4,047 is not a replacement for its 4,011 record. All four report
strict completion. The final eight-run protocol audit and NMS comparisons
remain pending, so the active goal is not yet marked complete.
