# Full local-refinement comparison

All 18 original full runs pass independent replay and strict limits. All nine declared controls match six complete trace fields. Every nonzero local-refinement setting loses throughput.

| Profile | No proposals | Five proposals | Fifteen proposals |
|---|---:|---:|---:|
| general RANDOM-01 | 647 | 642 | — |
| general RANDOM-02 | 1106 | 1090 | — |
| general RANDOM-03 | 1634 | 1616 | — |
| general RANDOM-04 | 1616 | 1564 | — |
| general RANDOM-05 | 2226 | 2160 | — |
| trick RANDOM-04 | 2782 | 2756 | 2750 |
| trick RANDOM-05 | 4254 | 4189 | 4242 |

The five-proposal serial controls also score 2,756 on RANDOM-04 and 4,189 on RANDOM-05, with all six fields identical to their parallel counterparts. On these single shared-host pairs, parallel/serial mean entry times are 479.301/513.502 ms and 589.765/629.057 ms respectively. This does not establish a general speedup or throughput gain.

The selected RANDOM-05 parent was frozen at 4,254 with terminal credit disabled; the newer 4,302 record is a separate configuration. General sparse reactive controls are distinct from the stronger windowed records. All options remain disabled in the selected configurations. No frontier changes.

[Independent audit](audit.json), [exact controls](control-equivalence.json).
