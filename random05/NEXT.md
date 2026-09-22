# PILOT campaign continuation

Updated 2026-09-22 03:19 UTC. The all-five campaign remains active.

## Latest user steering: prioritize RANDOM-01 and RANDOM-04

Make these two the main new development priorities. All five RANDOM instances
remain in scope. Neither priority case has an established inherent ceiling.
Always compare against matched max(NMS, Kitty Knight). Keep random05/ naming
until development ends; the user explicitly deferred the rename.

RANDOM-01 now has a completed, focused twelve-case repair-granularity test.
At identical nominal agent-repair work, every alternative loses in paired total.
Keep739. Next diagnose task assignment and task-to-task transitions on the current
trace, comparing common-task cohorts and early/late phases with KK692. Loaded
route slack is already small, so avoid repeating larger-iteration or group sweeps.
Whole-chain matching and next-pickup hints already have older negative trials;
inspect those controls before proposing an interaction or new mechanism.

RANDOM-04 has six frozen forecast-memory cases running/queued. Finish them and
measure100/500/900/1000-step throughput, exact off control and all strict limits.
The selected2782 and NMS2649 tie at2417 tasks after900steps; their entire final
score difference appears in the last100steps. This locates the gain in time,
without proving its cause. Target stronger traffic performance throughout the run.

## Boundaries

Only edit/stage/commit random05/, PILOT_PROGRESS.md and RANDOM05_PROGRESS.md.
Public fywu85/lorr pushes are authorized; preserve visibility. Leave shared CGAR
files, jobs and locks alone. No subagents. Fable's provider-credit failure has
not changed; do not retry without new information.

Full horizons600/600/800/1000/2000; strict1000ms entries,30000ms preprocessing,
32decimalGB guard. Allocate16 bound physical EPYC9354 cores /32SMT workers,
verify actual affinity/topology/no quota. Shared hosts are allowed. Fixed work
completes or fails; no time-triggered partial solution. Every tuned trick needs
--trick RANDOM-0N. Throughput is primary; preserve waiting-time evidence.

Held-out50001–50012 and50015–50022 are excluded from tuning. Seeds50013/14
remain reserved/ungenerated for04. Freeze protocols before fresh generation.
No new held-out input was generated in this turn.

ACTIVE_GOAL.md governs the all-five objective and latest01/04 priority. The stored
app goal still has older03/04-only wording and04reference2580/target2838.
Do not falsely complete/recreate it; current04reference is2649 and target2914.

## Selected records and qualification

Selected739/1408/2646/2782/4302; general727/1397/1634/1616/2314.
Matched max692/1256/2359/2649/3172: +6.79/+12.10/+12.17/+5.02/+35.62%.
R04 remains132short of2914. Every selected record passes full independent replay
and strict limits. Full settings, source hashes and timestamps are pinned in
random-frontiers.json, best.json, best-32-workers.json and the progress logs.

R01=739, source162/4fb9498ef65e0b92dcb51dbbe2cd367dc9e0a4ad, plannerseed1,
progress.25/span32 on anneal.25/physicalgate1. Mean73.552/max114.527ms,
RSS388.555MB. Exact repeat; five paired seeds736/739/726/732/732 versus
732/735/725/729/730: +.3835%,all5positive,max118.656ms. Fresh01V1 qualifies729only.

R02=1408 source132/027df4d9; general1397 source141/611aa2a6. Fresh02V1
qualifies1408 at+10.17% over max(NMS,KK). More iterations4096 failed atstep1;
retain that failure. Current stronger-window/progress/third-order trials lose.

R03=2646 source162, plannerseed3, progress.25/span32 on physicalgate1 and
heuristic1.2, H20/keep14/I6144/group6. Mean484.103/max676.507ms,RSS469.467MB.
Exact repeat; five paired seeds total13084versus13029 (+.4221%,allpositive),
max730.523ms. Frozen freshV3 candidate2660/2622 versus prior2639/2612:
+.590% paired, but only+9.109688% against max(NMS,KK)2461/2380. All12fresh
runs audited; the10% fresh milestone is still missed. Seeds50021/22 excluded.

R04=2782 source144/88551e69df5b6f5ee14600dfe3a7ae8fe586783c, plannerseed4.
Cap560/all700movable, B10/depth16/K8064/FIRST4032, field15/flip5/contrast2.2,
flow20, sharedrankings2048MiB/orders1, progress.25/span32, horizon1000.
Mean478.686/max739.553ms. Exact repeat and8pairs+.340%,6gain/2loss;
a later seed11 original failed atstep0/1050.759ms. Preserve that failure even
though the new first-work batch's same-budget seed11 succeeds. General1616
face8 repeats but five-pair mean-.101%; general auction1595 gains4.312%all5pairs.

R05=4302 source164/30a5664757b8ffad1150f5eb763c747ba1001bed, seed0,
terminal_pending.5 on4254 field80 profile. Mean582.723/max830.909ms,
RSS491.999MB. Exact repeat matches six fields; candidate seeds0/1/2/3 score
4302/4250/4276/4224. Three valid controls4254/4189/4252 give12828vs12695
(+1.0477%), but disabled seed3 failed atstep0/1045.313ms. All attempts complete
and audited; full four-pair qualification is NOT achieved. Do not substitute
historical source162seed3=4253. Largest valid qualification entry956.058ms.
FreshV6 qualifies4175only and is NMS-only. All92 selected frontier/waiting rows
were audited after4302 promotion. Longest completed wait1986,146initialunfinished,
110unopened, oldest censored2000. General2314 repeats exactly and gains3.4599%
over four paired seeds,3gains/1loss,max595.950ms; still27.05% below matched3172.

## Newly completed experiments (all full independent audits)

- Local refinement source165:18/18 complete, nine six-field controls exact.
  General01..05 off/five:647/642,1106/1090,1634/1616,1616/1564,2226/2160.
  Selected04 off/five/fifteen:2782/2756/2750; selected05:4254/4189/4242.
  Both serial/parallel five-proposal pairs are six-field exact. All changes lose.
- Current layouts source162:12/12 complete, two controls exact. R03 control2646,
  noflip2614, fields23/7/31/43=2598/2514/2566/2603. R04 control2782,
  noflip2743, fields105/109/111/115=2722/2668/2688/2704. All lose.
- RANDOM01 repair granularity frozen b8109a76, jobs8921556–8921567:
  twelve full runs audited, both controls exact. Groups8/1/2/4/16/mixed8 totals
  1475/1383/1457/1457/1452/1471 across seeds1/0. Source162 unchanged.
- RANDOM04 first-work frozen c6aacea3, jobs8921537–8921545:
  nine full runs audited, control2782 exact. FIRST4032/3072/2048 gives
  seed4=2782/2724/2746, seed0=2749/2751/2746, seed11=2736/2729/2718.
  Totals8267/8204/8210. LaterK8064 unchanged. Startup/later timing is in
  random04-first-work-split-full-v162/startup-latency.json. No new record.

Reports live under random05/results/<batch>/REPORT.md. Preserve all losses,
strict failures and previous partial evidence. General and selected remain distinct.

## Active batches

1. random04-future-memory-split-full-v162: sixjobs8921569–8921574,
   frozen2c79afc1. Source162; plannerseeds4/0 x FUTURE_ELITE_BLEND0/.5/1.
   Current2782profile otherwise fixed. Report phase throughput and strict timing.
   Seed4off must reproduce all six2782fields. All positive-cap04manifests previously
   kept blend0. This is a hypothesis, with no performance claim yet.
2. random05-first-work-split-full-v164: fourjobs8921546–8921549,
   frozen8bcefa2b. Selectedcredit.5,seeds0/3 x FIRST5952/4032,laterK16320.
   These changed configurations never replace the failed4302qualification pair.

Collect live summaries before assuming these jobs are still pending. Only selected
full strict-valid independently replayed records may be promoted. R05 promotion
remains manual; the generic promotion helper excludes R05. Exact repetitions,
paired planner seeds and fresh task/start validation are separate claims.

## Builds and tools

Source162 binary runs/random05/build-v162/build/lifelong_random05,
SHA fda9057b35c275b4e7efeaa6394f321c39902c76c09d9ce16fd60386a8e3e556;
full regression54.99s. Source164 binary build-v164,
SHA4cc3280f6b38749299e68e34794e38d5c3eb6e0f45affd783c77e95afb2b9acd,
regression57.30s. Source165/513cbe8910d17f70fdce12dc517f4391e7eb729f,
binarya9a32eb1e6f07443f5865be9fb2989dd0d288abf264aec4661345dde03020a9e,
regression62.69s. Source/test/binary proofs are archived. No source166 exists.

collect: python3 random05/tools/split_grid.py collect --output runs/random05/BATCH
independent audit: audit_random_cases.py --batch BATCH --build build-vNNN --source HASH
exact controls: audit_exact_controls.py --batch BATCH --references random05/experiments/REF.json
Add --allow-pending only to the independent audit for active batches. Do not
claim exact controls until the independent audit is complete. Render the dashboard
with random05/tools/render_pilot_progress.py after updates; do not erase history.

Shell calls require escalation because the sandbox namespace is broken. Default
Python3.7; env/bin/python has NumPy. Use scoped git operations, never reset/amend
or remove other work. Check HEAD before committing. Public push:
env -u GIT_ASKPASS -u SSH_ASKPASS GIT_TERMINAL_PROMPT=0 git -c credential.helper= -c 'credential.helper=!gh auth git-credential' push origin main

Hosts: research35|research36|research37|research39|research46|research47|
research48|research50|research51|research52|research55|research56.
CPU model exactly AMD EPYC 9354 32-Core Processor. Actual allocation guards are
mandatory. Map preprocessing precedes task revelation; task-dependent matching
cannot move into initialization. Original timeout causes remain unproven.
