# Exact native read-only fresh-pickup replay

The audit ON and OFF runs reproduce the same **800-step trajectory and all 101 real diagnostic lines**. The p90/weight-8 Warehouse prefix reference also matches exactly. This verifies observation without changing scheduling or planning; it is not a new full-throughput score.

The audit sees 21,410 eligible fresh holders, 1,099 positive permutation cycles and 83,870 guidance-cost units of aggregate positive gain. The existing acceptance rule (at least four forward-cost units and 10% improvement) retains **7 task-disjoint cycles involving 15 tasks**, totaling **1,352 raw guidance-cost units** (67.6 after dividing by the forward-cost scale of 20). These are neither physical robot steps nor predicted completions. Groups are bounded and exclude protected holders, so this is not a matching upper bound. No feasibility-tier edge is excluded in this prefix; the configured Warehouse cutoff still lies much later.

Full CGAR regressions pass, including actual forward-field matching, task-column tier guards, missing-field exclusion, deadline propagation, configuration rejection, and four native closed-loop OFF/ON pairs covering 24,576 independently checked actions. Source `778ac779aefd3d7f4557e399ea7544b1cebd8cd0`; binary `ef0973f7577b1e10d22d55d76060605f67ec3ee3ffa318ad4d0119fb9a360f15`. All 28 source/test hashes match the commit.

Both processes use four disjoint physical cores on research50, no CPU quota, a 5-second complete-decision deadline and a 32,000,000,000-byte RSS cap. All decisions finish and all validity checks pass. Shared-host timings do not certify one-second competition timing.

Verified 2026-09-20T18:40:30.168351+00:00. [Exact comparison](comparison.json), [audit samples](audit-samples.json), [run/resource evidence](run-summaries.json), [frozen profiles and source](spec.json).
