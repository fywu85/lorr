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
  max entries<=0.765267s, RSS<11.89GB, exact frozen controls. Ordinary refresh best134,519; strict variant later reaches135,177 on seed0.
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
memory. Independent matrices now run on separate hosts, with two, four or six concurrent
cases. Reservations scale with case count:24/48/72GiB for2/4/6cases. Actual
allocations are archived; no running job was interrupted.

- **8898637 /8898638 COMPLETE**, v32 strict wait turns: all eight valid, exact
  default controls. Frozen strict122076/121534; refresh strict135177/133672.
  Mean134424.5 is0.067% below ordinary refresh; no additive promotion. Only0.62%
  of late collapsed-seed turns are tied seed rotations; most are planned operations.
- **8898628 /8898629 COMPLETE**, v31 gentler costs1/4/8, seeds0/2.
  Scale4=121812/119927;scale8=119337/117897. All valid, exact default controls.
  They recover seed2 but lose healthy-seed throughput; remain below refresh512.
- **8898606 /8898607 COMPLETE**, v28 one/four-worker preparation, research52.
  All full hashes match v27 and within pairs. Global109244 wall1101.855/970.680s;
  regional111997 wall1371.622/1264.205s. Gains11.90%/7.83% are one pair each.
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


V33 cache-only refresh build8898651 and screen8898652 PASS; source patch hashes
verified. Full8898653/8898654 crosses frozen/cache-only512/real512 with strict0/1,
seeds0/2, six concurrent cases on24physicalcores/72GiB, exclusive research31.
All12cases use4cores and the usual individual1s/32GB limits. See FLOW_CACHE_ONLY.md.

Full8898655/8898657 separately compares refresh512,refresh256,refresh512+warm,
seeds0/2, frozen v30, all six cases concurrent on24physicalcores/72GiB, research50.
These are bounded existing-policy follow-ups, independent of cache-only diagnosis.
Six-seed confirmation8898647 is running on exclusive research57 with four cases
concurrent. All running matrices use frozen binaries, so workspace editing is safe.

Persistent Fable turn03 completed in the same verified session,397.255seconds,
$3.95705,12582bytes/four changed excerpts. It supports cache-only/strict crossed
controls and an offline dwell audit before new congestion costs. Be precise:
cache-only still uses the first learned field; a matching score would not prove
that field unnecessary. Remaining scheduler/branching ideas are untested hypotheses.
Current resource costs are recorded in WAREHOUSE_PROGRESS.md and
results/flow-refresh-resource-summary-v30.json:237–257ms mean,765ms max,
1.20–1.22average CPU cores of4reserved,11.84–11.89GB peak RSS per instance.


Offline refresh travel8898658 and dwell8898659 COMPLETE. Loaded elapsed/shortest
ratio1.1745 for refresh versus localKK1.0799; completed chain lengths242.32 vs233.84.
These are observational cohorts, not a causal decomposition. Last-window waits
are4.99%/4.86%, turns4.43%/4.44%. Top5%free cells hold26.39%/24.46%of waits, but
only22.32%/20.39%of stationary actions including turns. Strong localized congestion
is not established. Fable's density thresholds are suggestions, not statistical laws.
Audit also independently checks all5000 timing samples: p99=323.8/340.1ms.


Four refresh seeds0/1/2/3 are independently analyzed, mean134487.5, range134061–134859.
Completed subset8898668 checks new1/3 from the ongoing six-seed matrix without
modifying it. All valid, steady final windows near28200. Remaining4/5 and repeated
0/2 still running. First cache-only seed0 summaries123505legacy/123063strict are
well below real134511; complete analysis pending. Do not attribute the full refresh
gain merely to clearing the cache.

V34 read-only forward-conflict audit build8898664 PASSES; source archive reconstructed.
Screen8898667 tests off/audit64/prefetch4 on the same refresh512 policy. The audit
classifies the best lower-cost first-forward alternative for stationary ordinary
robots using the completed reservation table. It excludes own reservations and
fixed/idle roots; counts no-better,free,one/two/many movable or protected blockers.
It is a current-plan opportunity snapshot, NOT historical failure attribution.
Seven independent footprint fixtures, repeated-owner/self-owner tests, exact
subsequent RNG/search equivalence and4800production actions pass. No branching
search has been implemented. Prefetch4 is an existing latency-only option under
new flow conditions; full exact-action/timing validation remains necessary.


Refresh follow-up8898655/8898657 COMPLETE. Interval256134052/134187 slightly loses;
warm reuse134997/134794 gains0.283%mean. Exact controls. Ordinary refresh retains
broader four-seed support pending the complete six-seed analysis. No new record.

V34 screen8898667 PASS, all three exact fingerprints. Prefetch4 is unhelpful in the
cold pair:6400speculative tables,only418used,5982discarded,wall45.777vs43.259.
Full forward audit8898669/8898670 is running with stride64,seeds0/2,two cases,
4cores each/24GiB. Read-only opportunity classification, not failure history.
See FORWARD_CONFLICT_AUDIT.md and REFRESH_TRAVEL_AUDIT.md.

New existing-policy screens (no C++ change):8898671 refresh512 scales1/2/4,
8898672 refresh512 workers1x4M /4x1M /4x4M. Both use frozen v33,one4core instance,
200steps for feasibility/control equivalence. Full follow-ups require valid timing.
Worker budgets are fixed per worker; all must join, overrun is failure. No partial
completion or wall-clock-selected worker result is allowed.


New screens8898671/8898672 PASS. Scale1control and1workercontrol are exact; all
six cold entries<0.803seconds. Full refreshed scales1/2/4=8898673/8898674;
full workers1x4M,4x1M,4x4M=8898675/8898676. Each matrix runs all six seed0/2 cases
concurrently on24physicalcores/72GiB; per instance4cores/1second/32GB. See
REFRESH_COST_AND_WORK.md. No short-prefix ranking or full promotion.
Fable persistent turn04 now running via CLI exec20076, same verified UUID;
48383bytes, five changed excerpts plus newly relevant scheduler source.
