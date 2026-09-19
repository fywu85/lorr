# Removing a redundant root deadline check

Both public TemporalPibt::construct and ::repair call the deadline callback before
every root attempt. The private build then calls it again at counter0. Between them
are only root selection, small RNG draws and removal of one five-slot reservation.
The candidate removes this second call at counter0, retaining all checks at nonzero
multiples of16 recursive displacements. The attempt callers, regional completion,
worker completion and final complete-entry checks remain unchanged. No search state,
RNG, score, work count or timeout-to-partial fallback changes are introduced.

The patch stays outside production pending measurement. Twelve alternating paired
observations cover four synthetic modes, each20complete4M-candidate trials, including
one regional mode with actual clock callbacks. Plans, score bits and counters must
match exactly; independent vertex/edge checks remain. This is component timing, not
warehouse feasibility or throughput evidence. A useful result needs production
regressions and exact full trajectories before adoption.

Probe8899194 completes96observations (12pairs x4modes) with exact checksums and work.
The clocked regional median candidate/baseline time ratio is0.97329 (2.67% lower).
No-clock modes change+0.14% to+1.10%, so this is specifically a clock-callback saving,
not a universal loop speedup. All production roots use real deadline callbacks.
The exact tested header is now the V50 production candidate; full regressions and
warehouse equivalence/timing are still required. [Evidence](../results/deadline-duplicate-probe-v49/).

Build8899199 passes the full regression suite, including explicit timeout propagation,
protected paths, regional parallel equivalence and the next-errand edge cases. Every
one of22 frozen source/test hashes reconstructs fromdf3f8f1 plus the archived patch.
Frozen runs/cgar-deadline-duplicate-build-v50-20260919; build-provenance/v50.
A/B/A timing job8899204 compares frozen V48, V50 and V48 again on the SAME four
reserved physical cores at200steps. It checks exact prefixes and complete-entry
feasibility, not throughput. Full0/2 equivalence follows a passing screen.

A/B/A8899204 passed: all three complete200-step trajectories and sampled search
counts/scores are identical; all600 entries meet1s, peakRSS5.269GB. Four identical
physical cores, no quota, nonoverlapping run intervals; all22 source/test hashes and
actual binaries verified. V50 wall100.640s versus102.609/102.353s brackets (−1.796%
against their mean). Max entry905.37ms versus953.13/967.95ms. These are one cold-prefix
comparison, not a full-run latency bound. [Checks](../results/deadline-duplicate-timing-v50/equivalence.json).

FullV50 matrix8899209/analysis8899210 now runs seeds0/2 simultaneously, four physical
cores each/8cores/32GiB aggregate,32decimalGB process cap. Raw
runs/cgar-deadline-duplicate-full-v50-20260919. Require every full entry valid and exact
regional reference trajectories144510/144107 before claiming full preservation.
