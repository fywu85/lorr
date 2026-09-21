# CGAR competition progress

Updated 2026-09-21T09:06:12.707401+00:00. General CGAR improvements and explicit instance tricks proceed
together across all ten LoRR2024 instances. Throughput is primary; fairness is
reported separately. The independent RANDOM-05 solver remains untouched.

All selected results are complete runs under enforced **1,000 ms** entry and
**32 decimal GB** memory limits. A timeout fails the run. WAREHOUSE and SORTATION
use eight physical cores; the other selected rows use four.

| Instance | CGAR best | max(NMS, KK) | New target | Remaining | Seed | Max step (ms) |
|---|---:|---:|---:|---:|---:|---:|
| WAREHOUSE | 155,173 | 154,795 | 162,535 | 7,362 | 0 | 944.77 |
| SORTATION | 150,894 | 152,714 | 160,350 | 9,456 | 0 | 993.90 |
| CITY-01 | 8,427 | 8,445 | 8,868 | 441 | 2 | 753.90 |
| CITY-02 | 16,315 | 16,997 | 17,847 | 1,532 | 0 | 821.72 |
| GAME | 24,447 | 23,274 | 24,438 | 0 | 4 | 826.52 |
| RANDOM-01 | 668 | 688 | 757 | 89 | 2 | 467.96 |
| RANDOM-02 | 1,215 | 1,260 | 1,386 | 171 | 2 | 359.38 |
| RANDOM-03 | 1,939 | 2,334 | 2,568 | 629 | 0 | 586.09 |
| RANDOM-04 | 2,023 | 2,547 | 2,802 | 779 | 0 | 268.97 |
| RANDOM-05 | 3,065 | 3,050 | 3,355 | 290 | 10 | 709.47 |

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
[all-ten checks](experiments/allmaps-20260920/selected-results-checks-20260921-090612.json),
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

RANDOM-05 now reaches **3,065** (selected seed 10) with an explicit known-horizon admission trick
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

RANDOM-04 now reaches **2,023** with a general15-action common-continuation
comparison of four complete CGAR roots. The seed0control repeats1,999; shorter
10-action forecasts lose. The maximum is268.97ms on four cores. This selected-seed
gain is awaiting replication. Existing KK field, turn2, hotter regional search
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
match source965756fd. Full crowded-case benchmarks are running.
[Declared design](experiments/allmaps-20260920/common-futures/README.md).

RANDOM-01 subsequently reaches **668** with uniform forward costs at the same
4096repair attempts per island. Its662control repeats exactly; the uniform arm
at lower2048work had reached656. Turn40loses in both fields. Two additional
paired seeds are running before any average-gain claim. RANDOM-02 retains1215;
turn40and lower wait price lose. [Window cost factors](experiments/allmaps-20260920/results/random01-window-costs-full-v8/verification.json).
