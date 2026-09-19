Continue the SAME persistent lorr-warehouse-cgar-fable51 session, turn12.
Fable5.1 max, read-only, no tools or unperformed test claims. Use prior context;
the attached source updates cover V44 and complete task-finish lifecycle.

Goal unchanged: generic 5,000-step / 10,000-robot warehouse >=152,981 tasks on
six seeds, complete fixed-work schedule+plan <=1s or explicit whole-run failure,
RSS<32 decimal GB, isolated GRID physical cores, no map IDs/templates/aisle rules,
supplied weights, hidden future tasks or map-specific fleet caps.

We implemented and tested your turn11 hypotheses separately. V44 exact source
f6d0ae448504762136c4f5d85f25d7d207207965; all22 source/test hashes verified. Default
surcharge0 and mixedOFF reproduce old full trajectories, not just task totals.
Effective turn price T=turn_cost*scale+surcharge (bounded16) reaches reverseoracle,
PIBT turn-forward macros, temporal scoring and both pickup searches; fallback
retains original unit-turn. Mixed mode gives worker0 the validated suffix and
three others cold; every worker finishes before strict-score comparison. New
regressions:88,704 independent pickup goal costs,12,288 weighted distances,
724,608 paid-progress scores,120,768 pure-unit scaling scores,11,520 unchanged
neutral decisions,12 production scheduler cases and3,840 mixed serial/parallel
robot decisions. No demonstrated correctness defect. Review the incremental
implementation for a real inconsistency if you see one, but don't invent one to
explain a negative policy result.

All12 V44 full runs are valid, max937.83ms and RSS12.147GB, four cores per process.
Seed0/2 task counts with matched controls:
- One cold worker, surcharge0:141829/142988.
- Surcharge1:141768/141778, mean -0.446%; final windows -72/-109.
- Surcharge2:141263/142599, mean -0.335%; loaded turns -9.04%/-9.54%,
  loaded waits +12.86%/+10.93%. Fewer turns is not enough.
- Four cold workers:143340/143409, exact V43 full controls.
- Four warm workers:142094/142669, mean -0.693%; tails -214/-139;
  loaded waits +5.50%/+3.79%.
- One warm + three cold:142724/142817, mean -0.421%; tails -293/-248;
  loaded waits +4.99%/+2.98%.
- All5000-step actual constructor counts verified: mixed4999warm/15001cold;
  allwarm19996warm/4cold; cold0warm/20000cold. No vacuous experiment.
Allfour candidate policies remain OFF, not promoted.

Other completed evidence since turn11:
- Four workers confirmed six seeds:143340/143767/143409/143359/142868/139710.
  Mean142742.1667 (+0.1876% vs one-worker142474.8333), but seed5 loses3207tasks,
  final1000 -1933, agep90+53. Not promoted. Allone-workercontrols exact.
- Eight workers (16 reserved cores):143157/142645, mixed vsone worker and both
  lose vsfour. Sixteen workers:143749/143571, mean143660 (+0.879% vsone,
  +0.199% vsfour), both full/tail wins, empty -0.15%/-1.00%, agep90 -6/-2.
  All20k entries<=1s, max954.12ms,RSS11.976GB. Mean380-392ms,CPU7.43-7.68cores.
  Sixteen-worker remaining four seeds now running with matched controls.
- Flow strength2/4/6/8 atscale4:2=141776/143213 (+0.0604%,mixed),
  4=141829/142988,6=141085/142854,8=139351/142470. Keep4. Strength2 lowers
  loaded waits~6.85% but no reliable throughput gain established.
- Full turn provenance: only54-83 tied wait-seed rotations per confirmed run;
  >99.5% of~2.2M turns are planned rotations. No blind strict-wait repeat.

New independent turn-reversal audit: seven complete 50M-robot-step replays,
four hand-counted fixtures pass, all old fields exact for3 overlaps and every
production phase/action count exact for6 CGAR traces. Adjacent reversals are
back-to-back opposite quarter-turns; stationary reversals permit intervening
waits, but no forward/unknown step or task/phase boundary. Events can overlap.
Per COMPLETED task (turns / waits / stationary reversals):
1worker s0:11.191/15.311/0.524;4worker s0:10.334/13.830/0.376.
1worker s2:10.355/14.100/0.432;4worker s2:10.193/14.119/0.371.
1worker s5:10.367/14.209/0.424;4worker s5:11.777/15.974/0.697.
LocalKK:5.678/8.411/0.110. Different completed cohorts, unfinished work censored,
not unique wasted actions or causal/additive savings. KK used38.858GB, beyond32cap.

We need a more structural next experiment, not another small parameter sweep.
Please prioritize at most TWO concrete generic changes, with code basis, a
realizable small fixture and a fixed-work validation plan. Two local ideas:
1. The temporal window scores/reserves only the current errand. Its late-goal
   holding reservations could block others although the robot moves to its known
   next errand after actual service. You suggested a const audit last turn: make
   that audit precise enough to implement, and say what concrete policy it would
   distinguish. Can known-visible next-errand continuation improve coordination
   without inventing unrevealed tasks? TaskManager advances at most one errand
   per tick, irrespective of orientation; updated source includes this lifecycle.
2. task_chain_cost still sums spatial legs while pickup sees heading/traffic.
   TurnDistanceOracle::peek(goal) already exists and has no LRU effect. A cheap
   option could price known remaining legs from complete cached oriented tables
   (minimum initial heading), fallback toexisting estimates, no extra tablebuilds
   and no cache/LRU/admission effects. Is its cache-dependent sampling likely
   misleading? How to preserve units and publication/cache invalidation without
   changing default behavior? Prior spatial Manhattan-to-BFS refinement did
   essentially nothing, and direct-cost/age-removal increased empty travel badly.
Please challenge either idea if flawed and identify a better code-grounded change.
Separate demonstrated bugs, intentional approximations and hypotheses. No causal
claims from turn counts alone, no arbitrary proxy-only permanent rejection rules,
and no declaration of success before full and six-seed results.
