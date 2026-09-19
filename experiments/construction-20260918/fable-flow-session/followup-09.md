Continue the same lorr-warehouse-cgar-fable51 session, turn10. Read-only incremental
source review, Fable5.1max effort. No tools; do not claim tests were executed by you.
Target unchanged: generic5000-step/10000-robot warehouse >=152981repeatably on
six seeds, complete1second decisions, no partial clock-selected results, RSS<32GB,
isolated physical cores, no map-specific policy/templates or task forecasts.

Measured updates since turn09:
- Graded strength4/scale4 margin25/build128 is the new six-seed reference:
  138402/139716/139303/138506/138304/139457, mean138948 (+0.611% vs binary25).
  All six totals improve, four finalwindows improve. All12valid and controls exact.
- Normal v41pickup flow at binary50/build128 improves both seeds0/2:
  137508/137923vs137060/136841, mean+0.559%; full empty robot-steps fall~2%/1.5%,
  agep90 rises. HRRN0/weight5 enabled collapses seed2 to49732/final4522 against
  stable disabled137603/136560. Reject aggressive arm; all decisions valid.
- Final t4800normal pickup diagnostics:~96.5%cached probes,~1270settled cells per
  search,~34%hit8192pops. Probe share is not assignment share. This contradicts
  rare cached probes, and does not support an unmeasured half-reach claim.
- Your churn hypothesis: all four graded full traces/40publications reconstructed
  and matched move/penalized counts/full movement totals. At first refresh640,
  collapsed margin0seed0 has only3.87%more pair churn and7.50%more flips than healthy
  seed2; later churn lower. No large early global-churn signal; no hysteresis change.
- Cache8->16GiB saves197/217rebuilds of~150k but both full trajectories and
  guided/fallback counts exactly match. Keep8GiB.
- Graded pickup flow now completed: ON138963/139677 vs exact OFF138402/139303,
  mean+0.337%; final windows+244/+6, empty robot-steps down1.63%/1.46%, agep90
  up18/20steps. Remaining four seeds will verify before promotion.

V42 is a larger NMS-inspired candidate-discovery experiment. Selected free robots
in the existing rotating order get COMPLETE forward distance fields from observed
cell/heading using the last published metric. Fixed quota0–64(default0);1–32build
threads(default4), requiring v41pickup flow. Every selected graph traversal
finishes or the entry fails. All builders join before field slots are made usable.
Others still use the existing searches/fallback; this does not park/cap the fleet.

CompletePickupSearch uses positive edge costs1–16 and17circular buckets, returns
cell minima over terminal headings, and reuses per-worker scratch. All eligible
unassigned tasks are scanned and the existing top16/top8 limit is retained by the
unchanged Pair ranking. Finite complete-field values also supply fair/fallback
estimates for that robot. Physical graph/core restriction and existing approximate
unreachable fallback are explicit; no certificate-domain or real waiting-time
exactness claim. Scheduling still precedes plan publication, same snapshot contract.

Our ALL-regression build passed, including32256independent repeated-relaxation goal
costs for heap and complete bucket search, all headings/scales1/2/4/8/directed
costs/obstacles/disconnection, all reachable states settled,12fields identical
serial/parallel, injected in-search exception and joins, scratch reuse, invalid
config/costs, cold behavior, a cheaper task outside16nearest endpoints, fairness,
started protection, and2free robots under1field quota with distinct assignments
and unchanged simulator metadata. Scheduler fixtures ingest prescribed physically
valid observations to isolate costs; full simulator evaluates actual offered actions.

Five200-step screens passed below0.893s/RSS5.224GB. Disabled prefix equals v41ON;
16serial/parallel whole prefixes match. Eight full tests are running: fields0/16/
32/64 with4threads, seeds0/2. This is feasibility evidence, no short-prefix ranking.
The control is experimental graded/v41pickupON, which passed its first two full
paired seeds against the confirmed graded/OFF reference but is not promoted yet.

Please give at most three concrete prioritized findings, with function/line or a
minimal counterexample. Focus on circular-bucket correctness and integer bounds,
read-only parallel construction/publication on all exception paths, top-k heap
ordering and task/robot uniqueness, and the fixed-work/metric contract. Distinguish
bugs from explicit approximations. If none reproduced, say so. Identify one
bounded diagnostic/experiment if useful; do not repeat causal extrapolations from
coverage or aggregate throughput alone. Do not claim tests you did not execute.
