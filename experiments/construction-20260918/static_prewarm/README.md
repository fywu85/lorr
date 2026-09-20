# TRICK: complete native Warehouse distance precomputation

The next test prepares every free-cell goal table using only the known map and
its explicitly selected static Warehouse guidance. It reads no unrevealed tasks,
future outcomes or robot states. The CGAR primary, protection and recovery logic,
HRRN, forced-oldest admission, started ownership, one-retarget limit and cooldown
remain unchanged. Initial task dispatch still uses the original spatial metric.
Only early motion and later pickup estimates can benefit from the warmer cache.
The steady-state fixed search work remains unchanged.

Two isolated V116 probes use frozen V113 source, not the new production helper.
All 38,586 tables are resident, all self distances are zero, and four complete
goal tables agree with an independent heap traversal (617,376 oriented states).
The four-core probe takes 29.752 seconds, stores 11,911,035,168 table bytes and
peaks at 12,304,756,736 RSS bytes. The eight-core probe takes 23.656 seconds and
peaks at 12,319,297,536 RSS bytes. These shared-host measurements exclude the
rest of solver initialization and do not establish whole-solver memory use or
throughput improvement. Four cores leave too little preprocessing margin.

`CGAR_TRICK_NATIVE_PREWARM_THREADS` defaults to zero. Setting it requires explicit
`--trick WAREHOUSE` and the native metric. Positive values (1–32) also require
orientation mode 1, compact tables, adequate CPU affinity and logical cache
capacity for every free cell. Complete goal tables are built in fixed batches
of at most 32 and admitted in cell order. An expired deadline raises a timeout;
there is no successful partial warmup. The usual 30-second preprocessing limit
still applies to the entire initialization. Successful initialization prints
the goal count, thread count, actual table bytes and elapsed prewarm time.

V117 will compare three arms against the frozen 155,173/155,090 seed0/2 guard
profile: original 8 GiB logical cache; 25 GiB logical cache without prewarming;
and 25 GiB logical cache with eight prewarm threads. Logical capacity uses the
existing conservative 32-bit accounting; actual compact storage is smaller.
The middle arm distinguishes capacity changes from preparing the tables early.
Each process receives eight distinct physical cores, while ordinary planning
still uses the original one global worker and four preparation/regional/pickup
threads. Full arms run concurrently on disjoint allocations.

First require the full regression suite and an 800-step activation/resource
screen, with exact default trajectory and unchanged initial assignments. Then
compare complete 5,000-step runs at seeds 0 and 2, a 5-second development entry
deadline and RSS below 32,000,000,000 bytes. Rank only full valid runs, including
independent task/action/waiting accounting. No improvement is claimed yet.
