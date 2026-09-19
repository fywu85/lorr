# Cost-based discovery and pickup CPU allocation

V43fixes a confirmed configuration bug: pickup workers were parsed after the
CPU-affinity check. Initialization now checks the largest worker count that can
run in any phase, including min(pickup thread ceiling, field quota, robot count).
This also applies when temporal planning is OFF. A one-field quota can validly
use a larger configured ceiling because only one worker can run; disabled fields
reserve none. GRID benchmarks independently bind each process to physical cores.

The optional boolean CGAR_PICKUP_FULL_COST_KEY requires enabled complete fields.
Default0 preserves v42. Value1 retains the best16/8tasks by complete pickup plus
approximate chain cost, then applies the existing HRRN ranking to those candidates.
This separates discovery from final age-weighted assignment. Oldest-task admission,
started tasks, fixed work, complete fields, unique assignments and all-robot fallback
remain. It introduces no map identities, supplied weights or task forecasts.

The original full scan deliberately ranked all candidates by HRRN; that behavior
was documented, not an accidental algorithm defect. Fable proposed the cost key
as a separate policy test. The full v42matrix has positive16/64results and a
32-field collapse, so neither global-age discovery nor complete fields can yet be
judged categorically. Fable's proposed numerical rejection thresholds are heuristics.

Build8898877 passes all CGAR regressions, including actual one-CPU affinity
restriction with temporal OFF/ON, rejection of two possible workers, acceptance
when the quota limits actual concurrency, both shortlist keys with HRRN OFF/ON,
and fair admission beyond the cost shortlist with unchanged simulator metadata.
[Build provenance](build-provenance/v43/) independently reconstructs all22source/
test hashes; no production build remains active.

Screen8898884 passes all five profiles, max0.879275s and peakRSS5.224GB. Disabled
and32fields/default-key exactly reproduce v42prefixes;32fields/cost-key serial
and parallel prefixes match. No prefix throughput or full-memory claim is made.
[Screen and fingerprints](results/pickup-cost-key-screen-v43/).

Full8898901/analysis8898902 compares fields0/default,32/default,32/cost-key and
64/cost-key on seeds0/2 with four threads. Eight simultaneous cases reserve32
physical cores and128GiB aggregate, with32decimalGB checked per process. Existing
v42controls provide full-trajectory checks for disabled and32/default. Existing
v42/64 is also required for the64/cost comparison. No policy is promoted yet.
