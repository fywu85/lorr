# Prefix-progress window search: full comparison

Source20c9cb5d/build160. All14 full runs pass independent replay, source/input/resource checks and the strict one-second entry bound. All five unchanged controls reproduce all six complete trace fields exactly.

| Case | Tasks | Mean entry ms | Maximum entry ms |
|---|---:|---:|---:|
| general-random-01-search-progress0-0 | 727 | 74.775 | 107.353 |
| general-random-01-search-progress1-0 | 726 | 73.491 | 108.115 |
| general-random-01-search-progress1-1 | 726 | 124.134 | 197.688 |
| trick-random-01-search-progress0-0 | 729 | 69.006 | 113.531 |
| trick-random-01-search-progress1-0 | 723 | 71.781 | 107.181 |
| trick-random-01-search-progress1-1 | 722 | 80.299 | 138.926 |
| general-random-02-search-progress1-0 | 1397 | 529.622 | 735.237 |
| general-random-02-search-progress1-1 | 1394 | 656.746 | 959.964 |
| trick-random-02-search-progress0-0 | 1408 | 477.793 | 753.496 |
| trick-random-02-search-progress1-0 | 1399 | 365.133 | 535.793 |
| trick-random-02-search-progress1-1 | 1393 | 424.833 | 688.336 |
| trick-random-03-search-progress0-0 | 2620 | 496.303 | 691.497 |
| trick-random-03-search-progress1-0 | 2578 | 501.732 | 701.456 |
| trick-random-03-search-progress1-1 | 2584 | 578.560 | 817.884 |

Neither progress-only acceptance nor the new prefix labels beats any of the five current profiles. General02 already used progress-only acceptance; adding labels changes1397→1394. On03 the new labels recover6 tasks versus the progress-only2578, but2584 remains below the current2620. This is not a record or a promoted improvement.

The search-label variants have higher observed mean latency than their same-objective controls on every tested profile. General02 reaches959.964ms at its slowest entry, leaving little headroom. Shared-host measurements are observational; the added equal-cost searches and label bookkeeping remain an implementation cost, with no measured throughput benefit here. KeepR05_WINDOW_SEARCH_PROGRESS disabled.

[Independent audit](audit.json), [exact controls](control-equivalence.json). No new fresh task/start stream was used.
