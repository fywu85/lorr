# TRICK: pickup weighting on the combined policy

V72 remaining-potential scoring plus matching64 improves both full seeds over both
single components. V70 pickupweight5 improves both matching-only seeds by129/291.
Their interaction is unmeasured. This four-case experiment holds static lanes,
remaining potential, matching64 and fair admissions fixed, and compares pickup
weight1/5 on seeds0/2. Short preference is OFF. Explicit `--trick WAREHOUSE` is
required. Source c696d5f and its frozen V72 binary require no new solver edits.

All four runs use5,000steps and10,000robots. Each reserves four distinct physical
cores; shared-host5s deadlines and32GB RSS are enforced. The weight1 controls must
exactly reproduce V72 trajectories148132/148101. The analysis also checks source
hashes, explicit components, full action/task accounting, all entry deadlines,
work quotas and matching safeguards. Run `analyze.py --combined-score` to use this
predeclared protocol. No short prefix is treated as throughput evidence.
