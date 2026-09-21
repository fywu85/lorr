#pragma once
// Fixed-work rolling-window repair around a complete CGAR seed. Protected
// primary/recovery/support paths remain immutable. Longer ordinary paths are
// forecasts, not claims about future CGAR primary or recovery decisions.
#include "chain_potential.hpp"
#include "temporal_geometry.hpp"
#include <atomic>
#include <cmath>
#include <memory>
#include <numeric>
#include <queue>
#include <random>
#include <tuple>

namespace cgar {
struct WindowOptions {
    int horizon = 0, keep = 6, iterations = 128, nodes = 2048, group = 4;
    int workers = 4, threads = 4, wait_cost = 0, seed_rollout = 0, progress_ties = 0, protected_prefix = 0, history_rollout = 0;
    int delay_samples = 0;  // extra candidates on half the repair attempts; zero preserves the old random stream
    int temperature = 0;  // initial cost units; cooling is by completed iteration count
    int merge = 0;  // combine compatible paths only after every worker completes
};
using WindowPath = std::vector<int>;  // cell * 4 + heading, including time zero
struct WindowProblem {
    int rows = 0, cols = 0, horizon = 0, turn_cost = 1, wait_cost = 1;
    const ChainPotential* oracle = nullptr;
    std::vector<char> free, fixed;
    std::vector<std::array<int, 4>> forward;
    std::vector<std::vector<char>> allowed, entry_allowed;
    std::vector<ChainPotential::Chain> chains;
    std::vector<int> tasks, locked_prefix, first_cells;
    std::vector<WindowPath> seed;
    int prefix(int r) const { return locked_prefix.empty() ? 0 : locked_prefix[r]; }
    int first_cell(int r) const { return first_cells.empty() ? -1 : first_cells[r]; }
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
    int64_t remaining(int r, const WindowPath& path) const {
        size_t stage = 0;
        for (int t = 1; t <= horizon; ++t)
            if (stage < chains[r].goals.size() && path[t] / 4 == chains[r].goals[stage]) ++stage;
        return oracle->value(chains[r], stage, path.back() / 4, path.back() % 4);
    }
    int64_t remaining_score(const std::vector<WindowPath>& paths) const {
        int64_t value = 0;
        for (size_t r = 0; r < paths.size(); ++r) if (!fixed[r]) value += remaining(r, paths[r]);
        return value;
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
            forward.size() != free.size() || turn_cost < 1 || wait_cost < 1 ||
            (!locked_prefix.empty() && locked_prefix.size() != seed.size()) ||
            (!first_cells.empty() && first_cells.size() != seed.size()))
            throw std::invalid_argument("invalid rolling-window problem");
        std::vector<int> owners(cells, -1);
        for (int r = 0; r < robots; ++r) {
            if (seed[r].size() != size_t(horizon + 1) || paths[r].size() != seed[r].size() ||
                paths[r][0] != seed[r][0] || allowed[r].size() != free.size() || entry_allowed[r].size() != free.size() ||
                prefix(r) < 0 || prefix(r) > horizon || first_cell(r) < -1 || first_cell(r) >= cells ||
                (first_cell(r) >= 0 && paths[r][1] / 4 != first_cell(r)) || (fixed[r] && paths[r] != seed[r]))
                throw std::logic_error("rolling-window seed or protected path changed");
            for (int t = 0; t <= prefix(r); ++t)
                if (paths[r][t] != seed[r][t]) throw std::logic_error("rolling-window protected prefix changed");
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
// Extend a validated prefix (five actions by default) with complete joint
// temporal-PIBT chunks. This gives LNS coordinated future traffic instead of an artificial
// stationary wall after slot five. Existing protected tails remain unchanged.
template<class Check>
int extend_window_seed(WindowProblem& p, const TemporalGeometry& geometry,
                       const std::vector<int>& order, int displacement_limit,
                       uint64_t seed, Check check, int begin = 5) {
    if (begin < 5 || begin > p.horizon) throw std::invalid_argument("invalid joint continuation prefix length");
    p.validate(p.seed, check);
    const auto before = p.seed; const int64_t before_cost = p.score(before);
    const int robots = p.seed.size(), cells = p.free.size();
    std::vector<size_t> stages(robots, 0);
    auto advance = [&](int r, int cell) {
        if (stages[r] < p.chains[r].goals.size() && cell == p.chains[r].goals[stages[r]]) ++stages[r];
    };
    for (int r = 0; r < robots; ++r)
        for (int t = 1; t <= begin; ++t) advance(r, p.seed[r][t] / 4);
    std::mt19937_64 rng(seed); int batches = 0;
    for (int offset = begin; offset < p.horizon; offset += 5) {
        check(); const int length = std::min(5, p.horizon - offset);
        std::vector<TemporalPath> waits(robots);
        std::vector<std::vector<TemporalChoice>> choices(robots);
        std::vector<double> power(robots, 1.0);
        for (int r = 0; r < robots; ++r) {
            check(); const int start = p.seed[r][offset];
            auto cost = [&](const TemporalPath& path, int op) {
                if (p.fixed[r]) return int64_t(0);
                size_t stage = stages[r]; int state = start; int64_t paid = 0;
                for (int t = 0; t < length; ++t) {
                    const int a = TemporalGeometry::operations()[op][t];
                    paid += p.step_cost(r, stage, state, a); state = p.next(state, a);
                    if (stage < p.chains[r].goals.size() && state / 4 == p.chains[r].goals[stage]) ++stage;
                }
                const int64_t tail = p.oracle->value(p.chains[r], stage, state / 4, state % 4);
                if (tail >= ChainPotential::infinity) throw std::logic_error("unreachable ordinary rolling seed tail");
                return (paid + tail) * 1024 + op;
            };
            waits[r] = geometry.seed(start / 4, start % 4, 3);
            choices[r].push_back({&waits[r], cost(waits[r], 0), 0});
            if (p.fixed[r]) continue;
            const auto& paths = geometry.paths(start / 4, start % 4);
            for (int op = 1; op < 129; ++op) {
                const auto& path = paths[op]; if (!path.valid) continue;
                int from = start / 4; bool valid = true;
                for (int to : path.cells) {
                    if (!p.allowed[r][to] || (to != from && !p.entry_allowed[r][to])) { valid = false; break; }
                    from = to;
                }
                if (valid) choices[r].push_back({&path, cost(path, op), op});
            }
            std::sort(choices[r].begin() + 1, choices[r].end(), [](const auto& a, const auto& b) {
                return std::tie(a.cost, a.operation) < std::tie(b.cost, b.operation);
            });
        }
        TemporalPibt projection(cells, choices, p.fixed, power, displacement_limit, rng());
        projection.construct(order, check);
        for (int r = 0; r < robots; ++r) {
            const auto& choice = projection.choice(r); int state = p.seed[r][offset];
            for (int t = 0; t < length; ++t) {
                state = p.next(state, TemporalGeometry::operations()[choice.operation][t]);
                if (state < 0 || state / 4 != choice.path->cells[t])
                    throw std::logic_error("rolling seed projection disagrees with temporal geometry");
                p.seed[r][offset + t + 1] = state; advance(r, state / 4);
            }
        }
        ++batches;
    }
    // Validate against the original protected paths, not a self-referential
    // modified fixed seed. Complete all chunks before selecting either plan.
    for (int r = 0; r < robots; ++r)
        if (p.fixed[r] && p.seed[r] != before[r]) throw std::logic_error("rolling projection changed a protected tail");
    p.validate(p.seed, check); check();
    if (p.score(p.seed) > before_cost) p.seed = before;
    return batches;
}

struct WindowRollout {
    const TemporalGeometry* geometry = nullptr;
    const std::vector<int>* order = nullptr;
    int displacement_limit = 8192;
    uint64_t seed = 0;
};

struct WindowStats {
    bool completed = false;
    long long attempts = 0, accepted = 0, improved = 0, searches = 0, expanded = 0;
    long long capped = 0, failed = 0, retained = 0, history_resets = 0, partial_rollbacks = 0, history_batches = 0;
    long long delay_draws = 0, delay_replacements = 0;
    long long uphill_accepted = 0, incumbent_updates = 0, incumbent_restores = 0;
    long long merge_donors = 0, merge_components = 0, merge_accepted = 0, merge_robots = 0;
    int64_t merge_gain = 0, pre_merge_cost = 0;
    int64_t seed_cost = 0, initial_cost = 0, final_cost = 0;
    int64_t seed_remaining = 0, initial_remaining = 0, final_remaining = 0;
    int changed_first = 0, protected_robots = 0, selected_worker = 0;
    void merge(const WindowStats& other) {
        attempts += other.attempts; accepted += other.accepted; improved += other.improved;
        searches += other.searches; expanded += other.expanded; capped += other.capped; failed += other.failed; partial_rollbacks += other.partial_rollbacks; history_batches += other.history_batches;
        delay_draws += other.delay_draws; delay_replacements += other.delay_replacements;
        uphill_accepted += other.uphill_accepted; incumbent_updates += other.incumbent_updates; incumbent_restores += other.incumbent_restores;
        merge_donors += other.merge_donors; merge_components += other.merge_components;
        merge_accepted += other.merge_accepted; merge_robots += other.merge_robots; merge_gain += other.merge_gain;
    }
};

// Both parents must be complete, valid plans for the same CGAR problem. Connect
// every cross-parent vertex and head-on conflict. A component can then take one
// parent's whole paths without colliding with another component's choice.
template<class Check>
std::vector<std::vector<int>> window_merge_components(const WindowProblem& p,
        const std::vector<WindowPath>& current, const std::vector<WindowPath>& donor, Check check) {
    p.validate(current, check); p.validate(donor, check);
    const int robots = current.size();
    std::vector<int> parent(robots), owner(p.free.size(), -1);
    std::iota(parent.begin(), parent.end(), 0);
    auto root = [&](int r) { while (parent[r] != r) { parent[r] = parent[parent[r]]; r = parent[r]; } return r; };
    auto join = [&](int a, int b) { a = root(a); b = root(b); if (a != b) parent[std::max(a, b)] = std::min(a, b); };
    for (int t = 1; t <= p.horizon; ++t) {
        check(); std::fill(owner.begin(), owner.end(), -1);
        for (int r = 0; r < robots; ++r) owner[current[r][t] / 4] = r;
        for (int r = 0; r < robots; ++r) {
            int other = owner[donor[r][t] / 4];
            if (other >= 0) join(r, other);
            other = owner[donor[r][t - 1] / 4];
            if (other >= 0 && current[other][t - 1] / 4 == donor[r][t] / 4) join(r, other);
        }
    }
    std::vector<std::vector<int>> groups(robots), result;
    for (int r = 0; r < robots; ++r) groups[root(r)].push_back(r);
    for (auto& group : groups) {
        bool changed = false;
        for (int r : group) changed = changed || current[r] != donor[r];
        if (changed) result.push_back(std::move(group));
    }
    return result;
}

template<class Check>
void merge_window_donor(const WindowProblem& p, std::vector<WindowPath>& current,
        const std::vector<WindowPath>& donor, bool progress_ties, WindowStats& stats, Check check) {
    const auto groups = window_merge_components(p, current, donor, check);
    const int64_t before = p.score(current), previous_gain = stats.merge_gain;
    ++stats.merge_donors;
    for (const auto& group : groups) {
        check(); ++stats.merge_components;
        int64_t old_cost = 0, new_cost = 0, old_remaining = 0, new_remaining = 0;
        for (int r : group) if (!p.fixed[r]) {
            old_cost += p.cost(r, current[r]); new_cost += p.cost(r, donor[r]);
            if (progress_ties) { old_remaining += p.remaining(r, current[r]); new_remaining += p.remaining(r, donor[r]); }
        }
        if (new_cost < old_cost || (progress_ties && new_cost == old_cost && new_remaining < old_remaining)) {
            ++stats.merge_accepted; stats.merge_gain += old_cost - new_cost;
            for (int r : group) if (current[r] != donor[r]) { current[r] = donor[r]; ++stats.merge_robots; }
        }
    }
    p.validate(current, check); check();
    if (p.score(current) + stats.merge_gain - previous_gain != before)
        throw std::logic_error("rolling-window merged objective does not reconcile");
}

// Reused dense labels plus immutable predecessor records. Updating a state label
// cannot silently rewrite a previously queued path's ancestry.
struct WindowScratch {
    struct Node { int state, time, stage, parent; int64_t paid, estimate; };
    struct Earlier {
        bool progress = false;
        bool operator()(const Node& a, const Node& b) const {
            if (a.estimate != b.estimate) return a.estimate > b.estimate;
            if (progress && a.paid != b.paid) return a.paid < b.paid;
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
        if (o_.delay_samples) {
            costs_.resize(paths_.size()); lower_bounds_.resize(paths_.size());
            for (int r : eligible_) {
                costs_[r] = p_.cost(r, paths_[r]);
                const int start = paths_[r][0];
                lower_bounds_[r] = p_.oracle->value(p_.chains[r], 0, start / 4, start % 4);
                if (lower_bounds_[r] >= ChainPotential::infinity)
                    throw std::logic_error("unreachable rolling-window delay lower bound");
            }
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
            std::push_heap(scratch_.heap.begin(), scratch_.heap.end(), WindowScratch::Earlier{bool(o_.progress_ties)});
        };
        push(paths_[r][0], 0, 0, 0, -1);
        int expanded = 0;
        while (!scratch_.heap.empty()) {
            if ((expanded & 63) == 0) check();
            std::pop_heap(scratch_.heap.begin(), scratch_.heap.end(), WindowScratch::Earlier{bool(o_.progress_ties)});
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
                if (!p_.permits(r, node.state, next) || owner(time, next / 4) >= 0 ||
                    (time <= p_.prefix(r) && next != p_.seed[r][time]) ||
                    (time == 1 && p_.first_cell(r) >= 0 && next / 4 != p_.first_cell(r))) continue;
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
        std::vector<WindowPath> incumbent;
        std::vector<char> dirty;
        std::vector<int> changed;
        int64_t current_cost = 0, current_remaining = 0, best_cost = 0, best_remaining = 0;
        if (o_.temperature) {
            incumbent = paths_; dirty.assign(paths_.size(), false);
            current_cost = best_cost = p_.score(paths_);
            if (o_.progress_ties) current_remaining = best_remaining = p_.remaining_score(paths_);
        }
        auto better = [&](int64_t cost, int64_t remaining, int64_t other_cost, int64_t other_remaining) {
            return cost < other_cost || (o_.progress_ties && cost == other_cost && remaining < other_remaining);
        };
        for (int iteration = 0; iteration < o_.iterations; ++iteration) {
            check(); ++stats.attempts;
            if (eligible_.empty()) continue;
            int root = iteration < int(order.size()) ? order[iteration] : eligible_[rng_() % eligible_.size()];
            // Keep the initial sweep and half the later attempts unbiased. The
            // other half compare avoidable path cost, not remaining task length.
            // This bounded tournament follows PILOT's delayed-route heuristic.
            if (iteration >= int(order.size()) && (iteration & 1)) {
                for (int draw = 0; draw < o_.delay_samples; ++draw) {
                    const int candidate = eligible_[rng_() % eligible_.size()]; ++stats.delay_draws;
                    if (costs_[candidate] - lower_bounds_[candidate] > costs_[root] - lower_bounds_[root]) {
                        root = candidate; ++stats.delay_replacements;
                    }
                }
            }
            auto group = neighborhood(root);
            if (iteration & 1) std::shuffle(group.begin(), group.end(), rng_);
            std::vector<WindowPath> before; before.reserve(group.size());
            int64_t old_cost = 0, old_remaining = 0;
            for (int r : group) { old_cost += o_.delay_samples ? costs_[r] : p_.cost(r, paths_[r]); if (o_.progress_ties) old_remaining += p_.remaining(r, paths_[r]); before.push_back(paths_[r]); reserve(r, false); }
            int repaired = 0; int64_t new_cost = 0, new_remaining = 0;
            std::vector<int64_t> proposed_costs;
            if (o_.delay_samples) proposed_costs.reserve(group.size());
            for (int r : group) {
                WindowPath proposed;
                if (!path(r, proposed, check)) break;
                paths_[r] = std::move(proposed); const int64_t cost = p_.cost(r, paths_[r]); new_cost += cost;
                if (o_.delay_samples) proposed_costs.push_back(cost);
                if (o_.progress_ties) new_remaining += p_.remaining(r, paths_[r]);
                reserve(r, true); ++repaired;
            }
            bool accept = repaired == int(group.size()) && (new_cost < old_cost ||
                (new_cost == old_cost && (!o_.progress_ties || new_remaining <= old_remaining)));
            bool uphill = false;
            if (!accept && repaired == int(group.size()) && o_.temperature) {
                const double temperature = double(o_.temperature) * (o_.iterations - iteration) / o_.iterations;
                const int64_t increase = std::max(int64_t(0), new_cost - old_cost);
                accept = std::generate_canonical<double, 53>(rng_) < std::exp(-double(increase) / temperature);
                uphill = accept;
            }
            if (accept) {
                ++stats.accepted; stats.improved += better(new_cost, new_remaining, old_cost, old_remaining);
                stats.uphill_accepted += uphill;
                if (o_.delay_samples) for (size_t k = 0; k < group.size(); ++k) costs_[group[k]] = proposed_costs[k];
                if (o_.temperature) {
                    current_cost += new_cost - old_cost; current_remaining += new_remaining - old_remaining;
                    for (int r : group) if (!dirty[r]) { dirty[r] = true; changed.push_back(r); }
                    if (better(current_cost, current_remaining, best_cost, best_remaining)) {
                        // Snapshot only paths changed since the last retained best.
                        // Whole-plan copying on every improvement is unnecessary.
                        for (int r : changed) { incumbent[r] = paths_[r]; dirty[r] = false; }
                        changed.clear(); best_cost = current_cost; best_remaining = current_remaining;
                        ++stats.incumbent_updates;
                    }
                }
            } else {
                stats.partial_rollbacks += repaired > 0 && repaired < int(group.size());
                for (int k = 0; k < repaired; ++k) reserve(group[k], false);
                for (size_t k = 0; k < group.size(); ++k) { paths_[group[k]] = std::move(before[k]); reserve(group[k], true); }
            }
        }
        // A rejected or partially repaired group must never change the delay
        // cache. Reconcile every live cost before exposing a completed island.
        if (o_.delay_samples) for (int r : eligible_) {
            check();
            if (costs_[r] != p_.cost(r, paths_[r])) throw std::logic_error("rolling-window delay cache disagrees with committed paths");
        }
        if (o_.temperature) {
            check();
            if (p_.score(paths_) != current_cost || (o_.progress_ties && p_.remaining_score(paths_) != current_remaining))
                throw std::logic_error("annealed window walk cost disagrees with committed paths");
            if (p_.score(incumbent) != best_cost || (o_.progress_ties && p_.remaining_score(incumbent) != best_remaining))
                throw std::logic_error("annealed window retained best cost disagrees with snapshot");
            p_.validate(incumbent, check);
            stats.incumbent_restores = paths_ != incumbent;
            paths_ = std::move(incumbent);
            std::fill(owners_.begin(), owners_.end(), -1);
            for (int r = 0; r < int(paths_.size()); ++r) reserve(r, true);
            if (o_.delay_samples) for (int r : eligible_) costs_[r] = p_.cost(r, paths_[r]);
        }
        // No early best-so-far return: every declared attempt and validation
        // must finish. A deadline exception leaves completed false.
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
    std::vector<int64_t> costs_, lower_bounds_;
};

class RollingWindow {
public:
    template<class Check>
    std::vector<WindowPath> solve(const WindowProblem& p, const WindowOptions& options, int tick,
                                 const std::vector<uint64_t>& seeds, WindowStats& stats, Check check, const WindowRollout& rollout = {}) {
        stats = {};
        if (options.horizon != p.horizon || options.horizon < 1 || options.horizon > 32 ||
            options.keep < 0 || options.keep >= p.horizon || options.iterations < 1 || options.nodes < 1 ||
            options.group < 1 || options.workers < 1 || options.workers > 32 || options.threads < 1 ||
            options.threads > options.workers || options.delay_samples < 0 || options.delay_samples > 16 ||
            options.temperature < 0 || options.temperature > 65536 || options.merge < 0 || options.merge > 1 ||
            seeds.size() != size_t(options.workers))
            throw std::invalid_argument("invalid rolling-window work declaration");
        if (options.history_rollout < 0 || options.history_rollout > 1 ||
            (options.history_rollout && (options.keep < 5 || !options.seed_rollout || !rollout.geometry || !rollout.order || rollout.displacement_limit < 1)))
            throw std::invalid_argument("history rollout requires a kept prefix of at least five actions and complete joint continuation context");
        p.validate(p.seed, check);
        auto initial = p.seed;
        stats.seed_cost = p.score(initial); stats.seed_remaining = p.remaining_score(initial);
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
                    valid = valid && p.permits(r, candidate[r][t - 1], candidate[r][t]) &&
                        (t > p.prefix(r) || candidate[r][t] == p.seed[r][t]) &&
                        (t != 1 || p.first_cell(r) < 0 || candidate[r][t] / 4 == p.first_cell(r));
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
            if (options.history_rollout && std::count(retained.begin(), retained.end(), true)) {
                auto forecast = p; forecast.seed = candidate;
                stats.history_batches = extend_window_seed(forecast, *rollout.geometry, *rollout.order,
                    rollout.displacement_limit, rollout.seed, check, options.keep);
                for (size_t r = 0; r < candidate.size(); ++r) for (int t = 0; t <= options.keep; ++t)
                    if (forecast.seed[r][t] != candidate[r][t]) throw std::logic_error("history continuation changed a retained prefix");
                p.validate(forecast.seed, check); candidate = std::move(forecast.seed);
            }
            const int64_t cost = p.score(candidate), remaining = p.remaining_score(candidate);
            if (cost < stats.seed_cost || (cost == stats.seed_cost && (!options.progress_ties || remaining <= stats.seed_remaining))) {
                initial = std::move(candidate); stats.retained = std::count(retained.begin(), retained.end(), true);
            }
        }
        stats.initial_cost = p.score(initial); stats.initial_remaining = p.remaining_score(initial);
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
        int best = 0; int64_t best_cost = p.score(results[0]->paths()), best_remaining = p.remaining_score(results[0]->paths());
        for (int id = 0; id < options.workers; ++id) {
            if (!results[id]->stats.completed) throw std::logic_error("partial rolling-window result");
            stats.merge(results[id]->stats);
            const int64_t cost = p.score(results[id]->paths()), remaining = p.remaining_score(results[id]->paths());
            if (cost < best_cost || (options.progress_ties && cost == best_cost && remaining < best_remaining)) {
                best = id; best_cost = cost; best_remaining = remaining;
            }
        }
        auto result = results[best]->paths(); stats.pre_merge_cost = best_cost;
        if (options.merge) {
            for (int id = 0; id < options.workers; ++id) if (id != best)
                merge_window_donor(p, result, results[id]->paths(), options.progress_ties, stats, check);
            best_cost = p.score(result); best_remaining = p.remaining_score(result);
            if (stats.merge_donors != options.workers - 1 || best_cost + stats.merge_gain != stats.pre_merge_cost)
                throw std::logic_error("rolling-window merge did not complete its declared donors");
        }
        p.validate(result, check); check();
        if (best_cost > stats.initial_cost) throw std::logic_error("rolling-window objective regressed");
        if (options.progress_ties && best_cost == stats.initial_cost && best_remaining > stats.initial_remaining)
            throw std::logic_error("rolling-window tie regressed terminal progress");
        stats.final_cost = best_cost; stats.final_remaining = best_remaining; stats.selected_worker = best;
        for (size_t r = 0; r < result.size(); ++r) {
            stats.changed_first += result[r][1] != p.seed[r][1]; stats.protected_robots += bool(p.fixed[r] || p.prefix(r));
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
