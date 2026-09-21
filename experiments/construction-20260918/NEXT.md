# Continue CGAR across all ten instances, general improvements and explicit tricks

Updated2026-09-21 around02:34UTC. Follow actual user scope, not the obsolete
formal Warehouse160k goal: improve CGAR across all ten LoRR2024 instances, NMS
primary, throughput first, fairness secondary. Latest user steering explicitly
asks to pursue instance-specific tricks alongside general changes. RANDOM-04/05
should borrow ideas from the independent RANDOM-05 implementation, read-only.

## Ownership and execution

Never edit/stage/build/collect/interrupt `random05/`, `RANDOM05_PROGRESS.md`, its
agent's jobs or its held-out input streams50001..50008. Our CGAR RANDOM-05 jobs
and outputs are under `runs/cgar-*`, independent from that solver. No internal
subagents. Fable CLI permission persists, consistent session
1ebb1075-3538-49d1-93d1-a00c94fa256a, but turn46 ran out of credits; no quota change,
no retry. Next would be47 if availability changes.

Shell tools need require_escalated; default bwrap and apply_patch fail. Edit with
quoted Python. Python3.7, no shlex.join. All heavy builds/tests/analysis on GRID.
Shared main/index: explicit paths and `git commit --only`; never remove locks,
amend or stage other-agent edits. Public fywu85/lorr stays public. Commit/push
are authorized. A new checkpoint/push is being completed after this handoff.

Complete prescribed work or explicit timeout, never partial-quality success.
32decimalGB per process. Shared hosts allowed; exact physical-core binding and
no quota remain verified. Prefer research43/44/57 EPYC9354. GRID may ignore
binding; preserve rejected allocation and retry unchanged rather than weaken
checks. No user approval needed for already authorized builds, fixes or pushes.

## Current selected records (unchanged by latest dense negatives)

Root CGAR_PROGRESS.md, experiments/allmaps-20260920/BEST_HISTORY.md and
selected-full-results.json are canonical. All individual maxima allowed,
including explicit tricks and a selected seed. Store timestamp, commit, exact
config, binary, complete horizon, latency/RSS/error and waiting evidence.
Published NMS targets in TARGETS.md are not fresh matched local controls.

* Warehouse155173 vs154795: strict1s, source3e319f1,8cores, max945/958ms seeds0/2,
  RSS15.4GB,100million independently replayed actions, V123 exactly reproduces
  V110 whole trajectories. Do not retune Warehouse now.
* Sortation150353 vs152714: native NMS field bands1,5s development, max1152ms,
  four cores,12.47GB. Runtime qualification remains outstanding.
* CITY01=7755, CITY02=14851, GAME10080, all TRICK adapted4/16 NMS fields.
  Source596b609, binary41aea678..., seed0, strict1s maxima765/807/905ms,
  four physical cores; full3000/3000/5000steps. Generic controls7305/14068/6519
  are whole-trajectory identical to sourceeb99380. NMS8420/16787/23274, no
  matched NMS claim. Best configs/selection proofs city-game/best-*.json and
  *-selected.json; full comparisons results/{city-01,city-02,game}-native-full-v1.
* Generic R01=611, R02=1084, R03=1484; NMS639/1221/2334. Selected direct cost plus
  bounded matching from six-factor matrix. Original5s development.
* R04 generic1503 seed0, strict1s. Seeds0/2/4=1503/1480/1494, mean1492.33.
  Direct pickup4, HRRN0, matching64; no known-horizon or field trick.
* R05 TRICK2608 seed2 vs3050, strict1s. Seeds0/2/4=2574/2608/2528, mean2570.
  Tuned field +same direct/matching settings. Selected maxima<171ms, RAM<0.2GB.
  Dense source83522266d6ae8c88241d11e91a125e30848cbca9,
  binary7bbe4e2b516ffc76dee9d7e429194b7797475673dc2de9bde0d156e529e6e0d0.
  Exact best configs random-transfer/best-random04.json and best-random05.json.
  Three planner seeds on the same task/start stream, not three new inputs.

## Recent completed general and trick transfers

Read-only frozen standalone source233f5bf review is in random-transfer/REVIEW.md
and source-review.json. Reference mechanisms: exact oriented whole-chain DP,
coherent two-step movement pipeline, priority futures, common continuations,
full-pool assignment, pocket evacuation. Tuned field and known horizon are tricks.

Already measured: eight search candidates beat one on both dense maps; cold
noise50 helps R05 consistently, persistent offsets lose. Generic next-errand and
remaining-potential ablations lose. NMS-derived five-action kernel remains inside
CGAR protected primary/recovery/reservations; no relabelled standalone solver.
The explicit integer field helps R05 mean2032.67->2478.33; R04 mean1299 vs1301.33
(no gain). Added direct pickup4+matching64 gives the selected dense records above.
All reference/source/build/matrix proofs are in random-transfer and results/.

Capacity audit: R05 certified core788 versus800 robots,31parked. In the2036-task
control427/431 never-assigned tasks were structurally excluded by off-core stops.
Removing certificates lost both dense controls; keep them enabled. This fairness
limitation still exists in the selected field solver. Throughput is primary but
report it honestly. No claim of universal starvation-freedom.

## New after-turn promise: implemented, validated, negative

Source aa63d48cdb75f61774474faad025f049cc631f76, binary
9fd0c126b7546880e08bce1e646f487d4f15cccf47a9e7a9f60e4964a111b82d,
frozen runs/cgar-after-turn-build-v1-20260921. Full suite and31source/test hashes
verified. `CGAR_TEMPORAL_PROMISE_AFTER_TURN=1`, default OFF, incompatible warm/mixed.

Uses TemporalWarmStart exact consecutive-state/goal checks and monotone closure
against authoritative CGAR protected seeds. Keep ordinary choice0 as unchanged
score reference; retained suffixes start at remapped nonzero indices. Filter
searchable choices>=1 to promised first action. All searches/rollbacks preserve
it; final assertion prevents fallback to ordinary seed or changed action. This
avoids changing score offsets, diagnostics and ordinary-seed auto-rotation.
No map branches, no RNG changes when disabled. Remember actual expected headings.

All ten full seed0 cases passed1s; max227ms. Three disabled controls exactly
reproduce older whole trajectories:
* R04 generic control1503 vs promises1388.
* R04 field/direct4 control1481, promises1392, short-preference1346, both1334.
* R05 field/direct4/matching control2574, promises2548, short2539, both2546.

Short preference on these controls only removes forced-oldest admission because
HRRN was already OFF. Do not generalize one negative seed to all scheduling tricks.
No new best. Selected profiles remain OFF for both options.

Actual-action audit: inverse-turn percentages genericR04=12.77->11.14,
fieldR04=16.43->14.24, fieldR05=18.22->16.76. Turn->forward rises slightly but tasks
fall. Only10.2/12.0/6.6% of eligible suffixes survive collision closure: retaining
only previously turning robots is much narrower than a coherent fleet pipeline.
A joint rolling prefix or common-future rollout remains an untested general idea.
Proofs random-transfer/promise-summary.json, promise-rotation-audit-v1,
MOVEMENT_PROMISE_DESIGN.md; results/random04-promise-generic-full-v1,
random04-promise-trick-full-v1, random05-promise-trick-full-v1.

Follow-up existing `CGAR_TRICK_NATIVE_NEUTRAL_TAIL=1` also loses: R04field1481->1398,
R05field2574->2542. Four complete valid1s cases, max171ms, old controls identical.
Activation and669231/1576648 changed candidate scores confirmed. Leave OFF.
Proof random-transfer/NEUTRAL_SERVICE_CHECK.md, neutral-service-summary.json and
results/random0[45]-neutral-service-full-v1. Jobs8901200/02, verifiers8901201/03
complete. No more dense cases pending; don't rerun without a new hypothesis.

## Current active work: CITY/GAME explicit NMS tricks

Implemented and full-suite tested source596b609351f4653dcb404e264edb4f20de98d7eb,
binary41aea6783c66413dfc9df6970f18bae29c7ac431b592b1a8310b986a929ff758,
frozen runs/cgar-city-game-build-v2-20260921. Build job8901191complete.

NMS GraphGuidance constructor unchanged, native parity field20/200, turn1.
Independent formula/direction/wall and SHA/FNV checks. Explicit --trick CITY-01,
CITY-02 or GAME requires exact occupancy and1500/3000/6500 robots. No autodispatch,
no bands for these maps, no RANDOM uniform selector. Generated assets under
cgar/tricks/city_native.hpp, game_native.hpp; helpers/proof city-game/.
The benchmark CLI/receipt tools and matrix analyzer support the new names.

New regression independently checks1544008 field values,361564 oriented states,
22000 lanes-off/control actions and11000 native pickup/planning actions. The first
build failed a test assertion: timestep0 intentionally uses generic spatial mass
dispatch even with static fields. Set native test fixture to timestep10, matching
existing fixtures; production code unchanged. Failure receipts retained in
city-game/build-v1-failed, successful build-v2/checks.json.

All twelve full CITY/GAME cases are complete and verified. Adapted field wins
7755/14851/10080. Native gives6815/14185/6784; native+short6669/14265/6758.
All12 valid1s, all three generic controls exactly reproduced. Retain adapted
field, HRRN and forced-oldest admission. GAME waiting agep90 remains5000.

Latest guard source85c3f0fff5acc76b1ec7db7155fa48ebb0cedf12, binary
726eedf767129d409fad5d869e124368b6048fa74adbb5b5ae37fc4939d600f7,
frozen runs/cgar-promise-guard-build-v1-20260921, full suite passed. It rejects
positive branch work with after-turn promises (branch resolver can try ordinary
choice0). Neither option was combined in measured profiles. Proof
random-transfer/promise-guard-build-v1.

CITY01 adapted2x2 factor matrix8901238/verifier8901239 still running at02:34UTC:
control, remaining-chain order2/unequal rank; direct pickup4/HRRN0/matching64;
and both. The dispatch cases failed before simulation: generic matching flag
wrong under --trick. Retain failure receipts/original declaration. Corrected
selector CGAR_TRICK_UNOPENED_MATCH=1 in city-game/city-01-adapted-dispatch-corrected.json,
only those two resubmitted job8901254/verifier8901255, same frozen guard binary.
Raw prefixes runs/cgar-city-01-adapted-{factors,dispatch-corrected}-v1-20260921.
Original verifier cannot represent preinit missing trick receipts; enhanced
analyzer now supports --analysis-tag for a separate immutable verification
attempt and only demands receipts for valid rows. Resubmit original matrix
analysis with a new tag after completion/hold8901238; do not overwrite frozen
analysis support. Reject invalid rows without accepting partial throughput.
Matrix submitter now rejects that known flag conflict before queuing.

## Concrete next steps while large-map jobs finish

1. Finish CITY01 factors and corrected dispatch pair; verify original7755
   control unchanged. CITY/GAME records now logged. Commit/push owned evidence.
2. Implement an explicit trick for NMS's squared rank power, first for GAME and
   RANDOM-04/05, as a separate opt-in selector requiring --trick. Current dense
   ranks are linear. Default OFF must preserve trajectories and CGAR protections.
   Test paired R04 generic-best+lanesOFF1503 and R05field-best2574 seed0; do not
   accidentally compare R04's weaker1481field control with1503general optimum.
   Validate activation, malformed flags, serial/parallel/protected paths before
   full runs. NMS source pibts.cpp~710squares precisely these three instances.
3. Or inspect general coherent-prefix retention/common futures/full-chain DP.
   Don't repeat the narrow after-turn prototype or neutral-tail ablation expecting
   a gain without a mechanistic change.
4. TRICK_ROADMAP.md records remaining reference candidates with fresh hashes:
   KK R02random_weight_101.w, R03random_weight_040.w, R04random_700_w002.w;
   CITYparis_weight_014.w, GAMEbrc202d_weight_002.w and2750active-agent cap.
   NMS random arrow field uses2/6forward andturn2, not the20/200parity field.
   Only inspect the active kk/wppl_planner config/code, not my_planner.
   Fleet thinning must preserve CGAR protected/started paths and be separately
   labelled; no claim that merely disabling fairness reproduces KK's limit.
5. Sortation strict1s runtime qualification remains before calling all-map solved.

Known-horizon current scheduler requires full core for valid lower bounds.
RANDOM/CITY/GAME have pockets; do not bypass that check. Build a true free-space
bound or explicitly label a distinct heuristic. All bests may use tricks and a
single planner seed, but the task/start stream and official hardware caveats stay.
