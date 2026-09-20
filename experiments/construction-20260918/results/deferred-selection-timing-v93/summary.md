# Deferred-selection warehouse timing and equivalence

Checked2026-09-20T13:44:29.339217UTC, job8899832. Three sequential200-step native
Warehouse runs on the same four bound physical cores of shared research57:
V92 original, V93 deferred stores, V92 original again. Both exact28-file source
manifests and frozen binaries are checked. All600entries finish fixed work inside
5s and32decimalGB; all three action/task/event hashes and sampled search counters
and scores exactly reproduce the frozen V92 H5000/marginOFF startup control.

| Run | Wall seconds | Process CPU seconds | Maximum entry seconds |
|---|---:|---:|---:|
|original_a|84.544|151.801693|1.014827616|
|optimized|84.813|152.137818|1.004867306|
|original_b|84.760|152.119645|1.000090965|

Candidate versus mean original brackets: wall+0.190%, CPU+0.117%. This single
cold-prefix comparison establishes no solver speedup. The independent component
probe measured0.71–1.55% lower CPU time; that saving is not resolved here. No strict
one-second certification or full-run latency claim. PeakRSS6.112GB for this prefix.
The candidate source isbdf5e7a145626f87d0321eb8df8ab08b9ec3cb4f.

The separate800-step generic control also reproduces the existing18958-task hash
exactly. Test-only instrumentation confirms actual construction ancestor revisits
in every one of eight component settings (9–9933calls), retaining identical plans,
score bits and counters. Full production regressions pass. The next quality
comparison must retain an exact frozen control; no quality change is claimed here.
