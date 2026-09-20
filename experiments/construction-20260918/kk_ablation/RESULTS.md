# Full guidance × unopened-rematching diagnosis

All five runs finished by2026-09-20 02:14:44UTC; complete accounting finished
02:16:52UTC and pool replay02:18:56UTC. Each uses10,000robots/5,000steps,
the frozen V2 competitor binary and a separate exclusive16physical/32logical
EPYC9354 allocation. Four action fixtures, all five50-million-step identities,
source/binary/config/input hashes and assignment protections pass. The pool replay
reproduces every original accounting field exactly.

**Every run records one entry timeout and38.18–38.85GB RSS.** These are completed
competitor diagnostics, not admissible CGAR scores. The historical valid local
KK target remains152,981; it is not replaced by these timed-out controls.

| Guidance | Unopened assignments | Completed tasks | Completed chain | Loaded detours + turns + waits | All empty work |
|---|---|---:|---:|---:|---:|
| Supplied weights | Rematch, control0 | 152,872 | 233.834 | 18.830 | 65.734 |
| Supplied weights | Rematch, control1 | 152,786 | 233.806 | 18.987 | 65.794 |
| Supplied weights | Keep | 142,510 | 235.007 | 18.183 | 88.852 |
| Uniform weights | Rematch | 24,574 | 189.346 | 408.202 | 546.639 |
| Uniform weights | Keep | 23,620 | 197.657 | 479.220 | 687.006 |

The final three columns are robot steps per completed task. This compact table
omits unfinished work and timeout action markers; the
[complete accounting](../results/kk-guidance-rematch-full-v2/comparison.json)
retains them and conserves all robot steps. The two control totals differ by86,
which is an observed diagnostic difference, not a confidence interval or a pair
of qualifying controls.

## What this supports

The supplied weight field matters enormously to this configured KK system.
Uniform-weight rematching collapses even though its completed task chains are
shorter. Loaded waiting reaches300.265steps per completion, turns72.831,
detours35.106, and unfinished loaded work890.299. Task-chain selection alone
cannot describe that loss. The configuration is not retuned for uniform weights.

With supplied weights, keeping unopened assignments reduces completions by
10,362relative to control0. Completed-chain and loaded movement overhead remain
similar, while empty work increases by23.118steps per completion. This supports
investigating rematching as a way to reduce empty work under short-chain-oriented
assignment. It does not establish that CGAR will gain the same amount: CGAR's
current empty work is already about64.295steps per completion.

The contrast is not explained by less reported search time alone. Mean remaining
LNS time is0.874s supplied/rematch versus0.877s uniform/rematch. Mean merged
iteration records rise from21,142to40,872, with more failure records in the
uniform run. Those records are not all speculative attempts or a normalized
measure of useful search. Mean scheduling times are25.75/27.33ms, respectively.
Keeping assignments saves scheduler time, giving the planner a slightly larger
budget; this does not prevent the keep arm's empty-work loss.

## What remains confounded

The same weights affect task costs, selected cohorts, route guidance and the
amount/outcome of clock-limited search. This is a whole-policy weight intervention,
not an isolated causal estimate for motion guidance. Two controls and one run per
other arm do not establish generalization or statistical significance. The
diagnostic entry print excludes its own overhead, so there is no certified
complete1sentry series for these competitor runs.

Unassigned-task age also differs substantially. At step5,000 supplied/rematch
controls have mean unassigned chain527.55–527.77and age2,703–2,707;1,632/1,643tasks
remain unpicked after4,000steps. Supplied/keep has1,377such tasks. Uniform/rematch
has5,360, uniform/keep4,268, reflecting their congestion and incomplete work as
well as task selection. [Pool evidence](../results/kk-guidance-rematch-full-v2/pools/cohorts.json).

## Next decision

Prioritize guidance quality for the generic investigation while retaining the
separate assignment/rematching hypothesis. Complete the running current-state
joint-motion probe before another structural search rewrite.

The user's newly authorized trick track can test a static warehouse guidance
field separately, solely under `--trick WAREHOUSE`, with `[trick]` commits and
TRICK log entries. NMS lane rules or KK weights are possible starting points;
copying a field does not copy its original scoring or scheduler. Record cost
units, pickup/chain coupling, static-cache effects and all resource checks.
The generic frontier remains144,510single/143,941.2six-seed mean; there is no
CGAR trick result yet. [Track policy](../EXPERIMENT_TRACKS.md).
