Continue our existing CGAR consultation; read-only diagnosis, no tools or edits.
Use Fable5.1 max effort as requested by the owner. Our last attempt about16hours
ago failed for credits. Supplied full files are authoritative; older failed-call
source-cache entries may not have reached you. This turn supplies only current
new CGAR components, not the full codebase. Preserve this consistent session.

Objective changed since Warehouse: +10% over max(published NMS,KK) RANDOM01–05,
stretch+5%otherfive. Throughput primary, fairness secondary. All map tricks must
require --trick INSTANCE. Some selected seeds qualify; separate replication.
CGAR primary/recovery and supporting actual actions must remain authoritative.
The independent random05/PILOT agent owns its tree; no touching it. Reading frozen
reference code and transferring ideas into CGAR explicitly authorized.

Current full strict1s/32GB selected: Warehouse155173, Sortation150894, CITY01 8440,
CITY02 16315, GAME24447, RANDOM01 693,R02 1215,R03 1982,R04 2059,R05 3065.
Random targets757/1386/2568/2802/3355. GAME alone clears its new+5%target.
Warehouse/Sortation8physicalcores;others4. Disjoint core binding/noquota on shared
EPYC9354hosts. All algorithmic work uses fixed counts and deadline exceptions,
never successful partial best-so-far fallback. Full horizons used for comparisons.

Recent controlled evidence:
- R01 new CGAR-seeded window: longer coordinated ordinary forecasts with actual
  primary/recovery/support actions immutable. 20action window,4islandsx4096repairs.
  Uniform forward20beats NMSfield across3seeds+2.69%. Turn20,wait40. Refreshing
  retained10prefix's joint tail gains1.88%all3:685/681/693vs671/668/682. Doubling
  workloses676vs693;16384workfailsdeadline;group8=691.
- R02window1215. Neither tail refresh, turn/waitchanges, nor delay-directed roots
  help. Delay1/3/7=1182/1164/1187vs1215.
- R03windowhistoryoff1939/1900/1870vsold1902/1798/1848,+2.9%all3. Delay tournament
  (3extra draws onhalf repair roots, rank paid+tail minus initialpotential) gives
  1967/1982/1948,+3.29%all3. H20,4x4096,memory~43MBoracle. Largerfixedwork/8core
  variantslost. Long retained tails mostlylose, even refreshed.
- Window annealing borrowed from PILOT: complete legal repairs may increasecost,
  linearlycooled byiteration; exactinteger cost deltas, retained bestcompleteplan
  restored after ALL work. Temperature20/80/320allloseR01/02/03. R03seed2delay3
  2x2:control1982,keep14refresh1718,temp40=1642,combined1801. Unitandproduction
  regressionspass,12kserial/parallelactions,atomicrollback,latefailure.
- R04commonfutures(r4,b2,H15) improves3seedmean1.43%. H20selected2059butmeanlower
  thanH15. H25/H30/morerootslose. Comparingcompleteearlierregionalcheckpoints
  instead of global rootsloses1853/1947/1905atH20/15/10.
- R05pureTERMINAL exactchainpotential score(mode1, oldpriorityretained) improved
  ~3.6% across3seeds. Known-horizon admission margin only+0.19%mean. Selected
  seed10=3065. Existingcapacitymodeparks31pocketrobots(active769). Global common
  future rootsneverchangethe3,065trajectory. RegionalcheckpointrootsDOchange
  22–28kfirstactions,butlose2937/2978/2971atH10/15/20vs3065.
- CrowdedR04windowH10keep0/H20keep0/H20keep10refresh =1913/1734/1944vsroot1999.
  R05windowexperimentsrunning. PurewindowannealingstaysOFF.
- AllRANDOMorientedinitialdispatchtrialslost. CITY01pickupgrouping+0.11%all3.
  CITY02finite2retargetbudget+1.28%all3. LowerstaticlanepenaItiesloseCITY/GAME.
- BoundedwiderHungarianmatching nowpassedregression; widths32/64/128/256with
  quotas64/32/16/8keepmax2048participants. RunsR01/02/03/CITY01/02underway.
  InitialR02widergroupslose. Started/primary/fair/cooldown/taskbudgetsunchanged.

Important implementation distinctions: chainmode1 candidate cost is PUREterminal
remainingpotential*scale minus operationtiebreak, no paidactions. Window and
commonfutures use paidactions+terminalchain. Actual plannerexecutesonlyfirstaction.
No true two-phase motion pipeline has been transferred. Protected futuretails
are approximate; commonfutures freezes theirwholepath, windowallowscompatible
forecasts afterimmutableactualfirstaction, except real recovery pathsstayfixed.
No unknown tasks are peeked. PILOT uses a carried two-phase move pipeline, common
futures, orientedcosts and varying fields/density policies. Our staticdensefield
is its flow_seed15/flip1field, integerx10. In that reference wait0.5wouldbe5units;
our chainoracle/commonfutureswait20currently. But chainmode1chargesno paidwait,
so changing itswaitparameter alone mostly affects repeatedservice potential.

Please give a fresh, prioritized diagnosis rather than more blind parameter
sweeps. What structural limitation most plausibly explains the remaining RANDOM
gap? Which 2–3bounded experiments could distinguish seed/forecast invalidity,
objective mismatch, lack of a true movement pipeline, and scheduler restrictions?
If proposing a pipeline, outline a concrete CGAR-compatible integration preserving
real primary/recovery actions and complete fixed-work failure semantics. Also
review whether the supplied window/future code has a substantive correctness or
objective bias that explains non-scaling/annealinglosses. Distinguish verified
code findings from hypotheses and suggest specific diagnostics. Do not claim
measurements or tests you have not run. Be concise, focus on high-leverage changes.
