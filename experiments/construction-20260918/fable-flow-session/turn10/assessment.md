# Assessment of persistent Fable turn10

Reviewed exact tested source4872d048dd8155927074ac2a4d27a55756266fd5 in the
same Fable5.1max CLI session. Four incremental source updates,30660payload bytes.
Completed2026-09-19T17:37:53.667167UTC in209.132s, no errors or permission denials;
actual reported cost35.470251USD despite the requested12USD flag. The flag is
not treated as a reliable spending cap. Fable ran no tools or tests. Only its
visible final review, metadata and status are archived.

The bucket search, parallel publication and top-k heap had no reproduced bug.
We verified the CPU-affinity omission in initialize(): the previous check ran
before parsing pickup workers. Actual benchmark profiles use4workers on4reserved
physical cores, so this omission does not invalidate their allocations. V43moves
the check after parsing and includes the maximum usable pickup worker count,
min(thread ceiling, fixed field quota, robot count). Tests restrict actual affinity
to one CPU, reject two workers with temporal planning both OFF/ON, and accept
a one-field quota or disabled fields despite a larger inactive thread ceiling.

The global HRRN shortlist was intentional and described in PICKUP_FULL.md; it is
a policy change, not a search correctness defect. The original shortlist-choice
fixture only used direct-cost dispatch. V43adds cost-key discovery as an optional
separate experiment, preserving final HRRN assignment ranking and fair admission.
Additional fixtures separate HRRN versus direct cost, both shortlist keys, and
oldest-task admission beyond the retained shortlist. No full quality claim yet.

Mixed complete/table/approximate cost bases remain an explicit approximation.
A96.5%cached-probe share cannot establish low impact on chosen assignments.
Likewise a10%mean-pickup threshold does not falsify an age/locality mechanism,
and one failed cost-key setting would not refute complete fields generally.
We will use full trajectory accounting and paired quality/latency/memory results;
Fable's proposed numerical cutoffs are heuristics, not causal or statistical tests.
