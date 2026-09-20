# Assessment of persistent Fable turn33

Completed2026-09-20T07:39:36.942468UTC in the existing session. Model/effort receipt
confirms claude-fable-5-1/max, no tools or permission denials. This is a source
review and interpretation of supplied measurements; it ran no tests.

## Accepted correctness findings and local extension

The new include_fresh oldest scan compares a just-completed robot's stale ticket
before plan/sync_agents assigns its new task a fresh ticket. That can mask the
true next-oldest held unopened task. Fix by considering only already-held current
episodes for next-primary protection; freshly assigned tasks can remain eligible
in the separate candidate scan.

Local follow-up also found the fair-admission registration condition omits
reassign_match_. Although candidate filtering checks fair_tasks_, the new mode
alone never inserts its mandatory admissions. Extend that condition and exercise
an admission and matching pass in the same schedule call.

New production regressions cover stale-primary turnover, allowed fresh-holder
participation and mandatory fair admission, with simulator metadata assertions.
The first build is deliberately against the unfixed production source to check
that the fixtures reproduce both defects. The full V62 run8899360 and held
analysis8899361 were stopped; preserve cancellation.json and do not interpret
partial task totals as quality evidence. The disabled reference is unaffected.

## Useful hypotheses, with qualifications

- Four groups per10ticks provide at most128participant slots/pass. This is a real
  exposure ceiling, but slots are not unique task-life exposures: repeat selection,
  initial stock, cohort completion and eligibility matter. We have not measured
  per-task coverage or age at first exposure. A larger fixed quota is worth a
  controlled trial; the proposed15–25ms extra latency is unmeasured.
- Indexing all resident holders fixes the sparse-neighbor bug. Robot-nearby groups
  can still miss useful pickup-nearby partners. The directed state-distance triangle
  bound is valid under one immutable metric; geometric group diameter is not the
  same as weighted oriented diameter, and weighted units divided by scale are not
  realized steps saved.
- The quoted CGAR/KK/NMS empty-work ratios do NOT establish a two-step upper bound
  on rematching improvements. They involve different cohorts, assignment policies,
  guidance, timing and resources. No ideal-rematching ceiling or seed-noise claim
  follows from that comparison.
- Track per-row losses in accepted beneficial cycles if exposure improves without
  throughput: a group improvement can spend a losing row's one-time retarget.
- Explicit TRICK lanes/short-preference/matching is a plausible interaction test,
  not a predicted recovery of21.7steps. A permutation preserves the currently held
  task set but can change future completed cohorts. It is not cohort-neutral in
  that stronger sense. Any implementation needs the explicit CLI component gate,
  the static-metric activation condition and new regression/receipt coverage.

The priority is to repair and verify the two protection defects, then measure
bounded exposure before widening combinations. No new best or generic promotion.

Reproduction result: build8899362 fails both stale-next-primary and missing-fair
checks against V62 production. The positive fresh-holder test also needed a setup
correction to avoid a pending-move interaction with the primary; that is a test
fixture issue and is not counted as an additional production defect.
