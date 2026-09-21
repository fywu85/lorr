# Paid-action chain forecasts

The chain-scored temporal root currently minimizes terminal remaining potential.
The rolling window and common-future selector instead add paid actions. General
CGAR_TEMPORAL_CHAIN_PAID_COST=1 explicitly tests that objective on the root too:
price all five forecast actions plus the remaining revealed chain. Waits after
final service are free; physical moves/turns remain charged. It requires chain
score mode1, keeps priority ordering, and rejects legacy history/promises.

A necessary consistency change accompanies this opt-in objective: an ordinary
wait seed now contains the first rotation its executor would actually issue.
Its terminal heading and paid first action therefore match that forecast. The
executor checks agreement. Default0 preserves the full earlier trajectory/RNG.
Protected first actions, CGAR reservations/recovery, search quotas and deadline
failure semantics are unchanged. This is a forecast, not a commitment to execute
five steps or a claim of known future congestion.

Tests walk temporal actions independently over varied forward/turn/wait prices,
including repeated errands and free waits after completion. Production fixtures
compare serial/parallel preparation, construction and regional search; exact
default identity; explicit paid cost; both wait tie rules; primary, recovery,
pockets and capacity; and strict parser/resource guards. Full regression and
40file/binary provenance must qualify before benchmarking.

Declared RANDOM04 seed0 matrix: current2059, terminal-chain mode1, paid-chain
mode1, paid-chain plus strict improving wait turns. The terminal-chain arm is
needed to isolate paid scoring from enabling the chain oracle in the root.
Declared RANDOM05 seed10 matrix: current3065, paid-chain, strict improving wait
turns only, and both. Existing guidance/scheduler/future flags stay fixed. Full
1000/2000steps, strict1000ms,4 physical cores,32decimalGB, node-local logging,
explicit --trick INSTANCE. No new scheduler-chain cost or rolling-window repair
is composed into these comparisons. No throughput claim before full verification.

RANDOM04 full results: current2059; terminal chain1908; paid chain1714; paid chain with strict improving waits1683. All valid, max286.28ms, control exact full-trace identity. Paid scoring loses against its proper terminal-chain control too. No promotion. RANDOM05 pending.

RANDOM-05 seed10 full2000 comparison completed: control3065, paid2544,
strict-wait-only3018, paid+strict2553. All four valid under1s (maximum714.394ms).
Paid scoring reduces completed tasks sharply; neither switch is promoted.
