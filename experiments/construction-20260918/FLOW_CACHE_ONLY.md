# Separating cost refresh from cache clearing, v33

Periodic flow refresh changes both the route metric and the orientation-table
cache. Optional `CGAR_FLOW_CACHE_ONLY_REFRESH=1` retains the first complete
learned metric and forces cache clearing at the existing fixed observation-count
publication points. It requires enabled flow and a positive refresh interval,
defaults to zero, and uses no future-run observations or elapsed-time decisions.

The same call site clears ordinary tables, LRU state and speculative prefetch.
Spatial tables and goal lists remain untouched. `metric_changed` and
`cache_only_refresh` diagnostics distinguish an actual cost update from an explicit
reset. The first field and its publication timing are identical to ordinary flow.
Duplicate or skipped observations never manufacture movement or advance the
observation count. Every solve completes; an overrun raises an explicit timeout.

Build **8898651** passes all regressions. Independent checks preserve hand-counted
initial costs despite later reversed traffic, across three cost scales and15
publications, verify240 rebuilt oriented distances and30 speculative discards,
and validate gaps/duplicates/reversed time and invalid configuration. Production
fixtures check4800 serial/four-worker actions with warm reuse, protection and
exact reset-count conservation. The archived patch reconstructs all source hashes.

Screen **8898652** passes all four200-step cases. Default and real-refresh64
fingerprints match their v30 controls exactly. Cache-only512 matches the frozen
prefix before its first reset. Cache-only64 republishes at samples128 and192,
keeps17106 penalized edges, and clears the cache at192 with `metric_changed=0`.
All maximum entries are below0.775seconds. No screen throughput ranking is used.

Full **8898653 /8898654** crosses frozen, cache-only512 and real-refresh512 with
strict wait turns0/1 on seeds0/2. Six independent cases run concurrently on24
reserved physical EPYC9354 cores, exclusive research31, under72GiB total memory
reservation. Each case uses four cores, all5000steps, one-second decisions and
its own32,000,000,000-byte process RSS validity limit. The matrix has12cases.

Seed0 gives the clean comparison against healthy frozen flow. Frozen-legacy
seed2 is a collapsed baseline; recovering to122k there would not reproduce the
134.5k refreshed level. Strict-mode arms supply a healthy seed2 frozen control.
Both real refresh512 logs have10 publications and10 resets, so fixed cache-only
reset scheduling matches the observed real-reset schedule on these seeds.

A missing table disables both weighted candidate scoring and wait-seed orientation.
This control tests their combined cache effect. Even if cache-only equals refresh,
it still uses the INITIAL learned field; it cannot show that learned flow itself
is unnecessary. No full causal result exists at submission.

[Build](build-provenance/v33/), [screens](results/flow-cache-only-screen-v33/),
[full configuration](flow-cache-only-full-variants.json),
[persistent Fable assessment](fable-flow-session/assessment.md).
