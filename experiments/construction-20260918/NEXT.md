# Continue CGAR across all ten instances; dense RANDOM transfer is active

Updated 2026-09-21 around 01:20 UTC. User scope: improve the CGAR framework across
all ten 2024 instances, NMS primary. Throughput first, fairness secondary. Latest
specific request: study the independent RANDOM-05 solver and borrow mechanisms
for CGAR RANDOM-04/05. Warehouse strict1s is resolved; do not chase Warehouse160k
or falsely complete the obsolete formal goal. Continue authorized work.

## Ownership, tools and provenance

Never edit/stage/build/collect/interrupt `random05/`, `RANDOM05_PROGRESS.md`, or
its agent's jobs. Read-only frozen-source study is authorized. Our CGAR RANDOM-05
benchmarks use cgar binaries under `runs/cgar-*`. Do not relabel standalone scores
as CGAR. No internal subagents. Fable persistent session
1ebb1075-3538-49d1-93d1-a00c94fa256a ran out of credits on turn46; no quota change,
no retry. Next turn would be47. CLI review payload authorization persists.

Shell tools need require_escalated (default bwrap fails). Shared main/index:
explicit paths and git commit --only; wait/retry transient index locks, never
remove a lock or stage the other agent's work. Public fywu85/lorr remains public;
commit/push authorized. Last completed push through83e0029; newer final checkpoint push still needed. No production C++ edits are pending; current source
83522266d6ae8c88241d11e91a125e30848cbca9 is built and verified. Untracked/dirty
random05 changes belong to the other agent. Latest completed evidence commit6ef1a3f; the final scheduling replication/rotation audit checkpoint is being committed next.

All solver workers finish prescribed work or throw timeout, no partial results.
32 decimal GB process limit. Shared GRID hosts allowed; qualify with enforced1s.
Use four physical cores per dense case; independent cases actually run in
parallel on disjoint groups. GRID can ignore best-effort binding: retain failures
and retry unchanged, never weaken the exact-core/no-quota guards. No tests/builds
on the login host. Python3.7; no shlex.join. `apply_patch` fails; quoted Python works.

## Completed records and primary evidence

Root CGAR_PROGRESS.md, campaign experiments/allmaps-20260920, timestamped
BEST_HISTORY.md, selected-full-results.json. Timestamps are result-file completion
mtime UTC; individual maxima are allowed, not means. All profiles are explicit
exploratory configs, not one universal automatic dispatcher. Map tricks require
--trick INSTANCE and [trick] commit/log labels. Published targets are archived in
TARGETS.md / published-targets.json, not matched local controls:
Warehouse154795, Sortation152714, City01=8420, City02=16787, GAME23274,
R01=639, R02=1221, R03=2334, R04=2547, R05=3050.

Warehouse155173/155090 strict1s seeds0/2: source3e319f1, V123 exactly reproduces
older whole trajectories, 10000 entries/100million independently replayed actions.
Max944.766/957.827ms, means428/426ms, RSS15.4GB, eight physical cores. Evidence
construction results/strict-runtime-full-v123. All-goal prewarm OFF; no retuning.

Generic all-map region-cap sourceeb99380, binary19155f20...;16 complete runs.
Optional CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT defaults0, caps each region/round
at complete-attempt boundaries. Cap4M selected over8M for most maps. Generic best:
Sort146460, City01=7305, City02=14068, GAME6519, R01=611, R02=1084, R03=1484.
R01..03 use direct cost plus local matching from the six-factor full matrix.
Fairness worsens for some direct-cost profiles; secondary. Sourceeb99380 means
strict one-second qualification is not implied by means <1s.

SORTATION explicit NMS native field: source df94a520c77fcb3a73a4a8c74b5b13cab542d1d6,
binarya23a3c81...; full5000-step seed0 results146460(control)/149321(bands0)/
150353(bands1). Three valid cases, four cores each,5s development. Best mean524.64ms,
max1151.94ms,12.47GB; still needs strict1s runtime work. Control whole hash exactly
matches generic146460. Proof results/sortation-native-full-v1, commit5a6571f.

## Actual RANDOM transfers, all tested

Frozen standalone source233f5bf studied read-only. Hash receipt/report under
random-transfer/source-review.json, REVIEW.md. Ideas: oriented whole-chain DP,
two-step movement promises, priority futures, averaged common continuations,
full-pool idle/unopened matching, pocket/cycle evacuation; tuned flow field and
known horizon are tricks. Existing CGAR NEXT_ERRAND is not exact whole-chain DP.
The standalone solver/code/jobs are untouched. Its new scores are not ours.

General priority portfolio source5d3c5bf51147d9739de9c457a679d727e8492848,
binary878d47d086a120a866bd68fb0068850dee4db1085f514084f8c5c38a1bcf3a70.
Optional NOISE default0, PERSIST0, MUTATION30percent; worker0 parent, every fourth
alternative restart, others mutate. Construction order only; identical scores,
powers and CGAR primary/recovery/reservations; separate RNG. Complete workers.
Full regression proof random-transfer/build-v1 (30 source/test hashes).

Full5s portfolio10 cases: one-worker1249/1847, eight-worker1330/1989,
noise50cold1306/2036, noise50persistent1193/2003, noise200persistent1187/1954.
Persistence weaker. Exact one-worker whole traces unchanged from sourceeb99380.
Both alternate objectives lost: remaining potential1243/1749, next-errand1207/1760
versus1249/1847, six valid cases. Keep them off.

Strict1s replication COMPLETE18valid/0failed, seeds0/2/4. One-worker R04
1249/1198/1260, eight-worker1330/1296/1295, noise50=1306/1231/1367. R05:
one-worker1847/1836/1806, eight-worker1989/1922/1970, noise50=2036/2031/2031.
Means R04=1235.67/1307/1301.33, R05=1829.67/1960.33/2032.67. Max307.61ms.
All seed0 whole trajectories exact5s->1s. Best generic R04=1367seed4; R05=2036seed0.
Proof results/priority-portfolio-strict-seeds-v2, commit4b15b15.

Map-specific integer guidance transfer: source83522266d6ae8c88241d11e91a125e30848cbca9,
binary7bbe4e2b516ffc76dee9d7e429194b7797475673dc2de9bde0d156e529e6e0d0.
Build raw runs/cgar-random-native-build-v1-r2-20260920, job8901043 COMPLETE.
31 source/test hashes verified, full regressions pass, four bound cores.
Proof random-transfer/guidance/build-v1. First build8901026 failed a new fixture:
all task chains had off-core stops and were excluded, so no pickup work. Fixed
fixture to core-eligible tasks, no production change. Old helper ignored failed
binding (64cores); build.py now checks exact4/noquota and accepts --hosts.

Header cgar/tricks/random_native.hpp from frozen233f5bf field15/oneflip5,
normalized flow/betweenness. Original floats identical at1/4threads. Round half-up
x10, forward5..43, turn6, wall20, max original-unit error0.04999218. Guidance files
and reproducible generator/pack_asset.py are under random-transfer/guidance.
Explicit --trick RANDOM-04/05 requires exact map occupancy and700/800robots.
No automatic dispatch. Native metric, remaining-flow, scale20 required; no bands.
CGAR_TRICK_RANDOM_UNIFORM=1 supplies forward20/turn6 with identical scoring,
separating field effect from pure-potential/turn convention. Internal oracle
lower bound1 allows sub20costs; every edge installed. Other maps unchanged.

First full strict1s guidance matrices COMPLETE6valid/0failed, seed0:
R04 control1306/uniform1205/field1353, field mean99.58ms/max136.28ms.
R05 control2036/uniform1941/field2457, field mean97.83ms/max177.58ms.
New overall selected R05 best2457 (+20.68% matched generic control; still19.44%
below historical NMS3050). R04 generic seed4record1367 remains higher than field
seed0. Both lanes-off whole traces match earlier generic source exactly.
Evidence results/random04-guidance-full-v1 and random05-guidance-full-v1,
commit358b668. No cutoff, short-task trick or matching added.

Capacity audit COMPLETE, raw runs/cgar-dense-capacity-audit-v1-20260920,
job8901047. Actual certificate prefix identical5d3->835. R05 core788/free819,
capacity787,31 parked robots. Of431 never-assigned tasks in2036run,427 have an
off-core stop and are permanently excluded. None assigned/completed. Remaining
773 outstanding eligible. R04 capacity filter inactive and124 off-core tasks
completed. Evidence random-transfer/capacity-audit-v1; not a counterfactual gain.

Certificate-off diagnostic COMPLETE4valid strict1s: generic control1306/2036,
CGAR_CERT0=1274/1953. Keep certificate ON. Removing protections loses despite
admission exclusion. This is an ablation (original liveness premise absent),
not a recommended general solver. Results/dense-capacity-full-v1, commit83e0029.

## Completed final replications; no CGAR jobs pending

All selected code is built, committed and fully regression-tested; there is no
unfinished C++ edit. The new movement-promise prototype is DESIGN ONLY.

Guidance replication (seeds2/4) COMPLETE8valid/0failed. R04 field1266/1278 against
control1231/1367; withseed0, mean1299 vs1301.33 (-0.18%). Do not select the field
for R04 on this evidence. R05 field2537/2441 against2031/2031; withseed0, field
mean2478.33 vs2032.67 (+21.93%), all three pairs positive.
Results random04-guidance-seeds-v2 / random05-guidance-seeds-v2.
R04 verifier8901095 rejected unbound64cores onresearch57 before reading results;
unchanged retry8901105 onresearch44 passed. Original allocation/log and retry
receipt retained in committed results. No solver failure.

Scheduling screen COMPLETE16valid/0failed, source8352226/binary7bbe4e2..., full
strict1s, four cores percase, seed0. Profiles random-transfer/scheduler-*-variants.json.
Generic control/directpickup4/match64/both:
R04=1306/1343/1309/1503; R05=2036/2038/2062/2068.
Field control/directpickup4/match64/both:
R04=1353/1481/1356/1338; R05=2457/2421/2544/2574.
Jobs8901097/98/99, verifiers8901100/01/02 allcomplete. Results
 dense-scheduler-generic-full-v1,
 random04-scheduler-field-full-v1,
 random05-scheduler-field-full-v1.
All full control trajectories exactly reproduce earlier runs. Interactions
matter; this is not the reference's full-pool Hungarian scheduler. CGAR still
has bounded local matching,20step cooldown, one-retarget and primary/recovery/
started-task safeguards. Forced-oldest admission remains, no horizon cutoff.

Selected scheduling replication COMPLETE10valid/0failed, seeds2/4:
R04 generic control1231/1367 versus combined1480/1494. All three selected scores
1503/1480/1494, mean1492.33 versus1301.33 (+14.68%). Generic record stays1503seed0.
R05 field control2537/2441, match-only2475/2532, combined2608/2528.
All three combined scores2574/2608/2528, mean2570 versus2478.33 (+3.70%).
Matching-only mean2517 and mixed pairs. New overall CGAR R05 record2608seed2,
TRICK, source8352226, max155.36ms, still14.49% below historical NMS3050.
Overall selected R05 mean is26.43% above prior generic2032.67.
Jobs8901113/14, verifiers8901116/17 allcomplete. Results random04-scheduler-seeds-v2
and random05-scheduler-seeds-v2. Summary random-transfer/scheduling-three-seed-summary.json.
Exact selected profiles best-random04.json / best-random05.json; practical guide BEST.md.
No need to repeat these tests without new changes/failures/unresolved concerns.

Read-only action audit COMPLETE, job8901122. Raw runs/cgar-dense-rotation-audit-v1-20260920;
proof random-transfer/rotation-audit-v1, helper rotation_audit.py. In2574fieldrun:
583740turns/453960forwards,105055 immediate inverse-turn pairs with no adjacent
known task/goal/assignment change (18.22% of eligible turn transitions). Only
35.25% turn->forward. Generic R04:12.77% inverse pairs. This does NOT prove
forcing a forward move is legal or beneficial. Field improves tasks despite
fewer forwards; motion counts alone are not objective gains.

## Next concrete mechanism

Read random-transfer/MOVEMENT_PROMISE_DESIGN.md. Proposed opt-in generic promise
of the next action after a planned rotation, derived from previous complete
TemporalWarmStart suffix, reconciled through its existing monotone collision
reset closure. CGAR protected seeds dominate. Replace retained ordinary seed
and filter first-action alternatives; skip ordinary-seed auto-rotation for
promises, preserve actual operation/heading. Current code assumes selected0is
an ordinary wait seed; don't overlook that trap. Default OFF must reproduce
all old trajectories; independent cycle/protected-reset/deadline/thread tests
before benchmarking. Warm/mixed starts OFF in the first prototype to isolate it.
No implementation started. This is narrower than the reference's full pipeline.

Other future transfers: exact oriented whole-chain DP, faithful common-future
continuations, full-pool assignment and safe pocket evacuation. Preserve all
negative evidence (persistence, simple next-errand/remaining objective, certificate
removal). No Fable retry until quota actually changes. Do not touch standalone
R05 code/tests/jobs or held-out task streams50001..50008.

The reusable analyzer freezes helpers/profiles/inputs; verifies sources, exact
binary, complete horizon/errors,1s/RSS, cores, motion/waiting. It is not independent
full-action replay. Appropriate independent replay remains useful for a final
competition record, as already done on Warehouse. All our current GRID jobs are
finished; other remaining user jobs belong to the independent agent.
