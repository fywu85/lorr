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
