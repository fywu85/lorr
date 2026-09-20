Continue the same persistent Fable5.1/max read-only session. No tools or tests.
The primary goal remains GENERIC >=152,981 full tasks repeatably across six seeds,
with complete one-second entries and process RSS<32 decimal GB. Every map-specific
policy requires --trick WAREHOUSE and belongs to a separate TRICK frontier.

New VERIFIED results:

1. V54 full no-flag seeds0/2 exactly reproduce all reference trajectories and
144,510/144,107 tasks. All10,000entries pass. TRICK NMS lane DIRECTIONS at our
forward4/16, turn4 and still-paid forward surcharge reaches146,659/146,566;
max entries984.533/980.945ms, RSS11.914GBmax. Initial assignments exactly match.
Its final1000 completions29,911/29,906 vs generic30,004/29,856: almost no tail gain.
Its loaded turns904790/907357 vs1415227/1448441 and waits1700446/1709996 vs1952048/1987197
are lower, but empty robot-steps9852533/9863784 vs9269146/9263667 are higher.
Thus early startup gain largely persists as a fixed total advantage rather than
clear sustained-rate improvement. Not full NMS semantics or an isolated lane effect.
Sourceabbe36b, all26files/binary/physical allocation/entry deadlines verified.

2. V55 flow replay is COMPLETE. All400Mactions match independent accounting;
all vertex/edge actions valid; all60CGAR publications exactly match production
counters and independently recomputed per-edge costs; seven fixtures pass.
Across ALL60publications: ZERO mixed-sign short degree2chains, ZERO adjacent
opposite toll signs. Each chain is only2or3interiorcells; cross-junction behavior
is not answered. Movement coverage70.92-74.07%generic,82.06%KK,77.08%NMS.
Generic corridor minority share3.24-3.59%all,1.20-1.35%final1000; KK0%; NMS~0.18%.
These are volume-normalized, not simultaneous conflict estimates or matched cohorts.
At lastpublication, pooling changes202-240directededgecosts with only~11-14k
historicalcrossings over those changed directions out~32-33Mcorridorcrossings.
We DROP short-chain pooling as priority. Your cross-junction/epoch diagnostics
remain possible, but I want a narrow causal production test rather than more
indefinite descriptive diagnostics before any next policy experiment.

Concrete GENERIC candidate: default-OFF remaining-flow score ablation.
Current temporal_adapter adds TemporalGeometry::forward_surcharge to the
remaining weighted potential, cancelling paid extra tolls. NMS does not add
the same paid forward surcharge. Add CGAR_TEMPORAL_REMAINING_FLOW=1 to omit ONLY
that extra term, retaining the SAME learned potential, physical turn price,
scale/op tie preference, HRRN scheduler, all work quotas, protected primary,
fallback and explicit timeout. No imported lane field/map identity. Before
first learned publication128, scores are exactly unchanged. Unweighted/neutral
fields also unchanged. Do not change next_errand mode or guideroutes: reject
combinations initially. Keep default scoring/tests byte-equivalent.

This deliberately changes the optimization surrogate: an expensive weighted
forward edge can give a larger score improvement instead of physical-unit
normalization. It might improve evacuation through locally expensive edges,
or undermine lane discipline/increase opposing traffic. Neither is claimed.
The protected CGAR construction is untouched, but do not assume scalar
paid-progress identities still hold in this option.

Please audit this exact candidate:
1. Any concrete safety/liveness/overflow or wrong-sign issue beyond changing
   the heuristic objective? Does protected primary depend on this identity?
2. Is there a reason this factor is already redundant under current regional/
   global objective? Distinguish per-agent additive constants from path costs.
3. Recommend the smallest meaningful regression fixture(s) and a direct full
   0/2 causal comparison against unchanged controls, or name one clearly
   superior similarly bounded generic operator supported by the new evidence.
4. Address the TRICK tail result: does it weaken a learned-guidance-only
   explanation of the remaining gap? Do not turn this into proof of assignment
   dominance; complete-system interactions and cohorts differ.

No implementation/test has been performed for this candidate yet. No new
throughput claim. Keep the answer bounded and state uncertain predictions.
