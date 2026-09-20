# Local assessment of Fable turn27

Same persistent Fable5.1/max session, no tools or tests. It found no concrete
scoring, first/last-goal cutoff, default-path or protected-construction defect.
The exact-source default full controls remain necessary adapter-level evidence.

Adopted two improvements to regression coverage before any build started:

- Run OFF/ON planners on their own valid trajectories with active learned tolls;
  require a real action difference, positive penalized-edge counts and both initial
  primary goals served. The first divergence has common prior observations.
- Exercise the new rejection guard with scale1 for missing temporal/flow settings,
  checking its error text. Check the exact guard for next-errand, paid-progress
  audits, branching and static-trick combinations. Some preexisting invalid
  orientation/guide combinations still correctly reject earlier.

Original queued build8899283was cancelled before execution. Production code is
unchanged; stronger tests are frozen for replacement build8899285. Neither has
produced a test or benchmark result yet. Two-sided receipt checks were already
implemented in remaining_flow/analyze.py before the review arrived.

Corrections:

- The existing temporal_region_adapter_regression already asserts nonzero
  flow_penalized_edges, expected publications and repeated cache resets. Fable's
  claim that none of the ON integration tests asserted a published penalty is false.
  Its action-divergence coverage gap was valid and is addressed.
- Retaining paid turn extras is the explicitly intended forward-only ablation,
  including non-unit/fractional turn configurations. It is not an accidental mixed
  objective. The first benchmark holds these parameters at the reference values;
  unit/function tests deliberately cover turn extras too. No extra production
  restriction was added merely to call the entire objective pure potential.
- Pinned robots keep their existing certified decisions, and the certified PIBT
  pass still uses its paid metric. Only the subsequent temporal objective changes.
- The getenv save loop now checks for null before constructing a string.

No executed-test or throughput claim follows from this read-only review.
