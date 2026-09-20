# Late rematching and the configured cutoff

The V109 read-only audit compares each visibly retargeted task's old and new holders **at the same accepted decision**. Both pickup distances and every remaining task leg use independent complete unweighted BFS. Repeated stops consume separate service steps; equality with the remaining horizon is allowed. First-admission positions are not substituted for the departing holder's current position.

The audit also trains a prospective p90 excess-duration model from already completed single-holder tasks, using five physical-bound buckets. Same-tick completions and retargeted completions are excluded. Its exact spatial basis differs from production's mixed cached/Manhattan estimates; the audit's tiers are not a replay of the production comparator. Fresh permutations within one scheduling entry are invisible in the saved schedule, so this is not a complete account of matching cycles.

Seven targeted fixtures cover a harmful handoff, a rescue, current departing position, repeated service and equality, prospective training, same-tick exclusion and retargeted-training exclusion. The existing physical-work fixtures also pass. Both complete 50-million-robot-step accounts match their frozen V99 references and raw hashes. No production source or saved trajectory is modified.

V109 completed 2026-09-20 19:39:18 UTC, GRID8900541. The best seed0/2 runs have 15,803 /15,725 visible one-time retargets. Of these, 47 /63 move from a distance bound that fits the remaining horizon to one that cannot fit; 35 /44 go the other way, of which21 /14 subsequently complete. Diagnostic p90 tiers worsen for148 /167 and improve for168 /180. These observations do not imply a net loss from matching or a recoverable completion count. Any changed cycle can affect all of its tasks and later traffic.

## Proposed policy comparison

Keep the current p90/pickup8/ten-step profile fixed. Add a default-OFF `CGAR_TRICK_MATCH_HORIZON` option requiring explicit `--trick WAREHOUSE`, configured horizon, matching and ordinary fairness. After each bounded matching solution, inspect only cycles that already pass the existing gain threshold. Reject the entire cycle if any task would move to a worse current completion-feasibility tier. Compare the new holder to **that task's old holder**, not the row's previously held task. Keep unchanged cycles, identity fallback, primary/recovery/started/forced-oldest protection, one retarget and20-step cooldown.

Use the same frozen p90 snapshot for the whole matching entry. Bounds read only existing spatial tables, falling back to Manhattan without table construction or LRU mutation; every task's chain estimate is shared by its two holder comparisons. Before and after the configured horizon, the new filter follows the existing horizon activation rule. This is a conditional heuristic, not a guarantee of completion. It can reject a beneficial multi-task cycle and must be judged by full runs.

Require complete regressions, native active-cutoff screening, exact generic/default replay, then paired full seeds0/2 with unchanged controls. Archive all losses as well as gains. The independent read-only audit alone does not establish a new throughput result.

## V110 implementation and validation

Source `37f592a48bdb24b936f8d0be8be22ddd405b4ddb`; frozen binary `3f66eefd2a54589e48965b5b4031cbbe4d67bd8e6423c8cdc1cb3108f24bf641`. GRID8900579 passes the complete regression suite, with28exact source/test hashes and no source patch. Pure fixtures cover the task-column comparison, whole-cycle rejection, independent cycles, threshold-rejected cycles, invalid tiers and timeout propagation. Native closed-loop pairs replay24,576independent physical actions, repeat errands, preserve started tasks/one-retarget, and match exactly across preparation thread counts. Those small episodes check3accepted cycles but veto none; the larger screen must exercise a real veto. Missing and explicit-zero settings reproduce identical native traces.

The native screen (GRID8900591, verifier8900594) compares an unchangedH5000reference prefix with guardOFF/ON under configuredH200, all for800physical steps. It must reproduce the known reference exactly, reject at least one real cycle underH200, and stop updating filter counters afterH200. The separate generic800compatibility replay is GRID8900592/verifier8900595. All cases use4disjoint reserved physical cores per process, complete fixed work, the shared-host5sdevelopment deadline and32decimalGB RSS. Full5000paired comparisons follow only after both screens pass.
