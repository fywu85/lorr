# RANDOM-05 handoff

Updated: 2026-09-20 20:53 UTC. The performance objective is verified at the
requested “reasonably close” level. All submitted Random05 benchmarks are
terminal; no Random05 work remains queued. Final goal status can be completed
once this report and evidence are committed/pushed. See RESULTS.md and
results/completion-audit.json for the requirement-by-requirement verification.

Only edit/stage/commit random05/ and RANDOM05_PROGRESS.md in this fork. Warehouse
work shares main; leave its files, staged changes and GRID jobs alone. No
subagent delegation. Pushing to fywu85/lorr is authorized; leave visibility alone.

## Verified results

- Frozen independent V4:3680 versus NMS2907/2870;3641 versus2930/2918.
  Stronger-repeat gains26.59%/24.27%, **+25.42% aggregate**. All six original
  attempts valid, full2000/strict1s/four physical EPYC9354 cores/32decimalGB guard.
  Candidate means796/791ms, maxima859/852ms, RSS below499MB.
  Protocol **a7bad0c** committed before generating inputs50007/50008. Exact
  candidate source5f81613/build-v65, seed3, first4608/K5760/B12/s2/q4/G4/E8/P8.
  Six independent full replays pass collision, assignment-lock and waypoint
  accounting. All29 engine/simulator source/header files match the commit.
  V3 earlier gave+25.46% on different inputs with an older candidate; not a
  paired causal comparison. ALL50001--50008 remain excluded from tuning.
- Four-core development best **3770**, +29.4% vs strongest matched NMS4=2914.
  Source5f81613, seed3, firstK4608 thenK5760/B12, screen2/keep4,
  generations4/E8/P8, blend0/cache512/kinematic1/cyclemask0.
  Mean787.573ms/max845.398ms/RSS473328KiB. Evidence in
  worker-affinity-four-full-v65/summary.json (UNSPLIT batch), false-binding case.
  Both false/close cases EXACT3770, including all six trajectory fields, and
  exactly match fast32. All observed worker masks inside disjoint allocations.
- Overall /32workers /16physical cores best **3852**, +21.4% vs NMS32=3172.
  Source5f81613, seed4, firstK8000 thenK16320/B14, same staged search,
  blend0/cache64/kinematic1/cyclemask0. Mean478.098ms/max538.443ms/RSS565044KiB.
  Evidence: staged-record-seeds-32-split-full-v65/
  32-staged-k16320-b14-first8000-seed4/summary.json.
- Without horizon cutoff: **3503 four-core (+20.2% vs NMS4)** and
  **3632 on32workers (+14.5% vs NMS32)**. Same respective best configurations,
  only R05_HORIZON2000->0. Full strict runs, means806/482ms,max866/531ms.
  Cutoff gains7.6%/6.1%. Guidance remains a declared map trick.
  `results/staged-no-horizon-split-full-v65/horizon-ablation.json` verifies
  binary/input hashes, CPU/allocation and the single environment difference.
- All74 frontier rows audit successfully, including raw waiting/event accounting.
  best*.json files select the measured settings; avoid relying on defaults.

## Implementation and provenance

Working source is byte-for-byte the compiled engine/simulator from **5f81613**,
restored after an unsuccessful optimization. Build-v65 binary SHA:
5c012a610f8ccae75a515ae281f3099822f76a629725af3ebc30b34b7e1bfcf3.
Its full regression suite passed13.73s, and complete trajectories passed further
worker/cache/diagnostic equivalence checks. No new unvalidated kernel is active.

Common settings: flow field15,1flip/flipseed5/contrast2.4, turn.6/wait.5,
depth8/noise200/dispersion.8, guided Hungarian1000, keep.5/length.25,
per-step RNG, continuationstart2/local0/futuremutation.3. Exact prefix/order/
cache optimizations enabled. Known horizon2000/triage1.5 in the main records.
Map-tuned guidance/horizon require --trick RANDOM-05. Started tasks stay locked.

Staged search screens each root with s complete futures, retains1/q including
anchor within quota, then completes B for survivors. Only fully evaluated
candidates can win/become parents/persist. K is exact branch-evaluation work,
group cost q*s+B-s, whole groups per generation, >=2 survivors. No wall-clock
selection: timeout exits124 instead of returning a partial portfolio.

## Final experiments and failures retained

- K5760/B12/first4608 seeds0--4:3663/3669/3732/3770/3654, mean3697.6,
  versus earlier ordinaryK5120/B8/E1/P1 mean3606 (+2.54%,5/5positive).
  Complete configurations/different work, not an isolated screening effect.
- K16320/B14/first8000 original seeds0--4:3785/3721/FAILED/3794/3852.
  Seed2 timed out at1850/1422.350ms. Declared strict repeat and5s diagnostic
  both completed3705, entire trajectories identical, means506/507ms,max564/561ms.
  Step1850 was480/481ms. Original spike did not recur; cause not proved.
  Completed deterministic outcomes with the strict repeat average3771.4,
  versus stagedK8192/B10 mean3690.6 (+2.19%,5/5positive). Preserve the original
  failed attempt; this is not five-for-five first-attempt deadline reliability.
- Retention variants nearK5760: s2/q2=3675,s2/q8=3509,s1/q4=3639,s1/q8=3737.
  None beats3770. Exact work differs slightly to fit whole generation groups.
- Headroom: B12/first4608/seed3 atK6480/6768/7056=3733/3740/3788 on32workers.
  Both direct K7056 four-core attempts fail at timestep1: mask0=1073.008ms,
  mask1=1040.146ms. No four-core score/promotion. B14/first8000/seed4/K28800
  gives3706 (mean809/max916ms), below3852 atK16320. All attempts archived.
- Setup fusion sourcefde9172/build-v66 passed regressions but was slower.
  Full32 controls EXACT3794: old505.759ms versus fused518.560ms (+2.53%).
  Fullfour controls EXACT3770:785.942 versus798.825ms (+1.64%). Rejected and
  restored source65 in **e7410cf**; preserve both full audits and failed idea.
- Binding false/close gave no useful mean speedup. 32worker pair EXACT3794
  with means503/500ms,max585/874ms; four pair EXACT3770 means788/788ms.

## Secondary accounting

Current main records' completed max1941/1940 vs NMS1997/1976. Initial unfinished
137/138 vs219/206. Every solver has initial orders unfinished at2000; eventual
max is unknown and >=2000. Throughput stays primary. FreshV4 initial unfinished
110/125 vs stronger NMS215/222; completed means are higher for ours.

Independent development replays show fewer loaded rotations and less work on
final-unfinished tasks, but more loaded waiting. Shared completed orders use
6.62%/1.15% less loaded work; assignments and congestion histories still differ.
See results/action-audit-frontiers-v65/REPORT.md; do not infer all unfinished
work was recoverable or that the horizon trick explains all of the lead.

## Reproduction and any future authorized work

Use README/RESULTS and frozen manifests. Raw binaries/traces remain under runs/;
compact metadata, hashes, failures and reports are committed. Public/private
colleague absolute counts are not matched comparisons. Fresh runs are new
streams on one layout, not evidence on other maps. No fresh input may enter tuning.

Collect split batches with split_grid.py collect; unsplit jobs write their own
root summaries. Archive only direct *.json and */*.json metadata named
summary/completion/spec/allocation/submission/batch. Never recursively copy NMS
cwd or raw outputs. Full equivalence fields:actualPaths,plannerPaths,
actualSchedule,plannerSchedule,events,tasks. On new frontiers update best manifests,
progress/waiting manifests, audit_progress.py, audit_task_waits.py and
render_waiting_report.py. Commit only random05/ and RANDOM05_PROGRESS.md.

Optional Fable review was attempted at15:01UTC but provider returned429 /
model_requires_usage_credits, zero usage, NO REVIEW. Consistent session:
27a4316e-b79d-46cf-86b4-41b0f558938a. Exact approved79KB payload is under
runs/random05/fable-review-01/payload.txt, source7506a, not current staged search.
Do not claim feedback or silently replace those authorized payload bytes.
