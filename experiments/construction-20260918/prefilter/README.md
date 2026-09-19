# Candidate filtering without changing the search policy

The temporal kernel repeatedly skips paths that exceed its current three-,
four- or five-action horizon, or leave the selected repair region. These
restrictions and the sorted candidate order are fixed for a search object.
This experiment checks whether building ordered candidate lists once pays for
its setup cost. It changes no work limits, random draws or score comparisons.

The probe uses synthetic open-grid cases with 1,000 robots and three modes:
cold equal-weight global search, region-filtered search, and a valid warm
initial plan with unequal weights. Each observation completes four independent
kernel runs at a prescribed four-million candidate threshold and one-million
attempt ceiling. Initialization/indexing and destruction are included in the
reported kernel time; geometry, scoring and independent final collision audits
are outside that timer. It is a component measurement, not lifelong throughput.

Both experiments use one reserved EPYC 9354 physical core without a CPU quota,
six alternating baseline/candidate repetitions per mode, and identical compiler
options. All selected plans, exact score bits, and nine search counters plus
maximum depth are included in a shared checksum. Every final vertex/edge
reservation is independently checked. All checksums and work counters match.

| Implementation | Cold global | Regional | Warm/unequal global |
|---|---:|---:|---:|
| Index every search | +1.90% time | -7.72% | +1.49% |
| Index regional searches only, specialize kernel branches | -1.91% | -6.86% | -2.29% |

The all-search index is not adopted: its setup overwhelms any global-search
benefit. The regional-only variant builds flat ordered lists only when a region
mask exists. A template specialization keeps the global loop free of region
checks and index setup; the per-root dispatch preserves all recursion and RNG
behavior. The tested production header exactly matches the second probe's
candidate hash.

Build **8898560** passes the entire regression suite, including dense regional
cases, serial/four-thread equivalence, protected paths, warm starts, learned
flow, guide reconnection/refinement and explicit deadline failure. The v27 source
patch reconstructs every requested hash. Production screen **8898562** compares
the fixed-4M control and the established global-25k/two-regional-round profile,
using four reserved physical cores, 8 GiB and exact one-second complete decisions.
Full trajectory preservation and end-to-end speedup remain unmeasured for v27.

[First experiment](../results/prefilter-probe/),
[regional-only experiment](../results/prefilter-regions-probe/),
[build provenance](../build-provenance/v27/).


Both production screens pass and exactly preserve all four prior trajectory
fields. Max entries are 0.804657342 seconds for 4M global control and 0.694104470
for global 25k plus two regional rounds; RSS is 4.68 / 4.79 GB. These do not
establish an end-to-end speedup. Full **8898566**, analysis **8898567**, follows
the existing full queue, using two four-core instances and 24 GiB total. The
global profile uses one search thread within its four-core allowance; the
regional profile uses all four during regional work.
[Production screens](../results/prefilter-screen-v27/),
[exact short trajectories](../results/prefilter-trajectory-equivalence.json).
