# Read-only forward-conflict sampling, v34

Optional `CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE` (default0, range1–4096) samples the
completed temporal reservation table. For each ordinary, goal-bearing robot whose
first action stays in the same cell, it finds the lowest-cost first-forward
alternative strictly better than the selected operation. It partitions these robots
into no such alternative, no other owner, one/two/three-or-more non-fixed owners,
or at least one fixed owner. Repeated owners are deduplicated; the robot's own
future reservations do not count as blockers.

This is a snapshot of opportunities in the finished plan, not a trace of why an
earlier attempt failed. Stationary actions include useful turns. A non-fixed owner
is not guaranteed to have a feasible displacement. The samples therefore do not
prove a causal loss, estimate recoverable throughput, or by themselves justify
branching search. Sampling is systematic at fixed steps, not randomized.

The method is const and consumes no randomness, changes no reservations and
performs no speculative search. Build **8898664** passes the full regression suite:
seven independently constructed blocker fixtures; repeated-owner and own-future
reservation checks; strict-cost boundaries and fixed/idle exclusions; identical
subsequent RNG/search results; and4800 protected serial/parallel production actions
with a fixed sample schedule. The archived source patch reconstructs all hashes.

Screen **8898667** compares ordinary refresh512, audit stride64, and the existing
four-thread prefetch option. All three exact paths/schedules/events/tasks match the
old200-step control fingerprint. Audit frames occur at64/128/192, and every
category sum equals the eligible stationary count. All cases pass1-second decisions.

| Profile | Wall seconds | Max entry seconds |
|---|---:|---:|
| Control | 43.259 | 0.784978 |
| Audit every64 | 43.340 | 0.754873 |
| Prefetch4 | 45.777 | 0.743643 |

Prefetch built6400 speculative tables, used418 and discarded5982 (93.47%). Its
cold wall time increases5.82% in this single pair, despite a lower maximum entry.
It is not enabled in the leading profile. This result does not establish its full
warm-run timing, and no full prefetch speedup is claimed.

Full **8898669 /8898670** samples ordinary refresh512 at seeds0/2,5000steps,
four physical cores each, two concurrent cases under24GiB allocation. It must
reproduce the already measured full fingerprints before interpreting the samples.
No branching policy is included in this change.

[Build](build-provenance/v34/), [screen](results/forward-audit-screen-v34/),
[full configuration](forward-audit-full-variants.json).
