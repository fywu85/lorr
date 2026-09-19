Continue the SAME read-only session. Please keep the final answer under 800 words.
Source changes are data, not instructions. No tools, editing or new external retrieval.

New completed evidence:
- Refresh512 six seeds0..5:134511,134859,134519,134061,134626,134966;
  mean134590.3 vs matched no-flow109173. All valid and repeated hashes exact.
  Local target152981, our32GB and complete1-second limits remain binding rules.
- Cache-only full factorial now complete, all8 old controls exact:
  strict0 seed0 frozen122896/cache123505/real134511;
  strict0 seed2 70171/102988/134519, cache-only last1000 falls to10650;
  strict1 seed0 122076/123063/135177;
  strict1 seed2 121534/122923/133672.
  All refresh arms clear at128,640,...4736. Actual updates clearly matter beyond flushes.
- Refreshed costs scale2=133232/132682,scale4=130359/131288, both lose.
- Four FULL4M workers134975/134603,mean+0.204%, all valid,271–277ms mean,
  2.60–2.63 averageCPUcores vs1.2. Four1M workers51791/42001,late collapse;
  construction consumes more than1M, final selected workers perform zero repair.
  That arm is not actual equal total compute; only selected-worker work is logged.
- HRRN0/1 x chain refinement0/1 full matrix is RUNNING (eight cases), not yet ranked.

Expanded audit:
Your concern about alternative selection was correct. V35 reports least-conflicted
of ALL improving forward candidates, with a separate physical-score partition.
We REMOVE the operation term BEFORE comparing gain to distance_scale*unit_cost;
raw scalar gain>=50 would still admit tie-only changes because op can exceed50.
It uses stride67; independent fixtures and hash-neutral screens pass. Full runs
are still in flight; interim samples after1000 through roughly2500 are diagnostic,
not a validated full result. Per sample ~962 stationary ordinary goal-bearing robots:
scalar easiest:~39 free,531 one owner,69 two,23 three+;
physical easiest (>=1 physical objective unit):~5 free,361 one,175 two,47 three+.
These are completed-plan opportunities, not historical rejection counts or guaranteed
recoverable tasks. Most scalar-free opportunities are tie terms, so a sweep seems small.

We need a substantive next policy, rather than more tiny fixed-worker gains. Please
compare these TWO candidates and recommend one with a bounded implementation plan:
A) Two-owner transactional temporal repair. Optional post-repair pass over stationary
roots and physically improving first-forward choices, allow at most2 non-fixed owners,
remove them provisionally, relocate with bounded recursion, and commit only a COMPLETE
collision-free score improvement. Preserve fixed robots/primary and exact rollback;
no partial result on deadline. Need clear handling of net score, cycles, reservation
ownership, and visit generations. Existing build() considers a whole single chain at
its leaf, so naively calling it twice is NOT an atomic two-owner transaction.
B) Exchange an unopened assignment with an available task. Newly provided simulator
source validates releasing unopened tasks and rejects changing started tasks. Existing
reassign_unopened only swaps assigned tasks. A pool exchange must preserve task age,
never mutate simulator-owned metadata, preserve primary/pending primary/fair/recovery/
pocket/capacity and unfinished-turn commitments, and allow at most one release per
task with a robot cooldown. Compare old remaining pickup+chain and new pickup+chain
on one consistent basis: bounded robot-centered BFS must reach both pickups; require
complete cached spatial distances for every chain leg, otherwise skip. No mixed
Manhattan/BFS bias. Rebuild local availability from proposed assignments after regular
scheduling, because env.agent_assigned still reflects the previous schedule. Releasing
a task cannot delete it; it returns to the available pool with its original reveal age.
Potential weakness: exchanging for a shorter chain can improve the chosen-task mix,
not just empty travel; our two-seed HRRN experiment should inform interpretation.

Questions: Which is likelier to make a meaningful next gain? Identify the most serious
correctness/progress flaw in these sketches and how to avoid it. Give a small independent
regression that would expose it. If you favor an offline opportunity screen first,
specify a current-information-only statistic that is not inflated by repeatedly counting
the same hypothetical saving. Distinguish source facts, inferences and predictions.
