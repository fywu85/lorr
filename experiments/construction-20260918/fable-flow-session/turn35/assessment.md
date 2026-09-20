# Assessment of persistent Fable turn 35

Completed 2026-09-20T09:08:18.723221 UTC in the same Fable 5.1/max session,
with no tools or permission denials. Source review, not independent testing.
The V68 delta reads correctly. Actual local tests and full verification now show
147,422/147,502 against exact146,659/146,566, with both late windows improved.

The immediate next experiment combines static remaining-potential scoring with
bounded matching. Only the explicit static-lane combination is allowed; generic
remaining-flow+matching stays rejected. A production regression checks a real
beneficial cycle under both scoring modes, primary/metadata/finite-retarget
protection and a valid post-swap plan. V72 build is pending; no combination gain
is claimed. Compare against both single-component policies, not only lanes-only.

Fable recommends one native-cost fidelity experiment after this positive V68
result: forward20/opposed200, turn1, raw goal-slot and operation units, pure
remaining-potential score; optionally the separate native +1 row-band costs.
This is a compound metric/scalar/scheduler repricing, not an isolated cheap-turn
causal test. First keep matching and short preference OFF. Preserve exact defaults,
explicit CLI gating, fixed work, complete tables, timeout failure and32GB.

Local qualifications to carry into implementation:

- "Last cost-model difference" and "near-identical kernels" do not imply the
  planners/policies are equivalent. CGAR retains different protection, repair,
  scheduling, cache, fallback and search mechanisms.
- The bounded pickup search is heap-based; only reverse-oracle and complete-pickup
  searches use Dial buckets. Field costs and integer bounds must be validated in
  every consumer. Native bucket maximum is200 without bands,201 with bands;
  capability ceiling255 is separate from active maximum.
- Native scalar tie and last-hit service units must be implemented explicitly.
  Setting physical unit20 and guidance turn1 in the old paid-extra formula would
  introduce negative turn extras. A separate pure-potential function avoids that.
- More expensive fields can alter expansions, heap churn, cache contents, RSS,
  table fallbacks and latency. Bounds and real resource checks matter more than
  an assumed small complexity increment or an unverified real-distance ceiling.
- A metric loss does not establish that the entire motion gap lies elsewhere.
  Nor do reduced loaded overhead plus increased empty work causally prove that
  scheduler repricing caused the loss: completed cohorts and traffic diverge.
- Native observed contraflow/band replay can guide whether to test bands; it is
  diagnostic and cannot rehabilitate the archived NMS timeout/resource comparison.
