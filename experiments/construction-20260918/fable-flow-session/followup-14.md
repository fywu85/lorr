Continue SAME persistent lorr-warehouse-cgar-fable51 session, turn15.
Fable5.1 max, read-only, no tools or claims of tests you did not run. Please keep
visible response under1,100words. User explicitly authorizes these source updates.

Goal unchanged: >=152,981 on six full5000-step/10000-robot warehouse seeds,
complete fixed-work entries<=1s or explicit failure,RSS<32decimalGB, isolated cores,
generic policies only. Best single remains144510; confirmed six-seed mean143941.2.
Do not confuse a two-seed mean or a better scalar heuristic with a new frontier.

V47 common-baseline known-next scoring LOST full0/2:143529/144090 versus144510/
144107, pairedmean−0.3458%, tails−204/−55,agep90+1/−5. Loadedturns+5.61%/+1.14%,
waits+9.13%/−0.02%,emptyrobotsteps−0.81%/−0.47%. All4runs valid/all20kentries<=1s,
max967.45ms,RSS12.013GB,exactcontrolfullhashes,actualnextcounteruse in25frames,
source/binary/CPU bindings verified. Keep OFF. These associations are not causal
savings or a broad impossibility result for multi-errand planning. No6seed promotion.

Your suggested V48 readonly regional peak tracker is implemented and passes full
regressions, unchanged decisions and subsequent RNG trajectories. It records every
complete attempt, cumulative all-batch counters every200steps, never changes search.
At2200–2400steps,1000ppm four-region controls discarded only3 peaks each, total148/
262 scalar units across17600/19200batches. Eight regions discarded4–5 tiny peaks
by2000steps. This weakens the best-retention opportunity; not a throughput bound.
Full1000/0ppm and4/8region comparisons still running. No best-state retention added.
Equal final scores still count as reverted at0ppm, and existing RNG draws remain.

V49 closes both suggested edge tests with NO production change. A real core with
an excluded two-cell pocket has finite next distance at currentG and infinite at
an arriving path endpoint; whole-robot fallback exactly matches native actions.
An own-cell assignment initially faceswest; nextliesnorthwest. We first expected
clockwise rotation, but independent scores correctly choose WFRFW (-409), ahead of
RFCFW (-385); the production action waits, servicesG and keepswest heading. Native
uses the tied seed rotation. This was a wrong test expectation, now corrected. Full
suite passes with updated task index checked on following plan. Binary identicalV48.

A new existing-parameter experiment compares distance_scale50/513 under the current
regional/flow reference, nextOFF. With U4/op0..128, the op term spans512; S513 makes
one integer physical-heuristic unit dominate it. Older256/1024 no-flow results were
negative; this interaction is untested. Screen valid,full0/2 next. No assumed gain.

Focused next question: the failed known-next policy addresses the smaller audited
post-service class (~7 known-next-only robots/frame afterwarmup), while finishing-only
is larger (~23/frame overall). Our earlier offline neutral-tail probe changed both
holding examples to first-FW, but was NOT a production policy or warehouse result.
Is the following finishing-only neutral-tail score a coherent bounded experiment?

Eligible: nonpinned, nonguided robot whose actual revealed task has current goal G
matching idx_next_loc and that index is LAST in locations. No guessed next task,
no hidden arrivals, no spatial/map rule, no new table lookup. All other robots native.
For candidates with no G hit, native. For a candidate with any G hit, set physical
terminal d=-4U, charge only actual turn/forward extras THROUGH FIRST G hit (as native
already does), retain op term. Equivalently native_cost -(4-last_G_hit)*U*S, since
TemporalGeometry::cost overwrites d=-tU at EVERY G hit while its paid extras stop at
FIRST hit. All five reservations remain physically complete and checked; only cost
changes. Thus staying on G through slot4 is unchanged, while leaving after actual
service is no longer penalized solely for failing to hold G. Native wait-turn
projection and fixed work stay unchanged. Optional/defaultOFF, current known-next
modeOFF in the experiment. No promises about its unknown post-task destination.

Check that equivalence against actual source, including op0 first action, turns,
directed tolls, final virtual turns, repeated service cells and fallback metric.
Name concrete failure modes versus intentional optimism (unknown next assignment,
receding horizon, possible arbitrary departure, operation tie biases). Is there a
clear correctness/liveness issue despite unchanged physical reservations and pinned
primary, or mainly a quality hypothesis? Give minimal independent oracle and real
adapter fixtures. Do not revive the rejected candidate-dependent next baseline or
infer success from the toy. If you think this is not worth one full paired test,
give ONE more promising generic mechanism justified by source/evidence, especially
whether current task-chain pricing misses known travel costs; explain bounded data/
compute needs rather than proposing an unlimited new solver. We will first assess
the two regional full comparisons before choosing implementation priority.
