#pragma once
// Fixed-work rolling-window repair around a complete CGAR seed. Protected
// primary/recovery/support paths remain immutable. Longer ordinary paths are
// forecasts, not claims about future CGAR primary or recovery decisions.
#include "chain_potential.hpp"
#include <atomic>
#include <memory>
#include <numeric>
#include <queue>
#include <random>

namespace cgar {
struct WindowOptions {
    int horizon = 0, keep = 6, iterations = 128, nodes = 2048, group = 4;
    int workers = 4, threads = 4, wait_cost = 0;
};
using WindowPath = std::vector<int>;  // cell * 4 + heading, including time zero
struct WindowProblem {
    int rows = 0, cols = 0, horizon = 0, turn_cost = 1, wait_cost = 1;
    const ChainPotential* oracle = nullptr;
    std::vector<char> free, fixed;
    std::vector<std::array<int, 4>> forward;
    std::vector<std::vector<char>> allowed, entry_allowed;
    std::vector<ChainPotential::Chain> chains;
    std::vector<int> tasks;
    std::vector<WindowPath> seed;
    int neighbor(int cell, int d) const {
        if (d == 0) return cell % cols + 1 < cols ? cell + 1 : -1;
        if (d == 1) return cell / cols + 1 < rows ? cell + cols : -1;
        if (d == 2) return cell % cols ? cell - 1 : -1;
        return cell / cols ? cell - cols : -1;
    }
    int next(int state, int action) const {
        if (action == 0) {
            const int cell = neighbor(state / 4, state % 4);
            return cell >= 0 && free[cell] ? cell * 4 + state % 4 : -1;
        }
        return state / 4 * 4 + (state % 4 + (action == 1 ? 1 : action == 2 ? 3 : 0)) % 4;
    }
    bool permits(int r, int from, int to) const {
        return to >= 0 && allowed[r][to / 4] && (from / 4 == to / 4 || entry_allowed[r][to / 4]);
    }
    int action(int from, int to) const {
        if (from == to) return 3;
        for (int a = 0; a < 3; ++a) if (next(from, a) == to) return a;
        throw std::logic_error("invalid rolling-window kinematic transition");
    }
    int step_cost(int r, size_t stage, int from, int a) const {
        if (a == 3) return stage == chains[r].goals.size() ? 0 : wait_cost;
        return a == 0 ? forward[from / 4][from % 4] : turn_cost;
    }
    int64_t cost(int r, const WindowPath& path) const {
        size_t stage = 0; int64_t paid = 0;
        for (int t = 1; t <= horizon; ++t) {
            paid += step_cost(r, stage, path[t - 1], action(path[t - 1], path[t]));
            if (stage < chains[r].goals.size() && path[t] / 4 == chains[r].goals[stage]) ++stage;
        }
        return paid + oracle->value(chains[r], stage, path.back() / 4, path.back() % 4);
    }
    int64_t score(const std::vector<WindowPath>& paths) const {
        int64_t value = 0;
        // Unreachable fixed chains do not take part in the objective. Their
        // physical seed is nevertheless fully reserved and validated.
        for (size_t r = 0; r < paths.size(); ++r) if (!fixed[r]) value += cost(r, paths[r]);
        return value;
    }
    template<class Check>
    void validate(const std::vector<WindowPath>& paths, Check check) const {
        const int cells = free.size(), robots = seed.size();
        if (!oracle || !oracle->ready() || rows <= 0 || cols <= 0 || rows * cols != cells ||
            horizon < 1 || fixed.size() != seed.size() || allowed.size() != seed.size() || entry_allowed.size() != seed.size() ||
            chains.size() != seed.size() || tasks.size() != seed.size() || paths.size() != seed.size() ||
            forward.size() != free.size() || turn_cost < 1 || wait_cost < 1)
            throw std::invalid_argument("invalid rolling-window problem");
        std::vector<int> owners(cells, -1);
        for (int r = 0; r < robots; ++r) {
            if (seed[r].size() != size_t(horizon + 1) || paths[r].size() != seed[r].size() ||
                paths[r][0] != seed[r][0] || allowed[r].size() != free.size() || entry_allowed[r].size() != free.size() ||
                (fixed[r] && paths[r] != seed[r]))
                throw std::logic_error("rolling-window seed or protected path changed");
        }
        for (int t = 0; t <= horizon; ++t) {
            check(); std::fill(owners.begin(), owners.end(), -1);
            for (int r = 0; r < robots; ++r) {
                const int state = paths[r][t], cell = state / 4;
                if (state < 0 || cell >= cells || !free[cell] || owners[cell] >= 0 ||
                    (!fixed[r] && !allowed[r][cell]))
                    throw std::logic_error("rolling-window vertex or domain conflict");
                owners[cell] = r;
                if (t) {
                    action(paths[r][t - 1], state);
                    if (!fixed[r] && !permits(r, paths[r][t - 1], state))
                        throw std::logic_error("rolling-window protected intent entry");
                }
            }
            if (t) for (int r = 0; r < robots; ++r) {
                const int other = owners[paths[r][t - 1] / 4];
                if (other >= 0 && other != r && paths[other][t - 1] / 4 == paths[r][t] / 4)
                    throw std::logic_error("rolling-window edge conflict");
            }
        }
    }
};
struct WindowStats {
    bool completed = false;
    long long attempts = 0, accepted = 0, improved = 0, searches = 0, expanded = 0;
    long long capped = 0, failed = 0, retained = 0, history_resets = 0, partial_rollbacks = 0;
    int64_t seed_cost = 0, initial_cost = 0, final_cost = 0;
    int changed_first = 0, protected_robots = 0, selected_worker = 0;
    void merge(const WindowStats& other) {
        attempts += other.attempts; accepted += other.accepted; improved += other.improved;
        searches += other.searches; expanded += other.expanded; capped += other.capped; failed += other.failed; partial_rollbacks += other.partial_rollbacks;
    }
};

// Reused dense labels plus immutable predecessor records. Updating a state label
// cannot silently rewrite a previously queued path's ancestry.
struct WindowScratch {
    struct Node { int state, time, stage, parent; int64_t paid, estimate; };
    struct Earlier {
        bool operator()(const Node& a, const Node& b) const {
            if (a.estimate != b.estimate) return a.estimate > b.estimate;
            if (a.time != b.time) return a.time < b.time;
            if (a.paid != b.paid) return a.paid < b.paid;
            return a.parent > b.parent;
        }
    };
    std::vector<int64_t> best;
    std::vector<uint32_t> stamp;
    uint32_t generation = 0;
    std::vector<Node> nodes;
    // Heap nodes use parent as the immutable node index. Stored nodes use it
    // as a predecessor index; this keeps heap entries independent of reallocations.
    std::vector<Node> heap;
    void begin(size_t size) {
        if (size > size_t(std::numeric_limits<int>::max()))
            throw std::invalid_argument("rolling-window label space exceeds integer indexing");
        if (best.size() < size) { best.resize(size); stamp.assign(size, 0); generation = 0; }
        if (++generation == 0) { std::fill(stamp.begin(), stamp.end(), 0); ++generation; }
        nodes.clear(); heap.clear();
    }
};

class WindowSearch {
public:
    WindowSearch(const WindowProblem& problem, const WindowOptions& options,
                 const std::vector<WindowPath>& initial, WindowScratch& scratch, uint64_t seed)
        : p_(problem), o_(options), paths_(initial), scratch_(scratch), rng_(seed),
          owners_((p_.horizon + 1) * p_.free.size(), -1) {
        for (int r = 0; r < int(paths_.size()); ++r) {
            reserve(r, true); if (!p_.fixed[r]) eligible_.push_back(r);
        }
    }
    const std::vector<WindowPath>& paths() const { return paths_; }
    WindowStats stats;
    // Exposed for independent shortest-path tests; the robot must be removed
    // from reservations by the caller. Normal group repair handles this itself.
    void reserve(int robot, bool add) {
        for (int t = 0; t <= p_.horizon; ++t) {
            int& slot = owners_[t * p_.free.size() + paths_[robot][t] / 4];
            if (slot != (add ? -1 : robot)) throw std::logic_error("rolling-window reservation ownership mismatch");
            slot = add ? robot : -1;
        }
    }
    template<class Check>
    bool path(int r, WindowPath& result, Check check) {
        ++stats.searches;
        const int states = p_.free.size() * 4, times = p_.horizon + 1;
        const int stages = std::min<size_t>(p_.horizon, p_.chains[r].goals.size()) + 1;
        scratch_.begin(size_t(states) * times * stages);
        auto key = [&](int state, int time, int stage) { return (size_t(stage) * times + time) * states + state; };
        auto push = [&](int state, int time, int stage, int64_t paid, int parent) {
            const size_t id = key(state, time, stage);
            if (scratch_.stamp[id] == scratch_.generation && scratch_.best[id] <= paid) return;
            const int64_t heuristic = p_.oracle->value(p_.chains[r], stage, state / 4, state % 4);
            if (heuristic >= ChainPotential::infinity) return;
            scratch_.stamp[id] = scratch_.generation; scratch_.best[id] = paid;
            WindowScratch::Node node{state, time, stage, parent, paid, paid + heuristic};
            const int index = scratch_.nodes.size(); scratch_.nodes.push_back(node);
            node.parent = index; scratch_.heap.push_back(node);
            std::push_heap(scratch_.heap.begin(), scratch_.heap.end(), WindowScratch::Earlier{});
        };
        push(paths_[r][0], 0, 0, 0, -1);
        int expanded = 0;
        while (!scratch_.heap.empty()) {
            if ((expanded & 63) == 0) check();
            std::pop_heap(scratch_.heap.begin(), scratch_.heap.end(), WindowScratch::Earlier{});
            const int index = scratch_.heap.back().parent; scratch_.heap.pop_back();
            const auto node = scratch_.nodes[index];
            if (scratch_.best[key(node.state, node.time, node.stage)] != node.paid) continue;
            if (expanded == o_.nodes) { ++stats.capped; return false; }
            ++expanded; ++stats.expanded;
            if (node.time == p_.horizon) {
                result.resize(p_.horizon + 1);
                for (int at = index; at >= 0; at = scratch_.nodes[at].parent)
                    result[scratch_.nodes[at].time] = scratch_.nodes[at].state;
                if (p_.cost(r, result) != node.estimate) throw std::logic_error("rolling-window predecessor cost mismatch");
                return true;
            }
            for (int a = 0; a < 4; ++a) {
                const int next = p_.next(node.state, a), time = node.time + 1;
                if (!p_.permits(r, node.state, next) || owner(time, next / 4) >= 0) continue;
                const int other = owner(time, node.state / 4);
                if (other >= 0 && paths_[other][time - 1] / 4 == next / 4) continue;
                const int stage = node.stage + (node.stage < int(p_.chains[r].goals.size()) &&
                                                 next / 4 == p_.chains[r].goals[node.stage]);
                push(next, time, stage, node.paid + p_.step_cost(r, node.stage, node.state, a), index);
            }
        }
        ++stats.failed; return false;
    }
    template<class Check>
    void run(Check check) {
        auto order = eligible_; std::shuffle(order.begin(), order.end(), rng_);
        for (int iteration = 0; iteration < o_.iterations; ++iteration) {
            check(); ++stats.attempts;
            if (eligible_.empty()) continue;
            const int root = iteration < int(order.size()) ? order[iteration] : eligible_[rng_() % eligible_.size()];
            auto group = neighborhood(root);
            if (iteration & 1) std::shuffle(group.begin(), group.end(), rng_);
            std::vector<WindowPath> before; before.reserve(group.size());
            int64_t old_cost = 0;
            for (int r : group) { old_cost += p_.cost(r, paths_[r]); before.push_back(paths_[r]); reserve(r, false); }
            int repaired = 0; int64_t new_cost = 0;
            for (int r : group) {
                WindowPath proposed;
                if (!path(r, proposed, check)) break;
                paths_[r] = std::move(proposed); new_cost += p_.cost(r, paths_[r]); reserve(r, true); ++repaired;
            }
            if (repaired == int(group.size()) && new_cost <= old_cost) {
                ++stats.accepted; stats.improved += new_cost < old_cost;
            } else {
                stats.partial_rollbacks += repaired > 0 && repaired < int(group.size());
                for (int k = 0; k < repaired; ++k) reserve(group[k], false);
                for (size_t k = 0; k < group.size(); ++k) { paths_[group[k]] = std::move(before[k]); reserve(group[k], true); }
            }
        }
        check(); stats.completed = true;
    }
private:
    int owner(int time, int cell) const { return owners_[time * p_.free.size() + cell]; }
    std::vector<int> neighborhood(int root) {
        const int target = 1 + rng_() % std::min<size_t>(o_.group, eligible_.size());
        std::vector<int> group{root}, candidates;
        auto append = [&](int r) {
            if (r >= 0 && !p_.fixed[r] && std::find(group.begin(), group.end(), r) == group.end() &&
                std::find(candidates.begin(), candidates.end(), r) == candidates.end()) candidates.push_back(r);
        };
        // Unconstrained remaining-chain descent reveals who actually blocks a
        // useful future route. It is used only to choose a repair neighborhood.
        int state = paths_[root][0], stage = 0;
        for (int t = 1; t <= p_.horizon; ++t) {
            int best = state, next_stage = stage; int64_t best_cost = ChainPotential::infinity;
            const int start = rng_() % 4;
            for (int k = 0; k < 4; ++k) {
                const int a = (start + k) % 4, next = p_.next(state, a);
                if (!p_.permits(root, state, next)) continue;
                const int s = stage + (stage < int(p_.chains[root].goals.size()) && next / 4 == p_.chains[root].goals[stage]);
                const int64_t value = p_.step_cost(root, stage, state, a) + p_.oracle->value(p_.chains[root], s, next / 4, next % 4);
                if (value < best_cost) { best = next; next_stage = s; best_cost = value; }
            }
            append(owner(t, best / 4)); append(owner(t, state / 4));
            state = best; stage = next_stage;
        }
        std::shuffle(candidates.begin(), candidates.end(), rng_);
        for (int r : candidates) { if (int(group.size()) == target) break; group.push_back(r); }
        for (size_t head = 0; head < group.size() && int(group.size()) < target; ++head) {
            const int r = group[head], t = 1 + rng_() % p_.horizon, cell = paths_[r][t] / 4;
            const int start = rng_() % 4;
            for (int k = 0; k < 4 && int(group.size()) < target; ++k) {
                const int next = p_.neighbor(cell, (start + k) % 4);
                if (next < 0) continue;
                const int other = owner(t, next);
                if (other >= 0 && !p_.fixed[other] && std::find(group.begin(), group.end(), other) == group.end()) group.push_back(other);
            }
        }
        // A bounded global draw occasionally connects spatially separated routes.
        for (int draw = 0; draw < o_.group * 2 && int(group.size()) < target; ++draw) {
            const int r = eligible_[rng_() % eligible_.size()];
            if (std::find(group.begin(), group.end(), r) == group.end()) group.push_back(r);
        }
        return group;
    }
    const WindowProblem& p_;
    const WindowOptions& o_;
    std::vector<WindowPath> paths_;
    WindowScratch& scratch_;
    std::mt19937_64 rng_;
    std::vector<int> owners_, eligible_;
};

class RollingWindow {
public:
    template<class Check>
    std::vector<WindowPath> solve(const WindowProblem& p, const WindowOptions& options, int tick,
                                 const std::vector<uint64_t>& seeds, WindowStats& stats, Check check) {
        stats = {};
        if (options.horizon != p.horizon || options.horizon < 1 || options.horizon > 32 ||
            options.keep < 0 || options.keep >= p.horizon || options.iterations < 1 || options.nodes < 1 ||
            options.group < 1 || options.workers < 1 || options.workers > 32 || options.threads < 1 ||
            options.threads > options.workers || seeds.size() != size_t(options.workers))
            throw std::invalid_argument("invalid rolling-window work declaration");
        p.validate(p.seed, check);
        auto initial = p.seed;
        stats.seed_cost = p.score(initial);
        if (options.keep && tick == tick_ + 1 && history_.size() == p.seed.size()) {
            auto candidate = p.seed;
            std::vector<char> retained(p.seed.size(), false);
            for (size_t r = 0; r < p.seed.size(); ++r) {
                check();
                if (p.fixed[r] || tasks_[r] != p.tasks[r] || history_[r].size() != p.seed[r].size() ||
                    history_[r][1] != p.seed[r][0]) continue;
                const size_t advance = !goals_[r].empty() && history_[r][1] / 4 == goals_[r][0] ? 1 : 0;
                if (std::vector<int>(goals_[r].begin() + advance, goals_[r].end()) != p.chains[r].goals) continue;
                bool valid = true;
                for (int t = 1; t <= p.horizon; ++t) {
                    candidate[r][t] = history_[r][std::min(t + 1, options.keep + 1)];
                    valid = valid && p.permits(r, candidate[r][t - 1], candidate[r][t]);
                }
                if (valid) retained[r] = true;
                else candidate[r] = p.seed[r];
            }
            // Changes in tasks or protected seeds invalidate whole retained
            // paths. Monotone resets end at the independently validated seed.
            for (;;) {
                check(); std::vector<char> reset(p.seed.size(), false);
                std::vector<int> owners(p.free.size(), -1);
                auto conflict = [&](int a, int b) {
                    if (retained[a]) reset[a] = true;
                    if (retained[b]) reset[b] = true;
                    if (!retained[a] && !retained[b]) throw std::logic_error("fresh rolling-window seeds conflict");
                };
                for (int t = 1; t <= p.horizon; ++t) {
                    std::fill(owners.begin(), owners.end(), -1);
                    for (int r = 0; r < int(candidate.size()); ++r) {
                        int& other = owners[candidate[r][t] / 4];
                        if (other >= 0) conflict(r, other); else other = r;
                    }
                    for (int r = 0; r < int(candidate.size()); ++r) {
                        const int other = owners[candidate[r][t - 1] / 4];
                        if (other >= 0 && other != r && candidate[other][t - 1] / 4 == candidate[r][t] / 4) conflict(r, other);
                    }
                }
                int count = 0;
                for (size_t r = 0; r < reset.size(); ++r) if (reset[r]) {
                    retained[r] = false; candidate[r] = p.seed[r]; ++count;
                }
                stats.history_resets += count; if (!count) break;
            }
            p.validate(candidate, check);
            if (p.score(candidate) <= stats.seed_cost) {
                initial = std::move(candidate); stats.retained = std::count(retained.begin(), retained.end(), true);
            }
        }
        stats.initial_cost = p.score(initial);
        scratch_.resize(options.workers);
        std::vector<std::unique_ptr<WindowSearch>> results(options.workers);
        std::vector<std::exception_ptr> errors(options.workers); std::atomic<int> next{0};
        auto work = [&] {
            for (;;) {
                const int id = next.fetch_add(1); if (id >= options.workers) return;
                try {
                    results[id] = std::make_unique<WindowSearch>(p, options, initial, scratch_[id], seeds[id]);
                    results[id]->run(check);
                } catch (...) { errors[id] = std::current_exception(); }
            }
        };
        std::vector<std::thread> threads;
        try { for (int t = 1; t < options.threads; ++t) threads.emplace_back(work); }
        catch (...) { for (auto& t : threads) t.join(); throw; }
        work(); for (auto& t : threads) t.join();
        for (const auto& error : errors) if (error) std::rethrow_exception(error);
        int best = 0; int64_t best_cost = p.score(results[0]->paths());
        for (int id = 0; id < options.workers; ++id) {
            if (!results[id]->stats.completed) throw std::logic_error("partial rolling-window result");
            stats.merge(results[id]->stats);
            const int64_t cost = p.score(results[id]->paths());
            if (cost < best_cost) { best = id; best_cost = cost; }
        }
        auto result = results[best]->paths(); p.validate(result, check); check();
        if (best_cost > stats.initial_cost) throw std::logic_error("rolling-window objective regressed");
        stats.final_cost = best_cost; stats.selected_worker = best;
        for (size_t r = 0; r < result.size(); ++r) {
            stats.changed_first += result[r][1] != p.seed[r][1]; stats.protected_robots += bool(p.fixed[r]);
        }
        tick_ = tick; history_ = result; tasks_ = p.tasks; goals_.clear();
        for (const auto& chain : p.chains) goals_.push_back(chain.goals);
        stats.completed = true; return result;
    }
private:
    int tick_ = -2;
    std::vector<WindowPath> history_;
    std::vector<std::vector<int>> goals_;
    std::vector<int> tasks_;
    std::vector<WindowScratch> scratch_;
};
} // namespace cgar
