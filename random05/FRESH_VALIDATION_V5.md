# Fifth fresh-input validation: 4,000 milestone verified

The archived RANDOM-05 combined benchmark reaches **4,011 tasks** and repeats
exactly. The frozen candidate also passes the predeclared fresh-input comparison:
**+26.63% versus the stronger NMS repetition per input**, and **+0.442% versus
the immediately previous configuration**, positive on both inputs.

| Fresh task/start seed | Candidate | Previous configuration | NMS repeat 1 | NMS repeat 2 | Gain over stronger NMS |
|---|---:|---:|---:|---:|---:|
| 50009 | 3,912 | 3,901 | 3,095 | 3,090 | +26.40% |
| 50010 | 4,047 | 4,023 | 3,177 | 3,190 | +26.87% |

Aggregate: 7,959 versus 6,285 for NMS and 7,924 for the previous configuration.
The fresh 4,047 is on a different task stream; the archived-input record remains
4,011. The small gain over the previous configuration is separate from the
framework's much larger established advantage over NMS.

## Frozen experiment

Protocol [e46f57b](https://github.com/fywu85/lorr/commit/e46f57b) was committed
at 2026-09-21 03:56:06 UTC, before input generation at 03:56:45 UTC. Source
[a2ff2b2](https://github.com/fywu85/lorr/commit/a2ff2b2), build-v80, planner seed0,
firstK7968 thenK16320/B18/s2/q4/G4/E8/P8, move bias3, and triage1.25/mix1
remained fixed. The previous3,990 configuration differs only in mix0.75.
Guidance and the known horizon require `--trick RANDOM-05`.

All eight original runs use the same new inputs, 2,000 steps, 16 physical
EPYC9354 cores / 32 workers, strict1s entry /30s preprocessing, and a32 decimal
GB process guard. Actual affinity and CPU quota checks pass. Shared hosts are
allowed. Both NMS repetitions are retained, using the stronger one for each
comparison. No candidate substitution or retry was needed.

| Candidate input | Mean entry | Maximum entry | Peak RSS | Average CPU use |
|---|---:|---:|---:|---:|
| 50009 | 572.9ms | 623.9ms | 559.6MB | 30.12 logical CPUs |
| 50010 | 560.2ms | 643.7ms | 558.8MB | 30.09 logical CPUs |

All runs have zero planner errors, scheduler errors and entry timeouts. NMS
means are950.7ms, maxima below952.4ms, and RSS below1.734GB. Candidate search
completes its fixed work; the deadline does not truncate the portfolio.

## Verification

Every predeclared criterion passes:

- The archived repeat completes4,011 tasks and exactly matches all six
  action/schedule/event/task fields.
- Each fresh candidate beats the stronger NMS repetition, and the candidate
  aggregate exceeds the previous configuration's aggregate.
- Full source/binary/input, allocation, deadline and memory checks pass for
  all eight original attempts.
- Independent replay checks every movement, collision, locked assignment,
  waypoint event and order latency. A separate check matches recorded starts
  and task chains to the actual input files, including the archived repeat.
- All held-out input bundles50001–50010 retain their generation hashes and
  remain excluded from tuning. Earlier unsuccessful attempts remain preserved.

[Matched protocol audit](results/fresh-validation-v5/audit.json),
[all eight replays and waiting statistics](results/fresh-validation-v5/replay-audit.json),
[input/trace consistency](results/completion-audit-4000/input-trace-audit.json),
[archived repeat](results/record4011-repeat-split-full-v80/audit.json),
[exact equality](results/record4011-repeat-split-full-v80/exact-equivalence.json),
[completed/unfinished waiting table](results/fresh-validation-v5/WAITING.md).

The longest completed candidate orders take1961/1980 steps. Every solver still
has initial orders unfinished at2,000, so eventual maximum latency is unknown
and at least2,000. Throughput selected the configurations.

Two new task/start streams on the same map provide limited replication. They
are not unseen-geometry validation, the colleague's private inputs, or proof of
an average gain over arbitrary instances. Selected development maxima and these
frozen fresh results remain separately labelled.

Reproduce the principal checks from the repository root:

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

The replay helper was verified against all six completed V4 cases, with exact
agreement in every shared report field. Source and proof hashes are retained.
