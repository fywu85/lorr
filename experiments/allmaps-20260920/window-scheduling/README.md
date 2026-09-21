# Scheduling interactions with selected CGAR windows

Declared before results. The selected rolling windows changed motion substantially
since the earlier known-horizon and rematching comparisons. RANDOM01(seed4) and
RANDOM03(seed2) therefore get one bounded2x2 interaction test: current control,
known full horizon with empirical mean completion margin, unopened task rematching
at every step with a finite four-retarget budget, and both. RANDOM02(seed2) already
uses the latter matching profile, so its comparison adds only horizon selection.

Horizon selection affects new admissions, never drops held/started tasks, uses
Manhattan-plus-service bounds and only past completions for the margin. It is an
explicit --trick RANDOM-0N policy. All other planner and scheduler work remains
fixed. Controls693/1215/1982 must reproduce complete archived trajectories.
Full600/600/800steps, four physical cores per case, strict1000ms,32decimalGB;
node-local logs archived after all runs, including failures. These are interactions
with new selected window profiles, not repetitions of the earlier temporal-only
experiments. No promotion or mean-gain claim until verified.

Full results: RANDOM01 693control/691horizon/685rematch/686both; RANDOM02
1215control/1212horizon; RANDOM03 1982control/1982horizon/1872rematch/1883both.
Every control repeats its archived trajectory; all runs pass strict1s. No promoted
configuration. Faster motion alone does not make these scheduling policies useful.
