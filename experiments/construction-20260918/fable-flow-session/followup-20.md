Continue the same persistent CGAR research session, Fable5.1/max, read-only.
No code changes or tools. Use the existing context; production remains exactV50.
We are consolidating new evidence before choosing the next generic mechanism.
Do not promise a recoverable gain or treat pending runs as results.

Your turn20 review led to stronger KK fixtures: exact post-call owner metadata,
real TaskManager two-call lifecycle with pickup/completion/replenishment, synthetic
nonuniform directional costs, invalid-toggle failure, and native configuration
loader checks. Fifty case records pass. Native/default/explicit-enabled agree;
disabled retains holders. The proposed agents>unassigned-tasks case is outside
this input's invariant:15,000 ongoing tasks/10,000 agents, surplus5,000 regardless
of number assigned. We preserved the original native assumption.

Five full KK runs are pending: supplied/uniform weights × rematch/keep plus one
repeated supplied/rematch control, each exclusive16physical/32SMT EPYC9354 cores,
same binary/input/5,000steps/native1s clock limit. V2 logs scheduler wall time,
actual LNS limit/remaining time, merged iterations, costs, and entry elapsed
before final diagnostic print. The MAPFPlanner wrapper indeed subtracts scheduler
time. Uniform weights change both scheduling and motion. We do not call this
equal-work or a CGAR32GB/complete-entry-qualified score. All corrected200step
validity screens pass; scores are NOT used for quality ranking. Our initial
uniform config incorrectly used a scalar empty string instead of the conditional
array; failure evidence is retained and actual-loader fixtures validate the fix.

NEW VERIFIED TASK-POOL REPLAY (historical KK and six CURRENT regional CGAR seeds):
All7 original travel-accounting reports match exactly, raw hashes match earlier
independent replays, all4 base fixtures plus new hand-counted task-lifecycle
snapshot fixture pass. Snapshots at0,1000,...5000 reconstruct actual assignment
intervals and remove completed tasks. They distinguish unassigned, held-unopened,
started, never-assigned and previously-assigned-but-now-unassigned tasks.

At step5000:
- KK:5025 unassigned, mean whole-chain527.6386, mean age2706.8975,p904877.
  1959 held unopened, chain427.7397, age1761.6830.
  5130 unpicked tasks aged>=1000;1642 unpicked tasks aged>=4000.
  Unassigned includes4952 never-assigned and73 previously-assigned tasks.
- CGAR across6seeds:5021..5044 unassigned, chain306.2008..307.3131,
  meanage100.5215..104.0303,p90209..214. NO unpicked task aged>=1000
  at ANY sampled checkpoint on any seed (not claiming between-checkpoint bound).
- KK unassigned meanchain at1000/3000/5000:480.2283/513.9714/527.6386;
  corresponding meanage641.5987/1667.4712/2706.8975.
- CGAR seed0 corresponding chain305.8178/310.1487/306.2008,
  meanage105.3908/99.8847/100.5215; all6 show same qualitative pattern.
- Completed chain mean remains KK233.8357 vs CGAR pooled241.861.

This supports persistent long-task deferral before pickup, beyond simply counting
long tasks still in service at the horizon. It does NOT causally quantify the8.025
chain gap or prove all of it recoverable; different policies reveal different task
prefixes. We keep KK's score as the target, while making its waiting-time tradeoff
visible. A generic cost-based scheduler is legitimate; no map-template exploitation.

CURRENT CGAR assignment details relevant to design:
- Ordinary pairing uses cost=published-scale*spatial-chain + heading/traffic-aware
  pickup; HRRN orders by1+(age*published-scale)/cost, then cost. Existing HRRNoff switches ordinary
  ranking to cost while retaining explicit oldest-task admission.
- Fair admission inserts the oldest free task after10,000 regular admissions;
  it is a progress mechanism, not a tight waiting-time bound.
- Existing rematch eligibility protects started tasks, current/next primary,
  recovery transactions, fair-admitted tasks,20step cooldown and once-retargeted
  tasks. Native old pool pass uses32 exchanges/10ticks, up to256 source holders,
  spatial pickup-improvement guard, cached complete chain distances only.
- The confirmed HRRNoff test lost on the older reference, despite shorter chains;
  ~17.0/17.6% extra empty work. The later cost-only shortlist test retained HRRN
  and is different. Do not conflate or repeat these as a new discovery.
- Mean current complete entry is~0.4s with worst~0.92s; fixed work/counts only,
  deadline breach fails explicitly. A naive full oriented-distance field for all
  ~1700 unopened holders each tick is not an established feasible budget.

Please reassess the next implementation AFTER the pending KK factorial, using
these observations and the current blocker evidence below. Answer concisely:
1. Does this change your assignment/rematching hypothesis or its interpretation?
   What would each main factorial outcome actually support or weaken?
2. If assignment is selected, propose ONE bounded CGAR design that differs from
   the failed old pool pass. Specify retained assignments, eligible roots/tasks,
   cost/price consistency, released-task externalities, progress protection,
   deterministic work limits and a baseline/additive/rematch/both comparison.
   Keep richer chain pricing as a separate factor. Explain which fairness
   guarantees remain versus which waiting-time statistics may worsen.
3. If motion is selected, state the single most discriminating small experiment
   before a larger planner rewrite. Avoid merely repeating the old two-owner pass,
   stronger score weights, more geometric regions or intended-route guidance.
   A heuristic blocker is not proof of a feasible replacement.


NEW VERIFIED CURRENT BLOCKER AUDIT:
FullV50 seeds0/2, every67steps(74frames/seed), complete post-global+regional-repair
reservation snapshots. Diagnostic option is read-only: exact144510/144107 tasks,
full trajectory hashes, all25sampled global/start/rotation/regional/orientation
frames and50 regional-round frames reproduce. All10,000 complete entries<=1s,
max928.438ms, RSS11.334GB, isolated4physicalcores/seed,noquota.

The following final1000means have15frames/seed. Every stationary nonfixed
positive-power root is classified once. Stationary includes useful rotations.
The easiest physical class examines lower-scalar-cost first-forward candidates
with at least1unit heuristic travel/turn gain after removing operation-index
preference (scale50,unitcost4). Category counts are:
- reference_audit67-s0-r0: {"many_movable": 22.6, "no_lower_forward": 319.93333333333334, "one_movable": 322.93333333333334, "protected_blocker": 0.06666666666666667, "stationary": 808.6666666666666, "two_movable": 140.33333333333334, "unblocked": 2.8}
- reference_audit67-s2-r0: {"many_movable": 28.266666666666666, "no_lower_forward": 322.26666666666665, "one_movable": 332.2, "protected_blocker": 0, "stationary": 827.0666666666667, "two_movable": 141, "unblocked": 3.3333333333333335}

Easiest scalar-improving unblocked means39.07/40.13 versus physical2.80/3.33: most
apparent unblocked scalar opportunities disappear when op preference is removed.
A movable owner is NOT known to have a feasible displacement. This does not count
failed-attempt traces, regional-boundary crossings, or feasible joint alternatives.
No bound on future recoverable throughput follows from these snapshot counts.
Your earlier multiple-owner proposals are not untried: old extra transaction pass
lost0.288% sixseedmean on the older reference. A true joint counterfactual must
show a feasible complete replacement unavailable to current one-owner search.
