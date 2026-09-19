#pragma once
#include "cgar.hpp"
#include <atomic>
#include <exception>
#include <memory>
#include <thread>

namespace cgar {
void Cgar::plan_temporal(std::vector<Action>& actions) {
    const auto candidate_started = Clock::now();
    int exact_metric_robots = 0, fallback_metric_robots = 0;
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
    std::vector<int> priorities(n_, kInf), goals(n_, -1);
    GuideRouteStats guide_stats;
    if (guide_enabled_) {
        std::vector<char> eligible(n_, false);
        std::vector<const std::vector<int>*> tables(n_, nullptr);
        std::vector<const TurnTable*> unit_tables(n_, nullptr);
        for (int r = 0; r < n_; ++r) {
            goals[r] = agents_[r].goal; eligible[r] = !pinned[r];
            if (goals[r] >= 0) {
                tables[r] = oracle_.peek(goals[r]); unit_tables[r] = turn_oracle_.find(goals[r]);
            }
        }
        guide_stats = guide_routes_.update(loc_, ori_, goals, eligible,
            [&](int r, int cell, int orientation) {
                if (unit_tables[r]) {
                    const int d = turn_oracle_.value(*unit_tables[r], cell, orientation);
                    if (d < kInf) return d;
                }
                return tables[r] ? oracle_.value(*tables[r], cell) : oracle_.manhattan(cell, goals[r]);
            },
            [&] { check_deadline(deadline_, "temporal_guide_routes"); });
        stats_.guide_attempts += guide_stats.attempted; stats_.guide_solved += guide_stats.solved;
        stats_.guide_robot_steps += guide_stats.active; stats_.guide_expanded += guide_stats.expanded;
        stats_.guide_reconnections += guide_stats.reconnected;
        stats_.guide_refinements += guide_stats.refined;
    }
    const auto guides_finished = Clock::now();
    std::vector<int> heuristic_value(cells * 4), heuristic_stamp(cells * 4, -1);
    for (int i = 0; i < n_; ++i) {
        check_deadline(deadline_, "temporal_candidates");
        const int goal = agents_[i].goal; goals[i] = goal;
        const auto* spatial = goal < 0 ? nullptr : oracle_.peek(goal);
        const auto* oriented = goal < 0 ? nullptr : turn_oracle_.find(goal);
        if (oriented && turn_oracle_.value(*oriented, loc_[i], ori_[i]) >= kInf) oriented = nullptr;
        const bool guided = guide_enabled_ && guide_routes_.guided(i);
        const int robot_turn_cost = oriented && !guided ? turn_cost_ : 1;
        if (goal >= 0) { if (oriented) ++exact_metric_robots; else ++fallback_metric_robots; }
        auto original_distance = [&](int cell, int direction) {
            if (goal < 0) return 0;
            if (oriented) {
                const int d = turn_oracle_.value(*oriented, cell, direction);
                if (d < kInf) return d;
            }
            return TemporalGeometry::fallback_distance(cell, direction, kInf,
                [&](int u) { return spatial ? oracle_.value(*spatial, u) : oracle_.manhattan(u, goal); },
                [&](int u, int dir) { const int to = neighbor(u, dir); return to >= 0 && cert_.free[to] ? to : -1; });
        };
        auto distance = [&](int cell, int direction) {
            const int state = cell * 4 + direction;
            if (heuristic_stamp[state] != i) {
                heuristic_stamp[state] = i;
                heuristic_value[state] = guided ? guide_routes_.distance(i, cell, direction) : original_distance(cell, direction);
                if (heuristic_value[state] < 0) throw std::logic_error("guide window does not cover a temporal candidate");
            }
            return heuristic_value[state];
        };
        auto cost = [&](const TemporalPath& path, int op) {
            const int extra = oriented && !guided && turn_oracle_.weighted_forward() ?
                TemporalGeometry::forward_surcharge(path, loc_[i], goal, [&](int from, int to) {
                    return turn_oracle_.forward_cost(from, direction(from, to, cert_.cols));
                }) : 0;
            return TemporalGeometry::cost(path, op, goal, robot_turn_cost, distance, temporal_distance_scale_) +
                   int64_t(extra) * temporal_distance_scale_;
        };
        priorities[i] = goal < 0 ? kInf : original_distance(loc_[i], ori_[i]);
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
    TemporalWarmStats warm_stats;
    std::vector<int> initial;
    if (temporal_warm_start_) {
        initial = temporal_history_.selections(env_->curr_timestep, cells, loc_, ori_, goals, choices, pinned,
            warm_stats, [&] { check_deadline(deadline_, "temporal_warm_start"); });
        ++stats_.temporal_warm_calls;
        stats_.temporal_warm_retained += warm_stats.retained;
        stats_.temporal_warm_collision_resets += warm_stats.collision_resets;
    }
    std::vector<uint64_t> seeds_for_workers(temporal_workers_);
    for (auto& seed : seeds_for_workers) seed = temporal_rng_();
    const auto search_started = Clock::now();
    std::vector<std::unique_ptr<TemporalPibt>> results(temporal_workers_);
    std::vector<std::exception_ptr> errors(temporal_workers_);
    std::atomic<int> next_worker{0};
    auto work = [&] {
        for (;;) {
            const int worker = next_worker.fetch_add(1);
            if (worker >= temporal_workers_) return;
            std::unique_ptr<TemporalPibt> run;
            try {
                run = std::make_unique<TemporalPibt>(cells, choices, pinned, power, temporal_budget_, seeds_for_workers[worker],
                    initial.empty() ? nullptr : &initial);
                run->construct(order, [&] { check_deadline(deadline_, "temporal_construction"); });
                if (temporal_steps_) run->repair(temporal_steps_, [&] { check_deadline(deadline_, "temporal_repair"); }, temporal_candidate_limit_);
                check_deadline(deadline_, "temporal_worker_complete");
                results[worker] = std::move(run);
            } catch (const Timeout& failure) {
                if (diagnostics_ && run) {
                    const auto& partial = run->stats;
                    std::fprintf(stderr, "CGAR_TEMPORAL_FAILURE timestep=%d worker=%d stage=%s guide_seconds=%.6f preparation_seconds=%.6f search_seconds=%.6f guided=%d roots=%lld accepted=%lld recursion=%lld candidates=%lld max_depth=%d exhausted=%lld repairs=%lld repair_accept=%lld candidate_limit=%d\n",
                        env_->curr_timestep, worker, failure.what(),
                        std::chrono::duration<double>(guides_finished - candidate_started).count(),
                        std::chrono::duration<double>(search_started - candidate_started).count(),
                        std::chrono::duration<double>(Clock::now() - search_started).count(), guide_stats.active,
                        partial.roots, partial.accepted, partial.recursive_calls, partial.candidates,
                        partial.max_depth, partial.budget_exhausted, partial.repairs, partial.repairs_accepted,
                        temporal_candidate_limit_);
                }
                errors[worker] = std::current_exception();
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
    const auto global_finished = Clock::now();
    TemporalRegionStats region_stats;
    std::unique_ptr<TemporalPibt> regional;
    if (temporal_regions_) regional = repair_temporal_regions(cert_.rows, cert_.cols, loc_, choices, pinned, power,
        temporal_budget_, *results[best], temporal_region_options_, temporal_rng_, region_stats,
        [&] { check_deadline(deadline_, "temporal_region_repair"); });
    const auto regions_finished = Clock::now();
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
            if (guide_enabled_ && guide_routes_.guided(i)) {
                const int right = guide_routes_.distance(i, loc_[i], (ori_[i] + 1) % 4);
                const int left = guide_routes_.distance(i, loc_[i], (ori_[i] + 3) % 4);
                const int wait = guide_routes_.distance(i, loc_[i], ori_[i]);
                if (std::min({right, left, wait}) < 0) throw std::logic_error("missing guide rotation distance");
                const int best = std::min({right, left, wait});
                actions[i] = best == right ? Action::CR : best == left ? Action::CCR : Action::W;
            } else if (table) {
                const int right = turn_oracle_.value(*table, loc_[i], (ori_[i] + 1) % 4);
                const int left = turn_oracle_.value(*table, loc_[i], (ori_[i] + 3) % 4);
                const int wait = turn_oracle_.value(*table, loc_[i], ori_[i]);
                const int best = std::min({right, left, wait});
                actions[i] = best == right ? Action::CR : best == left ? Action::CCR : Action::W;
            }
        }
    }
    if (temporal_warm_start_) {
        std::vector<int> expected_orientation = ori_;
        for (int r = 0; r < n_; ++r) {
            if (actions[r] == Action::CR) expected_orientation[r] = (ori_[r] + 1) % 4;
            else if (actions[r] == Action::CCR) expected_orientation[r] = (ori_[r] + 3) % 4;
        }
        temporal_history_.remember(env_->curr_timestep, search, goals, expected_orientation);
    }
    const auto& construction_stats = results[best]->stats;
    if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
        std::printf("[cgar-temporal] step=%d workers=%d threads=%d selected_worker=%d candidate_limit=%d roots=%lld accepted=%lld recursion=%lld candidates=%lld max_depth=%d exhausted=%lld repairs=%lld repair_accept=%lld score=%.3f\n",
                    env_->curr_timestep + 1, temporal_workers_, temporal_threads_, best, temporal_candidate_limit_, construction_stats.roots, construction_stats.accepted, construction_stats.recursive_calls,
                    construction_stats.candidates, construction_stats.max_depth, construction_stats.budget_exhausted,
                    construction_stats.repairs, construction_stats.repairs_accepted, results[best]->score());
    if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0) {
        auto seconds = [](auto start, auto end) { return std::chrono::duration<double>(end - start).count(); };
        std::printf("[cgar-temporal-timing] step=%d candidates=%.6f global=%.6f regions=%.6f validation=%.6f exact_metric=%d fallback_metric=%d distance_scale=%d\n",
                    env_->curr_timestep + 1, seconds(candidate_started, search_started), seconds(search_started, global_finished),
                    seconds(global_finished, regions_finished), seconds(regions_finished, Clock::now()),
                    exact_metric_robots, fallback_metric_robots, temporal_distance_scale_);
        if (guide_enabled_)
            std::printf("[cgar-temporal-guide] step=%d seconds=%.6f attempted=%d solved=%d limited=%d invalidated=%d windows=%d active=%d expanded=%lld directed_uses=%lld batch=%d expansion_limit=%d lookahead=%d base=%d opposite=%d load=%d heuristic_weight=%d goal_resets=%d protected_resets=%d deviation_resets=%d reconnect_limit=%d reconnect_attempts=%d reconnected=%d reconnect_actions=%lld reconnect_expanded=%lld refine_batch=%d refine_attempted=%d refined=%d refine_limited=%d refine_expanded=%lld refine_cost_saved=%lld\n",
                env_->curr_timestep + 1, seconds(candidate_started, guides_finished), guide_stats.attempted,
                guide_stats.solved, guide_stats.limited, guide_stats.invalidated, guide_stats.windows, guide_stats.active,
                guide_stats.expanded, guide_stats.directed_uses, guide_options_.batch, guide_options_.expansions,
                guide_options_.lookahead, guide_options_.base_cost, guide_options_.opposite_cost, guide_options_.load_cost,
                guide_options_.heuristic_weight, guide_stats.goal_resets, guide_stats.protected_resets, guide_stats.deviation_resets,
                guide_options_.reconnect_steps, guide_stats.reconnect_attempts, guide_stats.reconnected, guide_stats.reconnect_actions, guide_stats.reconnect_expanded,
                guide_options_.refine_batch, guide_stats.refine_attempted, guide_stats.refined, guide_stats.refine_limited,
                guide_stats.refine_expanded, guide_stats.refine_cost_saved);
        if (temporal_warm_start_)
            std::printf("[cgar-temporal-warm] step=%d history=%d retained=%d initial_resets=%d collision_resets=%d\n",
                env_->curr_timestep + 1, int(warm_stats.history_valid), warm_stats.retained,
                warm_stats.initial_resets, warm_stats.collision_resets);
        if (temporal_regions_) {
            std::printf("[cgar-temporal-regions] step=%d regions=%d rounds=%d threads=%d temperature_ppm=%d active=%lld candidates=%lld repairs=%lld attempts_accepted=%lld kept=%lld reverted=%lld frozen_crossers=%lld score_before=%.3f score_after=%.3f\n",
                        env_->curr_timestep + 1, temporal_region_options_.parts, temporal_region_options_.rounds,
                        temporal_region_options_.threads, temporal_region_options_.temperature_ppm, region_stats.active_robots, region_stats.candidates,
                        region_stats.repairs, region_stats.accepted, region_stats.kept_regions, region_stats.reverted_regions,
                        region_stats.frozen_crossers, region_stats.score_before, region_stats.score_after);
            for (size_t round = 0; round < region_stats.round_scores.size(); ++round)
                std::printf("[cgar-temporal-region-round] step=%d round=%zu score_before=%.3f score_after=%.3f\n",
                    env_->curr_timestep + 1, round, round ? region_stats.round_scores[round - 1] : region_stats.score_before,
                    region_stats.round_scores[round]);
        }
    }
    check_deadline(deadline_, "temporal_complete");
}
}  // namespace cgar
