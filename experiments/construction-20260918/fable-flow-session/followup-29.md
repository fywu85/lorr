Continue our existing Fable5.1/max session. User target is now154795; reserved/bound shared GRID cores and5s development deadlines are explicitly authorized, fixed algorithm work and32GB RSS remain. Exact1s optimization deferred.

User asks to add short-task cherry-picking as a separately labelled TRICK to measure its contribution (among other tricks). We proposed a2x2 inside CGAR: lanes0/1 × short-task preference0/1, matched full5000 seeds0/2 first, then six seeds only if warranted. Preference disables HRRN age term AND independent forced-oldest admission, keeps started tasks and motion protections, no rematching changes, task deletion, hidden future or length cutoff. All four arms explicitly --trick WAREHOUSE;0/0 is a tagged counterfactual control that must equal generic. Two environment component selectors are rejected without CLI. Existing flag-only behavior stays lanes ON/short OFF. We will measure task counts and starvation at checkpoints, empty work, loaded work, completed cohorts. This cannot causally assign an exact fraction of competitor lead.

Please read this bounded patch for bugs or confounds, especially policy activation, factorial isolation and tests. Suggest one follow-up if this first trial does not improve. Do not equate this with full NMS/KK rematching. The generic remaining-flow0/1 full0/2 study and other9 maps are already running; the chain-price shadow diagnosis from your turn29 is pending while this explicit user request is implemented. No need to repeat your previous diagnostic review. The build/regression suite is running on GRID; you have run nothing.

Current patch relative to96d9bc4:
diff --git a/cgar/cgar_planner/cgar.cpp b/cgar/cgar_planner/cgar.cpp
index 7bb7a2b..f9db4ef 100644
--- a/cgar/cgar_planner/cgar.cpp
+++ b/cgar/cgar_planner/cgar.cpp
@@ -708,8 +708,10 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
     env_ = env;
     if (initialized_) return;  // the scheduler and the planner both call this
     n_ = env->num_of_agents;
-    static_trick_metric_ = !env->trick_instance.empty();
-    if (static_trick_metric_)
+    const auto trick_options = tricks::options(env->trick_instance);
+    static_trick_metric_ = trick_options.lanes;
+    short_task_trick_ = trick_options.short_tasks;
+    if (!env->trick_instance.empty())
         tricks::validate_map(env->trick_instance, env->map, env->rows, env->cols);
     stall_limit_ = env_int("CGAR_STALL", 4);
     commit_limit_ = env_int("CGAR_COMMIT_AGE", 3);
@@ -796,7 +798,7 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
     const int remaining_flow = env_int("CGAR_TEMPORAL_REMAINING_FLOW", 0);
     if (remaining_flow < 0 || remaining_flow > 1 ||
         (remaining_flow && (!temporal_ || !orientation_guidance_ || !flow_strength_ ||
-         static_trick_metric_ || guide_enabled_ || temporal_next_errand_ ||
+         !env->trick_instance.empty() || guide_enabled_ || temporal_next_errand_ ||
          temporal_service_audit_stride_ || temporal_conflict_audit_stride_ || temporal_transaction_options_.work)))
         throw std::invalid_argument("remaining-flow scoring requires generic temporal learned flow; incompatible with trick, guide, next-errand, paid-progress audits or branching");
     temporal_remaining_flow_ = remaining_flow != 0;
@@ -850,7 +852,7 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
     pibt_tickets_ = env_int("CGAR_PIBT_TICKETS", 0) != 0;
     pibt_commitments_ = env_int("CGAR_PIBT_COMMITMENTS", 0) != 0;
     enable_txn_ = env_int("CGAR_TXN", pibt_reference_ ? 0 : 1) != 0;
-    hrrn_ = env_int("CGAR_HRRN", 1) != 0;
+    hrrn_ = !short_task_trick_ && env_int("CGAR_HRRN", 1) != 0;
     pickup_weight_ = std::max(1, std::min(16, env_int("CGAR_PICKUP_WEIGHT", 1)));
     const int pickup_flow = env_int("CGAR_PICKUP_FLOW", 0);
     pickup_flow_nodes_ = env_int("CGAR_PICKUP_FLOW_NODES", 8192);
@@ -892,7 +894,7 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
     chain_flow_pricing_ = env_int("CGAR_CHAIN_FLOW_PRICING", 0);
     if (chain_flow_pricing_ < 0 || chain_flow_pricing_ > 4 ||
         (chain_flow_pricing_ && (!pickup_flow_ || !orientation_guidance_ || !flow_strength_ ||
-         static_trick_metric_ || temporal_remaining_flow_ || guide_enabled_ || reassign_ || reassign_pool_)))
+         !env->trick_instance.empty() || temporal_remaining_flow_ || guide_enabled_ || reassign_ || reassign_pool_)))
         throw std::invalid_argument("chain flow pricing requires generic learned pickup flow and mode0..4; incompatible with tricks, remaining-flow score, guide routes or rematching");
     if (chain_flow_pricing_)
         std::printf("[cgar-chain-pricing] mode=%d shadow=%d resident_only=1 extra_tables=0\n", chain_flow_pricing_, chain_flow_pricing_ == 4);
@@ -902,6 +904,8 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
     const size_t table_mb = static_cast<size_t>(env_int("CGAR_TABLE_MB", 2048));
     rng_.seed(static_cast<unsigned>(env_int("CGAR_SEED", 0)));
 
+    if (!env->trick_instance.empty() && (!temporal_ || !orientation_guidance_ || pibt_reference_ || guide_enabled_ || reassign_ || reassign_pool_))
+        throw std::invalid_argument("--trick WAREHOUSE requires temporal/oriented CGAR without guide routes or rematching");
     if (static_trick_metric_ && (!temporal_ || !orientation_guidance_ || pibt_reference_ || guide_enabled_ ||
         flow_cost_scale_ != 4 || turn_cost_ != 1 || turn_surcharge_ != 0 || cache_only_refresh))
         throw std::invalid_argument("--trick WAREHOUSE requires temporal/oriented CGAR, cost scale4, unit physical turns, no turn surcharge or cache-only refresh");
@@ -934,6 +938,13 @@ void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
             std::printf("[CGAR_TRICK] instance=%s provider=nms-lane-directions forward_base=4 opposing=16 turn=4 field_sha256=%s occupancy_sha256=%s learned_publications=disabled\n",
                 env->trick_instance.c_str(), tricks::warehouse_field_sha256, tricks::warehouse_occupancy_sha256);
         }
+        if (!env->trick_instance.empty()) {
+            if (!static_trick_metric_)
+                std::printf("[CGAR_TRICK] instance=%s provider=%s field_sha256=none learned_publications=enabled\n",
+                    env->trick_instance.c_str(), short_task_trick_ ? "short-task-preference" : "ablation-control");
+            std::printf("[CGAR_TRICK_COMPONENTS] instance=%s lanes=%d short_tasks=%d hrrn=%d oldest_admission=%d started_tasks=protected\n",
+                env->trick_instance.c_str(), static_trick_metric_, short_task_trick_, hrrn_, !short_task_trick_);
+        }
         if (flow_strength_ && !static_trick_metric_) flow_guidance_.initialize(cert_.free, cert_.rows, cert_.cols,
             env_int("CGAR_FLOW_WARMUP", 128), flow_strength_, env_int("CGAR_FLOW_MIN_SAMPLES", 8),
             env_int("CGAR_FLOW_MIN_MARGIN_PERCENT", 0), env_int("CGAR_FLOW_REFRESH_INTERVAL", 0), flow_cost_scale_, cache_only_refresh != 0);
@@ -2442,7 +2453,9 @@ void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vec
         return pair_for(r, t, static_cast<int>(std::min<long long>(kInf - 1, static_cast<long long>(d) * pickup_scale)));
     };
     auto fair_admission = [&]() {
-        if (regular_admissions_ < n_) return;
+        // Explicit competition-objective ablation: long unpicked tasks may wait
+        // indefinitely. Started assignments and motion protection are unchanged.
+        if (short_task_trick_ || regular_admissions_ < n_) return;
         const int t = oldest_task();
         if (t < 0) return;
         Pair best{};
diff --git a/cgar/cgar_planner/cgar.hpp b/cgar/cgar_planner/cgar.hpp
index d931ae6..f7f754c 100644
--- a/cgar/cgar_planner/cgar.hpp
+++ b/cgar/cgar_planner/cgar.hpp
@@ -441,6 +441,7 @@ private:
     bool enable_txn_ = true;
     bool enable_locks_ = true;
     bool hrrn_ = true;
+    bool short_task_trick_ = false;
     bool repair_fallback_ = true;
     bool refine_chain_costs_ = false;
     bool scheduler_cache_peek_ = false;
diff --git a/cgar/cgar_planner/tricks.hpp b/cgar/cgar_planner/tricks.hpp
index 5ce84d9..1113575 100644
--- a/cgar/cgar_planner/tricks.hpp
+++ b/cgar/cgar_planner/tricks.hpp
@@ -6,6 +6,7 @@
 #include <string>
 #include <vector>
 #include <cstdint>
+#include <cstdlib>
 
 namespace cgar { namespace tricks {
 
@@ -14,6 +15,28 @@ inline void validate_name(const std::string& name) {
         throw std::invalid_argument("unknown --trick instance: " + name + "; supported: WAREHOUSE");
 }
 
+struct Options { bool lanes = false, short_tasks = false; };
+
+// Environment settings select components only after explicit CLI activation.
+// Even a zero-valued setting without --trick is rejected to prevent silent use.
+inline Options options(const std::string& instance) {
+    const char* lanes = std::getenv("CGAR_TRICK_LANES");
+    const char* short_tasks = std::getenv("CGAR_TRICK_SHORT_TASKS");
+    if (instance.empty()) {
+        if (lanes || short_tasks)
+            throw std::invalid_argument("CGAR_TRICK component settings require --trick WAREHOUSE");
+        return {};
+    }
+    validate_name(instance);
+    auto boolean = [](const char* value, bool fallback) {
+        if (!value) return fallback;
+        if (std::string(value) == "0") return false;
+        if (std::string(value) == "1") return true;
+        throw std::invalid_argument("CGAR_TRICK component settings must be 0 or 1");
+    };
+    return {boolean(lanes, true), boolean(short_tasks, false)};
+}
+
 inline void validate_map(const std::string& name, const std::vector<int>& map, int rows, int cols) {
     validate_name(name);
     if (rows != warehouse_rows || cols != warehouse_cols || map.size() != size_t(rows) * cols)
diff --git a/cgar/src/driver.cpp b/cgar/src/driver.cpp
index 6d4d6f2..2f59af0 100644
--- a/cgar/src/driver.cpp
+++ b/cgar/src/driver.cpp
@@ -65,6 +65,12 @@ int main(int argc, char **argv)
     }
 
     po::notify(vm);
+    try {
+        cgar::tricks::options(vm.count("trick") ? vm["trick"].as<std::string>() : std::string());
+    } catch (const std::exception& error) {
+        std::cerr << "CGAR_TRICK_ERROR: " << error.what() << std::endl;
+        return 2;
+    }
     if (vm.count("trick")) {
         try {
             cgar::tricks::validate_name(vm["trick"].as<std::string>());
diff --git a/cgar/tests/cgar_regression.cpp b/cgar/tests/cgar_regression.cpp
index 3f42b47..01642f0 100644
--- a/cgar/tests/cgar_regression.cpp
+++ b/cgar/tests/cgar_regression.cpp
@@ -2931,7 +2931,42 @@ void warehouse_trick_regression() {
  }
  if(trick.stats().flow_publications||trick.stats().flow_cache_resets||generic.stats().flow_publications==0)
   throw std::runtime_error("static trick activation or publication isolation failed");
+ // Distinguish age preference from the independent forced-oldest admission.
+ // Both tasks have the same pickup, so only their chain and age differ.
+ int far=goal;for(int cell=0;cell<int(e.map.size());++cell)
+  if(!e.map[cell]&&std::abs(cell/e.cols-goal/e.cols)+std::abs(cell%e.cols-goal%e.cols)>50){far=cell;break;}
+ if(far==goal)throw std::runtime_error("short-task fixture lacks a long chain");
+ for(int lanes:{0,1})for(int short_tasks:{0,1})for(int age_preference:{0,1}){
+  setenv("CGAR_TRICK_LANES",std::to_string(lanes).c_str(),1);
+  setenv("CGAR_TRICK_SHORT_TASKS",std::to_string(short_tasks).c_str(),1);
+  setenv("CGAR_HRRN",std::to_string(age_preference).c_str(),1);
+  auto test=flagged;test.curr_timestep=100;test.curr_task_schedule={-1};test.goal_locations={{}};test.task_pool.clear();
+  Task old;old.task_id=0;old.t_revealed=0;old.locations={goal,far};test.task_pool.emplace(0,old);
+  Task fresh;fresh.task_id=1;fresh.t_revealed=100;fresh.locations={goal,goal};test.task_pool.emplace(1,fresh);
+  Cgar policy;policy.initialize(&test,30000);
+  policy.schedule(&test,30000,schedule);
+  const int expected=short_tasks||!age_preference?1:0;
+  if(schedule!=std::vector<int>{expected}||policy.stats().fair_assignments)
+   throw std::runtime_error("short-task preference did not isolate the age term");
+  policy.schedule(&test,30000,schedule);
+  if(schedule!=std::vector<int>{short_tasks?1:0}||policy.stats().fair_assignments!=!short_tasks)
+   throw std::runtime_error("short-task preference did not isolate oldest admission");
+  test.curr_task_schedule={0};test.task_pool.at(0).agent_assigned=0;test.task_pool.at(0).idx_next_loc=1;
+  test.goal_locations={{{far,0}}};policy.schedule(&test,30000,schedule);
+  if(schedule!=std::vector<int>{0}||test.task_pool.at(0).idx_next_loc!=1)
+   throw std::runtime_error("short-task trick redirected a started long task");
+ }
+ for(const char* key:{"CGAR_TRICK_LANES","CGAR_TRICK_SHORT_TASKS"}){
+  unsetenv("CGAR_TRICK_LANES");unsetenv("CGAR_TRICK_SHORT_TASKS");setenv(key,"0",1);rejected=false;
+  try{options("");}catch(const std::invalid_argument&){rejected=true;}
+  if(!rejected)throw std::runtime_error("trick component activated without CLI");
+  for(const char* bad:{"", "-1", "2", "true"}){setenv(key,bad,1);rejected=false;
+   try{options("WAREHOUSE");}catch(const std::invalid_argument&){rejected=true;}
+   if(!rejected)throw std::runtime_error("malformed trick component accepted");}
+ }
+ unsetenv("CGAR_TRICK_LANES");unsetenv("CGAR_TRICK_SHORT_TASKS");unsetenv("CGAR_HRRN");
  for(const char* key:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_STEPS","CGAR_FLOW_STRENGTH","CGAR_FLOW_COST_SCALE","CGAR_PICKUP_FLOW","CGAR_FLOW_WARMUP","CGAR_FLOW_REFRESH_INTERVAL"})unsetenv(key);
+ std::cout<<"TRICK_SHORT_TASKS passed age_and_oldest_admission_independent=1 lanes_factorial_cases=8 started_long_task_protected=1 explicit_cli_required=1 invalid_components_rejected=1\n";
  std::cout<<"WAREHOUSE_TRICK passed explicit_activation=1 map_identity_rejection=1 independent_oriented_states="<<compared<<" generic_initial_dispatch=1 static_pickup_from_tick1=1 no_flow_publications=1 generic_flow_preserved=1\n";
 }
 
diff --git a/tools/benchmark_cgar.py b/tools/benchmark_cgar.py
index 8a34eea..c4638a0 100644
--- a/tools/benchmark_cgar.py
+++ b/tools/benchmark_cgar.py
@@ -36,14 +36,22 @@ def header(path):
     return result
 
 
-def trick_receipt_valid(log, instance, expected_hash):
+def trick_receipt_valid(log, instance, expected_hash, expected_components=None):
     receipts = [line for line in log.splitlines() if line.startswith('[CGAR_TRICK] ')]
+    components = [line for line in log.splitlines() if line.startswith('[CGAR_TRICK_COMPONENTS] ')]
     if not instance:
-        return not receipts
+        return not receipts and not components
     if len(receipts) != 1:
         return False
     fields = dict(field.split('=', 1) for field in receipts[0].split()[1:] if '=' in field)
-    return fields.get('instance') == instance and fields.get('field_sha256') == expected_hash
+    valid = fields.get('instance') == instance and fields.get('field_sha256') == expected_hash
+    if expected_components is not None:
+        if len(components) != 1:
+            return False
+        actual = dict(field.split('=', 1) for field in components[0].split()[1:] if '=' in field)
+        valid = valid and actual == dict(instance=instance, started_tasks='protected',
+                                       **{k: str(v) for k, v in expected_components.items()})
+    return valid
 
 
 def main():
@@ -109,8 +117,21 @@ def main():
     binary_hash = hashlib.sha256(binary.read_bytes()).hexdigest()
     if provenance is not None and provenance["binary_sha256"] != binary_hash:
         parser.error("source-manifest does not describe this executable")
-    expected_trick_field = None
-    if args.trick:
+    component_keys = ['CGAR_TRICK_LANES', 'CGAR_TRICK_SHORT_TASKS']
+    explicit_components = any(k in environment for k in component_keys)
+    if explicit_components and not args.trick:
+        parser.error('CGAR_TRICK component settings require --trick WAREHOUSE')
+    if any(environment.get(k, '0') not in ('0', '1') for k in component_keys):
+        parser.error('CGAR_TRICK component settings must be 0 or 1')
+    expected_components = None
+    if args.trick and explicit_components:
+        short = int(environment.get('CGAR_TRICK_SHORT_TASKS', '0'))
+        expected_components = dict(lanes=int(environment.get('CGAR_TRICK_LANES', '1')),
+                                   short_tasks=short, hrrn=0 if short else int(environment.get('CGAR_HRRN', '1')) != 0,
+                                   oldest_admission=1-short)
+        expected_components['hrrn'] = int(expected_components['hrrn'])
+    expected_trick_field = 'none' if args.trick else None
+    if args.trick and environment.get('CGAR_TRICK_LANES', '1') == '1':
         asset_name = 'cgar/tricks/warehouse_lanes.hpp'
         asset = (ROOT / asset_name).read_bytes()
         if provenance is not None and provenance['sources'].get(asset_name) != hashlib.sha256(asset).hexdigest():
@@ -123,6 +144,7 @@ def main():
                 "experiment_track": "TRICK" if args.trick else "GENERIC",
                 "trick": args.trick, "trick_argv": ["--trick", args.trick] if args.trick else [],
                 "expected_trick_field_sha256": expected_trick_field,
+                "expected_trick_components": expected_components,
                 "jobs": args.jobs, "plan_time_limit_ms": args.plan_time_limit_ms, "preprocess_time_limit_ms": 30000,
                 "log_detail_level": args.log_detail_level,
                 "max_process_memory_bytes": MAX_PROCESS_MEMORY_BYTES,
@@ -187,7 +209,7 @@ def main():
                                                     0 <= entry_time <= args.plan_time_limit_ms / 1000.0)
         peak_bytes = usage.get("peak_rss_kib", 0) * 1024
         memory_valid = 0 < peak_bytes <= MAX_PROCESS_MEMORY_BYTES
-        receipt_valid = trick_receipt_valid((out / (name + ".log")).read_text(), args.trick, expected_trick_field)
+        receipt_valid = trick_receipt_valid((out / (name + ".log")).read_text(), args.trick, expected_trick_field, expected_components)
         # New binaries derive these labels from the CLI value received by BaseSystem.
         # Failed entries have no result; legacy generic binaries may omit the labels.
         track_valid = (not output.exists() or
