# Isolated temporal-kernel profile

Job8899186 used one isolated EPYC9354 physical core, the exact V49 production headers
(which equal V48), and a synthetic45x45 open grid with1,000 robots. It ran20 complete
fixed-work kernel trials per observation, including setup/destruction, with three
plain timing repetitions and one instrumented gprof observation per mode. Three
modes cover cold global, regional filtered, and warm unequal-weight search; mode3
repeats regional search with clock checks at the normal callback sites.

Every selected plan, score bit, search counter and independent five-slot reservation
check agrees between profiled/unprofiled runs. Regional clock/no-clock checksums
and work also agree. Plain median kernel time is0.9363s without checks and0.9792s
with checks across20trials (+4.59%). This is a synthetic component observation,
not warehouse end-to-end overhead. Construction/indexing is0.0056s in the regional
case, under1% here; the warehouse allocation/preparation proportions may differ.

The gprof flat/line samples concentrate in recursive build and its inlined blocker.
Coarse samples and instrumentation do not establish cache-miss or TLB rates. The
available perf launcher cannot run locally (perf_4.19 missing), and the local
perf_event_paranoid value is3; no host setting was changed. Gprof needs no such
privilege. Fable's proposed frozen real-decision replay remains a stronger future
measurement, especially for heavy late steps. No clock-granularity change was made.
[Archived observations](../results/kernel-profile-v49/).
