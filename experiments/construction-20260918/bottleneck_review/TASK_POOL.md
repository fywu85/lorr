# Task-pool age and chain-length diagnosis

Verified 2026-09-20T01:11:04.641026UTC, GRID8899247. This replays the saved local
KittyKnight run and all six CURRENT regional CGAR references. It adds end-of-tick
task-pool snapshots without changing any original travel-accounting field.
All seven raw input hashes agree with prior independent replays; the four original
fixtures and a new hand-counted ownership/reveal/pickup/completion fixture pass.
[Cohorts and verification](../results/task-pool-audit-v50/cohorts.json),
[provenance](../results/task-pool-audit-v50/provenance.json),
[reproducer](pool_audit.py).

The result supports a **persistent difference in task selection and waiting**, not
just a different set of tasks caught mid-delivery at the final horizon. It does
not establish how many extra completions CGAR would gain from another scheduler.

| End of step 5,000 | KK, one run | CGAR, range across six seeds |
|---|---:|---:|
| Completed shortest-chain mean | 233.836 | 241.843–241.875 |
| Currently unassigned tasks | 5,025 | 5,021–5,044 |
| Unassigned whole-chain mean | 527.639 | 306.201–307.313 |
| Unassigned mean age since reveal | 2,706.9 | 100.5–104.0 |
| Unassigned age p90 | 4,877 | 209–214 |
| Unpicked tasks aged at least 1,000 | 5,130 | 0 |
| Unpicked tasks aged at least 4,000 | 1,642 | 0 |

"Unpicked" includes held unopened tasks as well as unassigned tasks, excluding
every task already carrying a load. All cases maintain 15,000 outstanding tasks;
new tasks revealed at the sampled tick are included. The approximately5,000
unassigned count includes robots freed by that tick's completions. It is not
exactly the next scheduler's pre- or post-assignment pool size.

KK's unassigned mean chain grows from **480.23 at step1,000** to **513.97 at3,000**
and **527.64 at5,000**. Its corresponding mean ages are641.6,1,667.5,2,706.9.
CGAR seed0 stays at305.82/310.15/306.20 chain and105.4/99.9/100.5 age at those
checkpoints; all six CGAR seeds show the same qualitative pattern. None has an
unpicked task aged at least1,000 at any sampled checkpoint (0,1,000,...,5,000).
This does not assert a bound between those checkpoints or on unseen seeds.

At the end, KK has1,959 held unopened tasks with mean chain427.74 and age1,761.7.
This shows the backlog is not confined to never-assigned tasks. Its currently
unassigned pool contains4,952 never-assigned and73 previously assigned tasks.
CGAR's current unassigned tasks were never assigned, consistent with disabled
reassignment. Assignment intervals were replayed explicitly; a historical
assignment is not mistaken for a current holder.

## Interpretation for the next change

This supports Fable's hypothesis that KK leaves a long-task backlog. The temporal
and pre-pickup checks are stronger than inferring that from unfinished loaded
chains alone. Nevertheless, performance-dependent task reveals expose different
prefixes, and policy/congestion effects are coupled. There is no causal partition
of the8.025-step completed-chain difference yet.

Cost-oriented scheduling is a legitimate generic competition mechanism. Its
waiting-time tradeoff must remain visible; the KK score is still the target and
is not dismissed because it uses a different assignment policy. A CGAR test should
retain explicit oldest-task admission and protect primary/recovery/started work,
then measure full totals, all-empty travel, old-unpicked cohorts and late windows.
The existing ordinary HRRN-off test lost; this new observation does not reverse
that result or prove broader rematching will fix it.

The full KK supplied/uniform × rematch/keep experiment tests whether rematching
changes that tradeoff inside KK. Shared weights and clock-selected search work
remain confounds for interpreting individual mechanisms. A CGAR additive ×
bounded-rematching factorial is still a separate experiment. No new throughput
record or production change is established here.
