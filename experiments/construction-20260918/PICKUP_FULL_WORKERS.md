# Parallel planning on the complete-field policy

The64-field policy improves both initial seeds by2.217%, with1.47average CPU
cores out of four allocated. The full travel audit finds7.6–7.8%less empty travel;
loaded elapsed time changes much less. This motivates spending the spare cores
on complete alternative motion plans. The policy itself is still in six-seed
confirmation. An earlier four-worker test on binary50/build32 gained only0.204%;
that does not settle the interaction with this newer routing/assignment policy.

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

Full8898924/analysis8898927 compares all three settings on seeds0/2, six cases
running concurrently on24reserved physical cores. Each process gets four cores
and a32decimalGB RSS cap; aggregate reservation96GiB. The one-worker full control
must exactly reproduce v42/64fields141829/142988 before interpreting differences.
[Configuration](pickup-full-workers-variants.json).
