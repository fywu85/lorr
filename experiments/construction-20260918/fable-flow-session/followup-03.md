Continue the SAME persistent read-only consultation. Please answer in <=800words.
New source diffs include cache-only mode and the const forward-opportunity audit;
the scheduler implementation is supplied for the first time as a relevant excerpt.
Do not infer it was already reviewed. All default controls remain hash-equivalent.

New established full data:
- Ordinary refresh512 now independently analyzed on seeds0/1/2/3:
  134511/134859/134519/134061, mean134487.5. Remaining two running.
- Complete strict matrix: frozen legacy122896/70171 vs strict122076/121534;
  refresh legacy134511/134519 vs strict135177/133672. Mixed effects, mean -0.067%.
- Collapsed frozen seed2 late1000 rotations:2025221planned,623966seed,2075protected.
  Only16421tied seed turns,0.62%of all turns. Strict rescue is NOT simply subtracting
  bulk tie actions. Healthy refresh has only9/8late tied seed turns.
- Refresh256=134052/134187 (-0.294%mean), refresh512+warm=134997/134794 (+0.283%).
  Full independent analyses complete, exact controls, all valid. No major extra gain.
- Cache-only SEED0 full simulator summaries (independent complete-matrix analysis
  still pending): legacy frozen122896/cache-only123505/real134511;
  strict frozen122076/cache-only123063/real135177. This supports real cost updates
  adding value beyond flushes. Cache-only uses the INITIAL learned field: equality
  would NOT make all learned flow a red herring. Seed2 still running.

Independent offline audit of refresh512:
- late1000 F/W/turns seed0=9057998/499223/442779, seed2=9069116/486474/444410.
- unit-spatial forward-closer/farther seed0=8826539/231459;
  seed2=8844684/224432; no unknown. Weighted-metric deltas not computed.
- top5%free cells hold26.39%/24.46%of waits, but22.32%/20.39%of all stationary
  actions. Required turns and traffic volume confound dwell counts. No strong
  concentrated queue target established; your25%threshold is heuristic.
- completed-chain unit-shortest mean242.32, loaded elapsed284.62,
  loaded-elapsed/shortest=1.17455 (both seeds almost identical).
  LocalKK:233.84,252.52,1.07990. Different completed cohorts, not causal attribution.
- all empty robot-steps including reassignments:10.299m/10.303m vsKK10.050m.
  KK last-assignment pickup statistics are reset by many reassignments; do not
  compare those directly with our first/final single-assignment pickup legs.
- mean decision237–257ms,p99324–340ms,max765ms,1.2CPUcores average of4allocated,
  ~11.9GB peakRSS. So there is compute/memory headroom under1second/32GB.

The new read-only audit samples the completed reservation table every64steps:
for each stationary ordinary goal-bearing root, best STRICTLY lower-cost first-FW
alternative, excluding its own reservations, partition none/free/1/2/3+nonfixed/
any-fixed blockers. This is NOT historical rejection attribution or a proof of
recoverable throughput. Regression fixtures and exact200step hashes pass; full
0/2 audit runs are underway. No branching search implemented.

Existing prefetch4 is a poor CPU lever:6400speculative tables,418used,5982discarded;
200stepwall45.777s vs43.259s, exactactions. It speculates on early priority roots,
while actual demand follows recursive PIBT chains. We leave it off.

Queued screens before full tests: refreshed flow cost scales1/2/4, and search
workers1x4M,4x1M,4x4M candidates (fixed PER-worker work, all workers join; any
timeout fails). These are existing generic options, no new code path. Strongest
policy still ~12%below152981local target; KK reference exceeds our32GB budget.

Questions:
1. Any substantive flaw in the const audit or current causal interpretation?
2. Given remaining loaded/empty gaps, choose at most TWO next generic improvements
   beyond the already queued tests, with a concrete mechanism and a falsifier.
   Reconsider cached-only weighted scheduler scoring: missing tables may favor
   under-estimated candidates, and current metric publication happens after scheduling.
3. Is using more fixed search work a credible next step, versus objective/assignment
   changes? Do not assume search saturation or claim experiments not performed.
No map-name/aisle/parity templates, future tasks, or future-run field inputs in
production. Offline future-field transplants are diagnostic only and not currently
needed to establish that cache-only falls short on healthy seed0.
