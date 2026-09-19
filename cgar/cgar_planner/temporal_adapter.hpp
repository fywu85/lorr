#pragma once
#include "cgar.hpp"
#include <atomic>
#include <exception>
#include <memory>
#include <thread>

namespace cgar {
void Cgar::plan_temporal(std::vector<Action>& actions) {
    const int cells = static_cast<int>(cert_.free.size());
    std::vector<char> fixed(n_, false), witness(cells, false);
    for (int cell : txn_cells_) witness[cell] = true;
    for (int i = 0; i < n_; ++i) {
        const auto& a = agents_[i];
        fixed[i] = i == primary_ || a.in_txn || parked_[i] || a.lock >= 0 ||
                   !cert_.core[loc_[i]] || !cert_.core[next_[i]] || witness[loc_[i]] ||
                   (capacity_mode_ && !parking_ready_);
    }
    // Keep every supporting forward move needed by a protected first action.
    // The baseline has already passed independent collision cancellation.
    std::vector<char> pinned = fixed;
    std::vector<int> queue;
    for (int i = 0; i < n_; ++i) if (pinned[i]) queue.push_back(i);
    for (size_t head = 0; head < queue.size(); ++head) {
        const int r = queue[head];
        if (actions[r] != Action::FW) continue;
        const int other = occ_now_[neighbor(loc_[r], ori_[r])];
        if (other >= 0 && !pinned[other]) { pinned[other] = true; queue.push_back(other); }
    }
    std::vector<int> intent_owner(cells, -1);
    for (int i : order_) if (fixed[i] && agents_[i].committed >= 0) {
        const int target = agents_[i].committed;
        if (intent_owner[target] >= 0) throw std::logic_error("duplicate protected temporal intent");
        intent_owner[target] = i;
    }
    std::vector<TemporalPath> seeds(n_);
    std::vector<std::vector<TemporalChoice>> choices(n_);
    std::vector<int> priorities(n_, kInf);
    std::vector<int> heuristic_value(cells * 4), heuristic_stamp(cells * 4, -1);
    for (int i = 0; i < n_; ++i) {
        check_deadline(deadline_, "temporal_candidates");
        const int goal = agents_[i].goal;
        const auto* spatial = goal < 0 ? nullptr : oracle_.peek(goal);
        const auto* oriented = goal < 0 ? nullptr : turn_oracle_.find(goal);
        auto compute_distance = [&](int cell, int direction) {
            if (goal < 0) return 0;
            if (oriented) {
                const int d = turn_oracle_.value(*oriented, cell, direction);
                if (d < kInf) return d;
            }
            const int d = spatial ? oracle_.value(*spatial, cell) : oracle_.manhattan(cell, goal);
            if (d == 0 || d >= kInf) return d;
            int turns = 2;
            for (int dir = 0; dir < 4; ++dir) {
                const int to = neighbor(cell, dir);
                if (to < 0 || !cert_.free[to]) continue;
                const int h = spatial ? oracle_.value(*spatial, to) : oracle_.manhattan(to, goal);
                if (h < d) { const int delta = (dir - direction + 4) % 4; turns = std::min(turns, delta == 3 ? 1 : delta); }
            }
            return d + turn_cost_ * turns;
        };
        auto distance = [&](int cell, int direction) {
            const int state = cell * 4 + direction;
            if (heuristic_stamp[state] != i) {
                heuristic_stamp[state] = i;
                heuristic_value[state] = compute_distance(cell, direction);
            }
            return heuristic_value[state];
        };
        auto cost = [&](const TemporalPath& path, int op) {
            return TemporalGeometry::cost(path, op, goal, turn_cost_, distance);
        };
        priorities[i] = goal < 0 ? kInf : distance(loc_[i], ori_[i]);
        if (temporal_order_ == 2 && goal >= 0) {
            const auto found = env_->task_pool.find(agents_[i].task);
            if (found != env_->task_pool.end()) {
                const auto& task = found->second;
                int from = goal;
                for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k) {
                    const int to = task.locations[k];
                    const auto* table = oracle_.peek(to);
                    const int leg = table ? oracle_.distance_from(*table, from) : oracle_.manhattan(from, to);
                    priorities[i] = static_cast<int>(std::min<int64_t>(kInf - 1, int64_t(priorities[i]) + leg));
                    from = to;
                }
            }
        }
        seeds[i] = temporal_geometry_.seed(loc_[i], ori_[i], pinned[i] ? static_cast<int>(actions[i]) : 3);
        choices[i].push_back({&seeds[i], cost(seeds[i], 0), 0});
        if (pinned[i]) continue;
        const auto& paths = temporal_geometry_.paths(loc_[i], ori_[i]);
        for (int op = 1; op < 129; ++op) {
            const auto& path = paths[op];
            if (!path.valid || (!temporal_steps_ && path.depth > 3)) continue;
            bool valid = true; int from = loc_[i];
            for (int t = 0; t < 5; ++t) {
                const int to = path.cells[t];
                if (!cert_.core[to] || !allowed(i, to) || witness[to] ||
                    (to != from && intent_owner[to] >= 0 && intent_owner[to] != i)) { valid = false; break; }
                from = to;
            }
            if (valid) choices[i].push_back({&path, cost(path, op), op});
        }
        std::sort(choices[i].begin() + 1, choices[i].end(), [](const auto& a, const auto& b) {
            return std::tie(a.cost, a.operation) < std::tie(b.cost, b.operation);
        });
    }
    auto order = order_;
    if (temporal_order_) std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
        return std::tie(priorities[a], a) < std::tie(priorities[b], b);
    });
    std::vector<double> power(n_, 0);
    for (int rank = 0; rank < n_; ++rank) if (agents_[order[rank]].goal >= 0)
        power[order[rank]] = temporal_equal_weight_ ? 1.0 : static_cast<double>(n_ + 1 - rank) / (n_ + 1);
    std::vector<uint64_t> seeds_for_workers(temporal_workers_);
    for (auto& seed : seeds_for_workers) seed = temporal_rng_();
    std::vector<std::unique_ptr<TemporalPibt>> results(temporal_workers_);
    std::vector<std::exception_ptr> errors(temporal_workers_);
    std::atomic<int> next_worker{0};
    auto work = [&] {
        for (;;) {
            const int worker = next_worker.fetch_add(1);
            if (worker >= temporal_workers_) return;
            try {
                auto run = std::make_unique<TemporalPibt>(cells, choices, pinned, power, temporal_budget_, seeds_for_workers[worker]);
                run->construct(order, [&] { check_deadline(deadline_, "temporal_construction"); });
                if (temporal_steps_) run->repair(temporal_steps_, [&] { check_deadline(deadline_, "temporal_repair"); }, temporal_candidate_limit_);
                check_deadline(deadline_, "temporal_worker_complete");
                results[worker] = std::move(run);
            } catch (...) { errors[worker] = std::current_exception(); }
        }
    };
    std::vector<std::thread> threads;
    // The calling thread participates. Independent workers share only immutable
    // geometry, candidates, and scores; each owns all reservations and RNG state.
    try {
        for (int thread = 1; thread < temporal_threads_; ++thread) threads.emplace_back(work);
    } catch (...) {
        for (auto& thread : threads) thread.join();
        throw;
    }
    work();
    for (auto& thread : threads) thread.join();
    for (const auto& error : errors) if (error) std::rethrow_exception(error);
    int best = 0;
    for (int worker = 1; worker < temporal_workers_; ++worker)
        if (results[worker]->score() > results[best]->score()) best = worker;
    TemporalRegionStats region_stats;
    std::unique_ptr<TemporalPibt> regional;
    if (temporal_regions_) regional = repair_temporal_regions(cert_.rows, cert_.cols, loc_, choices, pinned, power,
        temporal_budget_, *results[best], temporal_region_options_, temporal_rng_, region_stats,
        [&] { check_deadline(deadline_, "temporal_region_repair"); });
    auto& search = regional ? *regional : *results[best];
    // Validate the complete temporal result before exposing its first action.
    std::vector<int> owners(cells, -1), previous(n_);
    for (int t = 0; t < 5; ++t) {
        for (int i = 0; i < n_; ++i) {
            const auto& path = *search.choice(i).path;
            const int to = path.cells[t]; previous[i] = t ? path.cells[t - 1] : loc_[i];
            if (to < 0 || to >= cells || !cert_.free[to] || owners[to] >= 0) throw std::logic_error("temporal vertex conflict");
            owners[to] = i;
        }
        for (int i = 0; i < n_; ++i) {
            const int other = owners[previous[i]];
            if (other >= 0 && other != i && previous[other] == search.choice(i).path->cells[t])
                throw std::logic_error("temporal edge swap");
        }
        for (int i = 0; i < n_; ++i) owners[search.choice(i).path->cells[t]] = -1;
    }
    for (int i = 0; i < n_; ++i) {
        if (pinned[i]) continue;
        const auto& path = *search.choice(i).path;
        actions[i] = static_cast<Action>(path.first_action); next_[i] = path.cells[0];
        agents_[i].committed = -1; agents_[i].commit_age = 0;
        if (search.selected(i) == 0 && agents_[i].goal >= 0) {
            // Waiting ordinary robots may orient toward their goal without
            // changing any occupied cell or claiming a future destination.
            const auto* table = turn_oracle_.find(agents_[i].goal);
            if (table) {
                const int right = turn_oracle_.value(*table, loc_[i], (ori_[i] + 1) % 4);
                const int left = turn_oracle_.value(*table, loc_[i], (ori_[i] + 3) % 4);
                const int wait = turn_oracle_.value(*table, loc_[i], ori_[i]);
                const int best = std::min({right, left, wait});
                actions[i] = best == right ? Action::CR : best == left ? Action::CCR : Action::W;
            }
        }
    }
    const auto& construction_stats = results[best]->stats;
    if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
        std::printf("[cgar-temporal] step=%d workers=%d threads=%d selected_worker=%d candidate_limit=%d roots=%lld accepted=%lld recursion=%lld candidates=%lld max_depth=%d exhausted=%lld repairs=%lld repair_accept=%lld score=%.3f\n",
                    env_->curr_timestep + 1, temporal_workers_, temporal_threads_, best, temporal_candidate_limit_, construction_stats.roots, construction_stats.accepted, construction_stats.recursive_calls,
                    construction_stats.candidates, construction_stats.max_depth, construction_stats.budget_exhausted,
                    construction_stats.repairs, construction_stats.repairs_accepted, search.score());
    if (diagnostics_ && temporal_regions_ && (env_->curr_timestep + 1) % 200 == 0)
        std::printf("[cgar-temporal-regions] step=%d regions=%d rounds=%d threads=%d active=%lld candidates=%lld repairs=%lld accepted=%lld score=%.3f\n",
                    env_->curr_timestep + 1, temporal_region_options_.parts, temporal_region_options_.rounds,
                    temporal_region_options_.threads, region_stats.active_robots, region_stats.candidates,
                    region_stats.repairs, region_stats.accepted, search.score());
    check_deadline(deadline_, "temporal_complete");
}
}  // namespace cgar
