# Comparison policy

Updated 2026-09-21 at the user's explicit request: every headline performance
comparison uses **max(NMS, Kitty Knight)** separately for each instance.

- Published combined-track scores provide a historical target. They use different
  inputs and hardware; never label them as a matched local comparison.
- Matched local max comparisons require measured results for both teams under
  the same input, horizon and allocation. Until Kitty Knight is measured, the
  retained NMS runs are NMS-only controls, not the two-team maximum.
- Fresh-input NMS-only protocols retain their original interpretation and cannot
  retrospectively establish a matched two-team maximum without Kitty Knight runs.
- Preserve original failures and report unavailable baselines explicitly. Do not
  treat a failed or missing run as zero to manufacture a maximum comparison.
- Distinguish selected development maxima, exact repetition, paired planner seeds,
  fresh task/start streams and unseen geometry. Throughput remains primary.

Source: [published target snapshot](references/published-nms-kk-combined-2024.json).
Both teams now have independently audited local results on all five RANDOM inputs.
The selected Kitty Knight source, existing compiler/linker flags and runtime assets
are preserved. Its RANDOM-05 runs use `MALLOC_ARENA_MAX=2` after the unchanged
allocator exhausted the 32 GB virtual-address guard. Source, work and limits are
unchanged; original failures remain visible. See the [matched comparison](NMS_KK_COMPARISON.md).
