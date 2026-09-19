#pragma once
// Online, per-agent intended routes. Only current goals and generic topology
// enter this module; routes guide scores and never reserve physical actions.
#include <algorithm>
#include <cstdint>
#include <limits>
#include <queue>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace cgar {
struct GuideRouteOptions {
    int batch = 128, expansions = 4096, lookahead = 8, reconnect_steps = 0, refine_batch = 0;
    int base_cost = 16, opposite_cost = 1, load_cost = 0, heuristic_weight = 1;
};

struct GuideRouteStats {
    int attempted = 0, solved = 0, limited = 0, invalidated = 0, windows = 0, active = 0;
    int goal_resets = 0, protected_resets = 0, deviation_resets = 0;
    int reconnect_attempts = 0, reconnected = 0;
    long long reconnect_actions = 0, reconnect_expanded = 0;
    int refine_attempted = 0, refined = 0, refine_limited = 0;
    long long refine_expanded = 0, refine_cost_saved = 0;
    long long expanded = 0, directed_uses = 0;
};

class GuideRoutes {
public:
    void initialize(const std::vector<char>& core, int rows, int cols, int robots,
                    GuideRouteOptions options = {}) {
        if (rows < 1 || cols < 1 || core.size() != size_t(rows) * cols || robots < 0 ||
            options.batch < 1 || options.batch > 4096 || options.expansions < 1 || options.expansions > 1000000 ||
            options.lookahead < 1 || options.lookahead > 32 || options.base_cost < 1 || options.base_cost > 1024 ||
            options.opposite_cost < 0 || options.opposite_cost > 64 || options.load_cost < 0 || options.load_cost > 64 ||
            options.heuristic_weight < 1 || options.heuristic_weight > 8 ||
            options.reconnect_steps < 0 || options.reconnect_steps > 128 ||
            options.refine_batch < 0 || options.refine_batch > 4096)
            throw std::invalid_argument("invalid guide-route configuration");
        core_ = core; rows_ = rows; cols_ = cols; options_ = options;
        routes_.clear(); routes_.resize(robots); cursor_ = refine_cursor_ = 0; epoch_ = join_epoch_ = own_epoch_ = 0; uses_ = 0;
        flow_.assign(core.size() * 4, 0); stamp_.assign(core.size() * 4, 0);
        distance_.resize(core.size() * 4); parent_.resize(core.size() * 4);
        join_stamp_.assign(core.size(), 0); join_index_.assign(core.size(), 0);
        own_stamp_.assign(options.refine_batch ? core.size() * 4 : 0, 0);
        own_flow_.assign(own_stamp_.size(), 0);
    }

    // An attempt that exhausts its prescribed expansion count produces no
    // guide. The ordinary complete temporal planner still runs in full.
    // Deadline exceptions propagate: there is no elapsed-time early success.
    template<class Heuristic, class Deadline>
    GuideRouteStats update(const std::vector<int>& locations, const std::vector<int>& orientations,
                           const std::vector<int>& goals, const std::vector<char>& eligible,
                           Heuristic heuristic, Deadline check) {
        const int n = routes_.size();
        if (locations.size() != size_t(n) || orientations.size() != size_t(n) ||
            goals.size() != size_t(n) || eligible.size() != size_t(n))
            throw std::invalid_argument("guide-route state size mismatch");
        GuideRouteStats stats;
        std::vector<char> admitted(n, false), created(options_.refine_batch ? n : 0, false);
        for (int r = 0; r < n; ++r) {
            if (!(r % 64)) check();
            const int from = locations[r], goal = goals[r];
            admitted[r] = eligible[r] && from >= 0 && from < int(core_.size()) && core_[from] &&
                          goal >= 0 && goal < int(core_.size()) && core_[goal];
            if (orientations[r] < 0 || orientations[r] > 3) throw std::invalid_argument("invalid guide orientation");
            auto& route = routes_[r];
            if (!admitted[r] || (!route.states.empty() && route.goal != goal)) {
                if (!route.states.empty()) {
                    invalidate(route); ++stats.invalidated;
                    if (!admitted[r]) ++stats.protected_resets; else ++stats.goal_resets;
                }
            }
            if (route.states.empty()) continue;
            size_t found = route.begin;
            while (found < route.states.size() && route.states[found] / 4 != from) ++found;
            if (found == route.states.size()) {
                if (options_.reconnect_steps) {
                    ++stats.reconnect_attempts;
                    if (reconnect(r, from, orientations[r], check, stats)) continue;
                }
                invalidate(route); ++stats.invalidated; ++stats.deviation_resets; continue;
            }
            // Rotations do not contribute edge flow; consume them at the same
            // cell even when execution chose a different equivalent heading.
            while (found + 1 < route.states.size() && route.states[found + 1] / 4 == from) ++found;
            while (route.begin < found) {
                change_edge(route.states[route.begin], route.states[route.begin + 1], -1); ++route.begin;
            }
        }
        for (int scanned = 0; scanned < n && stats.attempted < options_.batch; ++scanned) {
            const int r = cursor_; cursor_ = (cursor_ + 1) % n;
            if (!admitted[r] || !routes_[r].states.empty()) continue;
            check(); ++stats.attempted;
            auto path = search(locations[r] * 4 + orientations[r], goals[r],
                [&](int state) { return heuristic(r, state / 4, state % 4); }, check, stats);
            if (path.empty()) continue;
            auto& route = routes_[r]; route.goal = goals[r]; route.begin = 0;
            route.states = std::move(path);
            for (size_t k = 1; k < route.states.size(); ++k) change_edge(route.states[k - 1], route.states[k], 1);
            ++stats.solved;
            if (options_.refine_batch) created[r] = true;
        }
        // Reconsider only pre-existing complete routes, in a separate fair
        // fixed-count rotation. No route is changed unless a complete search
        // strictly improves its cost with its own remaining flow excluded.
        for (int scanned = 0; scanned < n && stats.refine_attempted < options_.refine_batch; ++scanned) {
            const int r = refine_cursor_; refine_cursor_ = (refine_cursor_ + 1) % n;
            if (!admitted[r] || created[r] || routes_[r].states.empty() || locations[r] == goals[r]) continue;
            check(); ++stats.refine_attempted;
            refine(r, locations[r] * 4 + orientations[r],
                [&](int state) { return heuristic(r, state / 4, state % 4); }, check, stats);
        }
        for (int r = 0; r < n; ++r) {
            if (!(r % 64)) check();
            auto& route = routes_[r];
            if (route.states.empty()) continue;
            ++stats.active;
            if (route.window.dist.empty() || route.begin >= route.waypoint) {
                route.waypoint = std::min(route.begin + size_t(options_.lookahead), route.states.size() - 1);
                build_window(route, check); ++stats.windows;
            }
            if (distance(r, locations[r], orientations[r]) < 0)
                throw std::logic_error("guide window lost its current route position");
        }
        stats.directed_uses = uses_;
        return stats;
    }

    bool guided(int robot) const { return !routes_.at(robot).window.dist.empty(); }
    int distance(int robot, int cell, int orientation) const {
        const auto& w = routes_.at(robot).window;
        if (cell < 0 || cell >= int(core_.size()) || orientation < 0 || orientation > 3 || w.dist.empty()) return -1;
        const int x = cell % cols_, y = cell / cols_;
        if (x < w.x0 || x > w.x1 || y < w.y0 || y > w.y1) return -1;
        const int d = w.dist[((y - w.y0) * w.width + x - w.x0) * 4 + orientation];
        return d == unreachable ? -1 : d + w.suffix;
    }
    const std::vector<int>& flow() const { return flow_; }
    const std::vector<int>& route_states(int robot) const { return routes_.at(robot).states; }
    size_t route_begin(int robot) const { return routes_.at(robot).begin; }
    long long directed_uses() const { return uses_; }

private:
    static constexpr int unreachable = std::numeric_limits<int>::max();
    struct Window {
        int x0 = 0, x1 = -1, y0 = 0, y1 = -1, width = 0, suffix = 0;
        std::vector<int> dist;
    };
    struct Route {
        int goal = -1;
        size_t begin = 0, waypoint = 0;
        std::vector<int> states;
        Window window;
    };
    struct Node {
        int64_t f, g; int state;
        bool operator<(const Node& other) const {
            return std::make_tuple(f, -g, state) > std::make_tuple(other.f, -other.g, other.state);
        }
    };
    int neighbor(int cell, int direction) const {
        if (direction == 0) return cell % cols_ + 1 < cols_ ? cell + 1 : -1;
        if (direction == 1) return cell / cols_ + 1 < rows_ ? cell + cols_ : -1;
        if (direction == 2) return cell % cols_ > 0 ? cell - 1 : -1;
        return cell / cols_ > 0 ? cell - cols_ : -1;
    }
    void change_edge(int from, int to, int amount) {
        if (from / 4 == to / 4) return;
        if (from % 4 != to % 4 || neighbor(from / 4, from % 4) != to / 4)
            throw std::logic_error("nonadjacent intended route edge");
        flow_[from] += amount; uses_ += amount;
        if (flow_[from] < 0 || uses_ < 0) throw std::logic_error("negative intended flow");
    }
    void invalidate(Route& route) {
        for (size_t k = route.begin + 1; k < route.states.size(); ++k)
            change_edge(route.states[k - 1], route.states[k], -1);
        route.states.clear(); route.window.dist.clear(); route.goal = -1; route.begin = route.waypoint = 0;
    }

    // Find the shortest connector to an aligned state on the remaining route,
    // within a prescribed action count and the cached local box. A route-to-go
    // potential may prefer moving alongside the route until a distant rejoin;
    // connector length must therefore be solved independently of that score.
    template<class Deadline>
    bool reconnect(int robot, int cell, int orientation, Deadline check, GuideRouteStats& stats) {
        auto& route = routes_[robot];
        if (distance(robot, cell, orientation) < 0) return false;
        if (++join_epoch_ == 0) { std::fill(join_stamp_.begin(), join_stamp_.end(), 0); ++join_epoch_; }
        for (size_t k = route.begin; k < route.states.size(); ++k) {
            if (!(k % 256)) check();
            join_stamp_[route.states[k] / 4] = join_epoch_; join_index_[route.states[k] / 4] = k;
        }
        if (++epoch_ == 0) { std::fill(stamp_.begin(), stamp_.end(), 0); ++epoch_; }
        const int start = cell * 4 + orientation;
        stamp_[start] = epoch_; distance_[start] = 0; parent_[start] = -1;
        reconnect_queue_.clear(); reconnect_queue_.push_back(start);
        for (size_t head = 0; head < reconnect_queue_.size(); ++head) {
            if (!(head % 256)) check();
            const int state = reconnect_queue_[head], u = state / 4, dir = state % 4;
            ++stats.reconnect_expanded;
            if (join_stamp_[u] == join_epoch_) {
                const size_t join = join_index_[u];
                if (join + 1 == route.states.size() || dir == route.states[join] % 4) {
                    std::vector<int> connector;
                    for (int v = state; v >= 0; v = parent_[v]) connector.push_back(v);
                    std::reverse(connector.begin(), connector.end());
                    std::vector<int> joined = connector;
                    joined.reserve(connector.size() + route.states.size() - join - 1);
                    for (size_t k = join + 1; k < route.states.size(); ++k) {
                        if (!(k % 256)) check();
                        joined.push_back(route.states[k]);
                    }
                    check(); // Complete preparation before replacing any flow/path.
                    for (size_t k = route.begin + 1; k <= join; ++k)
                        change_edge(route.states[k - 1], route.states[k], -1);
                    for (size_t k = 1; k < connector.size(); ++k) change_edge(connector[k - 1], connector[k], 1);
                    route.states = std::move(joined); route.begin = route.waypoint = 0; route.window.dist.clear();
                    ++stats.reconnected; stats.reconnect_actions += connector.size() - 1;
                    return true;
                }
            }
            if (distance_[state] == options_.reconnect_steps) continue;
            auto push = [&](int to, int direction) {
                if (to < 0 || !core_[to]) return;
                const auto& w = route.window;
                if (to % cols_ < w.x0 || to % cols_ > w.x1 || to / cols_ < w.y0 || to / cols_ > w.y1) return;
                const int next = to * 4 + direction;
                if (stamp_[next] == epoch_) return;
                stamp_[next] = epoch_; distance_[next] = distance_[state] + 1; parent_[next] = state;
                reconnect_queue_.push_back(next);
            };
            push(neighbor(u, dir), dir); push(u, (dir + 1) % 4); push(u, (dir + 3) % 4);
        }
        return false;
    }

    int64_t edge_cost(int from, int to, uint32_t excluded = 0) const {
        if (from / 4 == to / 4) return options_.base_cost;
        const int reverse_edge = (to / 4) * 4 + (from % 4 + 2) % 4;
        auto count = [&](int edge) {
            const int value = flow_[edge] - (excluded && own_stamp_[edge] == excluded ? own_flow_[edge] : 0);
            if (value < 0) throw std::logic_error("guide refinement subtracted absent self flow");
            return value;
        };
        const int forward = count(from), reverse = count(reverse_edge);
        return options_.base_cost + int64_t(options_.opposite_cost) * reverse +
               int64_t(options_.load_cost) * (forward + reverse);
    }

    template<class Heuristic, class Deadline>
    void refine(int robot, int start, Heuristic heuristic, Deadline check, GuideRouteStats& stats) {
        auto& route = routes_[robot];
        if (route.states[route.begin] / 4 != start / 4)
            throw std::logic_error("guide refinement did not start on its current route");
        if (++own_epoch_ == 0) { std::fill(own_stamp_.begin(), own_stamp_.end(), 0); ++own_epoch_; }
        // Sparse self counts handle even repeated connector edges without
        // mutating the shared flow. Interrupted/limited searches retain every
        // old count and the complete old path without a rollback operation.
        for (size_t k = route.begin + 1; k < route.states.size(); ++k) {
            if (!(k % 256)) check();
            const int a = route.states[k - 1], b = route.states[k];
            if (a / 4 == b / 4) continue;
            if (own_stamp_[a] != own_epoch_) { own_stamp_[a] = own_epoch_; own_flow_[a] = 0; }
            ++own_flow_[a];
        }
        const int delta = (route.states[route.begin] % 4 - start % 4 + 4) % 4;
        int64_t old_cost = int64_t(std::min(delta, 4 - delta)) * options_.base_cost;
        for (size_t k = route.begin + 1; k < route.states.size(); ++k) {
            if (!(k % 256)) check();
            old_cost += edge_cost(route.states[k - 1], route.states[k], own_epoch_);
        }
        GuideRouteStats search_stats;
        auto path = search(start, route.goal, heuristic, check, search_stats, own_epoch_);
        stats.refine_limited += search_stats.limited;
        stats.refine_expanded += search_stats.expanded; stats.expanded += search_stats.expanded;
        if (path.empty()) return;
        int64_t new_cost = 0;
        for (size_t k = 1; k < path.size(); ++k) {
            if (!(k % 256)) check();
            new_cost += edge_cost(path[k - 1], path[k], own_epoch_);
        }
        if (new_cost >= old_cost) return;
        check(); // Finish interruptible preparation before replacing complete routes.
        for (size_t k = route.begin + 1; k < route.states.size(); ++k)
            change_edge(route.states[k - 1], route.states[k], -1);
        for (size_t k = 1; k < path.size(); ++k) change_edge(path[k - 1], path[k], 1);
        route.states = std::move(path); route.begin = route.waypoint = 0; route.window.dist.clear();
        ++stats.refined; stats.refine_cost_saved += old_cost - new_cost;
    }

    template<class Heuristic, class Deadline>
    std::vector<int> search(int start, int goal, Heuristic heuristic, Deadline check, GuideRouteStats& stats,
                            uint32_t excluded = 0) {
        if (++epoch_ == 0) { std::fill(stamp_.begin(), stamp_.end(), 0); ++epoch_; }
        std::priority_queue<Node> open;
        stamp_[start] = epoch_; distance_[start] = 0; parent_[start] = -1;
        open.push({int64_t(std::max(0, heuristic(start))) * options_.base_cost * options_.heuristic_weight, 0, start});
        int expanded = 0;
        auto relax = [&](int to, int from, int64_t value) {
            if (stamp_[to] == epoch_ && distance_[to] <= value) return;
            stamp_[to] = epoch_; distance_[to] = value; parent_[to] = from;
            open.push({value + int64_t(std::max(0, heuristic(to))) * options_.base_cost * options_.heuristic_weight, value, to});
        };
        while (!open.empty()) {
            const Node node = open.top(); open.pop();
            if (node.g != distance_[node.state]) continue;
            if (node.state / 4 == goal) {
                std::vector<int> path;
                for (int state = node.state; state >= 0; state = parent_[state]) path.push_back(state);
                std::reverse(path.begin(), path.end()); return path;
            }
            if (expanded == options_.expansions) { ++stats.limited; return {}; }
            if (!(expanded % 256)) check();
            ++expanded; ++stats.expanded;
            const int cell = node.state / 4, dir = node.state % 4;
            relax(cell * 4 + (dir + 1) % 4, node.state, node.g + options_.base_cost);
            relax(cell * 4 + (dir + 3) % 4, node.state, node.g + options_.base_cost);
            const int to = neighbor(cell, dir);
            if (to >= 0 && core_[to]) {
                const int64_t cost = edge_cost(node.state, to * 4 + dir, excluded);
                relax(to * 4 + dir, node.state, node.g + cost);
            }
        }
        return {};
    }

    // The box covers the current route prefix and every five-action candidate.
    // Seed EVERY remaining route state in the box with its full suffix length,
    // including states beyond the cached waypoint. Otherwise crossing that
    // waypoint is incorrectly penalized and makes candidates park on the route.
    // Seed values are sorted by traversing the route backwards; merge that
    // stream with a FIFO of unit-edge relaxations for exact multi-source BFS.
    template<class Deadline>
    void build_window(Route& route, Deadline check) {
        const int cell = route.states[route.waypoint] / 4;
        const int radius = options_.lookahead + 5;
        auto& w = route.window;
        w.x0 = std::max(0, cell % cols_ - radius); w.x1 = std::min(cols_ - 1, cell % cols_ + radius);
        w.y0 = std::max(0, cell / cols_ - radius); w.y1 = std::min(rows_ - 1, cell / cols_ + radius);
        w.width = w.x1 - w.x0 + 1; w.suffix = 0;
        w.dist.assign(size_t(w.width) * (w.y1 - w.y0 + 1) * 4, unreachable);
        queue_.clear(); seed_queue_.clear();
        auto local = [&](int u, int d) { return ((u / cols_ - w.y0) * w.width + u % cols_ - w.x0) * 4 + d; };
        auto inside = [&](int u) {
            return u >= 0 && core_[u] && u % cols_ >= w.x0 && u % cols_ <= w.x1 &&
                   u / cols_ >= w.y0 && u / cols_ <= w.y1;
        };
        for (size_t k = route.states.size(); k-- > route.begin;) {
            if (!(k % 256)) check();
            const int target = route.states[k], u = target / 4;
            if (!inside(u)) continue;
            const size_t suffix = route.states.size() - k - 1;
            if (suffix >= size_t(unreachable)) throw std::logic_error("guide route suffix distance overflow");
            if (!suffix) for (int d = 0; d < 4; ++d) seed_queue_.emplace_back(u * 4 + d, 0);
            else seed_queue_.emplace_back(target, int(suffix));
        }
        auto push = [&](int u, int d, int value) {
            if (!inside(u) || w.dist[local(u, d)] != unreachable) return;
            queue_.emplace_back(u * 4 + d, value);
        };
        size_t head = 0, seed = 0, popped = 0;
        while (head < queue_.size() || seed < seed_queue_.size()) {
            if (!(popped++ % 256)) check();
            const bool take_seed = seed < seed_queue_.size() &&
                (head == queue_.size() || seed_queue_[seed].second <= queue_[head].second);
            const auto item = take_seed ? seed_queue_[seed++] : queue_[head++];
            const int u = item.first / 4, dir = item.first % 4, index = local(u, dir);
            if (w.dist[index] != unreachable) continue;
            w.dist[index] = item.second;
            if (item.second >= unreachable - 1) throw std::logic_error("guide window distance overflow");
            const int next_value = item.second + 1;
            push(u, (dir + 1) % 4, next_value); push(u, (dir + 3) % 4, next_value);
            push(neighbor(u, (dir + 2) % 4), dir, next_value);
        }
    }
    int rows_ = 0, cols_ = 0, cursor_ = 0, refine_cursor_ = 0;
    uint32_t epoch_ = 0, join_epoch_ = 0, own_epoch_ = 0;
    long long uses_ = 0;
    GuideRouteOptions options_;
    std::vector<char> core_;
    std::vector<int> flow_, parent_, own_flow_, reconnect_queue_;
    std::vector<std::pair<int, int>> queue_, seed_queue_;
    std::vector<uint32_t> stamp_, join_stamp_, own_stamp_;
    std::vector<size_t> join_index_;
    std::vector<int64_t> distance_;
    std::vector<Route> routes_;
};
}  // namespace cgar
