# Continuing warehouse work

Goal: at least 152,981 tasks over all 5,000 steps, repeatably over six seeds,
complete one-second decisions, process RSS below 32,000,000,000 bytes. No map-name
branches, aisle templates, supplied weights, future tasks or map-specific fleet caps.
The local KittyKnight target used 16 physical / 32 logical CPUs and 38.858 GB RSS;
this is a local reference, not an equal-resource official score. Goal remains active.

Maintain ../../WAREHOUSE_PROGRESS.md after each full evaluation, including failures,
actual completion timestamps, verified source/archive commits and seed confirmation.
Never present a dirty build's base HEAD as the exact tested source.

## Verified results at the resumed checkpoint

- No-flow fixed 4M: 109,244 / 109,249 / 109,182, mean **109,225** over seeds0/1/2.
- Frozen margin50 flow: 122,896 / 122,195 / 70,171, mean105,087.3. Do not promote.
- **Refresh512: 134,511 / 134,519 over seeds0/2**, final windows28,215 /28,214,
  max entries<=0.765267s, RSS<11.89GB, exact frozen controls. New best134,519.
  Refresh1024:133,652 /131,316. All six full cases pass. See FLOW_REFRESH.md.
- Frozen flow with warm reuse:123,373 /123,251 over seeds0/2, both valid,
  exact controls. Recovers bad seed but below refresh512. See FLOW_WARM_REUSE.md.
- Nearby frozen-flow margins25/75 and strength2 lose to margin50 at seed0;
  all valid, no promotion. Corrected guide profiles all lose badly; remain off.
- Regional best seed0:112,164. Global25k/two rounds:111,997. Broader seeds pending.
- V27 regional candidate prefilter preserves both FULL trajectories109,244/111,997.
  Isolated regional kernel gain6.9%; no isolated full speedup claim.
- Compact turn tables preserve trajectories and lower RSS16.208GB to11.883GB.
- Independent flow audit: neutral directed graphs are strongly connected on all
  three seeds. Seed2 fails through slow/churning movement; not mass static deadlock.

## Active queue

Matrices run concurrently on separate exclusive GRID hosts. Each instance has
four reserved physical EPYC9354 cores and no CPU quota. Previously serialized
matrices unnecessarily treated the 32GB per-process rule as aggregate experiment
memory. Three independent matrices now run simultaneously, two cases each under
24GiB per matrix. Actual allocations are archived; no running job was interrupted.

- **8898637 /8898638**, v32 strict wait turns: refresh0/512 crossed with strict0/1,
  seeds2 then0, exclusive research46. Four seed2 summaries pass: frozen70,171
  ->121,534 strict; refresh134,519 ->133,672 strict. Full analysis/seed0 pending.
- **8898628 /8898629**, v31 gentler costs1/4/8, seeds0/2, research44. Still running.
  Four initial summaries pass; no completed full analysis or promotion yet.
- **8898606 /8898607**, v28 one/four-worker preparation, global4M and regional,
  research52. Global summaries109,244 each, wall1101.855/970.680s. Full trajectory
  analysis and regional cases pending; do not treat this single timing pair as robust.
- **8898647 /8898648**, v30 refresh512 confirmation versus no-flow on ALL six
  seeds1/3/4/5/0/2, follows8898628/8898629. Four concurrent cases on16 physical
  cores under48GiB matrix allocation. Both policies use the same frozen v30 binary;
  repeated seeds0/2 additionally check reproducibility. All cases5000steps/1second.

Completed refresh8898624/8898625 and warm8898632/8898633 analyses include every
trajectory/entry/movement record. Frozen controls match previous full hashes.
New record134519 has exact source e14ecfd and actual completion11:26:47UTC.

## Current source and next decisions

V32 build8898635 and screen8898636 PASS. Source archive patch reconstructs all
requested hashes. Default-off CGAR_TEMPORAL_STRICT_WAIT_TURNS retains heading
on equal-cost wait-seed ties, plus separate seed/tied/planned/protected counters.
Independent blocked fixtures reproduce128 oscillating turns, prevent them all,
retain16 useful turns; production episodes validate9600 actions including warm
reuse, refreshed flow, protection, parallel preparation and counter conservation.
Default screen fingerprint is unchanged. Full effect above is still being analyzed.
C++ sources may now be edited; all running matrices use their own frozen binaries.

Next: isolate changed metric from cache flush/fallback effects using a fixed-field
cache-only control. No future-run field transplant in eligible performance runs.
Then test combinations only if full evidence supports them. Refresh and strict
turns are not automatically additive. Keep defaults unchanged until validated.
No time-dependent early return or partial-action publication; overrun is failure.
After full runs inspect windows, task age, exact fingerprints, RSS and all timings.

## Persistent Fable consultation

Use Claude Code CLI, model claude-fable-5-1, effort max, SAME session
1ebb1075-3538-49d1-93d1-a00c94fa256a. Explicit standing user permission covers
relevant project information and source. Use fable-flow-session/run_review.py,
which sends changed source diffs and new findings; raw protocol/session data stays
in ignored runs/. Preserve only visible reviews/metadata in Git.

Turns01/02 completed successfully. Turn02 sent5337bytes and ZERO changed source
files. Fable retracted rank-weight, irreversible-flow-ratchet and age-onset claims;
our independent tests/results remain authoritative. Wait-turn issue independently
reproduced. Cache-only ablation remains useful. Multi-blocker search should first
measure rejection causes, rather than assuming saturation. See assessment.md.
