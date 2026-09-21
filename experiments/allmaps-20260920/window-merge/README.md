# Merge compatible completed rolling-window plans

Declared before implementation benchmarks. CGAR currently selects one of four
complete window searches. PILOT source027df4d9 also merges independent path
improvements from other completed workers. Transfer that general mechanism into
CGAR without substituting PILOT's planner or changing CGAR's primary, recovery,
pocket permissions, commitments or protected first actions.

New optional CGAR_WINDOW_MERGE=1. After every worker completes its declared repair
attempts, start from the existing best worker and visit all other workers in
index order. Connect cross-parent vertex and reverse-edge conflicts over the
entire horizon. For each component, choose donor paths only for a strict cost
improvement, or equal cost with better remaining-chain potential when existing
progress ties are enabled. Revalidate complete paths and reconcile exact integer
cost changes. Zero is the old path and RNG behavior. A timeout anywhere fails
the complete entry; no partially merged answer or history is published.

Tests must cover independent pairwise conflict graphs and every component
mixture, isolated head-on conflicts, complementary independent improvements,
protected and first-cell constraints, complete donor counts, serial/parallel
identity and late timeout with unpublished history. Start with selected RANDOM02
and03 full strict1s controls versus merge, four physical cores each. No throughput
gain is assumed. Map-specific fields retain their explicit --trick flags.
