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
- Corrected turn cost 2 yields 100,323; cost 4 fails at step 902, cost 8 fails its
  screen. No failed case gets a partial score. Prefetch remains off after finding
  no benefit. The approved Fable v11 review is complete and its proven issues fixed.

Active full queue:

1. **8898445**, frozen v17, is running on research31. It tests one-round control,
   regional temperatures 100/0, and two rounds at 0. Two four-core instances,
   24 GiB total. Analysis **8898447** follows. See TEMPERATURE.md.
2. **8898517**, corrected frozen v20, follows temperature analysis. It tests 4M-work
   control, original learned flow, margin 50, and margin 50 with freeze at 1,024
   observations. Two single-core instances, 24 GiB total. Analysis **8898518** follows.
   Early-freeze/work screens pass; late guidance activates only in the full run.
   See FLOW_MARGIN.md. The incomplete first v20 build was canceled and never benchmarked.

3. **8898527**, frozen v21, follows v20 analysis. It compares control and generic
   intended routes with batch 512 and opposing costs 0/1/4. Two single-core
   instances, 24 GiB total. Analysis **8898528** follows. All feasibility cases pass;
   no full guide throughput result yet. See GUIDE_PATHS.md.

The v19 full warm-start matrix and analysis are complete: control 107,457,
warm 50k 108,492 (+0.96%), warm 25k 104,548, warm 10k 94,510. All pass, but
lower work loses throughput. Warm 50k max entry is 0.515 seconds and RSS 12.59 GB.
The disabled control exactly preserves the earlier full trajectory, including
v18's static reverse-neighbor optimization. See WARM_START.md.

The v16 matrix is complete: control 107,457; distance scales 256/1024 yield
105,891/105,811 and are not promoted. Flow strengths 1/2/4 all fail late deadlines.
The control trajectory is unchanged. Its traffic audit finds 30.6% of directions
favored after 128 steps have the opposite cumulative majority at 1,024, motivating
stricter margins/later observation; this does not prove the cause of failure.

The v18 static predecessor cache cuts isolated table build time by 19–26% with
matching checksums; the v19 control now validates exact full trajectories. No
whole-planner speedup is established. See ORACLE.md. Generic intended-route
guidance is implemented in frozen v21, passes the full regression suite, and
passes all five 200-step feasibility cases in **8898525**. Batch 128 guides only
36–41% of robots at step 200; batch 256 reaches 62%. A prescribed batch of 512
passes **8898526**, reaches 73–86% coverage, and is queued for full runs above.
V22 adds cached orientation lower bounds and fixed weighted A* alternatives,
passes regressions, and is in screen **8898530** (one EPYC 9354 core, 8 GiB).
No guide throughput gain is established. See GUIDE_PATHS.md.

After each matrix: preserve failures, check all 5,000 entry samples, errors,
timeouts, actual RSS, fingerprints, final-window rates and movement efficiency.
Keep deadline screens separate from throughput evidence. Compare standalone
variants before combining them. Six-seed confirmation remains required before
claiming repeatability or meeting the leader target. The active goal is not complete.
