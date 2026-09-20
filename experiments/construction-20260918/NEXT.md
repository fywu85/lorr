# Continue CGAR across the competition

Updated2026-09-20 23:28UTC. The latest user redirected work: finish Warehouse's
strict1s validation, then target all ten instances. Throughput is primary;
fairness is secondary and regressions must be reported but do not veto a faster
candidate. NMS is the primary target; KK supplies ideas/comparisons. The160000
Warehouse aspiration is deferred. Formal goal metadata may still contain that
superseded target: the latest user scope takes precedence; do not mark it achieved.

The other agent owns RANDOM-05. Do not touch its code, files, outputs, builds or
jobs. Read-only RANDOM05_PROGRESS.md and artifacts are allowed. No internal
subagents. Fable Claude session1ebb1075-3538-49d1-93d1-a00c94fa256a exhausted
credits at turn46; no change, so no retry. Next would be47. Shell calls require
require_escalated because bwrap fails. Shared main/index: explicit paths and
`git commit --only`, never remove index locks or include random05changes.

## Warehouse strict verification COMPLETE

V123 benchmark8900817, verifier8900819 and independentoutcomes8900820 allcomplete.
Source3e319f175859396b91f97364b320c6794a85cbb7; binary
e3ec32ecdfdaf06f7db139da3857af8fa0ad43b98bd67479f7b9bd969dc1302c.
Results strict-runtime-full-v123 and strict-runtime-full-v123-outcomes verified.
Seeds0/2 exactlyreproduce155173/155090 andboth V110fulltrajectoryhashes, sampled
global/regionalwork andpercentilemarginvalues. Enforced1000ms,8physicalcores
percase, exclusiveresearch44,32decimalGB. All10000completeentries and100Mactions.
Means428.056/425.740ms; maxima944.766/957.827ms; RSS15.396/15.393GB;
meanCPU1.770cores; wall36.834/36.633min. Completion23:23:25.627892UTCseed0 and
23:23:13.544866UTCseed2. Profile=8threadprefetch/prep/pickup,25GiBlogicalunwarmed
cache; all-goalprewarmOFF. Sameglobal4M/regions4x25kx2 work as V110. No newpeak.
Verified-variant.json andexecution-confirmation.json ready; work-budget.json and
summary.md generated. Originalpeak37f592a /20:36:47timestampremainunchanged.
This qualifies only testedseeds/hardware; no universaldeadlineguarantee.
V122screen withfirstcountercheckfailureandcorrectedr2 archivedinb643dbb.

Warehouse record155173; fourguardtotals[155173,155056,155090,155008],mean155081.75;
generic6seedmean144392.17. Userdeferred160kandwantsallteninstances. Strictissue
nowresolvedforverifiedprofile. No moreWarehouse-onlythroughputtuningnow.

## New generic regional candidate budget

Source eb9938090d7cfb331fdc124ce19b475626356502; binary
19155f20da9f518cfac5955c4bcc4a3af9926ff4828e552f3bd69026201b5c3f.
Build runs/cgar-allmaps-region-budget-build-v1-r2-20260920 complete, all28hashes
and fullregressions passed. Evidence experiments/allmaps-20260920/build-v1-r2.
New optionalCGAR_TEMPORAL_REGION_CANDIDATE_LIMIT default0; perregion/perround,
checked between complete attempts so final attempt canovershoot. It does not
return partial work on a deadline. Global4Mlimit already existed; regionshad
onlyattemptlimits. PriorRANDOM04step1000had1.187billioncandidates,4.122sregional.
Zero retains Warehouse/search defaults. Regressionfixturescovercollision-free
merges, primary/recoveryprotection, matching1/4threaddecisions, nonbindingcap
equivalence, measuredworkreduction, invalidsettings, timeoutfailure.
Initialbuild8900850failedan oldauditassertion assumingallattempts; correctedto
actualattemptcounts, retainingstrictoldassertionwhenuncapped. Failurearchived;
no benchmarksfromfailedattempt. Retry8900851passed.

## Full eight-instance matrix running

Job8900861 started23:09:47UTC on research57, sharedhost,32physicalcores,
8simultaneouscases4coreseach. Fullhorizons,5sdevelopmentdeadline,32decimalGB,
seed0, two profiles: perregion4Mvs8Mcandidates; allothergenericsettingssameas
priorgeneralization. Raw runs/cgar-allmaps-region-budget-full-v1-20260920.
AllinstancesexceptWAREHOUSEandRANDOM05. Verifier8900862heldbehindbenchmark;
results experiments/allmaps-20260920/results/region-budget-full-v1.
Frozenprofiles/inputhashes/helper/buildproofcommitted1a3de99. Newcode93cb18f,
regressioncorrectioneb99380. Source/testsmatchbuildexactly; no newbuildneeded.

Preliminary completed RANDOM rawtotals4M[588,988,1381,842],8M[588,985,1281,940].
Allmaxentries<=.254s. Originaluncapped[588,982,1409,997] had R04mean3.07s.
These are fastcompletepartialmatrixrows, not yetfullverified qualitypromotion.
Biggerregionalbudget is not uniformlybetter. Largefourmapsstillrunning.

## Next work

Read experiments/allmaps-20260920/{README,PROTOCOL,TRANSFER_REVIEW}.md and
CGAR_PROGRESS.md. Referenceinventory/reviewhashes andofficialpublishedtargets archivedinb643dbb.
Oldcompetitorreferences mostly1000stepcaps, mixed7532/9354CPUs andlarge-map
RSS32GBviolations; do not claim matched SoTA comparisons fromthem. NMS full
small-RANDOMreferences[638,1226,2350,2532] are16physical/32logicalEPYC7532,
where newCGARuses4physicalEPYC9354. Properfreshmatchedreferencesstillneeded.

Afterregionalmatrix, compare existingremaining-chainorder/unequalpower,
unopenedmatching and directcostHRRNoff as controlledgenericfactors. Preserve
CGARsafety/ownership; fairnessisnowsecondary. Uniformflowtestmustretain
schedulercandidatecoverage: justsettingFLOW_STRENGTH0alsoforbidsPICKUP_FLOW,
so avoidconfoundedon/offcomparison. NMSactiveguidanceisnativegeneratedfor
SORTATION/CITY/GAME, savedfieldforRANDOM; itsunusedconfigsLNSisnotactive.
KKactivecodeiswppl_planner, notmy_planner. Anyinstance-specificfieldor
configurationrequiresnewvalidated --trick INSTANCE gates and[trick]commits.
Do notpromotestandaloneR05scoresasCGARresults ormodifythatagent'scode.

Pushespublicfywu85/lorr authorized. UseGIT_ASKPASS=/bin/false
GIT_TERMINAL_PROMPT=0 git -c 'credential.helper=!/user/fw2449/.local/bin/gh auth git-credential' push origin main.

## Official targets now frozen

experiments/allmaps-20260920/TARGETS.md andpublished-targets.json come frompublic
2024-MainCombinedleaderboardAPI, queried2026-09-20. NMSprimarytargets:
WAREHOUSE154795,SORTATION152714,CITY01=8420,CITY02=16787,GAME23274,
R01=639,R02=1221,R03=2334,R04=2547,R05=3050. NMSWAREHOUSE/SORTATION/GAME
havepublishedTimeoutslabels, retained. These arehistoricaltargets, notmatched
localcontrols. FrozenAPImetadataandURLavailable; no needrepeatweblookup.
FullWarehousechecksconfirmed; updateprogressfiles/commitproof/push before
startingnextscope. PublicAPIJSdownloads are/tmpfilesonly, no activenetworksession.

Potential nextsubstantive directionafterbaseline: explicitSORTATIONnativeguidance
fromNMS (rowpattern1001,colpattern1100,forward20/200,turn1,andoptionalouterbands)
isplausiblegiven146147genericvs152714target. WouldneedextendvalidatedCLI/map
fingerprints andnativeprovider, notsilentlyenableWarehousefieldonSortation.
No such implementationbegun. Alternativegenericfactorsalreadylistedabove.
