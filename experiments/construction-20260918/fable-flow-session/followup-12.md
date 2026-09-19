Continue the SAME persistent lorr-warehouse-cgar-fable51 session, turn13.
Fable5.1 max, read-only, no tools or claims of tests you did not run. Turn12 emitted
findings but ended error_max_budget_usd; we archived them as partial findings. This
is a focused follow-up. Keep the visible answer under1,000 words plus formulas.

Goal unchanged: generic warehouse,5,000 steps/10,000 robots, >=152,981 across six
seeds, complete fixed-work decisions<=1s or explicit failure, RSS<32decimalGB,
isolated GRID physical cores, no map identities/templates/hidden future tasks.

Sixteen-worker six-seed results:143749/143241/143571/143643/141876/142587,
mean143111.1667 (+0.44663% vsone-worker142474.8333), with three total/two tail
regressions. Not promoted as confirmed reference. All12 valid; six one-worker full
trajectories exact V42. More global workers offers only a small average gain so far.

Existing regional repair now runs with confirmed64 pickup fields, one global4M
worker,4regions,25k fixed attempts/region, one/two rounds, seeds0/2 with disabled
controls. Screen valid/exact-control, actual retained repairs. Full one-round raw
totals142237/141623 vs141829/142988; two-round and complete validation pending.
This does not rule out other search structure.

V45 exactc339e8b4b3896c18234be0b1d4120a00ffcca519 (22 build hashes verified) adds
only an optional const post-service audit. Eligible: all nonfixed positive-weight
current-goal robots, including selected FW. Every candidate improving distance/turn
heuristic >=1 unit after removing op ties is considered, even if native scalar cost
loses. Choose cheapest in easiest free / only-post-service / other conflict class.
First owner current-goal hit is service; strictly later cell/edge slots post-service.
Own excluded; fixed/arrival-slot/pre-service/no-arrival owners count as other.
Tags known-next / finishing / mixed. No RNG/cache/reservation/search changes.
Counts and overlapping gains are not removable-blocker/throughput bounds. Source
includes13 hand-counted fixtures. Every regression passes;4,800 protected serial/
parallel decisions; disabled/stride67 screen exactly reproduces baseline trajectory.
Full seeds0/2 stride67 audit pending,74 frames/run, must reproduce full hashes.
No continuation policy implemented yet.

We exhaustively evaluated your3x9 fixture using exact native129-operation scores:
A=(1,2)E->(1,3),next(1,8);B=(1,1)E->(2,6). Collision-free joint minimum chooses
first FW for both (Aop58,Bop85,cost-772). Claimed first rotation does not reproduce
under exact minimization; bounded production search could differ. Our independent
probe DID find two realizable3x7 examples among987 valid two-robot inputs (attached).
ExampleA=(1,4)W->(1,3),next(0,3),B=(0,5)S->(1,1). Native minimum
A:FWWWW/B:RFFFF costs-1040; collision-free A:FRWFW/B:FRFFF witness costs-912,
serves A's next errand and lets B first-FW. This establishes a scoring limitation,
not warehouse prevalence or throughput.

Design ONE precise optional known-next-errand score for a bounded experiment.
Your proposed credit D_next(current_goal,arrival_heading)-D_next(endpoint,heading)
uses a candidate-dependent baseline: arriving with a worse heading can create more
credit. Is this a real failure? Give a realizable path pair or a rigorous accounting
argument; propose a consistent alternative if needed. Do not assume free tablebuilds.

Local alternative to challenge (NOT implemented): candidates reaching current goal
use common B=min_h D_next(current_goal,h), then d=-4*U + D_next(endpoint,h)-B +
paid extra costs of ALL actual turns/forwards, retaining terminal-wait virtual-turn
accounting. Others retain native score. This avoids candidate-dependent B but may
have a branch discontinuity or mishandle second service/repeated-location errands.
TemporalGeometry::cost overwrites d=-t*U on EACH current-goal hit (last hit wins),
charges extra turns only through FIRST goal hit, and forward_surcharge stops there.
TaskManager advances at most one errand per physical tick regardless of heading.

Give an exact score/data policy covering candidates without current service,
different arrival headings/times, known-next service inside window, repeated-location
errands, all actual turn/toll extras, final virtual rotations, fallback/table
availability, cache/publication invalidation and protected robots. Preserve default
trajectory when OFF. Prefer existing complete cached tables or a bounded common
spatial metric, no hidden future errands or clock-selected partial work. Identify
independent-oracle fixtures before200-step feasibility and full0/2 runs. If too
fragile, explain concretely and give one simpler generic alternative grounded in
source. Separate bugs, approximations and hypotheses; no toy-derived success claim.
