# Warehouse: strict one-second runtime

The user requests complete decisions within one second and32decimalGB.
The subsequent instruction defers the160000Warehouse aspiration and shifts
research to all ten instances once strict timing is verified. The preceding NMS154795 milestone
remains achieved; the current record is155173. All fairness/ownership safeguards
and explicit Warehouse trick gating remain unchanged. Full5000-step quality
comparisons remain necessary; 800-step runs test compatibility and resources only.

The saved V110 guarded seeds0/2 show a specific latency problem: the first
entry takes0.991801/1.003767s, while every other entry is at most0.632751/0.628155s.
Later1000–5000 entries peak0.537497/0.534509s. These shared-host observations
motivate reducing initialization of the first live decision without changing
any chosen work, goals, assignments or macro scores.

V122 compares four existing execution configurations on the same frozen V117
solver: original4-thread preparation/pickup; 8-thread invisible speculative
turn prefetch; 8-thread preparation/pickup; and those changes together with a
25GiB logical unwarmed cache. No live all-goal prewarming. All four processes
receive eight disjoint physical cores; global search stays one worker/4M
candidates, regions stay four threads/two rounds, and all admission limits remain.
Prefetched tables remain invisible until the original demand admits them, so
this test requires exact full800-step trajectory hashes and fixed search work,
not just matching task counts. Existing prefetch did waste work on an earlier
generic profile; retain all timing and usage counters and reject it if it does
not help here.

Screen with5s development deadline to record any slow controls, then require
complete strict1000ms full runs for the selected execution profile. A changed
trajectory disqualifies a candidate from this implementation-only comparison.
All source/test hashes and the 32GB process limit are checked. Frozen production
source3e319f175859396b91f97364b320c6794a85cbb7, binary
e3ec32ecdfdaf06f7db139da3857af8fa0ad43b98bd67479f7b9bd969dc1302c.


The800-step V122 screen is complete: all four full trajectories match the
reference. First/max entries are0.986230s(original),0.960702s(prefetch8),
0.984437s(preparation/pickup8), and0.953731s(combined8plus25GiBcache).
The combined mean is398.43ms versus430.60ms for the concurrent original;
shared-host placement makes this an observation, not an isolated speedup.
[Verified screen](../results/strict-runtime-screen-v122-r2/comparison.json).
The initial counter-check failure is retained in the original V122 folder:
the larger cache avoids rebuilds, while exact trajectories, global/regional
search work and guided/fallback counts remain equal.

V123 is the selected complete5000-step test: seeds0/2, strict1000ms,8physical
cores per process,32decimalGB, exclusive research44. Benchmark8900817,
verification8900819, independent outcome reconciliation8900820.
No strict qualification or new throughput peak is claimed before completion.
