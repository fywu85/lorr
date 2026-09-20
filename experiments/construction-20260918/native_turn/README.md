# TRICK: native Warehouse guidance turn price

V113 tests guidance turns1/2/4 with the same native20/200+1forward field, raw
50*potential-operation score, fixed4M global candidate work, two regional rounds,
pickup8, p90 cutoff admission and ten-step matching. Matching-horizon guard stays
OFF. A physical rotation still consumes one simulator step. Ordinary fairness,
CGAR primary/recovery protection, held/started ownership and one-retarget/cooldown
rules remain intact. This changes both route and pickup prices; it cannot isolate
a pure motion effect.

The saved-best V112 replay finds about17% of waits with a rotating forward
blocker, against about57% with an incoming moving blocker. The native guidance
charges a rotation1 versus20 for preferred forward movement. A modest increase
may reduce turns or their blocking effects, but it may also cause worse detours,
assignment choices or traffic. Only full matched runs decide throughput.

`CGAR_TRICK_NATIVE_TURN_COST` defaults to1 and accepts integers1..16 only with
explicit `--trick WAREHOUSE` and the native metric. Every oriented reverse table,
complete/ bounded pickup search and CGAR candidate quote uses the single chosen
guidance price when using oriented costs; the existing spatial fallback is unchanged.
The installed forward field retains the verified NMS fingerprint.
No selector means the original behavior; explicit1 must reproduce it exactly.
A nondefault startup receipt and every native periodic receipt disclose the price.

Regression coverage includes independent heap-oracle reverse and pickup distances,
parallel prefetch, physically replayed macro scores for turns1/2/4, and real
closed-loop service with turns2/4 and both native fields. Serial/parallel decisions,
started-task ownership and the one-retarget limit must hold; missing/explicit1
must agree. Invalid values and use without the explicit trick/native gate fail.

First run the complete regression suite, then native800-step feasibility/default
checks and the unchanged generic800-step control. Prefix totals are never ranked.
Then run paired full5000-step seeds0/2, three arms on24bound physical cores (4each),
shared GRID hosts,5s complete-entry deadline and32decimalGB RSS. Verify exact
turn1control totals/trajectories155120/155056 before interpreting other arms.
Retain all work, task-age, never-assigned, latency and memory evidence.

Build8900665 passes the complete suite on source dac3fc75cb6a149b3e9a6b4770b64170ead87ed9. All28source/test hashes match. The new turn-price episodes check24576physical actions, preserve serial/parallel equality and change two of four episode traces. Independent reverse/prefetch comparisons cover1852128/3704256oriented states;125022macros are independently replayed. Frozen binary4f194edc5a806ae31198ef5ff29b995372b2e92bf266f490c010dbf0574481a5. Native800screen8900675/verifier8900678 and generic800screen8900676/verifier8900679 are running; no quality conclusion yet.

Both screens pass: native verified20:41:37UTC, generic20:41:10UTC. Implicit/explicit turn1 reproduces21489 and the known800-step hash085e97f9...; generic reproduces18958/hashc0f1c9fb.... Turn2/4 activation and changed traces are verified, but no prefix quality ranking. Full benchmark8900690 and verifier8900695, followed by independent work accounting8900698, compare all six full seeds0/2cases. The new155173guard record belongs to a separate frozenV110solver; these turn trials deliberately keep the guard OFF.

Full V113 passes all six cases and independent action/task accounting. Turn2
loses95/37 and is rejected. Turn4 changes0/2 by−25/+90, scoring155095/155146;
physical turns decrease8772/10596, but waits rise19473 onseed0 and fall3587
onseed2. No new peak. V118 full seeds1/3 compares1vs4 on the frozen V113 binary
and exact V102 controls. V119 separately compares turn4 guardOFFvsON on seeds0/2,
with exact V113 turn4 controls. Full validation and independent accounting are
queued after each benchmark; no prefix selection or assumed additive gains.
