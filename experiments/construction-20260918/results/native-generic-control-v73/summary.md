# Native metric preserves the inactive generic solver

The full800-step seed0 startup control exactly reproduces the promoted generic
matching trajectory: `c0f1c9fb1cbdbab170dfdb7f2aec41d32f8fd3a4369d95a718bb997715d022f7`.
All entries are valid under shared5s/32GB, four bound physical cores and no CPU
quota. There is no TRICK/native activation. All28 source/test files and the binary
match exact source06a8258. [Verification](comparison.json).

This establishes unchanged behavior across the checked prefix; it is not a new
full-run performance result. The separate four-arm native screen and full suite
cover activation, field values, scalar units, bounds and lossless wide caches.
