# Continuing warehouse work

Goal: at least 152,981 completed tasks over all 5,000 steps, repeatably over six
seeds, complete one-second decisions, and measured process RSS below
32,000,000,000 bytes. No map-name branches, aisle templates, supplied weights or
map-specific fleet caps. The local leader reference exceeds the memory target;
retain that resource difference when comparing.

Established results:

- Equal-weight 50k averages 107,413.3 across seeds 0–2. Compact seed 0 preserves
  every full trajectory field at 107,457 and reduces RSS from 16.208 to 11.883 GB.
- Strongest completed seed-0 result: 112,164 with two regional rounds, maximum
  entry 0.891 seconds. One round reaches 111,411 at 0.680 seconds. Reducing global
  work to 25k with two rounds reaches 111,997 at 0.725 seconds. All use four cores
  and about 11.82 GB RSS. No six-seed confirmation.
- Regional gains persist in the final 1,000 steps and improve outstanding-task
  age. Earlier mixed scheduler/4M work reaches 111,118 but worsens the age tail.
- Completed regional-temperature tests reach 111,573 (one round, 100), 111,289
  (one round, 0), and 112,131 (two rounds, 0). No material gain or promotion.
  Their paired default is 111,411 with an exactly preserved full trajectory.
- Warm 50k reaches 108,492 (+0.96% over 107,457), maximum entry 0.515 seconds,
  RSS 12.59 GB. Warm 25k/10k fall to 104,548/94,510. Lower work loses throughput.
- Corrected turn cost 2 yields 100,323; cost 4 fails at step 902; cost 8 fails its
  screen. Distance scales 256/1024 yield 105,891/105,811 and are not promoted.
- The v18 predecessor cache cuts isolated table-build time by 19–26%, with
  matching checksums and exact v19 full control trajectories. This establishes
  no whole-planner speedup. See ORACLE.md.

Active full queue (two independent single-core EPYC 9354 instances, 24 GiB total):

1. **8898517/8898518**, frozen v20, running on research38: 4M-work control,
   original frozen flow, margin 50, and margin 50 with freeze at 1,024 observations.
   First pair completes 109,244 (control) and 46,186 (flow), both valid full runs;
   the flow policy loses heavily. Later variants and full motion analysis remain
   pending. Early screens did not establish this behavior. See FLOW_MARGIN.md.
2. **8898535/8898536**, frozen v24: 4M-work control, weight-2 intended routes,
   reconnection at batches 128/512, and reconnection with load cost 1. All five
   screens pass; no full throughput result yet. See GUIDE_PATHS.md.
3. **8898527/8898528**, frozen v21: 50k-attempt control and intended routes with
   batch 512 and opposing costs 0/1/4. This still-pending pair was moved after
   v24; source/profiles/resources are unchanged. See results/guide-queue-update.json.

The original learned-flow strengths 1/2/4 fail late v16 deadlines. The traffic
audit finds 30.6% of directions favored after 128 steps have the opposite
cumulative majority at 1,024. Stricter margins/later observation are hypotheses,
not established remedies. The full fixed-work flow result above is negative.

V22 route search weight 2 solves all 512 attempts in its step-200 sample, but
415 deviations cause route rebuilding. V23 reconnection reduces those rebuilds;
batch 128 reconnects 2,040 deviations at step 200. Batch 512 at 50k attempts
fails at timestep 90. V24 diagnostics reproduce the failure and attribute most
work to temporal search: 69.2 million candidate inspections, preparation 0.111
seconds versus search 0.837. Fixed 4M candidate-work cases pass their screens;
full validation is queued above. This is prescribed work, never clock-selected
partial success. Equal-weight 100k also passes a screen, while 150k fails at
step 32 with 61.7 million candidate inspections. No higher-work full result yet.

Fable's approved v11 review is complete and its proven issues fixed. A new
Fable 5.1 max review through Claude Code is running on verified-public frozen
commit 00bada6. It has no tools or filesystem access and has not completed.

After each matrix: preserve failures, check all 5,000 entry samples, errors,
timeouts, actual RSS, fingerprints, final-window rates and movement efficiency.
Keep deadline screens separate from throughput evidence. Compare standalone
variants before combining them. Six-seed confirmation remains required before
claiming repeatability or meeting the leader target. The active goal is not complete.
