# KittyKnight ablation implementation review

Continue the SAME persistent Fable5.1/max session. User authorized source sharing
via Claude Code. Read-only review, no external tools. This is a concrete follow-up
to your turn19 suggestion, not a new performance claim.

I accepted competitor ablations before a larger CGAR rewrite. Please check the
small source patch below for a confounded or incorrect treatment, and the fixture
for false confidence. Prioritize concrete defects over more speculative proposals.

Corrections to retain from turn19: supplied weights were already a stated confound;
we have not proved their offline training provenance. Uniform weights affect the
SHARED scheduler and motion heuristic, as well as work fitting the clock budget.
Motion per chain-cell does not control topology/leg count/congestion/censoring.
The8.025-step chain difference is not proved recoverable. Earlier HRRN-OFF had
17.0/17.6% extra empty steps;17.46/17.95% belongs to a cost-key discovery variant
that still used HRRN. A static cheaper pickup does not credit the released task's
later pickup and is not a rigorous bound on a different closed-loop policy.
Conflicts visible6..10steps ahead do not prove the5slot horizon is binding.

The compiled KK scheduler has skip_matching_if_no_free_agents=true on warehouse.
It does NOT necessarily rematch every tick; original skip behavior stays intact.
Current experimental change keeps copies of existing schedules, excludes assigned
unopened tasks and their robots from the matching pool when mode0, and changes
nothing else in matching. Default and1 retain original pool eligibility.
The setting is LORR_KK_REMATCH_UNOPENED=0/1, parsed once, invalid values throw.
Both native and patched fixture binaries call the actual warehouse matching
function against real orientation-aware heuristics on a9-cell corridor.
Native ignores the toggle; patched default/1 reproduces all native fixture schedules;
patched0 preserves unopened and started tasks and assigns free robots uniquely.
Original no-free-agent early exit and explicit override are both tested, as are
unchanged task progress/reveal time/simulator schedule and invalid option rejection.

Build8899236 PASSED, source419 original files untouched, frozen patched copy only,
22CGAR sources/tests still exactV50. Two KK source files change: eligibility and
post-LNS diagnostics. All arms use the same instrumented binary and native config
except uniform mode sets map_weights_path="" via CONFIG_PATH. Existing load code
initializes all weights1 then reads a nonempty supplied file. No map rule is added
to CGAR. No archive is edited.

Four200step screens8899237 are running concurrently, supplied/uniform x rematch/keep.
After screens, plan five full5000step cases: four factorial arms and a second
supplied/rematch control. Each exclusive EPYC9354 allocation uses16physical/
32logical cores, same as historic KK. Reserve128GiB per KK diagnostic like earlier
reference; explicitly label observed RSS and1s entry-timeout counters. These KK
runs are NOT CGAR32GB-compliant scores. Native KK remains clock-limited; independent
controls characterize noise, and counts do not establish fixed-work behavior.
No full results exist yet. Keep historic152981 target unchanged.

Diagnostics report matching-pool sizes every200steps when matching is called and
post-search LNS iteration_records/failed/initial/finalcost each call. Iteration
records count merged updates, NOT all speculative worker attempts. All source,
binary, configs, input dependencies, allocation and fixture results have hashes.
The runner verifies32logical/16physical cores/noquota/correctCPU, records actual
usage and errors, and keeps raw trajectories for independent exact work accounting.
Short screens are activation/feasibility only, not a throughput rank.

Please check the patch and hand-counted fixture below, especially unmatched held
tasks, native metadata mutation, skip behavior, and risks to factorial interpretation.
If sound, what extra observation is essential before treating a KK contrast as
support for the CGAR scheduler interaction? Keep review~600words.

## Frozen experimental patch

```diff
--- a/wppl_planner/src/scheduler.cpp
+++ b/wppl_planner/src/scheduler.cpp
@@ -1,3 +1,5 @@
+#include <cstdlib>
+#include <stdexcept>
 #include "scheduler.h"
 #include "util/Timer.h"
 #include <omp.h>
@@ -817,6 +819,13 @@
 
     // make a copy of the current schedule
     proposed_schedule = env->curr_task_schedule;
+    // Diagnostic intervention only: preserve every existing assignment when OFF.
+    static const bool rematch_unopened = [] {
+        const char* value = std::getenv("LORR_KK_REMATCH_UNOPENED");
+        if (!value || std::string(value) == "1") return true;
+        if (std::string(value) == "0") return false;
+        throw std::invalid_argument("LORR_KK_REMATCH_UNOPENED must be 0 or 1");
+    }();
     if (skip_matching_if_no_free_agents && env->new_freeagents.size() == 0){
         std::cout<<"no new free agents: skip matching..."<<std::endl;
         return;
@@ -830,7 +839,7 @@
     {
         auto & task = pair.second;
         if (task.agent_assigned == -1 
-            || task.idx_next_loc==0) {
+            || (rematch_unopened && task.idx_next_loc==0)) {
             assignable_tasks_indices.push_back(task.task_id);
         }
     }
@@ -855,7 +864,7 @@
             else {
                 auto task_id = env->curr_task_schedule[i_agent];
                 auto & task = env->task_pool[task_id];
-                if (task.idx_next_loc == 0)
+                if (rematch_unopened && task.idx_next_loc == 0)
                     assignable_agents_indices.push_back(i_agent);
             }
         }
@@ -865,6 +874,13 @@
         std::cout<<"assignable_agents_indices.size() "<<assignable_agents_indices.size()<<std::endl;
         std::cout<<"assignable_tasks_indices.size() "<<assignable_tasks_indices.size()<<std::endl;
     // )
+
+    if (env->curr_timestep % 200 == 0) {
+        std::cout << "[kk-rematch-diag] step=" << env->curr_timestep
+                  << " rematch=" << rematch_unopened
+                  << " agents=" << assignable_agents_indices.size()
+                  << " tasks=" << assignable_tasks_indices.size() << std::endl;
+    }
 
     if (assignable_agents_indices.size()>assignable_tasks_indices.size())
     {
--- a/wppl_planner/src/LNS/LNSSolver.cpp
+++ b/wppl_planner/src/LNS/LNSSolver.cpp
@@ -289,6 +289,15 @@
     ONLYDEV(g_timer.record_p("run_LNS_s");)
     // continue optimizing paths
     bool succ=lns->run(time_limiter);
+    // Post-search observations shared by every ablation arm. These are merged
+    // iteration records, not a count of every speculative worker search.
+    std::cout << "[kk-lns-diag] step=" << env.curr_timestep
+              << " success=" << succ
+              << " iteration_records=" << lns->iteration_stats.size()
+              << " failures=" << lns->num_of_failures
+              << " initial_cost=" << lns->initial_sum_of_costs
+              << " final_cost=" << lns->sum_of_costs << std::endl;
+
     // if (succ)
     // {
     //     cout<<"lns succeed"<<endl;

```

## Actual integration fixture

```cpp
// Real KittyKnight warehouse matching on a hand-counted nine-cell corridor.
// Compile once, then link against native and patched scheduler objects.
#include "scheduler.h"
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>

static void require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}

int main(int argc, char** argv) {
    require(argc == 2, "expected rematching behavior argument");
    const bool rematch = std::string(argv[1]) == "1";
    SharedEnvironment env;
    env.rows = 3; env.cols = 11; env.num_of_agents = 3;
    env.map.assign(33, 1);
    for (int x = 1; x <= 9; ++x) env.map[11 + x] = 0;
    env.map_name = "diagnostic_corridor.map";
    env.file_storage_path = ".";
    env.curr_states = {State(12, 0, 0), State(20, 0, 2), State(16, 0, 0)};
    env.goal_locations.resize(3);
    auto planner = std::make_shared<MyPlanner::WPPLPlanner>(&env);
    planner->map_weights = std::make_shared<std::vector<float>>(165, 1.0f);
    planner->heuristics = std::make_shared<HeuristicTable>(&env, planner->map_weights, true);
    planner->heuristics->compute_weighted_heuristics();
    require(planner->heuristics->get(12, 0, 20) == 8, "forward heuristic");
    require(planner->heuristics->get(20, 2, 13) == 7, "reverse heuristic");
    nlohmann::json cfg = {{"seed", 0}, {"use_external_executor", false},
        {"planning_window", 10}, {"execution_window", 1}, {"disable_agent_goals", false}};
    planner->lacam2_solver = std::make_shared<LaCAM2::LaCAM2Solver>(
        planner->heuristics, &env, planner->map_weights, 3, false, 1000000, cfg);
    planner->lacam2_solver->initialize(env);
    MyPlanner::MyScheduler scheduler(planner);
    scheduler.endpoint_locs = {20, 13, 16};
    for (int i = 0; i < 3; ++i) scheduler.endpoints_loc_to_id_mapping[scheduler.endpoint_locs[i]] = i;
    auto setup = [&] {
        env.task_pool.clear();
        env.task_pool.emplace(10, Task(10, {20, 19}, 0));
        env.task_pool.emplace(11, Task(11, {13, 14}, 3));
        env.task_pool.emplace(12, Task(12, {16, 17}, 1));
        env.task_pool.at(10).agent_assigned = 0;
        env.task_pool.at(12).agent_assigned = 2;
        env.task_pool.at(12).idx_next_loc = 1;
        env.curr_task_schedule = {10, -1, 12};
        env.new_freeagents = {1}; env.new_tasks = {11};
        scheduler.skip_matching_if_no_free_agents = true;
    };
    nlohmann::json records = nlohmann::json::array();
    for (int scenario = 0; scenario < 4; ++scenario) {
        setup();
        if (scenario == 1) {
            env.task_pool.at(10).agent_assigned = -1;
            env.curr_task_schedule[0] = -1;
            env.new_freeagents = {0, 1};
        } else if (scenario >= 2) {
            env.task_pool.at(11).agent_assigned = 1;
            env.curr_task_schedule[1] = 11;
            env.new_freeagents.clear(); env.new_tasks.clear();
            if (scenario == 3) scheduler.skip_matching_if_no_free_agents = false;
        }
        const auto before = env.curr_task_schedule;
        const auto tasks = env.task_pool;
        std::vector<int> proposed;
        scheduler.schedule_plan_greedy_matching_warehouse(1000, proposed, &env);
        const bool swaps = scenario == 1 || (rematch && (scenario == 0 || scenario == 3));
        const std::vector<int> expected = swaps ? std::vector<int>{11, 10, 12} : std::vector<int>{10, 11, 12};
        require(proposed == expected, "hand-computed complete assignment differs");
        require(std::set<int>(proposed.begin(), proposed.end()).size() == 3, "duplicate task");
        require(proposed[2] == 12, "started task reassigned");
        require(env.curr_task_schedule == before, "proposal overwrote simulator schedule");
        for (const auto& entry : tasks) {
            const auto& after = env.task_pool.at(entry.first);
            require(after.locations == entry.second.locations && after.idx_next_loc == entry.second.idx_next_loc
                    && after.t_revealed == entry.second.t_revealed && after.t_completed == entry.second.t_completed,
                    "task progress or age mutated");
        }
        records.push_back({{"scenario", scenario}, {"schedule", proposed}});
    }
    std::cout << "KK_FIXTURE " << records.dump() << std::endl;
    return 0;
}

```
