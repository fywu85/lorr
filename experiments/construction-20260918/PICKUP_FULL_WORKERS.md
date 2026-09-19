# Parallel planning on the complete-field policy

The64-field policy is now confirmed on all six seeds, with a2.215% mean gain and
1.425–1.494 average CPU cores out of four allocated. The full travel audit finds
7.43–7.84% fewer empty robot-steps; loaded elapsed time changes much less on the
first pair. This motivates spending spare cores on complete alternative motion
plans. An earlier four-worker test on binary50/build32 gained only0.204%; that
does not settle the interaction with this newer routing/assignment policy.

Use existing temporal workers1/2/4, each with the same4M candidate-work allowance,
with1/2/4execution threads. Every configured worker finishes before deterministic
selection of the best complete plan. Time only triggers an explicit entry failure;
it never selects a partial result or changes the number of completed alternatives.
All other settings are identical: graded4/scale4,margin25,refresh512,turn-build128,
64complete pickup fields/four pickup threads,original HRRN shortlist key.

Frozen v43source:[1661176](https://github.com/fywu85/lorr/commit/1661176ca3dd6854a7ba8d1a8a0ec8e33a2cffa9).
No new implementation change. All three200-step deadline screens pass, maximum
0.913763s andRSS5.232GB. The one-worker whole prefix exactly matches v42/64fields.
[Screen](results/pickup-full-workers-screen-v43/). These prefixes do not establish
throughput, full-horizon memory, or complete-horizon deadline feasibility.

Full8898924/analysis8898927 completed all six cases on24 reserved physical cores,
four per process and96GiB aggregate. All six full runs meet the entry/RSS limits;
both one-worker trajectories exactly reproduce v42/64. All22 source/test hashes
match1661176. [Complete comparison](results/pickup-full-workers-full-v43/comparison.json).

| Workers, each4M | Seed0 | Seed2 | Mean change | Final1000 differences |
|---|---:|---:|---:|---|
|1|141829|142988|control|control|
|2|141432|142935|−0.158%|−944/+26|
|4|143340|143409|+0.678%|+60/+83|

Two workers loses both full totals and is not promoted. Four workers improves
both totals/final windows with unchanged outstanding age p90. Empty robot-steps
change+0.65%/−0.76%; this is not an established empty-travel mechanism. Four-worker
mean entry latency is316.0–329.1ms, maximum912.7ms, CPU2.619–2.679cores and peak
RSS11.885GB; full wall time is27.39–28.47minutes. Quality evidence remains two seeds.

Confirmation8898962/analysis8898963 completed the remaining seeds1/3/4/5 with
32 physical cores and128GiB aggregate. All12 full one/four-worker runs pass,
and every one-worker control exactly matches the confirmed64-field reference.
All22 source/test hashes match1661176.

Four-worker seeds0..5 are143340/143767/143409/143359/142868/139710, mean142742.2
versus142474.8 (+0.188%). Five seeds improve, but seed5 loses3207 tasks,1933 in the
final1000, and age p90 rises53steps. The other final-window gains are60/130/83/195/165.
**Do not promote this policy; retain the steadier one-worker reference.** The
single-run record143767 remains in the history with this qualification.
[Six-seed comparison](results/pickup-full-workers-six-seed-v43.json).

Larger complete portfolios and mixed warm/cold workers are separate experiments;
this result does not establish that either will improve throughput.
