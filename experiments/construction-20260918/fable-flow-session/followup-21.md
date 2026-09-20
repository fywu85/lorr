Continue the SAME persistent Fable 5.1 max session. Read-only review, no tools or unperformed test claims. User specifically asks whether we checked NMS and wants it examined as well. We did a fresh active-source pass and replayed NMS against CURRENT six-seed CGAR. No new policy gain. KK five full factorial runs still pending; do not infer their outcomes.

Please assess which GENERIC NMS differences still deserve a controlled transfer, considering everything already tested in this session. Prioritize at most two next experiments. In particular:
1. The active NMS kernel is substantially ported and earlier conformance matched selected operations. Multi-owner try_echo_slam exists but the active solve never calls it. Warehouse robot power is already 1 in both. Avoid crediting disabled code.
2. Active warehouse guidance is hard-coded lane masks/coordinate bands and cannot be imported under our constraint. The no-new-map-specific-rules constraint still applies despite focusing on warehouse. Whole oriented-distance precomputation exceeds our memory budget in the observed process; bounded caches stay.
3. Assignment uses a global heap over partially refreshed all-task lists, reopens all unopened tasks, no age term in price, 5*pickup+chain. Old cost-only/pickup-weight CGAR variants regressed; no claim that copying the factor 5 works. Could fresh-task-centered matching exploit equivalent additive endpoint groups or a streaming task-to-holder pass efficiently? Identify a discriminating coverage measurement and a bounded first implementation, with finite-retarget and current protected/fair-admission rules retained. No liveness claim from time-varying cost decrease.
4. We added default-off read-only root+1/2-owner snapshot capture to current CGAR (source delta supplied). All regressions pass; paired 200-step identity screen finishing. Full captures next. Root/group choices retain outside cell/edge owners. Does the resulting complete group search provide a useful counterfactual distinct from our failed two-owner pass? Do not turn a small-group null into a global rejection or a witness into a production reachability claim.
5. The NMS replay exposed initial idle steps omitted in the OLD residual accounting. Corrected 10000 initial timeout steps, not a benchmark score change. Raw hash matches; all old fields except idle/loaded residual reproduce; five action fixtures + pool fixture pass. NMS one timeout and 39.744GB remains a diagnostic, never an admissible score.

Fresh source and evidence review follows:

# NMS follow-up against current CGAR

2026-09-20 UTC. This pass follows the active `Solution` tree in archive
`44950d1b61b7482b4529a9dc127fa9d220962bdc`, its compiled entry and enabled settings.
NMS has already contributed substantially to CGAR. Its five-action operations,
recursive displacement and randomized repair are in the current temporal kernel.
The earlier [native conformance study](../PROBE.md) matched all 51,600 geometry
cases and 15,360 selected robot operations across construction/repair fixtures.
Those tests and old fixed-goal scores precede today's much stronger CGAR policy;
they must not be presented as a current throughput gap.

## What the active code actually does

| Component | NMS implementation | Current CGAR implication |
|---|---|---|
| Operations and repair | 129 five-slot patterns; initial depth 3, repair depth 3–5; one distinct blocking owner; randomized displacement and annealing | Already transferred. The active solve calls `try_build`, not its unused `try_echo_slam` multi-owner routine. There is no missing active NMS multi-owner algorithm to copy. |
| Search portfolio | 32 independent worker copies, select the best final internal score; clock-limited work | CGAR has fixed-work workers and prescribed repair. More workers were already mixed across six seeds. This is not the highest-priority untested transfer. |
| Motion weights | Warehouse sets every robot's power to 1; other map families use remaining-chain ranks | Equal power is already our reference. Copying warehouse order or map-specific priority switches adds no general principle. |
| Task candidate coverage | For selected eligible robots, refresh a sorted list over every currently eligible task; refresh oldest lists first | Wider discovery is distinct from increasing CGAR's already-complete pickup-field quota. Measure missed useful fresh tasks and stale candidates before expanding coverage. |
| Assignment | Release all unopened tasks, preserve started tasks, greedily merge per-robot lists using a global heap; weighted pickup plus weighted whole chain | Broader rematching and metric consistency remain useful hypotheses. The lists can be stale, so this is not a globally optimal matching or a fully refreshed matrix each tick. Scheduler LNS is configured to zero time. |
| Assignment price | Warehouse uses `5 * pickup + chain`, with no task-age term in that cost; each inter-errand leg assumes orientation 0 | Larger pickup weights and cost-only variants have already failed in CGAR. Do not copy the factor 5 or assume the chain metric exactly models the robot's arrival orientation. Test coverage/rematching separately from richer chain prices. |
| Guidance | Active warehouse path calls `set_warehouse`: alternating lanes, an eight-column direction mask, hard-coded coordinate bands, forward weights 20/200 | This is map-specific guidance, not a generic online congestion estimator. It falls outside our constraint. Generic, consistent guidance remains a separate research direction; NMS does not supply a ready generic warehouse updater. |
| Heuristic coverage | Precompute weighted oriented distances to every free destination; precompute operation geometry | CGAR uses bounded caches and already shares candidate preparation. Reproduce useful coverage within 32 GB rather than importing the entire NMS memory footprint. |

Source anchors: `Solution/settings.hpp:20–75`, `Planner/eplanner.cpp:24–78`,
`Planner/PIBT/pibts.cpp:65–91,363–435,676–725,920–1008`,
`Objects/Environment/operations.cpp:70`, `environment.cpp:151–180`,
`graph_guidance.cpp:69–99,231–253`, `heuristic_matrix.cpp:61–84`,
`Scheduler/scheduler_solver.cpp:13–45,53–82,182–225,254–284,293–395`,
and `Scheduler/scheduler.cpp:42–49`. Paths are relative to NMS's `Solution`.
[Inspected file hashes and accounting](../results/nms-task-pool-audit/accounting.json).
The unused guidance-path planner, dynamic heuristic rebuilding and scheduler LNS
must not be credited for the active snapshot's results. The enabled fleet-cap
code permits all 10,000 robots in this warehouse case; it is not an explanation
for this comparison and is not a proposed transfer.

## Full trajectory evidence

The saved NMS run completed **154,981** tasks, with **one entry timeout** and
**39.744 GB RSS**. It is useful diagnostic evidence but does not meet our
one-second/32-billion-byte contract. Its original raw trajectory hash matches
the independent historical replay. The new replay accounts for all 50 million
robot steps, passes five hand-counted action fixtures plus a task-pool fixture,
and preserves every earlier metric except the two explicitly corrected below.

Robot steps per completed task:

| Component | Current CGAR, pooled six seeds | KK, one run | NMS, one diagnostic run |
|---|---:|---:|---:|
| Shortest chain of completed tasks | 241.861 | 233.836 | 234.794 |
| Extra loaded forward travel | 7.686 | 4.594 | 3.032 |
| Loaded turns | 9.747 | 5.678 | 5.802 |
| Loaded waits | 13.552 | 8.411 | 7.920 |
| All empty work | 64.295 | 65.691 | 62.021 |
| Unfinished loaded work | 10.223 | 8.628 | 8.672 |
| Idle | 0 | 0 | 0.379 |

NMS's completed loaded detour/turn/wait overhead totals **16.755**, versus about
**30.985** for current CGAR. This is a behavioral difference, not proof that
14.23 steps per completion can be recovered by changing the kernel. The active
kernels are close, while guidance, task cohorts, work budgets and memory differ.

NMS also leaves a long-task backlog: at step 5,000 its 5,029 unassigned tasks
average **506.89** cells and **2,703.02** steps since reveal. **1,562** tasks remain
unpicked after at least 4,000 steps; **4,933** after at least 1,000. CGAR has no
unpicked task aged 1,000 at the six sampled checkpoints. This reinforces the
need to report assignment delay alongside throughput. It does not prove which
part of NMS's advantage is caused by rematching or task preference.

The first replay attempt (GRID 8899254) correctly failed the complete-phase
identity. NMS's initial timeout occurs before TaskManager emits its first accepted
schedule. The older accounting omitted those 10,000 implicit unassigned steps
and included them in the loaded residual. The corrected replay (8899256) retains
the timeout markers and changes idle **48,814 → 58,814**, total loaded
**40,339,131 → 40,329,131**. All other historical fields reproduce exactly.
The original report is retained. Valid CGAR/KK schedules starting at step 0/1
do not need this correction. [Replay evidence](../results/nms-task-pool-audit/cohorts.json),
[reproduction](nms_account.py), [general replay](pool_audit.py).

## Next experiments

1. Finish the current same-state complete small-group probe. NMS's shared
   one-owner restriction makes it especially important to measure feasible joint
   replacements rather than infer a search limitation from blocker counts.
2. Audit useful fresh-task omissions and old/new cost-quote coverage before
   implementing bounded task-centered unopened rematching. Preserve started,
   primary, recovery and finite-retarget/fair-admission protection. Keep ordinary
   assignment and rematching as separate factors; report all empty work and age tails.
3. Use the pending KK guidance/rematching factorial to prioritize a generic
   guidance change versus assignment. NMS's map-specific lanes are diagnostic
   context, not a candidate CGAR implementation. Do not repeat the failed intended-route
   guide or treat larger cache quotas as a new coverage experiment.

No new throughput record or production-policy promotion is claimed by this pass.


NMS source Scheduler/scheduler_solver.cpp lines 13-45:
```cpp
void SchedulerSolver::rebuild_dp(uint32_t r) {
    dp[r].clear();
    for (uint32_t t: free_tasks) {
        dp[r].emplace_back(get_dist(r, t), t);
    }
    std::sort(dp[r].begin(), dp[r].end());
    timestep_updated[r] = env->curr_timestep + 1;
}

void SchedulerSolver::rebuild_dp(TimePoint end_time) {
    ETimer timer;
    std::vector<uint32_t> order = free_robots;
    std::stable_sort(order.begin(), order.end(), [&](uint32_t lhs, uint32_t rhs) {
        return timestep_updated[lhs] < timestep_updated[rhs];
    });

    std::atomic<uint32_t> counter{};
    auto do_work = [&](uint32_t thr) {
        for (uint32_t i = thr; i < order.size() && get_now() < end_time; i += THREADS) {
            rebuild_dp(order[i]);
            ++counter;
        }
    };

    std::vector<std::thread> threads(THREADS);
    for (uint32_t thr = 0; thr < THREADS; thr++) {
        threads[thr] = std::thread(do_work, thr);
    }
    for (uint32_t thr = 0; thr < THREADS; thr++) {
        threads[thr].join();
    }
    PRINT(
            Printer() << "SchedulerSolver::rebuild_dp: " << counter << "/" << order.size() << " ("
```


NMS source Scheduler/scheduler_solver.cpp lines 53-83:
```cpp
uint64_t SchedulerSolver::get_dist(uint32_t r, uint32_t t) const {
    if (t == -1) {
        return 1e6;
    }

    uint32_t source = get_robots_handler().get_robot(r).node;
    uint64_t dist_to_target = get_hm().get(source, task_target[t]);
    uint64_t dist = dist_to_target + task_metric[t];
    if (get_test_type() == TestType::WAREHOUSE || get_test_type() == TestType::SORTATION) {
        // dist = dist_to_target * dist_to_target + task_metric[t]; // 38242
        dist = dist_to_target * 5 + task_metric[t];// 38797
        // dist = dist_to_target * 3 + task_metric[t]; // 38566
        // dist = dist_to_target * 6 + task_metric[t]; // 38777
    } else if (get_map_type() == MapType::RANDOM) {
        // dist = dist_to_target * dist_to_target + task_metric[t]; // 7175
        // dist = dist_to_target + task_metric[t]; // 7107
        // dist = dist_to_target * 2 + task_metric[t]; // 7178
        // dist = dist_to_target * 3 + task_metric[t]; // 7239
        // dist = dist_to_target * 4 + task_metric[t]; // 7265
        dist = dist_to_target * 5 + task_metric[t];// 7277
        // dist = dist_to_target * 6 + task_metric[t]; // 7252
    } else if (get_map_type() == MapType::CITY) {
        dist = dist_to_target * 5 + task_metric[t];
    } else if (get_map_type() == MapType::GAME) {
        dist = dist_to_target * 5 + task_metric[t]; // 8647
        // dist = dist_to_target + task_metric[t];// 8617
        // dist = dist_to_target * dist_to_target + task_metric[t]; // 8460
    }
    ASSERT(static_cast<uint32_t>(dist) == dist, "overflow");
    return dist;
}
```


NMS source Scheduler/scheduler_solver.cpp lines 191-225:
```cpp
    // build free_tasks
    for (auto &[t, task]: env->task_pool) {
        int r = task.agent_assigned;
        if (
                r == -1// нет агента
#ifdef ENABLE_SCHEDULER_CHANGE_TASK
                || task.idx_next_loc == 0// мы можем поменять задачу
#endif
        ) {
#ifdef ENABLE_SCHEDULER_CHANGE_TASK
            task.agent_assigned = -1;// IMPORTANT! remove task agent assigned
#endif
            free_tasks.push_back(t);
        }
    }

    // build free_robots
    for (uint32_t r = 0; r < env->num_of_agents; r++) {
        int t = env->curr_task_schedule[r];

        // есть задача и она в процессе выполнения
        // не можем ее убрать
        if (env->task_pool.count(t) && env->task_pool.at(t).idx_next_loc != 0) {
            desires[r] = t;
            continue;
        }
        if (
                // нет задачи
                !env->task_pool.count(t)
#ifdef ENABLE_SCHEDULER_CHANGE_TASK
                || env->task_pool.at(t).idx_next_loc == 0
#endif
        ) {
            free_robots.push_back(r);
        }
```


NMS source Scheduler/scheduler_solver.cpp lines 254-284:
```cpp
    // build task_metric, task_target
    {
        ETimer timer;
        for (uint32_t t: free_tasks) {
            if (task_metric.size() <= t) {
                task_metric.resize(t + 1, -1);
                task_target.resize(t + 1);
            }
            auto &task = env->task_pool[t];
            task_target[t] = task.locations[0] + 1;

            uint32_t d = 0;
            for (int i = 0; i + 1 < task.locations.size(); i++) {
                int source = task.locations[i] + 1;
                int target = task.locations[i + 1] + 1;
                d += get_hm().get(get_graph().get_node(Position(source, 0)), target);
            }
            task_metric[t] = d;
        }
        PRINT(Printer() << "init task_dist: " << timer << '\n';);
    }

    for (uint32_t t: free_tasks) {
        task_to_robot[t] = -1;
    }

    cur_score = 0;
    for (uint32_t r: free_robots) {
        desires[r] = -1;
        cur_score += get_dist(r, desires[r]);
        cur_score += phantom_agent_dist[r];
```


NMS source Scheduler/scheduler_solver.cpp lines 317-395:
```cpp
#else
    std::unordered_set<uint32_t> used_task;

    // (dist, r, index)
    std::priority_queue<std::tuple<uint32_t, uint32_t, uint32_t>, std::vector<std::tuple<uint32_t, uint32_t, uint32_t>>, std::greater<>> Heap;
    for (uint32_t r: free_robots) {
        if (!dp[r].empty()) {
            Heap.push({dp[r][0].first + phantom_agent_dist[r], r, 0});
        }
    }

    int32_t allowed_assigned = 0;
    {
        int32_t cnt_assigned = 0;
        for (uint32_t r = 0; r < desires.size(); r++) {
            int t = env->curr_task_schedule[r];
            if (t != -1 && env->task_pool.count(t) && env->task_pool.at(t).idx_next_loc != 0) {
                // этот робот уже с задачей, которую нельзя менять
                cnt_assigned++;
            }
        }

        int32_t max_assigned = desires.size();

        allowed_assigned = std::max(0, max_assigned - cnt_assigned);

        PRINT(
                Printer() << "cnt_assigned: " << cnt_assigned << '\n';
                Printer() << "max_assigned: " << max_assigned << '\n';
                Printer() << "allowed_assigned: " << allowed_assigned << '\n';);
    }

    auto validate_task = [&](uint32_t task_id) {
        // task is already used
        if (used_task.count(task_id)) {
            return false;
        }
        auto it = env->task_pool.find(task_id);
        if (// this task is not available
                it == env->task_pool.end() ||
                // robot already used this task
                it->second.agent_assigned != -1) {
            return false;
        }
        return true;
    };

    uint32_t count_skip = 0;

    while (!Heap.empty() && get_now() < end_time && allowed_assigned > 0) {
        auto [dist, r, index] = Heap.top();
        Heap.pop();

        uint32_t task_id = dp[r][index].second;
        ASSERT(dist == dp[r][index].first + phantom_agent_dist[r], "invalid dist");

        if (!validate_task(task_id)) {
            index++;

            if (index < dp[r].size()) {
                Heap.push({dp[r][index].first + phantom_agent_dist[r], r, index});
            }

            count_skip++;

            continue;
        }

        ASSERT(env->task_pool.count(task_id), "no contains");
        ASSERT(env->task_pool[task_id].agent_assigned == -1, "already assigned");
        ASSERT(!used_task.count(task_id), "already used");

        add(r, task_id);
        used_task.insert(task_id);

        if (phantom_agent_dist[r] == 0) {
            allowed_assigned--;
        }
    }
```


NMS source Planner/PIBT/pibts.cpp lines 65-91:
```cpp
uint32_t PIBTS::get_used(uint32_t r) const {
    uint32_t answer = -1;

    auto &poses_path = get_omap().get_poses_path(robots[r].node, desires[r]);
    for (uint32_t depth = 0; depth < DEPTH; depth++) {
        uint32_t to_pos = poses_path[depth];
        if (used_pos[to_pos][depth] != -1) {
            if (answer == -1) {
                answer = used_pos[to_pos][depth];
            } else if (answer != used_pos[to_pos][depth]) {
                return -2;
            }
        }
    }

    const auto &edges_path = get_omap().get_edges_path(robots[r].node, desires[r]);
    for (uint32_t depth = 0; depth < DEPTH; depth++) {
        uint32_t to_edge = edges_path[depth];
        if (used_edge[to_edge][depth] != -1) {
            if (answer == -1) {
                answer = used_edge[to_edge][depth];
            } else if (answer != used_edge[to_edge][depth]) {
                return -2;
            }
        }
    }
    return answer;
```


NMS source Planner/PIBT/pibts.cpp lines 704-725:
```cpp
        const double workload = robots.size() * 1.0 / get_map().get_count_free();
        for (uint32_t r = 0; r < robots.size(); r++) {
            double power = (max_weight - weight[r]) * 1.0 / max_weight;
            if (robots[r].is_disable()) {
                power = 0;
            }
            if (get_test_type() == TestType::GAME) {
                power = power * power;
            } else if (get_test_type() == TestType::RANDOM_4) {
                power = power * power;
            } else if (get_test_type() == TestType::RANDOM_5) {
                power = power * power;
            } else if (get_test_type() == TestType::WAREHOUSE) {
                power = 1;// std::sqrt(power)
            } else if (get_test_type() == TestType::SORTATION) {
                power = 1;// std::sqrt(power)
            }
            robot_power[r] = power;
        }

        std::stable_sort(order.begin(), order.end(), [&](uint32_t lhs, uint32_t rhs) {
            return std::tie(robot_power[lhs], lhs) > std::tie(robot_power[rhs], rhs);
```


NMS source Planner/PIBT/pibts.cpp lines 920-962:
```cpp
void PIBTS::solve(uint64_t seed) {
    rnd = Randomizer(seed);

    auto update_current_depth = [&]() {
        if (get_test_type() == TestType::RANDOM_5) {
            current_depth = 4;
        } else if (get_map_type() == MapType::CITY || get_map_type() == MapType::GAME) {
            current_depth = rnd.get(3, 4);
        } else {
            current_depth = rnd.get(3, 5);
        }
    };

    temp = 0;
    for (uint32_t r: order) {
        if (get_now() >= end_time) {
            break;
        }
        if (desires[r] != 0) {
            continue;
        }
        build(r);
    }

    best_desires = desires;
    best_score = cur_score;

    temp = 0.001;

    if constexpr (true) {
        for (step = 0; get_now() < end_time && step < PIBTS_STEPS; step++) {
            if (step && step % 128 == 0) {
                update_current_depth();
            }
            uint32_t r = rnd.get(0, robots.size() - 1);
            try_build(r);
            temp *= 0.999;
        }
        if (best_score + 1e-6 < cur_score) {
            //Printer() << "improve: " << best_score << " -> " << cur_score << '\n';
            best_desires = desires;
            best_score = cur_score;
        }
```


NMS source Objects/Environment/graph_guidance.cpp lines 69-99:
```cpp
void GraphGuidance::set_warehouse() {
    set_grid();

    uint32_t msk = 0b10100101;
    int bit = 0;
    for (uint32_t y = 0; y < cols; y++) {
        if ((msk >> bit) & 1) {
            set(0, y, rows - 1, y, 1, 0, PENALTY_WEIGHT);
            set(0, y, rows - 1, y, 3, 0, OK_WEIGHT);
        } else {
            set(0, y, rows - 1, y, 3, 0, PENALTY_WEIGHT);
            set(0, y, rows - 1, y, 1, 0, OK_WEIGHT);
        }
        bit = (bit + 1) % 8;
    }

    // [KEK]: повышает вес верхней и нижней плашки, что уменьшает загруженность агентов там
    // реально улучшает
    // 36754 -> 37548
    {
        add(3, 0, 10, cols - 1, 0, 0, 1);
        add(3, 0, 10, cols - 1, 1, 0, 1);
        add(3, 0, 10, cols - 1, 2, 0, 1);
        add(3, 0, 10, cols - 1, 3, 0, 1);

        add(125, 0, 137, cols - 1, 0, 0, 1);
        add(125, 0, 137, cols - 1, 1, 0, 1);
        add(125, 0, 137, cols - 1, 2, 0, 1);
        add(125, 0, 137, cols - 1, 3, 0, 1);
    }
}
```


NMS source Objects/Environment/graph_guidance.cpp lines 231-253:
```cpp
GraphGuidance::GraphGuidance(SharedEnvironment &env) : rows(env.rows), cols(env.cols), graph(env.rows * env.cols + 1) {
    set_default();

#ifdef ENABLE_GG
    if (get_map_type() == MapType::WAREHOUSE) {
        //read("scripts/warehouse_large_weight_008.w");
        set_warehouse();
    } else if (get_map_type() == MapType::SORTATION) {
        //read("scripts/sortation_large_weight_008.w");
        set_sortation();
    } else if (get_map_type() == MapType::GAME) {
        //read("scripts/brc202d_weight_002.w");
        set_game();
    } else if (get_map_type() == MapType::CITY) {
        set_city();
    } else if (get_map_type() == MapType::RANDOM) {
        // set_grid();
    } else {
        FAILED_ASSERT("undefined map");
    }
#endif

    set_walls();
```
