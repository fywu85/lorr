# Checked findings from the independent warehouse review

The review inspected frozen v6, not later prototypes. Claude Code initialized `claude-fable-5-1` with `--effort max`, completed successfully, and used read-only repository tools. Invocation, prompt, returned model metadata and unedited review are retained alongside this assessment.

Verified defects:

- Keeping the primary's next action unchanged did not keep its unfinished target reserved. A turning primary stayed in its current cell in the time-expanded table, allowing another robot to enter the promised target. Prototype v11 excludes new entries into protected intent targets at every future step. An existing occupant may wait or leave; prohibiting its occupancy outright would make an otherwise feasible seed invalid.
- Publishing temporal paths could create duplicate untimed commitments. Prototype v11 reserves commitments in CGAR priority order and publishes at most one commitment per cell. A new regression gives the primary the higher robot index, admits a competing task during its half-turn, and checks that the primary completes its intended move.
- The wait-based seed suppressed useful initial rotations. Later prototypes add stationary turn candidates and optionally prepare them before repair. Their completed full warehouse runs show severe regressions; fixing this local defect did not establish a useful fleet policy. The independent pre-rotation test also loses on all six full-run seeds.

Qualifications:

- Kernel inspection and finite tests do not prove end-to-end liveness for the added planning layer. Earlier claims that only the primary's first action needed protection were incomplete.
- All compared temporal planners have a five-step maximum horizon. No claim that CGAR has a longer horizon than NMS is intended.
- Full v3/v4 results, which completed after the reviewed snapshot was prepared, show that an 8 GiB oriented cache alone does not rescue the original sequence refinement.
- A high `stall` counter is relative to the best distance previously observed during the task epoch. It is not by itself evidence of no recent forward progress, especially after the robot was displaced before becoming primary.
- Forward moves that increase distance need not all be wasteful, and a ratio of pickup distance to pickup delay does not establish that assignment has no remaining opportunity. The matched-goal study establishes a motion gap, with its recorded resource and sampling limitations.
- NMS's stationary-turn behavior can use an otherwise idle timestep; rotations still consume simulation steps. The proposed independent CGAR version requires a strict decrease in exact oriented distance and excludes protected robots and unfinished commitments.
- NMS's dynamic congestion heuristic is disabled in the selected snapshot. Supplied static map weights and other category-dependent settings remain; they are not copied into CGAR.

The review recommends a true sequence-space construction, followed by a controlled fixed-work parallel-search ablation, and independent pre-rotation/arrival-aware scoring. Its recommendations are hypotheses, not measured gains. No wall-clock-truncated search is accepted as success in CGAR.
