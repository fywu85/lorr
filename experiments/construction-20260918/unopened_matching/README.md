# Bounded unopened matching in CGAR

The experiment tests whether permuting already-assigned unopened tasks can reduce
empty travel without changing ordinary task selection. `CGAR_REASSIGN_MATCH=1`
is generic and default OFF. Current tested source is
[1d78892](https://github.com/fywu85/lorr/commit/1d788929db09f8f6a0aeb091548ff8c3ff7aba87).

Every10steps after a completed flow publication, collect eligible unopened holders
with complete resident oriented pickup tables. Preserve primary/next-oldest,
started/at-pickup, recovery/transaction, parked/locked, core/commitment, fair-admitted,
and cooldown exclusions. Each task can be retargeted once before pickup; robot
cooldown is20steps. No free tasks are released and simulator metadata is unchanged.

Rotate at most128anchor IDs, then search current physical neighborhoods using all
resident eligible holders. At most4disjoint groups,32holders/group and2048BFS
pops/group bound the extra matching work. Thus at most128matrix participants per
pass. The first version instead indexed only the128anchors, scattering the search
pool and starving local groups. The production regression places a useful partner
outside that anchor window and verifies it participates after the correction.

Each group uses Hungarian assignment with a finite identity: pickup cost is the
primary objective, fewer changed tasks breaks ties. Nonfinite edges are forbidden.
Commit only disjoint cycles saving at least4*cost_scale and10% of the cycle's
original pickup cost. Analyze all groups before mutation; an expired entry fails
explicitly. Estimated weighted savings are not measured travel-time savings.
The pass builds no tables and makes no RNG draws. Initially incompatible with
tricks, old reassignment/pool, guide routes, chain pricing and remaining-flow score.

## Evidence

- [V61 800-step feasibility screen](../results/unopened-matching-screen-v61/assessment.json):
  seeds0/2, control18954/18952; matching18921/18942. All4valid, max929.471ms,
  peak8.169GB. Its sparse spatial groups motivate the correction; no full ranking.
- [V62 exact source/build binding](../build-provenance/v62/exact-source-commit.json)
  and [regression result](../build-provenance/v62/regression-result.txt): all27source/test
  hashes match; the full suite passes, including permutation/brute-force, actual
  matching, outside-anchor partner, bijection, fixed limits and metadata checks.
- V62 screen job8899358 runs all4cases concurrently on16reserved physical cores;
  independent analysis8899359 verifies source/binary, disjoint actual allocations,
  prescribed planner work, matching limits and exact V61 OFF trajectories.
  Raw: `runs/cgar-local-match-screen-v62-20260920`.

`analyze.py` freezes its support files and submits GRID analysis for either800-step
screens or5000-step full pairs. Full pairs include complete movement accounting,
explicit failure handling, exact six-seed reference controls, tail throughput and
outstanding task ages. Early diagnostic samples are not final pass totals.

V62 screen complete: OFF18954/18952, ON18919/18944 on seeds0/2, all valid;
max927.438ms, peak8.074GB. Both disabled trajectory fingerprints exactly reproduce
V61. At t400,111/112groups reach32holders in each matching case. This fixes
coverage but does not establish a throughput gain. Full0/2 OFF/ON is queued as
8899360, four cases in parallel on16cores,5s/32GB; held analysis8899361.
[Independent screen checks](../results/local-match-screen-v62/comparison.json).

## Protection correction and exposure study

Fable turn33 identified stale-ticket selection of the next primary; local follow-up
found missing registration of mandatory fair-admitted tasks when only matching was
enabled. V62 full8899360/8899361 was cancelled. No partial quality result is accepted.
[Cancellation](../results/local-match-full-v62-cancelled/cancellation.json).

[f189cbf](https://github.com/fywu85/lorr/commit/f189cbf) fixes both: only existing-held
episodes nominate next-primary protection; fair admissions register under matching.
Fresh ordinary tasks remain eligible. The new production fixtures fail against the
old code and pass after the repair; the full suite passes.
[Exact build evidence](../build-provenance/v63/exact-source-commit.json),
[regression result](../build-provenance/v63/regression-result.txt).

The next exposure experiment adds `CGAR_REASSIGN_MATCH_GROUPS` in1..64, default4.
A nondefault quota requires enabled matching. The other constants stay fixed:
interval10,128anchor candidates,32holders/group,2048BFS nodes/group, one retarget
per task and20-step robot cooldown. The64-group arm has at most2048participants
and131072BFS pops per pass. It may visit fewer due to eligibility, prior group use,
anchor coverage and geography. No elapsed-time decision changes the quota.
`quota-variants.json` contains OFF/4/64 with otherwise identical reference settings.
The larger-quota production fixture verifies participation beyond the old128 cap,
bijection and primary protection. The larger quota is a hypothesis, not a gain.

## Repaired quota and explicit trick trials

V64 source [0196851](https://github.com/fywu85/lorr/commit/0196851) passes the full
suite. The [six-case quota screen](../results/match-quota-screen-v64/comparison.json)
is valid with exact controls. OFF/4/64 are now in a paired full comparison; prefix
scores do not establish a throughput gain. The generic selector stays OFF.
The [separate explicitly gated Warehouse factorial](../tricks/unopened_matching/README.md)
uses V65 source 5a8a51a. Historical V62 full statuses above are superseded by its
cancellation and the corrected V64/V65 comparisons.

## Optional mixed pickup neighborhoods (V66)

Source [5dc6ae4](https://github.com/fywu85/lorr/commit/5dc6ae4) adds
`CGAR_REASSIGN_MATCH_PICKUP_GROUPS=1`, default 0 and requiring matching enabled.
A group contains up to 16 nearby robots and up to 16 other eligible holders whose
pickup cells are nearby. The same BFS finds both streams, with at most 2,048
popped cells and 32 total holders per group. Group quota, cadence, exact resident
quotes, cycle thresholds, primary/fair protection, one retarget and cooldown stay
unchanged. The two indexes use current positions and already revealed pickups;
there is no map-specific rule. Sparse groups may leave one side underfilled.

The production regression uses far crossed holders outside either local BFS
neighborhood: local groups preserve identity, while mixed groups find the useful
swap. Both respect work bounds and simulator metadata. Invalid modes or activation
without matching are rejected. The full regression suite passes.
[Build binding](../build-provenance/v66/exact-source-commit.json).

`pickup-group-variants.json` isolates this grouping switch with matching enabled
at 64 groups in both arms. `analyze.py --pickup-groups` requires exact local-arm
trajectories against the V64 quota study and verifies nonzero pickup-index use.
800-step feasibility screen: jobs 8899396/8899397, paired seeds 0/2 on 16 reserved
physical cores, shared 5s development limit. No throughput gain claimed yet.
