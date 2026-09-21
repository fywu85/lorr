# CGAR competition progress

Updated 2026-09-21T06:56:53.762074+00:00. General CGAR improvements and explicit instance tricks proceed
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
| RANDOM-01 | 647 | 688 | 757 | 110 | 2 | 202.09 |
| RANDOM-02 | 1,197 | 1,260 | 1,386 | 189 | 4 | 183.38 |
| RANDOM-03 | 1,902 | 2,334 | 2,568 | 666 | 0 | 298.41 |
| RANDOM-04 | 1,999 | 2,547 | 2,802 | 803 | 0 | 238.42 |
| RANDOM-05 | 3,027 | 3,050 | 3,355 | 328 | 0 | 744.30 |

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
[all-ten checks](experiments/allmaps-20260920/selected-results-checks-20260921-0656.json),
[published targets](experiments/allmaps-20260920/TARGETS.md).

RANDOM-01 reaches **647** with general remaining-chain scoring and priority
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

RANDOM-05 now reaches **3,027** with complete-chain scoring and the previous
priority order, versus its exact 2,956 control. Replication is running. The prior
32-start change improved all three seeds: 2,956/2,900/2,889 versus 2,877/2,865/2,836.
Nominal global candidate caps sum to 4M in both cases, but construction and
completed-attempt work can differ. Eight regions and 16 rounds on eight cores
separately score 2,946 with a 799.65 ms maximum; composition is running. Peak
retention loses two of three seeds and the mean, so it remains off here.
[Replication](experiments/allmaps-20260920/regional-search/random05-diversity-three-seed-summary.json).

RANDOM-04 retains **1,999** with KK forward guidance, turn price 2, hotter search
and peak retention. Retention gives the best individual run but reduces the
three-seed mean against the hot control. RANDOM-02 scheduling improvements
replicate across three seeds and reach 1,197; nearby turn prices lost. CITY-02
similarly reaches 16,315. More frequent finite rematching loses on RANDOM-03.

SORTATION retains **150,894**. Its identical trajectory repeats at 989.60 ms
maximum, still close to the deadline. A reduced-work alternative scores 150,780
with a 900.70 ms maximum and is recorded separately. Parallel starting-plan
comparisons are running. Earlier failed runs retain no accepted partial score.

The solver retains CGAR primary/recovery mechanisms and its documented NMS-derived
temporal PIBT component. These guidance, scheduling and fleet adaptations do not
replace it with either competitor's complete solver. [Next work](experiments/allmaps-20260920/TRICK_ROADMAP.md).

The user explicitly authorized further PILOT references. Its sparse/medium maps
use longer coordinated windows; its crowded maps use a two-phase movement pipeline
and common future continuations. These mechanisms remain distinct from the chain
score already transferred. [Frozen review and next implementation](experiments/allmaps-20260920/pilot-transfer/NEXT_TRANSFER.md).
