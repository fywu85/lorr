# Checked assessment of the Fable 5.1 Max review

The review ran through Claude Code CLI with model `claude-fable-5-1`, effort `max`,
and read-only Read/Grep/Glob tools. The unedited [review](review.md), [prompt](prompt.md)
and [metadata](metadata.json) are retained. It primarily reviewed frozen build v4;
subsequent fixes and experiments are independently identified below.

- **Initialization exceptions:** confirmed. `planner_initialize()` did not consume
  the initialization future, and `Cgar::initialize()` marked itself ready before
  potentially throwing. Build v7 sets readiness last, consumes the future, and
  makes the entry exit 125 for initialization exceptions or 124 for timeout.
  Four process checks cover incompatible guidance, insufficient affinity,
  preprocessing timeout and successful initialization. Failed cases produce no
  score or preprocessing-success message. A regression also retries an object
  after failed initialization and obtains valid actions.
- **Ambiguous timeout stage:** confirmed and fixed in v7. Construction and repair
  have separate labels. Earlier `temporal_construction` failures cannot be
  retrospectively assigned to one phase from that label alone.
- **CPU allocation:** the check only tests logical affinity size. Its wording now
  says so. The GRID harness reserves cores, selects one logical CPU per physical
  core, and gives each concurrent process a disjoint affinity set. Serial versus
  four-thread full trajectories match exactly. New optional host and CPU-model
  controls prevent silently mixing EPYC generations in deadline comparisons.
- **Stack concern:** the 1,500-robot chain now runs on a `std::thread` stack and
  reaches depth 1,499. One correction to the review: recursion is not bounded only
  by the configurable root limit. Construction's `counter > 3000 && depth >= 6`
  guard prevents descent beyond depth 3,001, and repair's effective counter limit
  is at most 1,000. This source bound and finite test do not establish safety for
  arbitrarily small externally configured thread stacks.
- **Idle blocker hypothesis:** an additional fixture leaves the target-cell
  occupant idle permanently. The protected primary reaches its goal in three
  steps without a transaction. This exercises the missing case; it is not a
  general latency or liveness proof and does not justify changing pinning rules.
- **Timing anomaly:** the cross-process timestamps support a pause after planner
  completion, during or after simulator logging. The system cause remains
  unidentified. Exact entry-completion timing now accompanies the original outer
  timing series. Simulator verbosity is configurable, and the validated repeat
  uses warning-level logging.

The review found no kernel safety or threading defect by inspection. Independent
evidence includes native construction/repair conformance on 15,360 robot decisions,
randomized five-layer collision checks, protected-primary/pocket/recovery/capacity
fixtures, and full serial/parallel trajectory equivalence. Warehouse has no
pockets; its full runs do not validate the pocket mechanism on other maps.

The search-work and scheduler proposals are hypotheses. Deterministic candidate
work is now opt-in and undergoing deadline screening; it is not a validated
replacement for 50k attempts. The completed travel decomposition confirms a
large loaded-motion gap and explicitly accounts for the leaders' reassignments.
No parity-based lane template or supplied map-specific weights were adopted.
