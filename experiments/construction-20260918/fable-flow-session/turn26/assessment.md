# Local assessment of Fable turn26

The review used the same Fable5.1/max session, with no tools or test execution.
It supports a default-off generic remaining-flow scoring ablation, with a neutral
to slightly negative prior. This is a falsifiable experiment, not an expected win.
The path-dependent payment is not an additive per-robot constant: omitting it
changes both candidate ranking and conflict allocation. It can reward a tolled
move with regret smaller than the omitted toll. Existing protected primary,
spatial recovery and certified PIBT comparisons remain outside this score change.

Adopt independent macro-action score checks, neutral-field and prepublication
production equivalence, invalid-combination checks, serial/parallel agreement,
and protected-primary/pocket/capacity regressions. Reject static trick, guide,
next-errand and paid-progress audit/transaction combinations in this first ablation.
Use full matched seeds0/2; only advance a valid improvement to more seeds.

Corrections and limits:

- Removing a nonnegative term lowers the numeric cost; its absolute magnitude
  can grow when the cost is negative. Overflow safety follows bounded five-action
  tolls and int64 arithmetic, not a magnitude-shrinking argument.
- Fable's three different one-step forward choices are not available from one
  oriented robot state. Test legal five-action macros instead.
- The replay measures executed toll shares1.502–1.657%, not an estimate of how
  many candidate rankings will change. Candidate alternatives can be tolled too.
- Early differences at625steps are1764/1865, versus final2149/2459 (82.1%/75.8%).
  At1000they are1977/2011 (92.0%/81.8%). Coarse25-step curve bins cannot give the
  exact difference at publication640. This motivates studying startup, but does
  not identify no-field time, cache refreshes or pickup quotes as the cause.
- The trick includes NMS-derived direction changes in the special bands; native
  extra band costs are omitted. Saying all bands are untested is too broad.
- Similar tails do not prove the learned field itself is as good as static lanes.
  Different scoring, assignment, cache and task cohorts can offset one another.
  Empty-step and loaded-overhead differences do not establish causal cancellation.
- More opposing movement alone is not a rejection criterion. Evaluate full totals,
  consistency, deadline/memory validity, tails and task-age tradeoffs together.

No production change, throughput benefit or six-seed promotion follows from review.
