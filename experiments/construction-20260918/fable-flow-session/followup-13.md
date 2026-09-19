Continue the SAME persistent lorr-warehouse-cgar-fable51 session, turn14.
Fable5.1 max, read-only, no tools, no claims of tests you did not run. Please keep
visible findings under1,200 words. Prior source comes through incremental diffs.

Goal: generic full warehouse,5,000steps/10,000robots,>=152,981 across six seeds;
complete fixed-work decisions<=1s or explicit failure, RSS<32decimalGB, reserved
physical cores. No map templates/identity/hidden future tasks. User explicitly
permits sharing source for your review. Do not suggest clock-selected partial work.

NEW confirmed reference: existing two-round regional repair with64 pickup fields,
1x4M global worker,4regions/4threads,25k attempts/region/round, temperature1000ppm.
Six totals144510/143933/144107/143134/143934/144029, mean143941.1667 (+1.029%).
All six matched totals improve; five tails/ages improve, seed1tail-108/age+1.
All60k candidate/control entries<=1s, maxcandidate962.69ms,RSS13.909GB; all controls
full-hash exact prior142474.8333 reference. Meanentry401.2-419.4ms,CPU1.726-1.770
cores/4reserved. Regional loadedturns fall4.72-13.44%, waits1.78-12.82% across six.
Higher pickup quotas64/128/256 with16fieldthreads are EXACT full-trajectory equals
on both0/2: no extra quality. Four/sixteen globalworkers remain mixed six-seed.

Post-service audit full0/2 is exact trajectory. Afterstep1000, only-known-next
postservice blocker class averages7.32/7.7 robots per sampled frame, about0.073% of
eligible/0.306-0.326% of qualifying improving observations. Finishing-only class
larger23/frame overall. This is not an upper bound on objective-change throughput,
and changed scores change the candidate universe; no causal bucket claim.

V47 now implements OPTIONAL CGAR_TEMPORAL_NEXT_ERRAND (defaultOFF), common
B=min_h D_N(G,h), as discussed. Existing native score untouched. Nonpinned robot
with task_pool task matching current goal, a revealed distinct valid next errand,
current exact oriented nonguided metric and already-cached next table only.
All builds finish before const non-LRU peeks; no next table admission/builds. All
headings atG and endpoints of geometrically valid arriving choices must be finite;
otherwise whole robot candidate set uses native metric. Candidates not reachingG
stay native. Arriving choices use terminal next potential, commonB, all actual turn/
forward extras, native virtual trailing-wait turns and op term. Next visited then
left still uses next terminal potential: deliberate surrogate, not exact service-time
objective. G=N falls back to native; pinned/protected behavior unchanged. Five
counters document known/eligible/unavailable/arriving/changed. Does not release any
reservation or alter fixed work/RNG/first-action validation. Final seed turn projection
still uses current-goal metric; kept fixed for this test. No neutral-tail policy.

All regression checks pass on reserved build8899126, source snapshotV47. Independent
Bellman distances plus explicit weighted action replay and enumerated suffix turns
check54,378 production scores (U1/4/8,T=U+0/1/2,directed tolls), headingpair-24/-16,
and two exhaustive compatible joint fixtures. Real adapter cases cover active next,
missing next table,G=N,current fallback,onlypinned next,disconnected next,guided
metric; exact native decisions on all six fallbacks, no extra builds/metadata writes.
An80-tick four-robot real task lifecycle fixture has46services and repeated-location
service, multiple flow publications and missing caches; serial/prep4 actions match.
4,800 protected regional action checks and existing primary/pocket/capacity tests pass.
200-step ON/OFF screen now running against newly confirmed regional control; full0/2
will follow only after timing/validity/exactOFF checks. No performance claim yet.

Please review the actual V47 diff for concrete bugs, invalid score accounting, cache
lifetime, protected/fallback behavior, missing meaningful tests and deadline risk.
Distinguish a demonstrable bug from an intentional approximation or unknown effect.
Do not turn the surrogate into a much larger redesign before the bounded experiment.

Second focused diagnosis: TemporalPibt::repair currently keeps FINAL annealed state
only if better than initial; otherwise rolls all work back. Regional counters show
many late batches rollback despite100k accepted attempts. We might retain the best
complete state encountered, but return only after ALL prescribed work finishes;
timeout still fails. That is fixed-work, not interruption-based output. Can retaining
best improve useful regional search without changing its trajectory/RNG? Propose a
cheap implementation avoiding an O(10k robots) copy on every improvement (dirty
selection journal since last best?), with reservation/score invariants and meaningful
fixtures. Global baseline should remain unchanged; optional regional mode only.
Also flag any reason these sampled rollbacks do not establish that a better interim
state exists. No code or experimental result is assumed beyond supplied evidence.
