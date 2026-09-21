# Recursive blocker neighborhoods

Declared 2026-09-21T14:18:07.884367+00:00. General default-off transfer inspired by immutable PILOT
027df4d97ad737ad6319354abe8b92afc2840473, random05/src/window.cpp lines349-391.
Only that committed source was read; no independent agent code, builds or live
outcomes were changed. The current CGAR neighborhood already collects root-route
blockers, then samples nearby robots. This extension caches an individual guide,
follows blockers of blockers, excludes source occupancy that is not a true reverse
edge, and optionally rotates its horizon scan. It preserves CGAR's complete seed,
protected actions, permissions, rollback and validated fixed-work repair.

Separate CGAR_WINDOW_FULL_GROUP changes sampled1..group membership to the full
configured size, capped by eligible robots. It is an independent work factor;
more repaired agents at equal attempts are not equal compute. Both switches
are disabled by default. New diagnostics reconcile guide construction, bounded
scans, selected members and exact full-group work; old sources remain auditable.

First full comparisons, after functional qualification: RANDOM02seed2 and
RANDOM03seed2, four arms each: original control, full groups only, rotated causal
blockers only, both. All existing fields, objective, delay sampling, attempts,
node caps and worker counts stay fixed. Four bound physical cores, strict1000ms,
32decimalGB, full600/800steps. Any timeout remains a failure; no timed partial
answer. Controls must exactly repeat1215/1982. Treat results as first-seed signals
and replicate gains. These selected map profiles require explicit --trick INSTANCE;
the new mechanism itself has no map-name or density dispatch.
