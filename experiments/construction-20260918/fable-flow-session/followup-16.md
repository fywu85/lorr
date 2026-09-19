Continue SAME persistent session, turn17, Fable5.1/max, read-only/no tools.
Visible response<=1,000words. User authorization persists. Source delta is only
V50's removal of duplicate root deadline callback: construct/repair already check
immediately before attempt; build now checks nonzero counter multiples16. Final
worker/regional/complete-entry checks unchanged. Full regression suite passes;
clocked synthetic regional test12pairs all faster, medianpaired−2.52%, exact plans/
scorebits/counters. ProductionA/B/A screenrunning. Layout and equal-owner-first
patches showed no useful speedup and were NOT adopted. No broad latency claim.

Scale513 full failsseed0 at2230; seed2valid142171 versus144107 (−1936),tail−281,
agep90+6, turns+17.5%,waits+21.8%. Reject. Work4M/2rounds vs3M/4rounds vs2M/4rounds
fullstillrunning;2Mseed0failscoldstep36. Currentreference/peak143941.2/144510 unchanged.

Focused next hypothesis is assignment pricing of the REVEALED task chain. Read cached
schedule/task_chain_cost, FullPickupField, TurnDistanceOracle. Current scheduler
constructs complete forward oriented fields for up to64idle robots/fourthreads, but
retains only min-heading distance per cell. Paircost =W*pickup_cost +U*spatial_chain,
then HRRN1+age*U/cost. Spatial chain ignores turns and learned directed tolls. Earlier
spatial-table refinement was inert; it did NOT price oriented directed costs.
No claim that changing a denominator automatically improves throughput.

Concrete bounded optional/defaultOFF experiment, no extra table builds or hiddentasks:
A. For each free task with >=2remaining revealed locations, non-LRUpeek complete
reverse oriented tables of ALL future destinations. If any required table/state is
unavailable/infinite, whole-task chain uses the ORIGINAL U*spatial estimate. Otherwise
chain_lower_bound =sum_k min_h D_(goal[k])(goal[k-1],h), in published metric units.
Cache/publication lifecycle stays unchanged: schedule reads the last complete
published metric. Recompute these cheap task scalars eachcall or cache with explicit
publication, stop and full-locations signature; first implementation can recompute.
Call original task_chain_cost regardless to preserve its native spatial admissions/
LRU/table work; oriented peeks cause no admissions or recency updates. No guessed
starting heading per later leg: this is explicitly a lower bound with free heading
reset at each service, NOT exact whole-chain coupling.

B. When an existing complete forward pickup field is available, optionally retain
all4heading distances F(cell,h), by swapping completed worker distance scratch into
a new output vector after each fullsearch. Both worker and output buffers remain
private until join; public min-heading field preserved.64*70000*4ints adds~72MB,
wellwithin32GB. Whole forward Dijkstra work/order/costs unchanged. DefaultOFF skips
this retention. With first loaded leg's cachedD_next(first,h), replace the independent
pickup+firstleg subtotal with min_h[W*F(first,h)+D_next(first,h)]. Equivalently add
nonnegative regret =thismin−W*min_hF−min_hD_next, retaining all later minheading leg
bounds. W1reference, but formula should work forpositiveW. This is exact coupling
through the FIRST pickup heading in the relaxed independent-robot metric, not exact
fullchain or actual collision-aware travel. Without complete forward field or whole
chain availability, use independent original/relaxed estimates per policy; never
pretend a reverse-to-first table determines terminal arrival heading. Current owncell
pickupFstartorientation0, otherheadingscostturns. Repeatedgoals havezero legdistance;
per-tick service time is omitted as in native and must be acknowledged.

Possible arms: nativeOFF; A (relaxed known-chainpricing); A+B (first-headingcoupling).
All preserve old assignment/fairadmission/rng/work budgets and primary protections;
onlytaskpair costs/rankingschange. Native fallback whenno publishedmetric,1stop,
missingtable/infinity; need coherent units/caps/metadata. Cases remain genericallmaps.
Eligibility/coverage can itself bias ranking, especially aftereverypublication.
Diagnostics must countwholechaincoverage, changedchaincosts, coupledpositive-regret
pairs and actualchangedassignments if possible. Screen overhead before full0/2;
use sixseeds beforepromotion. MoreCPU not automaticallymorequality.

Please critique exactsource feasibility, unit/heading errors, cache lifetime, fallback
bias, and fairness/protection invariants. Is this meaningfully different from failed
known-next PLANNING objective and prior spatial-chain refinement, and worth onebounded
paired experiment, or is there a specificsource/evidence reason toreject? Identify
minimum independent oracle/toy and real adapter tests (serial/parallel, cachechange,
missingtables, repeatedowncell goals,assignmentuniqueness/protected tasks). Estimate
complexity symbolically; no unsupportedsub-ms claims. Prefer a concrete decision on
this proposal over a long list of unrelated knobs. No implementation has been made.
