# CGAR competition progress

Updated 2026-09-21T14:04:45.147566+00:00. General CGAR improvements and explicit instance tricks proceed
together across all ten LoRR2024 instances. Throughput is primary; fairness is
reported separately. The independent RANDOM-05 solver remains untouched.

All selected results are complete runs under enforced **1,000 ms** entry and
**32 decimal GB** memory limits. A timeout fails the run. WAREHOUSE and SORTATION
use eight physical cores; the other selected rows use four.

| Instance | CGAR best | max(NMS, KK) | New target | Remaining | Seed | Max step (ms) |
|---|---:|---:|---:|---:|---:|---:|
| WAREHOUSE | 155,173 | 154,795 | 162,535 | 7,362 | 0 | 944.77 |
| SORTATION | 150,894 | 152,714 | 160,350 | 9,456 | 0 | 993.90 |
| CITY-01 | 8,453 | 8,445 | 8,868 | 415 | 6 | 764.78 |
| CITY-02 | 16,840 | 16,997 | 17,847 | 1,007 | 8 | 802.56 |
| GAME | 24,447 | 23,274 | 24,438 | 0 | 4 | 826.52 |
| RANDOM-01 | 693 | 688 | 757 | 64 | 4 | 488.91 |
| RANDOM-02 | 1,215 | 1,260 | 1,386 | 171 | 2 | 359.38 |
| RANDOM-03 | 1,982 | 2,334 | 2,568 | 586 | 2 | 662.92 |
| RANDOM-04 | 2,398 | 2,547 | 2,802 | 404 | 4 | 548.10 |
| RANDOM-05 | 3,610 | 3,050 | 3,355 | 0 | 10 | 729.83 |

Every selected profile is a **TRICK**, enabled through `--trick INSTANCE`. These
are selected seed maxima, not averages of one universal configuration. The new
goals are +10% over max(NMS, KK) on RANDOM-01–05 and a stretch +5% on the other
five. GAME currently meets its new target. Published scores are historical targets,
**not matched local competitor runs**. WAREHOUSE has exclusive-host qualification
and independent action replay. Other rows use shared GRID hosts with disjoint
bound physical cores, no CPU quota, simulator validation and complete movement
and waiting accounting.

[Timestamped history and source commits](experiments/allmaps-20260920/BEST_HISTORY.md),
[exact settings and evidence](experiments/allmaps-20260920/selected-full-results.json),
[all-ten checks](experiments/allmaps-20260920/selected-results-checks-20260921-140445.json),
[published targets](experiments/allmaps-20260920/TARGETS.md).

The earlier RANDOM-01 chain profile reached **647** with general remaining-chain scoring and priority
ordering, combined with its existing field and scheduler tricks. Three new seeds
score 645/647/595 versus 621/635/631 for the control: both means are 629. The earlier
seed 8 scored 644 versus 638. Thus three observed seeds exceed published NMS 639,
but this is not an average-gain claim. The chain option remains default-off.
[Replication](experiments/allmaps-20260920/chain-potential/random01-three-seed-summary.json).

The chain calculation matches independent action/service shortest paths across
162,900 states, including orientation, repeated stops and pocket escape. Production
tests cover 8,000 identical serial/parallel robot actions. Full regression passes;
38 source/test files match [7783736](https://github.com/fywu85/lorr/commit/7783736fec8dd929316fdaa2bd6d0b4fdd92c94c).
The same option loses on RANDOM-02/03/04. RANDOM-03 scores 1,902 control / 1,491
score-only / 1,677 order-only / 1,203 both; loaded waiting rises sharply. RANDOM-04
scores 1,999/1,971/1,951/1,775. Exact static cost is not congested travel time.
[Factor diagnosis](experiments/allmaps-20260920/chain-potential/first-factor-results.json).

GAME reaches **24,447** with pickup weight 12 and 3,250 active task receivers.
It improves all three tested seeds: 24,326/24,360/24,447 versus
23,977/23,917/23,888. All 6,500 robots remain movable, but half get no new tasks:
this is a deliberate fairness tradeoff, not a starvation-free claim. Held and
started tasks remain protected. Known-horizon admission loses on GAME and CITY-02.
[Replication](experiments/allmaps-20260920/game-fleet/dispatch-three-seed-summary.json).

The previous RANDOM-05 frontier reached **3,065** (selected seed 10) with an explicit known-horizon admission trick
composed with complete-chain scoring. The mean-margin variant adds 16 tasks to the
identical 3,027 control on seed 0. Its three-seed aggregate gain is 0.19%, with
one loss. A declared seed scan gives 2,988/3,019/3,065/2,913 on seeds 6/8/10/12.
This exceeds historical NMS 3,050 at one seed; it is not a matched comparison.
[Horizon comparison](experiments/allmaps-20260920/chain-potential/random05-horizon-summary.json).
The underlying chain-score gain, retaining the previous priority order, replicates across seeds0/2/4: 3,027/3,010/3,022 versus
2,956/2,900/2,889 (+3.59% aggregate), all with identical repeated control trajectories.
[Chain replication](experiments/allmaps-20260920/chain-potential/random05-three-seed-summary.json). The prior
32-start change improved all three seeds: 2,956/2,900/2,889 versus 2,877/2,865/2,836.
Nominal global candidate caps sum to 4M in both cases, but construction and
completed-attempt work can differ. Eight regions and 16 rounds on eight cores
separately score 2,946 with a 799.65 ms maximum; composition with32starts loses
(2,926/2,893/2,946 versus2,956). Peak
retention loses two of three seeds and the mean, so it remains off here.
[Replication](experiments/allmaps-20260920/regional-search/random05-diversity-three-seed-summary.json).

RANDOM-04 now reaches **2,059** with a general20-action common-continuation
comparison of four complete CGAR roots. The15-action version improves all three seeds:2,023/1,960/1,913versus
1,999/1,938/1,876 (+1.43%aggregate). Extending it to20actions then gives2,059on
seed0, with283.86ms maximum. The20-action version loses the other two seeds and reduces the three-seed mean
by1.58%against the15-action version; its2,059is a selected-seed maximum.
10-action forecasts and adding five-step chain scoring lose. Existing KK field, turn2, hotter regional search
and peak retention remain explicit tricks. Peak retention alone had reduced the
three-seed mean against the hot control. RANDOM-02 scheduling improvements
replicate across three seeds and reach 1,197; nearby turn prices lost. CITY-02
similarly reaches 16,315. More frequent finite rematching loses on RANDOM-03.

SORTATION retains **150,894**. Its identical trajectory repeats at998.94ms maximum,
still close to the deadline. Two parallel starts of1Mcandidate work score150,830
with an888.48ms maximum; this improved runtime alternative is recorded separately
without replacing the throughput frontier. Four-start variants also lose.
[Runtime alternative](experiments/allmaps-20260920/sortation/runtime-alternative-v2.json).
Earlier failed runs retain no accepted partial score.

The solver retains CGAR primary/recovery mechanisms and its documented NMS-derived
temporal PIBT component. These guidance, scheduling and fleet adaptations do not
replace it with either competitor's complete solver. [Next work](experiments/allmaps-20260920/TRICK_ROADMAP.md).

The user explicitly authorized further PILOT references. Its sparse/medium maps
use longer coordinated windows; its crowded maps use a two-phase movement pipeline
and common future continuations. These mechanisms remain distinct from the chain
score already transferred. [Frozen review and next implementation](experiments/allmaps-20260920/pilot-transfer/NEXT_TRANSFER.md).

The optional CGAR-seeded 20-step window now reaches **662 on RANDOM-01** and
**1,215 on RANDOM-02**, above the previous selected 647 and 1,197. The key change
keeps real primary/support actions immutable while allowing compatible later
forecasts to move. All three matched window controls reproduce their full earlier
trajectories. Against the previous frontier profiles, RANDOM-01 scores 658/662/648 versus
645/647/595 (+4.29% aggregate, all positive). RANDOM-02 scores 1180/1215/1202
versus 1188/1189/1197 (+0.64% aggregate, one loss). Earlier control trajectories repeat.
RANDOM-03 subsequently improves to **1,939** with retained history disabled.
It scores 1,939/1,900/1,870 versus 1,902/1,798/1,848 on seeds0/2/4, a **2.90%**
aggregate gain with all three positive. Its maximum across the replication is
633.22ms on four cores. Keeping long prior tails loses here. More starts on eight
cores also lose; doubling four-island work to8192times4 exceeded the deadline.
[Replication](experiments/allmaps-20260920/rolling-window/random03-three-seed-summary.json).
More search and safer future forecasts are separate factors; neither is universal.
[Protected-forecast comparisons](experiments/allmaps-20260920/rolling-window/protected-prefix-results.json).

SORTATION's new runtime alternative scores **150,865**, only 29 below its frontier,
with a maximum **894.99 ms** in the measured run. It combines two parallel starts
with more frequent bounded rematching. This remains an alternative with more
observed timing headroom, not a throughput record.
[Exact configuration](experiments/allmaps-20260920/sortation/runtime-alternative-v3.json).

The static-field follow-up finds no new gain: the exported PILOT field loses on
RANDOM-01/02/03, and lowering opposing-lane penalties loses on both CITY cases and GAME.
Existing fields remain selected. [Complete comparisons](experiments/allmaps-20260920/field-options/first-results.json).
A separate default-off experiment now compares complete CGAR roots under common
longer continuations. It preserves protected actions, uses fixed complete work,
and does not claim to reproduce PILOT's two-phase pipeline. Full regression passes, including7,200serial/parallel production actions, exact
one-root trajectory identity and late-timeout failure. Forty source/test hashes
match source965756fd. Full crowded-case benchmarks and replications are running.
[Declared design](experiments/allmaps-20260920/common-futures/README.md).

RANDOM-01 first reaches **682** with uniform forward costs at the same
4096repair attempts per island. Its662control repeats exactly; the uniform arm
at lower2048work had reached656. Turn40loses in both fields. The gain repeats across seeds0/2/4:671/668/682versus658/662/648,
**+2.69%aggregate**, allpositive. RANDOM-02 retains1215;
turn40and lower wait price lose. [Window cost factors](experiments/allmaps-20260920/results/random01-window-costs-full-v8/verification.json).

Completing the joint tail of a retained10-action prefix then raises RANDOM-01 to
**693**. It improves all three seeds:685/681/693versus671/668/682, **+1.88%aggregate**.
Maximum488.91ms on the record run, four cores. This exceeds the published NMS/KK
maximum688at one seed, but remains below the new757target. The same history option
loses on RANDOM-02and falls just below the history-off RANDOM-03record.
[Replication](experiments/allmaps-20260920/history-rollout/random01-three-seed-summary.json).

CITY-01pickup-selected matching groups reach **8,440**, with8430/8440/8434versus
8423/8427/8425on seeds0/2/4 (+0.11%aggregate). It is a small replicated gain, still
below the published maximum8445and new8868target. More allowed rematches loses.
[Replication](experiments/allmaps-20260920/rematch-budget/city01-transfer-three-seed-summary.json).

All three original common-future RANDOM-05arms reproduce the exact3,065trajectory
and change zero first actions; keep the option off there. Their fresh control
failed at timestep0(1000.312ms, regional repair), and has no accepted score. The
cause is not established. A new control is included in the startup comparison.
[Failure and identity evidence](experiments/allmaps-20260920/common-futures/random05-first-diagnosis.json).
The new oriented-startup option passes functional tests but loses on RANDOM01–04;
RANDOM05is still running. No startup option has been promoted.

RANDOM-01more work does not improve the selected method:8192attempts per island
scores676versus693, group8scores691, and16384attempts fails at timestep163. The
record keeps4096attempts/group4. Next work targets high-delay repair neighborhoods
instead of increasing all work. [Fixed-work scaling](experiments/allmaps-20260920/history-rollout/work-scaling-results.json).

The regional-checkpoint future pool passed full regression after a lifetime fix
caught by its new integration test. All40source/test hashes match697f423e; the
failed first build is preserved and was never benchmarked. Full RANDOM04/05
matrices now compare complete regional checkpoints, because the original global
roots never displaced RANDOM05's stronger regional incumbent.
[Qualification](experiments/allmaps-20260920/common-futures/regional-build-v2/verification.json).

Delay-directed window repair raises RANDOM-03 to **1,967**, compared with an
identical 1,939 control trajectory. The selected run peaks at 611.68 ms on four
physical cores; replication on seeds 2/4 is running. The same option loses on
RANDOM-01 and RANDOM-02. [Declared comparisons](experiments/allmaps-20260920/window-delay/first-results.json).

The RANDOM-02 delay3 run first failed its deadline after the diagnostic sample at
step 199. Node-local log spooling then reproduced its 1,215 control and completed
the candidate at 1,164. The candidate is rejected; the earlier failure is retained.
Shared-filesystem logging is a plausible source of that timing spike, not proven.
[Full rerun and log preservation](experiments/allmaps-20260920/local-log-spool/full-run-results.json).

Regional-checkpoint future selection changes actions on RANDOM-05, but all three
horizons lose: 2,937 / 2,978 / 2,971 versus the reproduced 3,065 control. RANDOM-04
regional roots also lose to its 2,059 global-root selection. Keep these extensions
off in both selected profiles. The oriented-startup factor loses on all five
RANDOM cases. Two startup verifier errors were fixed and preserved; the final
audit reconciles 769 initial assignments, excluding 31 parked robots.
[Regional RANDOM-05 results](experiments/allmaps-20260920/common-futures/regional-random05-results.json).

A separate default-off annealed window now passes the full regression suite,
including 12,000 serial/parallel actions, complete rollback, best-plan restoration
and explicit failure after a deadline despite finding a useful plan. It completes
all declared work before returning. Full sparse-map factors are running, plus a
bounded check of the existing window on crowded maps.
[Annealed design and qualification](experiments/allmaps-20260920/window-anneal/README.md).

The RANDOM-03 delay3 gain now replicates on all three seeds: **1,967 / 1,982 /
1,948 versus 1,939 / 1,900 / 1,870**, a **3.29%** aggregate gain. Both new controls
repeat their earlier complete trajectories. The selected best becomes **1,982**,
seed 2, with a maximum 662.92 ms. [Replication](experiments/allmaps-20260920/window-delay/random03-three-seed-summary.json).

All first annealing factors lose on RANDOM01/02/03. One follow-up checks its
interaction with long retained/refreshed history on RANDOM-03, as used by the
PILOT reference. The crowded window variants lose on RANDOM-04 (1,913 / 1,734 /
1,944 versus 1,999 without futures); RANDOM-05 is still pending. Wider complete
assignment groups are the next general scheduling experiment, with unchanged
primary/started-task protections and total participant quota.
[Assignment experiment](experiments/allmaps-20260920/matching-width/README.md).

The long-history/annealing interaction also loses on RANDOM-03. Wider matching
leaves the complete RANDOM-01 trajectory unchanged and loses on RANDOM-02/03;
CITY comparisons are pending. RANDOM-05's refreshed window reaches 2,921 from
its 2,844 chain-score-off control, still below the selected chain-scored 3,065.
This motivates testing whether the window can improve the stronger chain-scored
seed with consistent oracle units. No composition has yet been implemented.

Fable was retried in the same authorized Claude Code session after 16 hours, but
still reports exhausted usage credits. No review findings were produced.
[Consultation receipt](experiments/allmaps-20260920/fable-followup/turn47/metadata.json).

2026-09-21T11:37 UTC — Full RANDOM-05 paid-action scoring loses:3065control,
2544paid,3018strict-wait-only,2553both. Four valid full2000 cases under1s;
no frontier promotion. See allmaps/chain-paid/first-results.json. Dynamic task-cap
and action-price matrices remain separate experiments, with their selected controls.

2026-09-21 — RANDOM-05 reaches **3,130** completed tasks with the explicit700-task
admission cap, against an identical3,065 control trajectory on seed10 (+2.12%).
The full2000-step run has585.66ms mean,695.18ms maximum entry time and240MB peak RSS.
The assignment audit preserves all held tasks; agep90 remains2000, so this is no
starvation-free claim. Caps600/750 and all RANDOM04 caps lose. Seeds0/2 replication
is declared. [Evidence](experiments/allmaps-20260920/task-cap/first-results.json).

2026-09-21 — One-action movement commitments raise RANDOM-04 to **2,237** from
an identical **2,059** control (+8.65%, seed0). Forward-only commitments score
2,190; carrying forward/wait occupied cells scores2,237. All three complete the
full1000 steps under1s; the selected run peaks at302.54ms. The mechanism remains
CGAR with current primary/recovery protections and a replanned temporal tail.
Both modes are being checked on seeds2/4. [Evidence](experiments/allmaps-20260920/move-promises/first-results.json).

The movement-commitment gain replicates on RANDOM-04 seeds0/2/4: **2,237 / 2,298 /
2,215** versus **2,059 / 1,901 / 1,843**, a **16.32% aggregate improvement**.
Forward-only promises also improve all three (+11.53%). The selected best is now
**2,298**, seed2, with306.43ms maximum entry time. This measures improvement over
our previous solver, not over NMS. [Matched replication](experiments/allmaps-20260920/move-promises/random04-three-seed-summary.json).

2026-09-21 — RANDOM-05 reaches **3,571** on seed10 with forward/wait movement
commitments, against an identical **3,065** uncapped control (+16.51%). Forward-only
commitments score3,391. All three full2000-step runs pass strict1s; the selected
run peaks at712.32ms. This clears our3,355target and is17.08%above historical
max(NMS,KK)=3,050, not a matched competitor result. No new task cap is used.
Seeds0/2 replication and a separate cap700 composition are declared.
[Evidence](experiments/allmaps-20260920/move-promises/first-results.json).

2026-09-21T12:21:40.759210+00:00 — Full window/commitment composition loses on all three sparse
selected seeds: RANDOM01 693→687/688, RANDOM02 1215→1197/1172, RANDOM03
1982→1848/1942 (forward-only/forward-and-wait). All controls repeat exact
trajectories and all runs pass strict1s. Keep the sparse profiles unchanged.
The new movement commitment remains useful on the crowded cases. Removing
RANDOM04 futures loses22 tasks in aggregate across three seeds; retain H20.
[Window results](experiments/allmaps-20260920/window-move-promises/first-results.json).

2026-09-21T12:36:14.043899+00:00 — RANDOM05 selected maximum becomes **3610**, seed10, by adding
explicit cap700 to mode2 movement commitments (3571matchedcontrol,+39). Bothfull
2000-step runs pass strict1s; control is trajectory-identical. This cap interaction
is single-seed evidence. No starvation-free claim.
[Evidence](experiments/allmaps-20260920/move-promises/cap-composition-results.json).

2026-09-21T13:00:02.970449+00:00 — CITY-02 reaches **16,830** at opposing lane price24, up515
(+3.16%) from its matched16,315 control, max809.83ms. RANDOM-04 reaches**2,304**
with terminal crowd price16, a six-task selected-seed gain. Both full controls
repeat exact trajectories; replication is queued before claiming consistency.
RANDOM-05 movement commitments now improve all three matched seeds by16.56%
in aggregate (3532/3531/3571 vs3043/3015/3065), all strict1s. The3610 cap result
remains a separate selected-seed record.

2026-09-21T13:13:58.770458+00:00 — RANDOM-04 selected maximum is **2,321**, seed4, with
terminal crowd price16. Three-seed comparison:2176/2304/2321 against2237/2298/2215,
aggregate+0.756%, with one loss. This is a modest seed-sensitive gain; all full
runs and controls pass strict verification.
