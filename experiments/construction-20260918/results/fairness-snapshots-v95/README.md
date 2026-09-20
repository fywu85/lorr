# Saved-run waiting snapshots

Read-only audits requested by the user after the154889 record. At the5000-step
cutoff, oldest never-assigned ages are185/184 for the two winning-configuration
seeds, versus5000 in the local NMS/KittyKnight diagnostic runs. NMS/KK retain442/
453 never-assigned tasks revealed at step0. In both CGAR candidates every task
revealed by step2500 has completed. Earlier CGAR tasks did sometimes wait longer
before eventual first assignment: the observed maxima were700/691steps.

Age means current simulation tick minus reveal tick. Never assigned means no
accepted assignment to any robot anywhere in the history; it excludes held tasks
and tasks previously assigned and subsequently released. These are final
snapshots, not maximum waiting times over all task histories or a liveness proof.
The new native_work/analyze.py repeats this audit for every valid new full case.

Competitor input SHA256 hashes match their earlier independently verified pool
replays. Never-assigned counts, sums and p90 exactly match the saved cohorts.
The NMS run completed154981 but had one timeout and39.744GB RSS; it is diagnostic,
not the published154795 target or a resource-matched comparison. KittyKnight's
local score152981 used38.858GB. No causal throughput/fairness partition is claimed.
