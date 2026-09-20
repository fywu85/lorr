Continue the SAME persistent CGAR Warehouse session. Read-only; source deltas are authorized. No tools and no claimed executions. Please focus on the next throughput experiment, with a quick correctness check of the small native-matching and bucket-index changes.

Update at2026-09-20~10:24UTC. Target NMS154795, any valid full5000step/10000robot seed under32GB, explicit --trick WAREHOUSE allowed. Fixed work; timeout failure, never partial quality. Shared GRID/5s development labels; subsecond exclusive optimization later. CGAR framework/protection remains.

Verified full V72: lanes+remaining potential+matching64 =148132/148101; vs score-only147422/147502, match-only147328/147204. Both totals/tails improve. Saved-run replay: chain242.275/242.292, empty64.368/64.401 per task, loaded overhead21.396/21.416. The control chains stay~242.2; no causal partition claimed.
Verified V74: add pickupweight5 to that combination ->148373/148440 (+241/+339), tails+105/+108, agep90-37/-37, empty/task63.040/62.990. All4fullvalid/exactcontrols. Short preference OFF. This is current VERIFIED best148440; sourcec696d5f.
V73 native20/200,turn1,raw pure potential, matching/short OFF: no-bands full runs have finished with solver summaries149915/149871 (max1.0041/1.0048s), both legacy controls exactly at146659/146566 and147422/147502. Final independent full analysis is held until band-arm completion, now~4600steps. Thus1499xx is promising but not yet promoted. Don't infer the pending band score. Exact-table/fallback counts at sampled late preparation are10000/0. No-band maxRSS remains to be independently summarized.

Your turn36 issues were checked:
- Prefetch allocates scratch using CURRENT max_edge_cost inside workers, after set_forward_costs. Added1,234,752 independent heap state comparisons at1/4threads; passes.
- Choice.cost already int64; fallback clamped before50x. No stored32-bit narrowing.
- Matching unit ceiling was genuinely incompatible with20 but masked by the old gate. V75 adds optional unit_limit default16 and passes20 only in native; native matching now allowed. Scale invariance through255, default bound rejection, two real native beneficial cycles, primary/metadata/finite-retarget protection all pass.
- Actual installed native vector now FNV-verified and receipt reports it; corruption/dimension tests pass.
- Independent replay now includes2372 op0 seeds/all four first actions,41674 macro scores/1474service cases total.
- Four128tick/24robot native closed-loop episodes pass with exact serial/parallel schedules/actions/cache work; two unique traces have276services,111repeats,125seed rotations (each repeated in parallel). Independent collision checks cover12288 actions.
- Native matching200 factorial all4valid, OFF hashes exactly V73; no-band3848/3858, bands3872/3847 OFF/ON (startup only). No-flag800check EXACT promoted generic prefix18958.
- All129 operation strings/order exactly match local NMS44950d1. Policy equivalence still not claimed.
- Removed runtime modulo from complete pickup Dial via relative bucket index. Full suite passes. One reserved9354core,24 alternating pairs/profile: identical distance/pops/states; CPU medians improve23.3/13.0/10.6/9.4% on uniform/legacy/native/nativebands versus dynamic modulo. Also12.3/8.8% better than original constexpr17 version on two legacy fields. Kernel speed only. V77 all4native200prefixes EXACT V75 (both matched candidates included); generic800 fullfinished18958, finalfingerprintanalysispending. Sourcec6d9755.

Proposed immediate experiment after V73 full verification: for BOTH native fields, three arms {unmatched/weight1, matching64/weight1, matching64/weight5}, seeds0/2 full. This is6profilesx2seeds,4boundcores/process; extra weight5 startup checks first. It measures the matching increment and the weight increment without serializing two entire full-run stages. Short preference stays OFF. Controls must exactly reproduce V73. No algorithm change beyond validated V75/V77.

Please assess:
1. Any concrete correctness or isolation defect in native matching / relative Dial indexing? Don't repeat already-covered checks unless you see a gap.
2. Does this6profile comparison remain the highest-value next step? Briefly explain any stronger bounded alternative.
3. If it leaves~2-3% gap, rank ONLY two next mechanisms using evidence: (a) allow explicit short preference with native/remaining score and pickupweight5 (current guard only allows it with old paid score; it disables HRRN/oldest but never redirects started tasks), (b) a small fixed multiworker global search before regional repair, (c) more than one finite unopened-task retarget per task, or another more grounded mechanism. V70 oldpaid lanes+match+short+weight5 was146873/146820 vs ordinary147457/147495; short alone was much worse through empty travel. Do not assume these effects transfer to the native metric. Native cheap-turn metric may change the interaction.
Keep hidden future tasks and assumed run-end tricks out of the proposal. Preserve primary, started work, finite retarget and timeout failure. Concise response, say what is verified versus conjecture.
