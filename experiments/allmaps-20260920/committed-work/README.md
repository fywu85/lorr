# Fixed work with RANDOM04 motion commitments

Declared before submission on 2026-09-21T13:48:53.656377+00:00. The selected
motion-promise2/crowd16 profile peaks near0.30s per complete entry, leaving measured
headroom. Earlier work-scaling experiments preceded these constraints. Test a2x2
of global complete proposals8->16 and regional repair rounds4->8, keeping four
physical threads, per-worker candidate allowance500000, region per-round work,
all scores, matching, fields, future roots4/branches2/horizon20 and seed4 fixed.
More complete work can improve the local objective without improving throughput;
this is a hypothesis, not an expected monotone task gain. Every declared worker
and round must finish. No wall-clock early return or adaptive quality fallback.

Full1000steps/700robots, strict1000ms complete entry,32decimalGB,4bound physical
cores per arm, explicit --trick RANDOM-04. Qualified sourcef22bc68b. Control2321
must reproduce its full trace. Replicate any positive signal before a robustness
claim. Failed entries remain failures without accepting partial completed tasks.

2026-09-21T13:59:13.435607+00:00: Current2321 versus workers16=2278,rounds8=2358,both2398. Select2398seed4 (+3.3175%) under0.549s; replicate both positive work treatments on seeds0/2 before a robustness claim. Controls reproduce entire selected trajectories.


Declared 2026-09-21T14:09:17.046290+00:00: after completing the three-seed eight-round comparisons, test current16workers/8rounds against16workers/12rounds and8workers/12rounds at seed4. Same fixed candidate allowances, same4physical cores, strict1s/full1000steps. Prior16/8max0.549s suggests room, but any timeout fails. The three-seed16/8effect is modest and seed-sensitive; this is a further selected-seed work-scale probe, not a robust gain claim.


Replication complete: seeds0/2/4 controls2176/2304/2321;8workers/8rounds2082/2326/2358;16workers/8rounds2198/2275/2398. More rounds alone lose0.5146% aggregate;16workers/8rounds gain1.0293% with one losing seed. Keep2398 as a selected-seed record, without claiming uniformly improved throughput. All nine pass full horizons, strict1s and32GB. [Three-seed results](three-seed-results.json).
