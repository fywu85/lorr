No safety or determinism bug turned up in the temporal kernel, the adapter, or the threading. Rollback, seed consistency, recovery protection and deadline propagation all hold on a line-by-line reading against native `pibts.cpp`. The defects I found are in failure reporting and diagnostics. The most useful performance finding is that the fixed attempt count is limited by the cold-start phase, and about 80% of the steady-state decision budget goes unused.

## Proven findings

**P1 (Medium): initialization failures are swallowed.**
- `BaseSystem::planner_initialize` never calls `init_future.get()` (`cgar/src/CompetitionSystem.cpp:97-113`). `Entry::initialize` has no try/catch (`Entry.cpp:13-17`).
- `Cgar::initialize` sets `initialized_ = true` at `cgar.cpp:533`, before its throw sites: the CPU-allocation check (`:549-550`), the guidance requirement (`:558`), and the `temporal_preprocess` Timeout (`:600-601`).
- A throw there is therefore logged as "Preprocessing success". The first `schedule()` then indexes `at_cell` sized from an empty `cert_` (`cgar.cpp:1648-1655`), which is undefined behaviour rather than an explicit failure.
- Completed runs are unaffected, because they print the allocation line.
- Fix: catch in `Entry::initialize` and `_Exit` with a distinct code, and set `initialized_` last.

**P2 (Low, diagnostics): the timeout stage label is ambiguous.**
- One `check` lambda labelled `"temporal_construction"` is passed to both `construct` and `repair` (`temporal_adapter.hpp:137-139`).
- The 150k failure (`runs/cgar-temporal-full-v3-20260918/goal_ranked_150000-s0-r0/WAREHOUSE.log:15072-15073`, 1000.885 ms) cannot say which phase overran.

**P3 (Medium, performance boundary): work is sized by the cold-start peak.**

| Step (50k profile, `search_50000-s0`) | Wall time per decision (log timestamps) |
|---|---|
| 0–10 | about 0.27 s |
| 28–30 | 0.47–0.48 s |
| 40 | 0.65 s |
| 60 | 0.50 s |
| 100 | 0.44 s |
| Run average | about 0.20 s (1012 s / 5000) |

- Candidates examined per decision range from 1.65M to 4.16M at a fixed 50k attempts.
- The 150k run died at step 29 with 2.16 GB RSS, so caches were still cold.
- For calibration, single-worker NMS in your probe did a median of 124,090 attempts per decision, p95 223,205 (`experiments/construction-20260918/results/probe/summary.json:403-409`).

**P4 (Low): the CPU check is weaker than its message.**
- `CPU_COUNT(affinity) < threads` (`cgar.cpp:548-550`) counts logical CPUs. It passes on an unrestricted host and cannot detect SMT siblings.
- Allocation correctness currently rests on the harness (`benchmark_matrix.py:22-32`, `benchmark_cgar.py:120-122`).
- For full-v4 the groups [0-3], [8-11], [16-19], [24-27] are one logical CPU per physical core (judging by CPU numbering, each group looks like one L3 domain; `allocation.json` records no cache topology), and the log prints `workers=4 threads=4 allowed_cpus=4`.
- Worker threads are not individually pinned.
- Memory is 15,895,492 KiB, about 16.3e9 bytes, which is within the 32e9 target. The target is measured by `ru_maxrss` but not enforced: the job's address-space limit is 96 GiB.

**P5 (Low): recursion depth on thread stacks.**
- `build()` depth is bounded only by `limit_`, which the env clamp allows up to 32768 (`cgar.cpp:555`).
- Both the planner and the workers run on `std::thread` stacks (`CompetitionSystem.cpp:74`, `temporal_adapter.hpp:149`).
- The default of 8192 has wide margin, with observed `max_depth` at most 221.
- The 1,500-robot chain test runs on the main thread, so it does not exercise a thread stack.

## Verified by inspection, no defect found

- **Rollback and cycles.**
  - Every exit path of `build` (`temporal_pibt.hpp:126-159`) restores both `selected_` and the reservation tables.
  - Construction-mode re-entry of a robot already on the stack matches native, whose visited check is commented out (`pibts.cpp:572-590`). It stays consistent because `add`/`remove` always use the current `selected_`.
  - The random draws happen in the same order as native.
- **Parts the conformance test does not cover.**
  - The adapter's `cost()` (`temporal_adapter.hpp:71-83`) matches `get_smart_dist_IMPL` by reading.
  - The `old_score_ > 0` guard (`temporal_pibt.hpp:122`) is unreachable in practice.
  - With equal weights, native order degenerates to descending robot index (`pibts.cpp:728-730`) while the port keeps distance order. Your v3 seed-0 orders differ by at most 1% (107,457 / 107,512 / 106,485).
  - `score_` is accumulated incrementally in double, so with ranked weights it drifts slightly. The drift is deterministic, and with equal weights the sum is exact.
- **Thread safety.**
  - Workers share only const data.
  - Worker seeds are drawn before launch (`temporal_adapter.hpp:126-127`).
  - Results and exceptions are stored per worker index, all threads are joined, and the best-score selection with lowest-index tie-break is deterministic.
  - The cache lookup that reorders the LRU (`turn_oracle_.find`) runs only before the workers start.
- **Deadline propagation.**
  - Checks run per robot in candidate generation, per construction root, per repair step, and every 16 displacements.
  - A worker Timeout is rethrown on the calling thread and reaches `_Exit(124)`.
  - A microsecond-scale race after the final check would show up as `numEntryTimeouts>0`, which the harness marks invalid (`benchmark_cgar.py:140`).
- **Seeds and recovery protection.**
  - After `make_safe` reaches its fixed point, a forward move into an occupied cell implies the occupant also moves forward. The pin closure (`temporal_adapter.hpp:21-29`) therefore makes seeds conflict-free.
  - Transaction cells are barred at all five time steps (`temporal_adapter.hpp:109-110`).
  - Transaction robots are fixed.
  - The independent validator (`temporal_adapter.hpp:161-176`) runs before any action is exposed.
- **Scope caveat.** WAREHOUSE has `pockets=0` (log line 1). The full runs therefore exercise only primary and transaction protection. Lock, pocket and capacity paths rest on the small fixtures alone.

## Hypotheses, not proven

**H1 (Low–Medium): the primary's blocker is not obliged to leave.**
- The pin closure follows only forward moves (`temporal_adapter.hpp:26-28`). When the primary is turning, or its forward move was cancelled because the occupant must turn first, the occupant is left unpinned.
- Its base PIBT displacement is then discarded (`cgar.cpp:1217-1223`, `temporal_adapter.hpp:180`). No other robot may enter the protected cell, and the occupant's own cost decides whether it moves.
- Progress falls back to transaction recovery. That path covers this case by design, but I did not see it exercised, and it costs the primary latency.
- The regression fixture (`cgar_regression.cpp:444-457`) only covers an occupant that gets its own goal elsewhere at t=1.
- Evidence against severity comes from the temporal run's sampled logs:
  - Primary stalls are at most 6 after t=600, against 490–2134 in the control.
  - There are 450 transactions, against 1323 in the control.
  - The early samples of 130 and 256 are per-agent stall since the ticket was issued, not time spent as primary.
- To substantiate: log whether the primary's committed cell holds an unpinned robot and whether that robot's selected path leaves the cell.
- To exercise it: add a fixture with an idle occupant that must turn.
- A generic fix would pin the occupant of any fixed robot's committed cell with its base action.

**H2 (Low): protected robots are modelled as stationary after their first action.**
- Their seed is the base first action followed by four waits (`temporal_geometry.hpp:49-52`).
- Ordinary robots may therefore plan into the primary's onward corridor.

## Timing anomaly

- **Same pause in three processes.** Three full-v1 processes each show one ~29 s `plannerTimes` entry, at different simulation steps:

  | Run | plannerTimes index | Entry | "planner returns" stamped |
  |---|---|---|---|
  | `search_50000-s0` | 1560 | 28.980 s | 21:13:06.912 |
  | `search_50000-s1` | 1543 | 29.035 s | 21:13:06.863 |
  | `search_10000-s0` | 2570 | 29.035 s | 21:13:06.806 |

- **Planner compute had already finished.**
  - That record is stamped 0.12–0.18 s after the step began (log lines 79084-85, 78415-16 and 107641-42).
  - The next record in all three processes is stamped 21:13:35.735, within 0.6 ms of each other.
- **Where the pause lies.**
  - Boost evaluates the timestamp when the record is opened.
  - The outer timer's `end` is taken right after `log_info` returns (`CompetitionSystem.cpp:86-87,153`).
  - The pause therefore falls between opening that record and `plan()` returning, at the same moment in three processes.
  - This is the same signature as the 3.7 s outliers audited in the motion README.
- **What this does and does not show.**
  - It supports "not planner compute; a shared external pause during or after the log write".
  - It does not identify the system cause.
  - No `-l` log file is passed, so the simulator's Boost log goes to stdout/stderr, which the harness redirects to `WAREHOUSE.log` under `/shared`.
  - That is consistent with an I/O stall, but it does not prove one.
- **Related risk.**
  - With `CGAR_DIAGNOSTICS=1` the planner itself prints and flushes inside the deadline (`temporal_adapter.hpp:195-199`, `cgar.cpp:1348-1350,1449`).
  - A similar pause landing there would produce a `CGAR_TIMEOUT` unrelated to planning.
  - Consider writing logs to node-local scratch.

## Experiments

1. **Value of search depth (diagnostic only, not a score).** Run one worker at 50k, 100k, 150k and 250k attempts with `--plan-time-limit-ms 5000`, seeds 0–2.
   - 10k → 50k gave +17%.
   - The 4×12.5k portfolio scored 93,328, below the 104,573 of one worker at 50k.
   - NMS's median depth is 124k attempts per decision.
   - This run decides whether experiments 2 and 3 are worth building.
2. **Deterministic work-unit budget.** Stop repair after the attempt in which a deterministic counter crosses W. The counter would be candidates examined plus BFS node expansions in that decision.
   - Work stays fixed and prescribed, with no clock, and Timeout stays as it is.
   - This removes the 2.5× per-decision spread and the ~3× cold-start inflation from P3.
   - Substantiate with the new precise per-decision time series (maximum under about 0.8 s across six seeds) and paired scores against the 50k profile.
3. **Use threads for sequential depth, not a portfolio.** Partition cells into fixed stripes by column index.
   - Robots whose 5-step reach touches a stripe boundary are frozen for that phase.
   - Each thread owns its stripe's reservation entries and its own RNG, and the two offset partitions alternate within a decision.
   - The merge is over disjoint sets, so the result is deterministic and your serial-versus-parallel regression still applies.
   - A cheaper first probe is a synchronized portfolio that copies the best state to all workers every K attempts.
4. **Generic direction-aware costs.** Your matched probe shows NMS's supplied guidance is worth +9% on warehouse (8,769 against 8,047 arrivals).
   - In the temporal run, 8.5% of robot-steps move away from the goal and 7% wait.
   - One candidate is decayed per-directed-edge congestion counts fed into bucketed-Dijkstra oriented tables.
   - Another is a small parity-based counter-flow penalty. Whether that counts as a "template" under your rules is your call.
   - Screen either with the fixed-goal probe first, then run full seeds and re-probe a t=4000 snapshot.
5. **Scheduler decomposition, then repair.** At t=4800, 35,802 of 110,308 assignments came from the fallback path.
   - The fallback fires when the local 2,048-node search comes back empty. It assigns the oldest task unless one of 64 sampled tasks scores better.
   - Estimated pickup distance is 56 cells against a task chain of 246.
   - CGAR spends 478 robot-steps per task against KK's 327.
   - Step one (no new runs): from the existing result JSONs, compute for CGAR, KK and NMS the pickup distance at assignment, completed versus revealed chain lengths, and steps per shortest-path cell per leg.
   - Step two: replace the fallback with one multi-source BFS from free pickup cells, giving every robot its exact nearest free task.
   - Step three: add a bounded preference for shorter chains.

## What would substantiate performance claims

- Use the same binary hash with paired seeds. The seed range at 50k is only 0.25% (104,573 / 104,788 / 104,523), so effects above 1% are meaningful with three seeds, and six would match your control.
- Every run should have zero errors and timeouts, peak RSS reported in bytes, a recorded affinity, and its maximum precise entry time.
- The pending `workers4_serial_12500` run must reproduce 93,328 exactly, the score of `workers4_parallel_12500`, and ideally an identical action hash. That is your full-run check for races.
- Diagnostic runs with a relaxed deadline must never be reported as scores.
