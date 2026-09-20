# Generic bounded matching: six-seed confirmation

**Promoted Warehouse reference: mean 144,392.17**, up451 tasks (+0.31332%) from143,941.17. All six full totals improve. Source [0196851](https://github.com/fywu85/lorr/commit/019685109769788dfcc318815e8d102a376982a9). No map-specific flag or layout logic is used.

| Seed | Previous reference | Matching64 | Difference | Final 1,000 change | Age p90 change |
|---|---:|---:|---:|---:|---:|
| 0 | 144,510 | 144,967 | +457 | +51 | -5 |
| 1 | 143,933 | 144,869 | +936 | +309 | -5 |
| 2 | 144,107 | 144,511 | +404 | +123 | -3 |
| 3 | 143,134 | 143,340 | +206 | +2 | +2 |
| 4 | 143,934 | 144,446 | +512 | +112 | -3 |
| 5 | 144,029 | 144,220 | +191 | -49 | +0 |

All12 full cases and60,000 complete decisions pass. Every control exactly reproduces the prior regional reference. Five tails improve; seed5 loses49 final-window tasks. Four age-p90 values improve, seed3 rises2steps, seed5 is unchanged. All six empty-work totals decrease. These are six planner seeds on one Warehouse instance, not an across-map generalization claim.

Candidate mean-entry range 387.332–401.221ms; maximum 0.921360580s; peak RSS 12,105,203,712 bytes; average CPU 1.727–1.772 cores of four reserved; wall 33.33–34.47 minutes. Shared-host5s development, not strict1s certified.

`warehouse-reference-variants.json` now selects matching64; the prior profile is preserved as `warehouse-regional-reference-variants.json`. Optional mixed pickup grouping stays OFF. The production default remains opt-in; this updates the tested reference configuration.

[Combined evidence](../match-quota-six-seed-v64.json) · [First two seeds](../match-quota-full-v64/summary.md) · [Four confirmation seeds](comparison.json).
