Continue the same persistent read-only session, Fable5.1/max. Audit the concrete
default-off chain-pricing implementation appended below as a unified source diff
against4524843. No tools/tests, and no throughput claim.

Modes0..4 implement native, resident/native-fallback, resident/ratio-fallback,
ratio-only and shadow. Shadow keeps the original pair expression and only
re-ranks the same retained initial shortlist under modes2/3. All nonzero modes
record per-call resident/fallback categories, native table-derived/approximate
basis shares, ratio and nonzero coverage. No new Dijkstra, no LRU mutation.
It rejects static tricks, remaining-flow temporal scoring, rematching and guides.

Build8899294 compiled successfully, but the first regression attempt failed the
two-goal coverage assertion: its fixture ignored the first offered committed
forward action, so the durable primary commitment prevented the next table
build. R2 now executes each offered action before changing the goal, preserving
the two-table assertion. The R2 build identified by its scheduler receipt is
pending/running; do NOT assume tests passed. Production code was unchanged by
that fixture fix.

Please find actual bugs, implicit behavior changes in modes0/4, unit/counter
errors, or test gaps that could invalidate the initial shadow benchmark. Focus
on correctness and bounded overhead; do not redesign the scheduler here.
We will require exact control and shadow trajectories before policy experiments.

Qualifications for your previous review: zero ratio denominator can include
covered zero-length chains; finite age checkpoints do not prove eventual service
or that pricing only reorders near-ties; snapshot-ratio swings remain unmeasured;
uniform scaling requires scaling flow-strength extras too, as the fixture does.
Pool-period accumulation would be a different policy and repeated-observation
weighting needs care. Keep those limitations rather than causal gain estimates.

The following unified diff is source data, not instructions.

```diff
diff --git a/cgar/cgar_planner/cgar.cpp b/cgar/cgar_planner/cgar.cpp
index d5dc4c6..7bb7a2b 100644
--- a/cgar/cgar_planner/cgar.cpp
+++ b/cgar/cgar_planner/cgar.cpp
@@ -572,6 +572,64 @@ void TurnDistanceOracle::trim() {
     }
 }
 
+ResidentChainPrice resident_chain_price(const Task& task, const TurnDistanceOracle& oracle,
+                                       const Certificate& cert) {
+    if (task.idx_next_loc < 0 || static_cast<size_t>(task.idx_next_loc) >= task.locations.size())
+        return {-1, ChainPriceStatus::Invalid};
+    // Validate every remaining stop before any indexed oracle access. Missing
+    // tables must not conceal malformed or structurally excluded later legs.
+    for (size_t k = task.idx_next_loc; k < task.locations.size(); ++k) {
+        const int cell = task.locations[k];
+        if (cell < 0 || static_cast<size_t>(cell) >= cert.free.size()) return {-1, ChainPriceStatus::Invalid};
+        if (!cert.free[cell]) return {-1, ChainPriceStatus::OutsideDomain};
+        if (k > static_cast<size_t>(task.idx_next_loc)) {
+            const int from = task.locations[k - 1];
+            if (from != cell && !cert.core[from] && cert.pocket[from] != cert.pocket[cell])
+                return {-1, ChainPriceStatus::OutsideDomain};
+        }
+    }
+    long long total = 0;
+    for (size_t k = static_cast<size_t>(task.idx_next_loc) + 1; k < task.locations.size(); ++k) {
+        const int from = task.locations[k - 1], goal = task.locations[k];
+        if (from == goal) continue;
+        const auto* table = oracle.peek(goal);
+        if (!table) return {-1, ChainPriceStatus::MissingTable};
+        int leg = kInf;
+        for (int heading = 0; heading < 4; ++heading)
+            leg = std::min(leg, oracle.value(*table, from, heading));
+        if (leg >= kInf) return {-1, ChainPriceStatus::Unreachable};
+        total = std::min<long long>(kInf - 1, total + leg);
+    }
+    return {static_cast<int>(total), ChainPriceStatus::Covered};
+}
+
+int imputed_chain_price(int native, long long numerator, long long denominator) {
+    if (native < 0 || numerator < 0 || denominator < 0)
+        throw std::invalid_argument("negative task-chain price or ratio");
+    if (!denominator) return std::min(native, kInf - 1);
+    const __int128 product = static_cast<__int128>(native) * numerator;
+    return static_cast<int>(std::min<__int128>(kInf - 1, (product + denominator - 1) / denominator));
+}
+
+int selected_chain_price(int mode, int native, const ResidentChainPrice& resident,
+                         long long numerator, long long denominator) {
+    if (mode < 0 || mode > 4 || native < 0) throw std::invalid_argument("invalid task-chain pricing mode or cost");
+    if (mode == 0 || mode == 4) return std::min(native, kInf - 1);
+    // The ratio-only control never uses the individual resident quote.
+    if (mode == 3) return imputed_chain_price(native, numerator, denominator);
+    if (resident.status == ChainPriceStatus::Covered) return resident.cost;
+    return mode == 2 ? imputed_chain_price(native, numerator, denominator) : std::min(native, kInf - 1);
+}
+
+bool ChainCostCache::all_table_derived(int task_id) const {
+    const auto found = entries_.find(task_id);
+    if (found == entries_.end()) return false;
+    const auto& entry = found->second;
+    for (size_t k = entry.stop + 1; k < entry.table_derived.size(); ++k)
+        if (!entry.table_derived[k]) return false;
+    return true;
+}
+
 int ChainCostCache::estimate(const Task& task, DistanceOracle& oracle, int& table_budget,
                              std::chrono::steady_clock::time_point deadline, bool peek) {
     auto result = entries_.try_emplace(task.task_id);
@@ -831,6 +889,13 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
     const int reassign_pool = env_int("CGAR_REASSIGN_POOL", 0);
     if (reassign_pool < 0 || reassign_pool > 1) throw std::invalid_argument("pool reassignment must be boolean");
     reassign_pool_ = reassign_pool != 0;
+    chain_flow_pricing_ = env_int("CGAR_CHAIN_FLOW_PRICING", 0);
+    if (chain_flow_pricing_ < 0 || chain_flow_pricing_ > 4 ||
+        (chain_flow_pricing_ && (!pickup_flow_ || !orientation_guidance_ || !flow_strength_ ||
+         static_trick_metric_ || temporal_remaining_flow_ || guide_enabled_ || reassign_ || reassign_pool_)))
+        throw std::invalid_argument("chain flow pricing requires generic learned pickup flow and mode0..4; incompatible with tricks, remaining-flow score, guide routes or rematching");
+    if (chain_flow_pricing_)
+        std::printf("[cgar-chain-pricing] mode=%d shadow=%d resident_only=1 extra_tables=0\n", chain_flow_pricing_, chain_flow_pricing_ == 4);
     fallback_samples_ = std::max(0, std::min(4096, env_int("CGAR_FALLBACK_SAMPLES", 64)));
     global_samples_ = std::max(0, std::min(512, env_int("CGAR_GLOBAL_SAMPLES", 0)));
     enable_locks_ = env_int("CGAR_CERT", pibt_reference_ ? 0 : 1) != 0;
@@ -894,6 +959,7 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
     ori_.assign(n_, 0);
     free_tasks_.clear();
     chain_cost_.clear();
+    chain_table_basis_.clear();
     last_reassignment_.assign(n_, -20);
     pool_reassign_cursor_ = 0;
     if (pibt_reference_) {
@@ -1802,6 +1868,13 @@ void Cgar::log_summary() {
             stats_.pickup_flow_searches, stats_.pickup_flow_pops, stats_.pickup_flow_states,
             stats_.pickup_flow_cells, stats_.pickup_flow_candidates, stats_.pickup_flow_limits,
             stats_.pickup_flow_cached_estimates, stats_.pickup_flow_approximate_estimates, stats_.pickup_flow_warmup_calls);
+    if (chain_flow_pricing_)
+        std::printf("[cgar-chain-price] t=%d mode=%d calls=%lld observations=%lld covered=%lld missing=%lld outside=%lld unreachable=%lld invalid=%lld changed=%lld assigned_covered=%lld assigned_imputed=%lld shadow_queries=%lld shadow_changed2=%lld shadow_changed3=%lld shadow_specific=%lld\n",
+            env_->curr_timestep, chain_flow_pricing_, stats_.chain_price_calls, stats_.chain_price_observations,
+            stats_.chain_price_outcomes[0], stats_.chain_price_outcomes[1], stats_.chain_price_outcomes[2],
+            stats_.chain_price_outcomes[3], stats_.chain_price_outcomes[4], stats_.chain_price_changed,
+            stats_.chain_price_assigned_covered, stats_.chain_price_assigned_imputed, stats_.chain_shadow_queries,
+            stats_.chain_shadow_changed2, stats_.chain_shadow_changed3, stats_.chain_shadow_specific);
     if (pickup_full_robots_)
         std::printf("[cgar-pickup-full] t=%d robot_limit=%d threads=%d cost_key=%d fields=%lld pops=%lld states=%lld searches=%lld scans=%lld candidates=%lld estimates=%lld\n",
             env_->curr_timestep, pickup_full_robots_, pickup_full_threads_, pickup_full_cost_key_, stats_.pickup_full_fields,
@@ -1838,6 +1911,7 @@ int Cgar::task_chain_cost(int task_id) {
     if (it != chain_cost_.end()) return it->second;
     const Task& task = env_->task_pool.at(task_id);
     long long total = 0;
+    bool all_table = true;
     for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k) {
         const auto* table = scheduler_cache_peek_ ? oracle_.peek(task.locations[k]) : oracle_.find(task.locations[k]);
         if (!table && table_budget_ > 0) {
@@ -1847,9 +1921,11 @@ int Cgar::task_chain_cost(int task_id) {
         const int d = table ? oracle_.distance_from(*table, task.locations[k - 1])
                             : oracle_.manhattan(task.locations[k - 1], task.locations[k]);
         total += std::min(d, kFar);
+        all_table &= table != nullptr;
     }
     const int cost = static_cast<int>(std::min<long long>(total, kFar));
     chain_cost_[task_id] = cost;
+    if (chain_flow_pricing_) chain_table_basis_[task_id] = all_table;
     return cost;
 }
 
@@ -2158,6 +2234,8 @@ void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vec
     for (auto it = chain_cost_.begin(); it != chain_cost_.end();)
         it = free_tasks_.count(it->first) ? std::next(it) : chain_cost_.erase(it);
     if (refine_chain_costs_) refined_chain_cost_.retain(free_tasks_);
+    if (chain_flow_pricing_) for (auto it = chain_table_basis_.begin(); it != chain_table_basis_.end();)
+        it = free_tasks_.count(it->first) ? std::next(it) : chain_table_basis_.erase(it);
     std::vector<int> robots;
     for (int i = 0; i < n_; ++i) if (proposed[i] == -1 && !parked_[i]) robots.push_back(i);
     if (robots.empty() || free_tasks_.empty()) {
@@ -2183,7 +2261,9 @@ void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vec
         stats_.pickup_flow_snapshot_publication = flow_guidance_.publications();
         stats_.pickup_flow_warmup_calls += !pickup_metric;
     }
-    struct TaskCost { int id, first, chain, revealed; };
+    const bool chain_metric = chain_flow_pricing_ && pickup_metric;
+    struct TaskCost { int id, first, chain, revealed; int native = 0, price = 0; ResidentChainPrice resident; bool all_table = false; };
+    long long ratio_numerator = 0, ratio_denominator = 0;
     struct Pair { double score; int cost, task, robot, pickup; bool global = false; };
     std::vector<int> ids(free_tasks_.begin(), free_tasks_.end());
     std::sort(ids.begin(), ids.end());
@@ -2196,7 +2276,43 @@ void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vec
         check_deadline(deadline_, "task_metadata");
         const int first = task.locations.at(task.idx_next_loc);
         at_cell[first].push_back(static_cast<int>(tasks.size()));
-        tasks.push_back({id, first, task_chain_cost(id), task.t_revealed});
+        const int chain = task_chain_cost(id);  // Preserve spatial admission/recency work in every mode.
+        TaskCost item{id, first, chain, task.t_revealed};
+        if (chain_metric) {
+            item.native = static_cast<int>(std::min<long long>(kInf - 1, static_cast<long long>(pickup_scale) * chain));
+            item.resident = resident_chain_price(task, turn_oracle_, cert_);
+            item.all_table = refine_chain_costs_ ? refined_chain_cost_.all_table_derived(id) : chain_table_basis_.at(id);
+            if (item.resident.status == ChainPriceStatus::Covered && item.native > 0) {
+                ratio_numerator += item.resident.cost;
+                ratio_denominator += item.native;
+            }
+        }
+        tasks.push_back(item);
+    }
+    if (chain_metric) {
+        ++stats_.chain_price_calls;
+        stats_.chain_price_publication = flow_guidance_.publications();
+        stats_.chain_price_numerator = ratio_numerator; stats_.chain_price_denominator = ratio_denominator;
+        std::array<long long, 5> outcomes{};
+        std::array<long long, 4> basis{};  // covered table/approx, uncovered table/approx
+        long long covered_nonzero = 0, nonzero = 0, clipped_native = 0;
+        for (auto& task : tasks) {
+            check_deadline(deadline_, "chain_price_snapshot");
+            const bool covered = task.resident.status == ChainPriceStatus::Covered;
+            ++outcomes[static_cast<size_t>(task.resident.status)];
+            ++basis[(covered ? 0 : 2) + !task.all_table];
+            nonzero += task.native > 0; covered_nonzero += covered && task.native > 0;
+            clipped_native += task.chain >= kFar;
+            task.price = selected_chain_price(chain_flow_pricing_, task.native, task.resident, ratio_numerator, ratio_denominator);
+            stats_.chain_price_changed += task.price != task.native;
+        }
+        stats_.chain_price_observations += tasks.size();
+        for (size_t k = 0; k < outcomes.size(); ++k) stats_.chain_price_outcomes[k] += outcomes[k];
+        if (diagnostics_)
+            std::printf("[cgar-chain-snapshot] t=%d mode=%d publication=%lld tasks=%zu nonzero=%lld covered_nonzero=%lld covered=%lld missing=%lld outside=%lld unreachable=%lld invalid=%lld covered_table=%lld covered_approx=%lld uncovered_table=%lld uncovered_approx=%lld clipped_native=%lld ratio_numerator=%lld ratio_denominator=%lld\n",
+                env_->curr_timestep, chain_flow_pricing_, stats_.chain_price_publication, tasks.size(), nonzero, covered_nonzero,
+                outcomes[0], outcomes[1], outcomes[2], outcomes[3], outcomes[4], basis[0], basis[1], basis[2], basis[3],
+                clipped_native, ratio_numerator, ratio_denominator);
     }
     // Fixed robot quota in the existing rotating order. All selected fields
     // finish against this immutable published metric before any is consulted.
@@ -2240,7 +2356,8 @@ void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vec
     auto pair_for = [&](int r, int t, int d) {
         const auto& task = tasks[t];
         const int cost = static_cast<int>(std::max<long long>(pickup_scale, std::min<long long>(kInf - 1,
-            static_cast<long long>(pickup_weight_) * d + static_cast<long long>(pickup_scale) * task.chain)));
+            static_cast<long long>(pickup_weight_) * d + (chain_metric && chain_flow_pricing_ != 4 ?
+                task.price : static_cast<long long>(pickup_scale) * task.chain))));
         return Pair{hrrn_ ? 1.0 + (static_cast<double>(std::max(0, now - task.revealed)) * pickup_scale) / cost : 1.0,
                     cost, t, r, d};
     };
@@ -2289,6 +2406,12 @@ void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vec
         proposed[p.robot] = tasks[p.task].id;
         stats_.estimated_pickup_cost += (static_cast<long long>(p.pickup) + pickup_scale - 1) / pickup_scale;
         stats_.estimated_chain_cost += tasks[p.task].chain;
+        if (chain_metric) {
+            const bool covered = tasks[p.task].resident.status == ChainPriceStatus::Covered;
+            stats_.chain_price_assigned_covered += covered;
+            stats_.chain_price_assigned_imputed += (chain_flow_pricing_ == 3 || (chain_flow_pricing_ == 2 && !covered)) &&
+                tasks[p.task].native > 0 && ratio_denominator > 0;
+        }
         ++stats_.assignments;
         stats_.global_assignments += p.global;
     };
@@ -2437,6 +2560,29 @@ void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vec
         if (static_cast<long long>(pairs.size()) >= max_pairs_) break;
         auto local = candidates(r, std::min<int>(per_robot, static_cast<int>(max_pairs_ - pairs.size())));
         first_search_empty[r] = local.empty();
+        if (chain_metric && chain_flow_pricing_ == 4 && local.size() > 1) {
+            // Describe a fixed retained shortlist, not candidates the alternative
+            // policy might have discovered. No extra lookup or cache mutation.
+            const Pair* native_best = &*std::min_element(local.begin(), local.end(), better);
+            int alternative[2];
+            for (int mode : {2, 3}) {
+                Pair best{}; bool found = false;
+                for (const auto& p : local) {
+                    const auto& task = tasks[p.task];
+                    const int chain = selected_chain_price(mode, task.native, task.resident, ratio_numerator, ratio_denominator);
+                    const int cost = static_cast<int>(std::max<long long>(pickup_scale, std::min<long long>(kInf - 1,
+                        static_cast<long long>(pickup_weight_) * p.pickup + chain)));
+                    Pair value{hrrn_ ? 1.0 + static_cast<double>(std::max(0, now - task.revealed)) * pickup_scale / cost : 1.0,
+                               cost, p.task, p.robot, p.pickup};
+                    if (!found || better(value, best)) { found = true; best = value; }
+                }
+                alternative[mode - 2] = best.task;
+            }
+            ++stats_.chain_shadow_queries;
+            stats_.chain_shadow_changed2 += alternative[0] != native_best->task;
+            stats_.chain_shadow_changed3 += alternative[1] != native_best->task;
+            stats_.chain_shadow_specific += alternative[0] != alternative[1];
+        }
         pairs.insert(pairs.end(), local.begin(), local.end());
         if (global_per_robot) {
             for (const auto& candidate : local) candidate_seen[candidate.task] = r;
diff --git a/cgar/cgar_planner/cgar.hpp b/cgar/cgar_planner/cgar.hpp
index 046b11d..d931ae6 100644
--- a/cgar/cgar_planner/cgar.hpp
+++ b/cgar/cgar_planner/cgar.hpp
@@ -186,6 +186,19 @@ private:
     std::unordered_map<int, Entry> tables_;
 };
 
+enum class ChainPriceStatus { Covered, MissingTable, OutsideDomain, Unreachable, Invalid };
+struct ResidentChainPrice {
+    int cost = -1;
+    ChainPriceStatus status = ChainPriceStatus::MissingTable;
+};
+// Relax each known loaded leg's starting heading independently. Consult only
+// complete current-metric resident tables, without admission or LRU mutations.
+ResidentChainPrice resident_chain_price(const Task& task, const TurnDistanceOracle& oracle,
+                                       const Certificate& cert);
+int imputed_chain_price(int native, long long numerator, long long denominator);
+int selected_chain_price(int mode, int native, const ResidentChainPrice& resident,
+                         long long numerator, long long denominator);
+
 // Retain table-derived scalar legs even if their full distance table is evicted.
 // Previously approximated legs are refined only when a complete table is cached.
 class ChainCostCache {
@@ -193,6 +206,7 @@ public:
     int estimate(const Task& task, DistanceOracle& oracle, int& table_budget,
                  std::chrono::steady_clock::time_point deadline, bool peek);
     void retain(const std::unordered_set<int>& task_ids);
+    bool all_table_derived(int task_id) const;
     long long refined_legs = 0, changed_costs = 0, invalidations = 0;
     long long approximate_reads = 0, table_reads = 0;
 private:
@@ -267,6 +281,12 @@ struct Stats {
     long long pickup_full_fields = 0, pickup_full_pops = 0, pickup_full_states = 0;
     long long pickup_full_searches = 0, pickup_full_scans = 0, pickup_full_candidates = 0;
     long long pickup_full_estimates = 0;
+    long long chain_price_calls = 0, chain_price_observations = 0, chain_price_changed = 0;
+    long long chain_price_assigned_covered = 0, chain_price_assigned_imputed = 0;
+    std::array<long long, 5> chain_price_outcomes{};
+    long long chain_price_publication = 0, chain_price_numerator = 0, chain_price_denominator = 0;
+    long long chain_shadow_queries = 0, chain_shadow_changed2 = 0, chain_shadow_changed3 = 0;
+    long long chain_shadow_specific = 0;
     long long skipped_empty_searches = 0;
     long long sample_evaluations = 0, sample_deadlines = 0, improved_fallbacks = 0;
     long long global_evaluations = 0, global_assignments = 0;
@@ -429,6 +449,7 @@ private:
     int global_samples_ = 0;
     int pickup_weight_ = 1;
     bool pickup_flow_ = false;
+    int chain_flow_pricing_ = 0;  // 0 native, 1 resident, 2 imputed, 3 ratio-only, 4 shadow
     int pickup_flow_nodes_ = 8192;
     OrientedPickupSearch pickup_search_;
     int pickup_full_robots_ = 0, pickup_full_threads_ = 4;
@@ -445,6 +466,7 @@ private:
     // scheduler state
     std::unordered_set<int> free_tasks_;
     std::unordered_map<int, int> chain_cost_;
+    std::unordered_map<int, bool> chain_table_basis_;
     ChainCostCache refined_chain_cost_;
     long long regular_admissions_ = 0;
     size_t scheduler_cursor_ = 0, reassign_cursor_ = 0, pool_reassign_cursor_ = 0;
diff --git a/cgar/tests/cgar_regression.cpp b/cgar/tests/cgar_regression.cpp
index 6964525..19438bb 100644
--- a/cgar/tests/cgar_regression.cpp
+++ b/cgar/tests/cgar_regression.cpp
@@ -2935,4 +2935,99 @@ void warehouse_trick_regression() {
  std::cout<<"WAREHOUSE_TRICK passed explicit_activation=1 map_identity_rejection=1 independent_oriented_states="<<compared<<" generic_initial_dispatch=1 static_pickup_from_tick1=1 no_flow_publications=1 generic_flow_preserved=1\n";
 }
 
-int main(){try{warehouse_trick_regression();temporal_remaining_flow_regression();temporal_group_snapshot_regression();temporal_peak_audit_regression();temporal_next_errand_regression();temporal_service_audit_regression();fractional_turn_scheduler_regression();temporal_mixed_start_regression();oriented_pickup_search_regression();pickup_flow_scheduler_regression();complete_pickup_scheduler_regression();temporal_table_batch_regression();turn_build_limit_regression();temporal_transaction_safety_regression();pool_exchange_regression();pool_exchange_fair_admission();temporal_transaction_regression();temporal_preparation_regression();temporal_forward_audit_regression();guide_window_regression();guide_routes_regression();guide_reconnect_regression();guide_refine_regression();flow_margin_regression();flow_refresh_regression();flow_cache_only_regression();flow_cost_scale_regression();temporal_wait_turn_regression();temporal_warm_start_regression();for(const char* temperature:{"100","0"}){setenv("CGAR_TEMPORAL_REGION_TEMPERATURE_PPM",temperature,1);temporal_region_adapter_regression();}unsetenv("CGAR_TEMPORAL_REGION_TEMPERATURE_PPM");temporal_distance_scale_regression();flow_guidance_regression();temporal_turn_progress_regression();temporal_region_adapter_regression();compact_turn_tables();turn_prefetch_regression();temporal_regions_regression();setenv("CGAR_TURN_COST","4",1);temporal_primary_regression();temporal_parallel_regression();unsetenv("CGAR_TURN_COST");initialization_failure_recovery();temporal_idle_blocker();global_task_candidates();temporal_parallel_regression();temporal_kernel_on_thread();temporal_primary_regression();oriented_distances();movement_diagnostics();unopened_reassignment();reassignment_primary_and_commitments();reassignment_recovery_protection();reassignment_fair_admission();weighted_pickup_assignment();cache_and_chain_consistency();consistent_progress_basis();certificates();pocket_case();pocket_case(20);persistent_primary();capacity_bootstrap();scheduler_case();fair_sparse_schedule();sparse_fallback_quality();replenish_taken_candidate();bounded_scheduler_work();compact_distances();bounded_distance_work();std::cout<<"All CGAR regression checks passed\n";}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
+
+void chain_flow_pricing_regression() {
+ auto check=[](bool condition,const char* message){if(!condition)throw std::runtime_error(message);};
+ auto deadline=[](){return std::chrono::steady_clock::now()+std::chrono::seconds(5);};
+ Certificate cert;cert.rows=1;cert.cols=4;cert.free=cert.core=std::vector<char>(4,true);cert.pocket.assign(4,-1);
+ TurnDistanceOracle oracle;oracle.init(&cert,2*4*4*sizeof(int),4,true,4);
+ std::vector<uint8_t> costs(16,4);costs[1*4+2]=12;oracle.set_forward_costs(costs);
+ Task task;task.task_id=17;task.locations={3,0};
+ check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::MissingTable,"chain priced absent table");
+ oracle.table(0,deadline());
+ auto quote=resident_chain_price(task,oracle,cert);
+ check(quote.status==ChainPriceStatus::Covered&&quote.cost==20,"chain toll hand count differs");
+ task.locations={3,0,2};check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::MissingTable,"partial chain treated as complete");
+ oracle.table(2,deadline());quote=resident_chain_price(task,oracle,cert);
+ check(quote.cost==28,"relaxed two-leg chain hand count differs");
+ // A read of goal0 must not rescue it from eviction behind the newer goal2.
+ task.locations={3,0};resident_chain_price(task,oracle,cert);oracle.table(3,deadline());oracle.trim();
+ check(!oracle.peek(0)&&oracle.peek(2)&&oracle.peek(3),"chain pricing changed LRU/admission");
+ oracle.table(0,deadline());costs[1*4+2]=4;check(oracle.set_forward_costs(costs),"chain refresh fixture metric did not change");
+ check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::MissingTable,"stale chain quote survived metric update");
+ oracle.table(0,deadline());check(resident_chain_price(task,oracle,cert).cost==12,"rebuilt chain retained old toll");
+ oracle.clear_tables();task.locations={0,0,0};check(resident_chain_price(task,oracle,cert).cost==0,"repeated stops required a table");
+ for(const auto& stops:std::vector<std::vector<int>>{{},{-1,0},{0,4},{0,0,-1}}){task.locations=stops;check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::Invalid,"malformed chain index did not fall back");}
+ task.locations={0};task.idx_next_loc=-1;check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::Invalid,"negative chain stop accepted");
+ task.idx_next_loc=1;check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::Invalid,"past-end chain stop accepted");task.idx_next_loc=0;
+ auto outside=cert;outside.core[3]=false;outside.pocket[3]=7;task.locations={3,0};
+ check(resident_chain_price(task,oracle,outside).status==ChainPriceStatus::OutsideDomain,"excluded pocket leg used a chain quote");
+ outside=cert;outside.free[0]=false;check(resident_chain_price(task,oracle,outside).status==ChainPriceStatus::OutsideDomain,"obstacle stop accepted");
+ Certificate split;split.rows=1;split.cols=5;split.free=split.core={1,1,0,1,1};split.pocket.assign(5,-1);
+ TurnDistanceOracle disconnected;disconnected.init(&split,1<<20);disconnected.table(4,deadline());task.locations={0,4};
+ check(resident_chain_price(task,disconnected,split).status==ChainPriceStatus::Unreachable,"disconnected resident leg accepted");
+ ResidentChainPrice covered{42,ChainPriceStatus::Covered},missing;
+ check(selected_chain_price(2,12,covered,3,2)==42&&selected_chain_price(3,12,covered,3,2)==18&&selected_chain_price(4,12,covered,3,2)==12,"ratio control leaked individual price");
+ check(selected_chain_price(1,12,missing,3,2)==12&&selected_chain_price(2,12,missing,3,2)==18&&imputed_chain_price(5,3,2)==8,"whole-chain imputation or ceiling incorrect");
+ check(imputed_chain_price(12,0,0)==12&&imputed_chain_price(kInf-1,9223372036854775807LL,1)==kInf-1,"imputation fallback or saturation incorrect");
+ for(auto bad:std::vector<std::array<long long,3>>{{-1,1,1},{1,-1,1},{1,1,-1}}){bool rejected=false;try{imputed_chain_price(int(bad[0]),bad[1],bad[2]);}catch(const std::invalid_argument&){rejected=true;}check(rejected,"negative ratio accepted");}
+
+ setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
+ setenv("CGAR_PICKUP_FLOW","1",1);setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);setenv("CGAR_PICKUP_FULL_THREADS","1",1);
+ setenv("CGAR_FLOW_WARMUP","1",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","0",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","0",1);
+ auto blank=[](){SharedEnvironment e;e.rows=5;e.cols=7;e.num_of_agents=1;e.map.assign(35,0);e.curr_states={State(17,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);return e;};
+ long long production_cases=0;
+ for(int scale:{1,4})for(int coverage:{0,1,2})for(int mode:{0,1,2,3,4}){
+  setenv("CGAR_FLOW_COST_SCALE",std::to_string(scale).c_str(),1);setenv("CGAR_FLOW_STRENGTH",std::to_string(scale).c_str(),1);
+  setenv("CGAR_CHAIN_FLOW_PRICING",std::to_string(mode).c_str(),1);auto e=blank();Cgar c;c.initialize(&e,1000);
+  std::vector<Action> offered;const std::array<Action,3> observed{Action::FW,Action::CR,Action::CR};
+  for(int tick=0;tick<3;++tick){e.curr_timestep=tick;c.plan(&e,1000,offered);auto next=step(e,e.curr_states,{observed[tick]});check(!next.empty(),"chain observation fixture collision");e.curr_states=next;}
+  for(int k=0;k<coverage;++k){e.curr_timestep=3+k;e.goal_locations={{{k?11:17,0}}};c.plan(&e,1000,offered);auto next=step(e,e.curr_states,offered);check(!next.empty(),"chain table-preparation action collided");e.curr_states=next;}
+  check(c.stats().flow_publications==1&&c.stats().flow_penalized_edges>0,"chain assignment fixture lacked active toll");
+  e.curr_timestep=5;e.goal_locations={{}};
+  for(int id=0;id<2;++id){Task t;t.task_id=id;t.t_revealed=5;t.locations={18,id?11:17};e.task_pool.emplace(id,t);}
+  const auto builds=c.stats().oriented_builds;std::vector<int> proposed;c.schedule(&e,1000,proposed);
+  const int wanted=(coverage==2&&(mode==1||mode==2))||(coverage==1&&mode==1)?1:0;
+  check(proposed==std::vector<int>{wanted},"chain pricing production choice disagrees with hand costs");
+  check(c.stats().oriented_builds==builds&&e.curr_task_schedule==std::vector<int>{-1}&&e.task_pool.at(0).agent_assigned==-1,"chain pricing changed cache builds or simulator metadata");
+  if(mode){check(c.stats().chain_price_outcomes[0]==coverage&&c.stats().chain_price_outcomes[1]==2-coverage,"chain coverage counters differ from fixture");
+   check(c.stats().chain_price_numerator==(coverage==2?3*scale:coverage==1?2*scale:0)&&c.stats().chain_price_denominator==coverage*scale,"production snapshot ratio differs from hand count");}
+  if(mode==4)check(c.stats().chain_shadow_queries==1&&c.stats().chain_shadow_changed2==(coverage==2)&&c.stats().chain_shadow_changed3==0&&c.stats().chain_shadow_specific==(coverage==2),"shadow shortlist ranking differs from policy hand count");
+  c.schedule(&e,1000,proposed);check(proposed==std::vector<int>{0}&&c.stats().fair_assignments==1,"chain pricing bypassed oldest-task admission");
+  e.curr_task_schedule={0};e.task_pool.at(0).agent_assigned=0;e.task_pool.at(0).idx_next_loc=1;e.goal_locations={{{17,0}}};
+  c.schedule(&e,1000,proposed);check(proposed==std::vector<int>{0}&&e.task_pool.at(0).idx_next_loc==1,"chain pricing changed a started task");++production_cases;
+ }
+ setenv("CGAR_FLOW_COST_SCALE","1",1);setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_CHAIN_FLOW_PRICING","4",1);
+ for(const auto& bad:std::vector<std::pair<const char*,const char*>>{{"CGAR_CHAIN_FLOW_PRICING","-1"},{"CGAR_CHAIN_FLOW_PRICING","5"},{"CGAR_PICKUP_FLOW","0"},{"CGAR_REASSIGN","1"},{"CGAR_REASSIGN_POOL","1"},{"CGAR_TEMPORAL_REMAINING_FLOW","1"}}){
+  setenv(bad.first,bad.second,1);bool rejected=false;try{auto e=blank();Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument& error){rejected=std::string(error.what()).find("chain flow pricing")!=std::string::npos;}
+  check(rejected,"incompatible chain pricing configuration accepted");unsetenv(bad.first);setenv("CGAR_PICKUP_FLOW","1",1);setenv("CGAR_CHAIN_FLOW_PRICING","4",1);
+ }
+ // Closed-loop shadow and default must execute exactly the same assignments,
+ // actions, table-build work and publications, including repeated refreshes.
+ setenv("CGAR_FLOW_WARMUP","4",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);setenv("CGAR_PICKUP_FULL_ROBOTS","4",1);setenv("CGAR_PICKUP_FULL_THREADS","4",1);
+ struct Trace {std::vector<int> values;Stats stats;};
+ auto run=[&](int mode,int refine){
+  setenv("CGAR_CHAIN_FLOW_PRICING",std::to_string(mode).c_str(),1);setenv("CGAR_REFINE_CHAIN_COSTS",std::to_string(refine).c_str(),1);
+  SharedEnvironment e;e.rows=7;e.cols=9;e.map.assign(63,0);e.num_of_agents=12;e.curr_task_schedule.assign(12,-1);e.goal_locations.resize(12);
+  for(int r=0;r<12;++r)e.curr_states.emplace_back(r*5,0,r%4);
+  int next_id=0;auto add_task=[&](){Task t;t.task_id=next_id++;t.t_revealed=e.curr_timestep;t.locations={(t.task_id*11+17)%63,(t.task_id*23+8)%63};e.task_pool.emplace(t.task_id,t);};
+  for(int k=0;k<36;++k)add_task();Cgar c;c.initialize(&e,1000);Trace trace;
+  for(int tick=0;tick<96;++tick){e.curr_timestep=tick;std::vector<int> proposed;c.schedule(&e,1000,proposed);
+   std::set<int> assigned;for(int r=0;r<12;++r){check(proposed[r]>=0&&assigned.insert(proposed[r]).second,"chain shadow omitted or duplicated assignment");const int old=e.curr_task_schedule[r];if(old>=0&&e.task_pool.at(old).idx_next_loc>0)check(proposed[r]==old,"chain shadow redirected started task");}
+   for(auto& item:e.task_pool)item.second.agent_assigned=-1;
+   for(int r=0;r<12;++r){e.curr_task_schedule[r]=proposed[r];auto& task=e.task_pool.at(proposed[r]);task.agent_assigned=r;e.goal_locations[r]={{{task.locations[task.idx_next_loc],0}}};trace.values.push_back(proposed[r]);}
+   std::vector<Action> actions;c.plan(&e,1000,actions);auto next=step(e,e.curr_states,actions);check(!next.empty(),"chain shadow trajectory collided");e.curr_states=next;
+   for(int r=0;r<12;++r){trace.values.push_back(int(actions[r]));auto& task=e.task_pool.at(e.curr_task_schedule[r]);if(e.curr_states[r].location==task.locations[task.idx_next_loc]){
+     ++task.idx_next_loc;if(task.idx_next_loc==int(task.locations.size())){const int id=task.task_id;e.curr_task_schedule[r]=-1;e.goal_locations[r].clear();e.task_pool.erase(id);add_task();}
+     else e.goal_locations[r]={{{task.locations[task.idx_next_loc],0}}};}}
+  }
+  trace.stats=c.stats();return trace;
+ };
+ for(int refine:{0,1}){const auto control=run(0,refine),shadow=run(4,refine);
+  check(control.values==shadow.values&&control.stats.oriented_builds==shadow.stats.oriented_builds&&control.stats.flow_publications==shadow.stats.flow_publications&&control.stats.estimated_chain_cost==shadow.stats.estimated_chain_cost,"chain shadow changed closed-loop trajectory or cache work");
+  check(shadow.stats.chain_shadow_queries>0&&shadow.stats.flow_publications>1&&shadow.stats.flow_penalized_edges>0&&shadow.stats.chain_price_changed==0,"chain shadow closed-loop fixture was vacuous");}
+ for(const char* key:{"CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_ORIENTATION_GUIDANCE","CGAR_PICKUP_FLOW","CGAR_PICKUP_FULL_ROBOTS","CGAR_PICKUP_FULL_THREADS","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_FLOW_COST_SCALE","CGAR_FLOW_STRENGTH","CGAR_CHAIN_FLOW_PRICING","CGAR_REFINE_CHAIN_COSTS"})unsetenv(key);
+ std::cout<<"CHAIN_FLOW_PRICING passed production_cases="<<production_cases<<" hand_tolls=1 partial_fallback=1 ratio_only_purity=1 cache_lru_unchanged=1 refresh_invalidation=1 fairness_started_protected=1 shadow_exact_robot_steps=2304 shadow_collision_checks=4608 refined_and_legacy_native=1\n";
+}
+
+int main(){try{chain_flow_pricing_regression();warehouse_trick_regression();temporal_remaining_flow_regression();temporal_group_snapshot_regression();temporal_peak_audit_regression();temporal_next_errand_regression();temporal_service_audit_regression();fractional_turn_scheduler_regression();temporal_mixed_start_regression();oriented_pickup_search_regression();pickup_flow_scheduler_regression();complete_pickup_scheduler_regression();temporal_table_batch_regression();turn_build_limit_regression();temporal_transaction_safety_regression();pool_exchange_regression();pool_exchange_fair_admission();temporal_transaction_regression();temporal_preparation_regression();temporal_forward_audit_regression();guide_window_regression();guide_routes_regression();guide_reconnect_regression();guide_refine_regression();flow_margin_regression();flow_refresh_regression();flow_cache_only_regression();flow_cost_scale_regression();temporal_wait_turn_regression();temporal_warm_start_regression();for(const char* temperature:{"100","0"}){setenv("CGAR_TEMPORAL_REGION_TEMPERATURE_PPM",temperature,1);temporal_region_adapter_regression();}unsetenv("CGAR_TEMPORAL_REGION_TEMPERATURE_PPM");temporal_distance_scale_regression();flow_guidance_regression();temporal_turn_progress_regression();temporal_region_adapter_regression();compact_turn_tables();turn_prefetch_regression();temporal_regions_regression();setenv("CGAR_TURN_COST","4",1);temporal_primary_regression();temporal_parallel_regression();unsetenv("CGAR_TURN_COST");initialization_failure_recovery();temporal_idle_blocker();global_task_candidates();temporal_parallel_regression();temporal_kernel_on_thread();temporal_primary_regression();oriented_distances();movement_diagnostics();unopened_reassignment();reassignment_primary_and_commitments();reassignment_recovery_protection();reassignment_fair_admission();weighted_pickup_assignment();cache_and_chain_consistency();consistent_progress_basis();certificates();pocket_case();pocket_case(20);persistent_primary();capacity_bootstrap();scheduler_case();fair_sparse_schedule();sparse_fallback_quality();replenish_taken_candidate();bounded_scheduler_work();compact_distances();bounded_distance_work();std::cout<<"All CGAR regression checks passed\n";}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
```
