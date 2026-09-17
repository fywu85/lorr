
    env_ = env;
    const auto deadline = Clock::now() + std::chrono::milliseconds(std::max(1, time_limit_ms));
    proposed = env->curr_task_schedule;
    proposed.resize(n_, -1);
    table_budget_ = sched_tables_;
    // Rebuild from the pool so a rejected schedule cannot lose a task permanently.
    free_tasks_.clear();
    for (const auto& entry : env->task_pool) {
        if (entry.second.agent_assigned == -1) free_tasks_.insert(entry.first);
    }
    for (auto it = chain_cost_.begin(); it != chain_cost_.end();) {
        it = free_tasks_.count(it->first) ? std::next(it) : chain_cost_.erase(it);
    }
    std::vector<int> robots;
    for (int i = 0; i < n_; ++i) {
        if (proposed[i] == -1) robots.push_back(i);
    }
    if (robots.empty() || free_tasks_.empty()) return;
    std::vector<int> tasks(free_tasks_.begin(), free_tasks_.end());
    std::sort(tasks.begin(), tasks.end());

    struct Pair {
        double score;
        int cost, task, robot;
    };
    std::vector<Pair> pairs;
    pairs.reserve(std::min<size_t>(robots.size() * tasks.size(), 8u << 20));
    const int now = env->curr_timestep;
    for (int t : tasks) {
        if (Clock::now() > deadline) break;
        const Task& task = env->task_pool.at(t);
        const int first = task.locations.at(task.idx_next_loc);
        const int chain = task_chain_cost(t);
        const double wait = std::max(0, now - task.t_revealed);
        const std::vector<int>* table = oracle_.find(first);
        if (table == nullptr && table_budget_ > 0) {
            --table_budget_;
            table = &oracle_.table(first);
        }
        if (static_cast<long long>(pairs.size()) >= max_pairs_) break;
        for (int r : robots) {
            const int from = env->curr_states[r].location;
            int d;
            if (table) {
                d = (*table)[from];
                if (d >= kInf) {
                    const int exit = cert_.exit_cell[from];
                    d = exit >= 0 && (*table)[exit] < kInf ? cert_.exit_dist[from] + (*table)[exit] : kInf;
                }
            } else {
                d = oracle_.manhattan(from, first);
            }
            if (d >= kInf) continue;
            const int cost = std::max(1, d + chain);
            pairs.push_back({hrrn_ ? 1.0 + wait / cost : 1.0, cost, t, r});
        }
    }
    // Sorting is unbounded work, so shrink an oversized list to its cheapest
    // candidates first (cost is the HRRN denominator, so this keeps the best pairs).
    if (static_cast<long long>(pairs.size()) > max_pairs_) {
        std::nth_element(pairs.begin(), pairs.begin() + max_pairs_, pairs.end(),
                         [](const Pair& a, const Pair& b) { return a.cost < b.cost; });
        pairs.resize(max_pairs_);
    }
    std::sort(pairs.begin(), pairs.end(), [](const Pair& a, const Pair& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.cost != b.cost) return a.cost < b.cost;
        if (a.task != b.task) return a.task < b.task;
        return a.robot < b.robot;
    });

    std::vector<char> robot_used(n_, 0);
    std::unordered_set<int> task_used;
    std::vector<int> by_age = tasks;
    std::sort(by_age.begin(), by_age.end(), [&](int a, int b) {
        const int ra = env->task_pool.at(a).t_revealed, rb = env->task_pool.at(b).t_revealed;
        return ra != rb ? ra < rb : a < b;
    });
    size_t oldest = 0;
    auto assign = [&](const Pair& p) {
        robot_used[p.robot] = 1;
        task_used.insert(p.task);
        proposed[p.robot] = p.task;
        ++stats_.assignments;
    };
    size_t remaining = std::min(robots.size(), tasks.size());
    for (const Pair& p : pairs) {
        if (remaining == 0) break;
        if (regular_admissions_ >= n_) {
            while (oldest < by_age.size() && task_used.count(by_age[oldest])) ++oldest;
            if (oldest < by_age.size()) {
                const int t_old = by_age[oldest];
                for (const Pair& q : pairs) {
                    if (q.task == t_old && !robot_used[q.robot]) {
                        assign(q);
                        ++stats_.fair_assignments;
                        --remaining;
                        break;
                    }
                }
            }
            regular_admissions_ = 0;
            if (remaining == 0) break;
        }
        if (robot_used[p.robot] || task_used.count(p.task)) continue;
        assign(p);
        ++regular_admissions_;
        --remaining;
    }
    for (int t : task_used) free_tasks_.erase(t);
