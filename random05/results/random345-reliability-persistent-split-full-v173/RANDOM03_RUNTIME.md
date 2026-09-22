# RANDOM-03 exact runtime qualification

All ten full800-step runs pass strict1,000ms entry deadlines,30-second preparation,32GB and independent movement/assignment/task/waiting replay. All ten complete traces match available previous implementations in all six compared fields.

Planner seeds0–9: 2611, 2606, 2611, 2646, 2610, 2633, 2620, 2626, 2598, 2604.

Mean 2616.5; sample SD 14.744; best 2646; worst entry 772.972ms; peak RSS 460.857MB.

The mean is+10.92% and the best is+12.17% above matchedmax(NMS,KK)=2,359. This is planner-seed variation on one development-selected archived input, not fresh-input validation.

Selected source8870903e uses IPO, exact SIMD assignment, a synchronous persistent worker and explicit OpenMP binding. Search budgets, objective and horizon stay unchanged. The original seed6startup failure remains in the original batch. Other instances in this v173batch still have timing failures and require separate qualification.

[Full audit](audit.json), [complete-trace equivalence](control-equivalence.json), [all-instance attempted-repair statistics](REPORT.md).
