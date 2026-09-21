#pragma once
#include "cgar.hpp"
#include "nlohmann/json.hpp"
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
    prepare_temporal_tables(pinned);
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
    // find() promotes turn-table recency. Resolve those lookups in exactly
    // the original robot order before parallel read-only scoring. No table can
    // be built or evicted while worker-local heuristic scratch is in use.
    std::vector<const std::vector<int>*> prepared_spatial;
    std::vector<const TurnTable*> prepared_oriented;
    if (temporal_prepare_threads_ > 1) {
        prepared_spatial.resize(n_, nullptr); prepared_oriented.resize(n_, nullptr);
        for (int i = 0; i < n_; ++i) {
            if (!(i % 64)) check_deadline(deadline_, "temporal_candidate_tables");
            const int goal = agents_[i].goal;
            if (goal >= 0) {
                prepared_spatial[i] = oracle_.peek(goal); prepared_oriented[i] = turn_oracle_.find(goal);
            }
        }
    }
    // All builds are finished. These optional peeks neither promote recency nor
    // admit tables; references are consumed only during this preparation phase.
    std::vector<int> known_next;
    std::vector<const TurnTable*> prepared_next;
    if (temporal_next_errand_) {
        known_next.assign(n_, -1); prepared_next.assign(n_, nullptr);
        for (int i = 0; i < n_; ++i) {
            if (!(i % 64)) check_deadline(deadline_, "temporal_next_tables");
            if (pinned[i] || agents_[i].goal < 0) continue;
            const auto found = env_->task_pool.find(agents_[i].task);
            if (found == env_->task_pool.end()) continue;
            const auto& task = found->second;
            const int stop = task.idx_next_loc;
            if (stop < 0 || stop >= static_cast<int>(task.locations.size()) - 1 ||
                task.locations[stop] != agents_[i].goal) continue;
            const int next = task.locations[stop + 1];
            if (next < 0 || next >= cells || !cert_.free[next] || next == agents_[i].goal) continue;
            known_next[i] = next; prepared_next[i] = turn_oracle_.peek(next);
        }
    }
    std::vector<std::array<long long, 5>> next_metrics(temporal_prepare_threads_, {0, 0, 0, 0, 0});
    std::vector<std::array<long long, 6>> chain_metrics(temporal_chain_mode_ ? temporal_prepare_threads_ : 0, {0, 0, 0, 0, 0, 0});
    std::vector<std::array<long long, 2>> native_service_metrics(native_neutral_tail_ ? temporal_prepare_threads_ : 0, {0, 0});
    std::vector<std::array<int, 2>> prepared_metrics(temporal_prepare_threads_, {0, 0});
    run_temporal_preparation(temporal_prepare_threads_, [&](int worker) {
        std::vector<int> heuristic_value(cells * 4), heuristic_stamp(cells * 4, -1);
        const int begin = int(int64_t(n_) * worker / temporal_prepare_threads_);
        const int end = int(int64_t(n_) * (worker + 1) / temporal_prepare_threads_);
        for (int i = begin; i < end; ++i) {
            check_deadline(deadline_, "temporal_candidates");
            const int goal = agents_[i].goal; goals[i] = goal;
            const auto* spatial = goal < 0 ? nullptr : (temporal_prepare_threads_ == 1 ? oracle_.peek(goal) : prepared_spatial[i]);
            const auto* oriented = goal < 0 ? nullptr : (temporal_prepare_threads_ == 1 ? turn_oracle_.find(goal) : prepared_oriented[i]);
            if (oriented && turn_oracle_.value(*oriented, loc_[i], ori_[i]) >= kInf) oriented = nullptr;
            const bool guided = guide_enabled_ && guide_routes_.guided(i);
            const int robot_turn_cost = oriented && !guided ? guidance_turn_cost_ : flow_cost_scale_;
            if (goal >= 0) { if (oriented) ++prepared_metrics[worker][0]; else ++prepared_metrics[worker][1]; }
            auto original_distance = [&](int cell, int direction) {
                if (goal < 0) return 0;
                if (oriented) {
                    const int d = turn_oracle_.value(*oriented, cell, direction);
                    if (d < kInf) return d;
                }
                const int d = TemporalGeometry::fallback_distance(cell, direction, kInf,
                    [&](int u) { return spatial ? oracle_.value(*spatial, u) : oracle_.manhattan(u, goal); },
                    [&](int u, int dir) { const int to = neighbor(u, dir); return to >= 0 && cert_.free[to] ? to : -1; });
                return d >= kInf ? kInf : int(std::min<int64_t>(kInf - 1, int64_t(d) * flow_cost_scale_));
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
            ChainPotential::Chain chain;
            bool chain_available = false;
            int64_t chain_initial = ChainPotential::infinity;
            if (temporal_chain_mode_ && !pinned[i] && goal >= 0) {
                const auto task = env_->task_pool.find(agents_[i].task);
                auto check = [&] { check_deadline(deadline_, "temporal_chain_prepare"); };
                if (task != env_->task_pool.end() && task->second.idx_next_loc >= 0 &&
                    task->second.idx_next_loc < static_cast<int>(task->second.locations.size()) &&
                    task->second.locations[task->second.idx_next_loc] == goal)
                    chain = chain_potential_.make_chain(task->second.locations, task->second.idx_next_loc, check);
                else chain = chain_potential_.make_chain(std::vector<int>{goal}, 0, check);
                chain_initial = chain_potential_.value(chain, 0, loc_[i], ori_[i]);
                chain_available = chain_initial < ChainPotential::infinity;
                ++chain_metrics[worker][chain_available ? 0 : 1];
            }
            const TurnTable* next_table = nullptr;
            int next_baseline = kInf;
            bool use_next = false;
            std::array<char, 129> arrives{};
            if (temporal_next_errand_ && known_next[i] >= 0) {
                ++next_metrics[worker][0]; next_table = prepared_next[i];
                use_next = oriented && !guided && next_table;
                if (use_next) {
                    for (int h = 0; h < 4; ++h) {
                        const int d = turn_oracle_.value(*next_table, goal, h);
                        if (d >= kInf) use_next = false;
                        next_baseline = std::min(next_baseline, d);
                    }
                    if (use_next && oracle_.manhattan(loc_[i], goal) <= 5) {
                        const auto& paths = temporal_geometry_.paths(loc_[i], ori_[i]);
                        // Decide once for the whole set. No candidate switches
                        // to another metric because its own endpoint is missing.
                        for (int op = 0; op < 129; ++op) if (paths[op].valid && TemporalGeometry::first_goal_hit(paths[op], goal) >= 0) {
                            arrives[op] = true;
                            for (int h = 0; h < 4; ++h)
                                if (turn_oracle_.value(*next_table, paths[op].cells[4], h) >= kInf) use_next = false;
                        }
                    }
                }
                ++next_metrics[worker][use_next ? 1 : 2];
            }
            auto cost = [&](const TemporalPath& path, int op) {
                if ((temporal_chain_mode_ & 1) && chain_available) {
                    const size_t served = ChainPotential::advance(chain, path);
                    const int64_t remaining = chain_potential_.value(chain, served, path.cells[4], path.orientation);
                    if (remaining >= ChainPotential::infinity)
                        throw std::logic_error("finite task chain became unreachable after a valid temporal path");
                    ++chain_metrics[worker][2];
                    chain_metrics[worker][3] += served > 0;
                    chain_metrics[worker][4] += served > 1;
                    chain_metrics[worker][5] += served == chain.goals.size();
                    return remaining * temporal_distance_scale_ - int64_t(op) * (native_trick_metric_ ? 1 : flow_cost_scale_);
                }
                if (native_trick_metric_) {
                    if (native_neutral_tail_ && TemporalGeometry::first_goal_hit(path, goal) >= 0) {
                        ++native_service_metrics[worker][0];
                        native_service_metrics[worker][1] += path.cells[4] != goal;
                    }
                    return TemporalGeometry::pure_potential_cost(path, op, goal, distance, native_neutral_tail_);
                }
                const auto native = oriented && !guided && turn_oracle_.weighted_forward() ?
                    TemporalGeometry::flow_cost(path, op, loc_[i], goal, robot_turn_cost, distance,
                        [&](int from, int to) { return turn_oracle_.forward_cost(from, direction(from, to, cert_.cols)); },
                        temporal_distance_scale_, flow_cost_scale_, temporal_remaining_flow_) :
                    TemporalGeometry::cost(path, op, goal, robot_turn_cost, distance, temporal_distance_scale_, flow_cost_scale_);
                if (use_next && arrives[op]) {
                    ++next_metrics[worker][3];
                    const auto continued = TemporalGeometry::next_errand_cost(path, op, loc_[i], robot_turn_cost, next_baseline,
                        [&](int cell, int h) { return turn_oracle_.value(*next_table, cell, h); },
                        [&](int from, int to) { return turn_oracle_.forward_cost(from, direction(from, to, cert_.cols)); },
                        temporal_distance_scale_, flow_cost_scale_);
                    next_metrics[worker][4] += continued != native;
                    return continued;
                }
                return native;
            };
            priorities[i] = goal < 0 ? kInf : original_distance(loc_[i], ori_[i]);
            if (temporal_order_ == 2 && goal >= 0) {
                const auto& tasks = env_->task_pool;
                const auto found = tasks.find(agents_[i].task);
                if (found != tasks.end()) {
                    const auto& task = found->second;
                    int from = goal;
                    for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k) {
                        const int to = task.locations[k];
                        const auto* table = oracle_.peek(to);
                        const int leg = table ? oracle_.distance_from(*table, from) : oracle_.manhattan(from, to);
                        priorities[i] = static_cast<int>(std::min<int64_t>(kInf - 1, int64_t(priorities[i]) + int64_t(leg) * flow_cost_scale_));
                        from = to;
                    }
                }
            }
            if ((temporal_chain_mode_ & 2) && chain_available)
                priorities[i] = static_cast<int>(std::min<int64_t>(kInf - 1, chain_initial));
            choices[i].reserve(pinned[i] ? 1 : 129);
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
    });
    for (const auto& count : prepared_metrics) {
        exact_metric_robots += count[0]; fallback_metric_robots += count[1];
    }
    for (const auto& count : chain_metrics) {
        stats_.chain_robot_steps += count[0]; stats_.chain_fallback_steps += count[1];
        stats_.chain_scored_choices += count[2]; stats_.chain_service_choices += count[3];
        stats_.chain_multi_service_choices += count[4]; stats_.chain_completed_choices += count[5];
    }
    for (const auto& count : next_metrics) {
        stats_.temporal_next_known += count[0]; stats_.temporal_next_eligible += count[1];
        stats_.temporal_next_unavailable += count[2]; stats_.temporal_next_arriving_choices += count[3];
        stats_.temporal_next_changed_choices += count[4];
    }
    for (const auto& count : native_service_metrics) {
        stats_.native_service_choices += count[0];
        stats_.native_service_changed_choices += count[1];
    }
    stats_.temporal_prepared_robots += n_;
    if (temporal_prepare_threads_ > 1) ++stats_.temporal_parallel_preparations;
    auto order = order_;
    if (temporal_order_) std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
        return std::tie(priorities[a], a) < std::tie(priorities[b], b);
    });
    std::vector<double> power(n_, 0);
    for (int rank = 0; rank < n_; ++rank) if (agents_[order[rank]].goal >= 0) {
        double weight = temporal_equal_weight_ ? 1.0 : static_cast<double>(n_ + 1 - rank) / (n_ + 1);
        // Explicit NMS-derived instance tuning. Use the same objective for all
        // worker proposals and repairs; protected seeds remain fixed separately.
        if (temporal_rank_squared_) {
            stats_.temporal_rank_squared_robots += weight > 0 && weight < 1;
            weight *= weight;
        }
        power[order[rank]] = weight;
    }
    TemporalWarmStats warm_stats, promise_stats;
    std::vector<int> initial, promised_first;
    if (temporal_warm_start_) {
        initial = temporal_history_.selections(env_->curr_timestep, cells, loc_, ori_, goals, choices, pinned,
            warm_stats, [&] { check_deadline(deadline_, "temporal_warm_start"); });
        ++stats_.temporal_warm_calls;
        stats_.temporal_warm_retained += warm_stats.retained;
        stats_.temporal_warm_collision_resets += warm_stats.collision_resets;
    }
    if (temporal_promise_after_turn_) {
        auto check = [&] { check_deadline(deadline_, "temporal_after_turn_promise"); };
        initial = temporal_history_.selections(env_->curr_timestep, cells, loc_, ori_, goals, choices, pinned,
            promise_stats, check, true);
        promised_first = TemporalWarmStart::constrain_first_actions(choices, initial, pinned, check);
        ++stats_.temporal_promise_calls;
        stats_.temporal_promise_retained += promise_stats.retained;
        stats_.temporal_promise_collision_resets += promise_stats.collision_resets;
    }
    std::vector<uint64_t> seeds_for_workers(temporal_workers_);
    for (auto& seed : seeds_for_workers) seed = temporal_rng_();
    TemporalPriorityBatch priority_batch;
    if (temporal_priority_noise_) priority_batch = temporal_priority_portfolio_.prepare(
        priorities, order, goals, temporal_workers_, flow_cost_scale_, env_->curr_timestep,
        [&] { check_deadline(deadline_, "temporal_priority_portfolio"); });
    const auto search_started = Clock::now();
    std::vector<std::unique_ptr<TemporalPibt>> results(temporal_workers_);
    std::vector<std::exception_ptr> errors(temporal_workers_);
    std::vector<char> warm_started(temporal_workers_, false);
    std::atomic<int> next_worker{0};
    auto work = [&] {
        for (;;) {
            const int worker = next_worker.fetch_add(1);
            if (worker >= temporal_workers_) return;
            std::unique_ptr<TemporalPibt> run;
            try {
                // All alternatives finish. In mixed mode only worker0 reuses
                // the previous complete suffix; exact-score ties retain it.
                const auto* worker_initial = !initial.empty() && (!temporal_mixed_start_ || worker == 0) ? &initial : nullptr;
                warm_started[worker] = temporal_warm_start_ && worker_initial != nullptr;
                run = std::make_unique<TemporalPibt>(cells, choices, pinned, power, temporal_budget_, seeds_for_workers[worker], worker_initial);
                run->construct(temporal_priority_noise_ ? priority_batch.orders[worker] : order,
                    [&] { check_deadline(deadline_, "temporal_construction"); });
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
    int warm_workers = 0;
    for (char warm : warm_started) warm_workers += warm;
    stats_.temporal_warm_worker_runs += warm_workers;
    stats_.temporal_cold_worker_runs += temporal_workers_ - warm_workers;
    stats_.temporal_selected_warm_runs += warm_started[best];
    const auto global_finished = Clock::now();
    TemporalRegionStats region_stats;
    std::unique_ptr<TemporalPibt> regional;
    if (temporal_regions_) regional = repair_temporal_regions(cert_.rows, cert_.cols, loc_, choices, pinned, power,
        temporal_budget_, *results[best], temporal_region_options_, temporal_rng_, region_stats,
        [&] { check_deadline(deadline_, "temporal_region_repair"); });
    if (temporal_region_options_.audit_peaks) stats_.regional_peaks.merge(region_stats.peaks);
    stats_.regional_peaks_restored += region_stats.peaks_restored;
    const auto regions_finished = Clock::now();
    TemporalTransactionStats transaction_stats;
    auto transaction = repair_temporal_transactions(cells, choices, pinned, power, temporal_budget_,
        regional ? *regional : *results[best], temporal_transaction_options_, order, env_->curr_timestep,
        transaction_stats, [&] { check_deadline(deadline_, "temporal_branch_transaction"); });
    if (temporal_transaction_options_.work) {
        ++stats_.temporal_transaction_calls;
        stats_.temporal_transaction_roots += transaction_stats.roots;
        stats_.temporal_transaction_candidates += transaction_stats.candidates;
        stats_.temporal_transaction_accepted += transaction_stats.accepted;
        stats_.temporal_transaction_rollbacks += transaction_stats.rollbacks;
        stats_.temporal_transaction_exhausted += transaction_stats.work_exhausted;
        if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
            std::printf("[cgar-temporal-transaction] step=%d owners=%d work=%d roots=%lld candidates=%lld accepted=%lld terminals=%lld rollbacks=%lld exhausted=%lld multi_owner=%lld max_agents=%d score_before=%.3f score_after=%.3f seconds=%.6f\n",
                env_->curr_timestep + 1, temporal_transaction_options_.max_owners, temporal_transaction_options_.work,
                transaction_stats.roots, transaction_stats.candidates, transaction_stats.accepted, transaction_stats.terminals,
                transaction_stats.rollbacks, transaction_stats.work_exhausted, transaction_stats.multi_owner_choices,
                transaction_stats.max_agents, transaction_stats.score_before, transaction_stats.score_after,
                std::chrono::duration<double>(Clock::now() - regions_finished).count());
    }
    auto& search = transaction ? *transaction : (regional ? *regional : *results[best]);
    // A promise constrains only an ordinary first action. Search and regional
    // repair may change its tail but must never return to the unconstrained seed.
    for (size_t i = 0; i < promised_first.size(); ++i) if (promised_first[i] >= 0 &&
        (pinned[i] || search.selected(i) == 0 || search.choice(i).path->first_action != promised_first[i]))
        throw std::logic_error("temporal search violated an after-turn promise");
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
        if (pinned[i]) {
            stats_.temporal_protected_rotations += actions[i] == Action::CR || actions[i] == Action::CCR;
            continue;
        }
        const auto& path = *search.choice(i).path;
        actions[i] = static_cast<Action>(path.first_action); next_[i] = path.cells[0];
        agents_[i].committed = -1; agents_[i].commit_age = 0;
        if (search.selected(i) != 0)
            stats_.temporal_planned_rotations += actions[i] == Action::CR || actions[i] == Action::CCR;
        else ++stats_.temporal_wait_seeds;
        if (search.selected(i) == 0 && agents_[i].goal >= 0) {
            auto orient_wait = [&](int wait, int right, int left) {
                const int chosen = TemporalGeometry::wait_action(wait, right, left, temporal_strict_wait_turns_);
                actions[i] = static_cast<Action>(chosen);
                if (chosen != 3) {
                    ++stats_.temporal_seed_rotations;
                    stats_.temporal_tied_seed_rotations += (chosen == 1 ? right : left) == wait;
                }
            };
            // Waiting ordinary robots may orient toward their goal without
            // changing any occupied cell or claiming a future destination.
            const auto* table = turn_oracle_.find(agents_[i].goal);
            if (guide_enabled_ && guide_routes_.guided(i)) {
                const int right = guide_routes_.distance(i, loc_[i], (ori_[i] + 1) % 4);
                const int left = guide_routes_.distance(i, loc_[i], (ori_[i] + 3) % 4);
                const int wait = guide_routes_.distance(i, loc_[i], ori_[i]);
                if (std::min({right, left, wait}) < 0) throw std::logic_error("missing guide rotation distance");
                orient_wait(wait, right, left);
            } else if (table) {
                const int right = turn_oracle_.value(*table, loc_[i], (ori_[i] + 1) % 4);
                const int left = turn_oracle_.value(*table, loc_[i], (ori_[i] + 3) % 4);
                const int wait = turn_oracle_.value(*table, loc_[i], ori_[i]);
                orient_wait(wait, right, left);
            }
        }
    }
    if (window_options_.horizon) {
        const auto started = Clock::now();
        auto check = [&] { check_deadline(deadline_, "rolling_window"); };
        WindowProblem problem;
        problem.rows = cert_.rows; problem.cols = cert_.cols; problem.horizon = window_options_.horizon;
        problem.turn_cost = guidance_turn_cost_;
        problem.wait_cost = window_options_.wait_cost ? window_options_.wait_cost : flow_cost_scale_;
        problem.oracle = &chain_potential_; problem.free = cert_.free; problem.fixed = pinned;
        problem.locked_prefix.assign(n_, 0);
        problem.forward.resize(cells);
        for (int u = 0; u < cells; ++u) for (int d = 0; d < 4; ++d)
            problem.forward[u][d] = turn_oracle_.forward_cost(u, d);
        problem.allowed.resize(n_); problem.entry_allowed.resize(n_); problem.chains.resize(n_); problem.tasks.resize(n_); problem.seed.resize(n_);
        for (int r = 0; r < n_; ++r) {
            check(); problem.tasks[r] = agents_[r].task;
            // CGAR still dictates the physical primary/support action. Only its
            // ordinary future forecast may be repaired; active evacuation waves,
            // witness cells and parked/capacity bootstrap paths remain frozen.
            if (window_options_.protected_prefix && pinned[r] && !parked_[r] && !agents_[r].in_txn &&
                !witness[loc_[r]] && next_[r] >= 0 && next_[r] < cells && !witness[next_[r]] && (!capacity_mode_ || parking_ready_)) {
                problem.fixed[r] = false; problem.locked_prefix[r] = 1;
            }
            std::vector<int> stops;
            const auto found = env_->task_pool.find(agents_[r].task);
            if (agents_[r].goal >= 0) {
                if (found != env_->task_pool.end() && found->second.idx_next_loc >= 0 &&
                    found->second.idx_next_loc < int(found->second.locations.size()) &&
                    found->second.locations[found->second.idx_next_loc] == agents_[r].goal)
                    stops.assign(found->second.locations.begin() + found->second.idx_next_loc, found->second.locations.end());
                else stops.push_back(agents_[r].goal);
            }
            problem.chains[r] = chain_potential_.make_chain(stops, 0, check);
            if (chain_potential_.value(problem.chains[r], 0, loc_[r], ori_[r]) >= ChainPotential::infinity)
                problem.fixed[r] = true;
            problem.allowed[r].assign(cells, false); problem.entry_allowed[r].assign(cells, false);
            for (int u = 0; u < cells; ++u) {
                problem.allowed[r][u] = (cert_.core[u] || problem.locked_prefix[r]) && allowed(r, u) && !witness[u];
                problem.entry_allowed[r][u] = problem.allowed[r][u] && (intent_owner[u] < 0 || intent_owner[u] == r);
            }
            auto& path = problem.seed[r]; path.reserve(problem.horizon + 1); path.push_back(loc_[r] * 4 + ori_[r]);
            const auto& choice = search.choice(r);
            for (int t = 0; t < problem.horizon; ++t) {
                const int action = t == 0 ? int(actions[r]) : t < 5 ? TemporalGeometry::operations()[choice.operation][t] : 3;
                const int state = problem.next(path.back(), action);
                if (state < 0 || (t < 5 && state / 4 != choice.path->cells[t]))
                    throw std::logic_error("CGAR rolling-window seed conversion changed occupied cells");
                path.push_back(state);
            }
        }
        const int rollout_batches = window_options_.seed_rollout ?
            extend_window_seed(problem, temporal_geometry_, order, temporal_budget_, window_rng_(), check) : 0;
        std::vector<uint64_t> seeds(window_options_.workers);
        for (auto& seed : seeds) seed = window_rng_();
        WindowStats window;
        const auto paths = rolling_window_.solve(problem, window_options_, env_->curr_timestep, seeds, window, check);
        if (!window.completed) throw std::logic_error("CGAR exposed an incomplete rolling window");
        for (int r = 0; r < n_; ++r) {
            const Action selected = static_cast<Action>(problem.action(paths[r][0], paths[r][1]));
            if ((pinned[r] || problem.fixed[r]) && selected != actions[r]) throw std::logic_error("rolling window changed a protected first action");
            if (!pinned[r] && !problem.fixed[r]) { actions[r] = selected; next_[r] = paths[r][1] / 4; }
        }
        ++stats_.window_calls; stats_.window.merge(window);
        stats_.window_changed_first += window.changed_first;
        stats_.window_retained += window.retained; stats_.window_history_resets += window.history_resets;
        if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
            std::printf("[cgar-window] step=%d complete=1 rollout_batches=%d attempts=%lld accepted=%lld improved=%lld searches=%lld expanded=%lld capped=%lld failed=%lld retained=%lld history_resets=%lld seed_cost=%lld initial_cost=%lld final_cost=%lld seed_remaining=%lld initial_remaining=%lld final_remaining=%lld changed_first=%d protected=%d selected_worker=%d calls=%lld total_attempts=%lld total_changed_first=%lld total_retained=%lld total_history_resets=%lld seconds=%.6f\n",
                env_->curr_timestep + 1, rollout_batches, window.attempts, window.accepted, window.improved, window.searches,
                window.expanded, window.capped, window.failed, window.retained, window.history_resets,
                static_cast<long long>(window.seed_cost), static_cast<long long>(window.initial_cost), static_cast<long long>(window.final_cost),
                static_cast<long long>(window.seed_remaining), static_cast<long long>(window.initial_remaining), static_cast<long long>(window.final_remaining),
                window.changed_first, window.protected_robots, window.selected_worker, stats_.window_calls,
                stats_.window.attempts, stats_.window_changed_first, stats_.window_retained, stats_.window_history_resets,
                std::chrono::duration<double>(Clock::now() - started).count());
    }
    if (temporal_priority_noise_) {
        temporal_priority_portfolio_.remember(priority_batch, best, env_->curr_timestep);
        if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
            std::printf("[cgar-priority-portfolio] step=%d workers=%d noise=%d changed_orders=%d reused=%d selected_worker=%d offsets_fnv1a64=%llu\n",
                env_->curr_timestep + 1, temporal_workers_, temporal_priority_noise_, priority_batch.changed_orders,
                priority_batch.reused, best, static_cast<unsigned long long>(TemporalPriorityPortfolio::fingerprint(priority_batch.offsets[best])));
    }
    if (temporal_warm_start_ || temporal_promise_after_turn_) {
        std::vector<int> expected_orientation = ori_;
        for (int r = 0; r < n_; ++r) {
            if (actions[r] == Action::CR) expected_orientation[r] = (ori_[r] + 1) % 4;
            else if (actions[r] == Action::CCR) expected_orientation[r] = (ori_[r] + 3) % 4;
        }
        temporal_history_.remember(env_->curr_timestep, search, goals, expected_orientation);
    }
    if (temporal_conflict_audit_stride_ && (env_->curr_timestep + 1) % temporal_conflict_audit_stride_ == 0) {
        const auto audit = search.audit_forward_blockers(temporal_distance_scale_, flow_cost_scale_);
        ++stats_.temporal_conflict_audits;
        if (diagnostics_) std::printf("[cgar-temporal-forward-audit] step=%d stationary=%d no_lower_forward=%d unblocked=%d one_movable=%d two_movable=%d many_movable=%d protected_blocker=%d\n",
            env_->curr_timestep + 1, audit.stationary, audit.no_lower_forward, audit.unblocked,
            audit.one_movable, audit.two_movable, audit.many_movable, audit.protected_blocker);
        auto log_partition = [&](const char* basis, const TemporalForwardPartition& part) {
            if (diagnostics_) std::printf("[cgar-temporal-forward-options] step=%d basis=%s stationary=%d no_lower_forward=%d unblocked=%d one_movable=%d two_movable=%d many_movable=%d protected_blocker=%d distance_scale=%d unit_cost=%d\n",
                env_->curr_timestep + 1, basis, audit.stationary, part.no_lower_forward, part.unblocked,
                part.one_movable, part.two_movable, part.many_movable, part.protected_blocker,
                temporal_distance_scale_, flow_cost_scale_);
        };
        log_partition("scalar", audit.easiest); log_partition("physical", audit.physical);
        if (temporal_group_snapshot_count_) {
            const auto groups = search.snapshot_forward_groups(temporal_group_snapshot_count_,
                (env_->curr_timestep + 1) / temporal_conflict_audit_stride_ - 1,
                temporal_distance_scale_, flow_cost_scale_, [&] { check_deadline(deadline_, "temporal_group_snapshot"); });
            for (const auto& group : groups) {
                nlohmann::json record = {{"schema", 1}, {"step", env_->curr_timestep + 1},
                    {"distance_scale", temporal_distance_scale_}, {"unit_cost", flow_cost_scale_},
                    {"root", group.root}, {"trigger", group.trigger}, {"robots", nlohmann::json::array()}};
                for (const auto& robot : group.robots) {
                    nlohmann::json row = {{"robot", robot.robot}, {"selected", robot.selected},
                        {"power", robot.power}, {"fixed", robot.fixed}, {"start", loc_[robot.robot]},
                        {"orientation", ori_[robot.robot]}, {"goal", goals[robot.robot]},
                        {"emitted_action", static_cast<int>(actions[robot.robot])}, {"choices", nlohmann::json::array()}};
                    for (const auto& candidate : robot.choices) {
                        const auto& path = candidate.path;
                        row["choices"].push_back({{"cost", candidate.cost}, {"operation", candidate.operation},
                            {"cells", path.cells}, {"edges", path.edges}, {"orientation", path.orientation},
                            {"first_action", path.first_action}, {"depth", path.depth}, {"valid", path.valid},
                            {"cell_owners", candidate.cell_owners}, {"edge_owners", candidate.edge_owners}});
                    }
                    record["robots"].push_back(std::move(row));
                }
                std::printf("[cgar-temporal-group] %s\n", record.dump().c_str());
            }
            std::printf("[cgar-temporal-group-frame] step=%d groups=%zu count_per_class=%d\n",
                env_->curr_timestep + 1, groups.size(), temporal_group_snapshot_count_);
            check_deadline(deadline_, "temporal_group_snapshot_complete");
        }
    }
    if (temporal_service_audit_stride_ && (env_->curr_timestep + 1) % temporal_service_audit_stride_ == 0) {
        std::vector<char> known_next(n_, false);
        for (int r = 0; r < n_; ++r) {
            const auto task = env_->task_pool.find(agents_[r].task);
            if (task != env_->task_pool.end())
                known_next[r] = task->second.idx_next_loc + 1 < static_cast<int>(task->second.locations.size());
        }
        const auto audit = search.audit_post_service(goals, known_next, temporal_distance_scale_, flow_cost_scale_,
            [&] { check_deadline(deadline_, "temporal_service_audit"); });
        if (diagnostics_) {
            std::printf("[cgar-temporal-service-audit] step=%d eligible=%d no_improving=%d unblocked=%d other_blocker=%d known_only=%d finishing_only=%d mixed=%d arriving=%d arriving_known_next=%d distance_scale=%d unit_cost=%d\n",
                env_->curr_timestep + 1, audit.eligible, audit.no_improving, audit.unblocked, audit.other_blocker,
                audit.post_service[0].robots, audit.post_service[1].robots, audit.post_service[2].robots,
                audit.arriving, audit.arriving_known_next, temporal_distance_scale_, flow_cost_scale_);
            const char* names[] = {"known", "finishing", "mixed"};
            for (int k = 0; k < 3; ++k) {
                const auto& part = audit.post_service[k];
                std::printf("[cgar-temporal-service-options] step=%d owners=%s robots=%d selected_wait=%d selected_turn=%d selected_forward=%d candidate_forward=%d physical_gain=%lld\n",
                    env_->curr_timestep + 1, names[k], part.robots, part.selected_wait, part.selected_turn,
                    part.selected_forward, part.candidate_forward, static_cast<long long>(part.physical_gain));
            }
        }
    }
    const auto& construction_stats = results[best]->stats;
    if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
        std::printf("[cgar-temporal] step=%d workers=%d threads=%d selected_worker=%d candidate_limit=%d roots=%lld accepted=%lld recursion=%lld candidates=%lld max_depth=%d exhausted=%lld repairs=%lld repair_accept=%lld score=%.3f\n",
                    env_->curr_timestep + 1, temporal_workers_, temporal_threads_, best, temporal_candidate_limit_, construction_stats.roots, construction_stats.accepted, construction_stats.recursive_calls,
                    construction_stats.candidates, construction_stats.max_depth, construction_stats.budget_exhausted,
                    construction_stats.repairs, construction_stats.repairs_accepted, results[best]->score());
    if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0) {
        auto seconds = [](auto start, auto end) { return std::chrono::duration<double>(end - start).count(); };
        if (native_neutral_tail_)
            std::printf("[cgar-native-service] step=%d served_choices=%lld changed_choices=%lld\n",
                env_->curr_timestep + 1, stats_.native_service_choices, stats_.native_service_changed_choices);
        if (temporal_chain_mode_)
            std::printf("[cgar-chain] step=%d mode=%d robots=%lld fallback=%lld choices=%lld service=%lld multi_service=%lld completed=%lld\n",
                env_->curr_timestep + 1, temporal_chain_mode_, stats_.chain_robot_steps, stats_.chain_fallback_steps,
                stats_.chain_scored_choices, stats_.chain_service_choices, stats_.chain_multi_service_choices, stats_.chain_completed_choices);
        if (temporal_next_errand_)
            std::printf("[cgar-temporal-next-errand] step=%d enabled=1 known=%lld eligible=%lld unavailable=%lld arriving_choices=%lld changed_choices=%lld\n",
                env_->curr_timestep + 1, stats_.temporal_next_known, stats_.temporal_next_eligible,
                stats_.temporal_next_unavailable, stats_.temporal_next_arriving_choices, stats_.temporal_next_changed_choices);
        std::printf("[cgar-temporal-starts] step=%d mixed=%d warm=%d cold=%d selected_warm=%d warm_runs=%lld cold_runs=%lld unit_cost=%d turn_cost=%d turn_surcharge=%d\n",
                    env_->curr_timestep + 1, int(temporal_mixed_start_), warm_workers, temporal_workers_ - warm_workers,
                    int(warm_started[best]), stats_.temporal_warm_worker_runs, stats_.temporal_cold_worker_runs,
                    flow_cost_scale_, guidance_turn_cost_, turn_surcharge_);
        std::printf("[cgar-temporal-timing] step=%d candidates=%.6f global=%.6f regions=%.6f validation=%.6f exact_metric=%d fallback_metric=%d distance_scale=%d\n",
                    env_->curr_timestep + 1, seconds(candidate_started, search_started), seconds(search_started, global_finished),
                    seconds(global_finished, regions_finished), seconds(regions_finished, Clock::now()),
                    exact_metric_robots, fallback_metric_robots, temporal_distance_scale_);
        std::printf("[cgar-temporal-rotation] steps=%d strict_wait_turns=%d wait_seeds=%lld seed_rotations=%lld tied_seed_rotations=%lld planned_rotations=%lld protected_rotations=%lld\n",
                    env_->curr_timestep + 1, int(temporal_strict_wait_turns_), stats_.temporal_wait_seeds,
                    stats_.temporal_seed_rotations, stats_.temporal_tied_seed_rotations,
                    stats_.temporal_planned_rotations, stats_.temporal_protected_rotations);
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
        if (temporal_rank_squared_)
            std::printf("[cgar-trick-rank] step=%d squared=1 changed_total=%lld\n",
                env_->curr_timestep + 1, stats_.temporal_rank_squared_robots);
        if (temporal_promise_after_turn_)
            std::printf("[cgar-temporal-promise] step=%d after_turn=1 history=%d retained=%d initial_resets=%d collision_resets=%d calls=%lld retained_total=%lld collision_resets_total=%lld\n",
                env_->curr_timestep + 1, int(promise_stats.history_valid), promise_stats.retained,
                promise_stats.initial_resets, promise_stats.collision_resets, stats_.temporal_promise_calls,
                stats_.temporal_promise_retained, stats_.temporal_promise_collision_resets);
        if (temporal_warm_start_)
            std::printf("[cgar-temporal-warm] step=%d history=%d retained=%d initial_resets=%d collision_resets=%d\n",
                env_->curr_timestep + 1, int(warm_stats.history_valid), warm_stats.retained,
                warm_stats.initial_resets, warm_stats.collision_resets);
        if (temporal_regions_) {
            if (temporal_region_options_.keep_peak)
                std::printf("[cgar-regional-keep-peak] step=%d restored_batches=%lld total_restored_batches=%lld\n",
                    env_->curr_timestep + 1, region_stats.peaks_restored, stats_.regional_peaks_restored);
            if (temporal_region_options_.audit_peaks) {
                const auto& p = stats_.regional_peaks;
                std::printf("[cgar-regional-peaks] step=%d batches=%lld attempts=%lld peak_updates=%lld lost_peaks=%lld lost_improvements=%lld peak_gain=%.6f final_gain=%.6f discarded_gain=%.6f peak_attempt_sum=%lld max_peak_attempt=%d\n",
                    env_->curr_timestep + 1, p.batches, p.attempts, p.peak_updates, p.lost_peaks,
                    p.lost_improvements, p.peak_gain, p.final_gain, p.discarded_gain, p.peak_attempt_sum, p.max_peak_attempt);
            }
            std::printf("[cgar-temporal-regions] step=%d regions=%d rounds=%d threads=%d temperature_ppm=%d active=%lld candidates=%lld repairs=%lld attempts_accepted=%lld kept=%lld reverted=%lld frozen_crossers=%lld score_before=%.3f score_after=%.3f\n",
                        env_->curr_timestep + 1, temporal_region_options_.parts, temporal_region_options_.rounds,
                        temporal_region_options_.threads, temporal_region_options_.temperature_ppm, region_stats.active_robots, region_stats.candidates,
                        region_stats.repairs, region_stats.accepted, region_stats.kept_regions, region_stats.reverted_regions,
                        region_stats.frozen_crossers, region_stats.score_before, region_stats.score_after);
            if (temporal_region_options_.candidate_limit)
                std::printf("[cgar-regional-work] step=%d candidate_limit=%lld limited_batches=%lld max_batch_candidates=%lld\n",
                    env_->curr_timestep + 1, temporal_region_options_.candidate_limit,
                    region_stats.candidate_limited_batches, region_stats.max_batch_candidates);
            for (size_t round = 0; round < region_stats.round_scores.size(); ++round)
                std::printf("[cgar-temporal-region-round] step=%d round=%zu score_before=%.3f score_after=%.3f\n",
                    env_->curr_timestep + 1, round, round ? region_stats.round_scores[round - 1] : region_stats.score_before,
                    region_stats.round_scores[round]);
        }
    }
    check_deadline(deadline_, "temporal_complete");
}
}  // namespace cgar
