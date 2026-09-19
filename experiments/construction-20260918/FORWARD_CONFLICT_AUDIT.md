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


## Completed v34 full results

Both full fingerprints match ordinary refresh512 exactly, with134511 /134519
tasks, all decisions valid. Each seed has78samples, with16in the final1,000steps.
Every partition conserves the eligible stationary-robot count.

| Late-window mean per sampled step | Seed0 | Seed2 |
|---|---:|---:|
| Eligible stationary |935.81|921.75|
| No improving first-forward candidate |283.06|283.88|
| Best candidate unblocked |1.00|0.56|
| Best candidate touches one movable owner |306.25|294.94|
| Best candidate touches two movable owners |194.50|201.56|
| Best candidate touches three or more |150.88|140.62|
| Best candidate touches a fixed owner |0.13|0.19|

The cheapest candidate is rarely free, but a slightly more expensive improving
one might be free. These counts cannot yet decide whether extra single-chain
search, a free-move sweep, or branching is the useful change. The expanded v35
audit will inspect every improving alternative, remove operation tie terms before
testing a full physical-step gain, and use stride67 to cover different phases of
the512-step publication cycle. The policy remains unchanged.

[Full fingerprints and samples](results/forward-audit-full-v34/forward-audit.json).


## Expanded audit v35

The optional audit now retains the original best-cost partition and adds the
least-conflicted alternative over all strictly improving forward choices. Any
alternative containing a fixed owner ranks after every movable-owner class.
A second partition additionally requires a full unit of the distance/turn
objective after removing the operation-index term. Merely comparing scalar
gain to50 would be insufficient: the operation term itself can exceed50.
The physical partition still measures a heuristic objective, not actual future
travel time or guaranteed throughput.

Build8898686 passes all regressions. New independent fixtures distinguish a
three-owner cheapest path from a one-owner alternative, a protected cheapest
path from a movable alternative, and a two-owner path from a free alternative.
Nine gain-boundary checks across three cost scales reject tie-only changes.
The frozen patch reconstructs every requested source hash.

Screen8898687 passes both audit-off and stride67 cases with the exact v30
200-step fingerprint. All scalar and physical partitions conserve counts at
steps67 and134. Full jobs8898688 /8898689 audit seeds0/2 with stride67, which
is coprime to512, on exclusive research38, four physical cores per case.
The policy is unchanged; no free-move sweep or branching search is enabled.

[Expanded source archive](build-provenance/v35/),
[expanded screen](results/forward-options-screen-v35/),
[expanded full configuration](forward-options-full-variants.json).


## Expanded full audit complete

Both full fingerprints match the ordinary baseline, with 134,511 / 134,519 tasks,
no timing/memory failures, and 74 samples per seed. All 296 scalar/physical
partitions conserve the stationary count. The final 1,000 steps contain 15 samples.

| Late mean per sampled step | Seed 0 | Seed 2 |
|---|---:|---:|
| Stationary eligible robots | 938.73 | 928.33 |
| No full-unit improving forward option | 359.00 | 356.33 |
| Easiest full-unit option has no owner | 4.00 | 3.87 |
| Easiest full-unit option has one owner | 355.93 | 349.40 |
| Easiest full-unit option has two owners | 175.47 | 171.40 |
| Easiest full-unit option has three or more | 44.27 | 47.20 |

Scalar-only free options are 37.20 / 36.33 per step, mostly below the physical
threshold. A free-only physical sweep has little measured immediate opportunity.
Two-owner search still must show positive net displacement benefit and actual
full-run throughput. These are not mathematical bounds on later traffic effects.

[Full exact equivalence and samples](results/forward-options-full-v35/forward-options-audit.json),
[controlled transaction experiment](TEMPORAL_TRANSACTIONS.md).
