# Saved per-entry latency by window

Entry-completion wall time in milliseconds, all5000samples/case checked against the verified full summaries and exact motion input hashes. Four bound physical cores per case, shared-host5s development. No strict1s certification or controlled speedup claim.

| Case | First1000 mean | Final1000 mean | Final1000 max |
|---|---:|---:|---:|
|trick_native_horizon0-s0-r0|427.982|451.102|538.335|
|trick_native_horizon5000-s0-r0|426.238|443.104|535.509|
|trick_native_horizon0-s2-r0|419.674|447.356|525.708|
|trick_native_horizon5000-s2-r0|424.748|441.836|529.084|

The minimal horizon tier shows no late latency spike in these runs. The new calibration ledger has not been timed yet.
