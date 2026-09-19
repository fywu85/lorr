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
    int batch = 128, expansions = 4096, lookahead = 8, reconnect_steps = 0;
    int base_cost = 16, opposite_cost = 1, load_cost = 0, heuristic_weight = 1;
};

struct GuideRouteStats {
    int attempted = 0, solved = 0, limited = 0, invalidated = 0, windows = 0, active = 0;
    int goal_resets = 0, protected_resets = 0, deviation_resets = 0;
    int reconnect_attempts = 0, reconnected = 0;
    long long reconnect_actions = 0;
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
            options.reconnect_steps < 0 || options.reconnect_steps > 128)
            throw std::invalid_argument("invalid guide-route configuration");
        core_ = core; rows_ = rows; cols_ = cols; options_ = options;
        routes_.clear(); routes_.resize(robots); cursor_ = 0; epoch_ = join_epoch_ = 0; uses_ = 0;
        flow_.assign(core.size() * 4, 0); stamp_.assign(core.size() * 4, 0);
        distance_.resize(core.size() * 4); parent_.resize(core.size() * 4);
        join_stamp_.assign(core.size(), 0); join_index_.assign(core.size(), 0);
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
        std::vector<char> admitted(n, false);
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
    static constexpr uint16_t unreachable = 65535;
    struct Window {
        int x0 = 0, x1 = -1, y0 = 0, y1 = -1, width = 0, suffix = 0;
        std::vector<uint16_t> dist;
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

    // Reuse a still-current route after a nearby execution deviation. Descend
    // its complete local unit-distance field until first touching the remaining
    // route, align with its outgoing orientation, and retain the full suffix.
    // Only the replaced prefix changes flow counts. Every published path is
    // complete; failure to connect within the fixed action count changes nothing.
    template<class Deadline>
    bool reconnect(int robot, int cell, int orientation, Deadline check, GuideRouteStats& stats) {
        auto& route = routes_[robot];
        int potential = distance(robot, cell, orientation);
        if (potential < 0) return false;
        if (++join_epoch_ == 0) { std::fill(join_stamp_.begin(), join_stamp_.end(), 0); ++join_epoch_; }
        for (size_t k = route.begin; k < route.states.size(); ++k) {
            if (!(k % 256)) check();
            join_stamp_[route.states[k] / 4] = join_epoch_; join_index_[route.states[k] / 4] = k;
        }
        std::vector<int> connector{cell * 4 + orientation};
        for (;;) {
            const int used = int(connector.size()) - 1;
            if (!(used % 16)) check();
            if (join_stamp_[cell] == join_epoch_) {
                const size_t join = join_index_[cell];
                if (join + 1 < route.states.size()) {
                    const int target_orientation = route.states[join] % 4;
                    const int delta = (target_orientation - orientation + 4) % 4;
                    const int turns = delta == 3 ? 1 : delta;
                    if (used + turns > options_.reconnect_steps) return false;
                    for (int k = 0; k < turns; ++k) {
                        orientation = (orientation + (delta == 3 ? 3 : 1)) % 4;
                        connector.push_back(cell * 4 + orientation);
                    }
                }
                std::vector<int> joined = connector;
                joined.reserve(connector.size() + route.states.size() - join - 1);
                for (size_t k = join + 1; k < route.states.size(); ++k) {
                    if (!(k % 256)) check();
                    joined.push_back(route.states[k]);
                }
                check();  // Finish all interruptible preparation before replacing counts.
                for (size_t k = route.begin + 1; k <= join; ++k)
                    change_edge(route.states[k - 1], route.states[k], -1);
                for (size_t k = 1; k < connector.size(); ++k) change_edge(connector[k - 1], connector[k], 1);
                route.states = std::move(joined); route.begin = route.waypoint = 0; route.window.dist.clear();
                ++stats.reconnected; stats.reconnect_actions += connector.size() - 1;
                return true;
            }
            if (used == options_.reconnect_steps) return false;
            const int forward = neighbor(cell, orientation);
            bool moved = false;
            for (int action = 0; action < 3; ++action) {
                const int to = action == 0 ? forward : cell;
                const int dir = action == 0 ? orientation : (orientation + (action == 1 ? 1 : 3)) % 4;
                if (to < 0 || !core_[to]) continue;
                const int value = distance(robot, to, dir);
                if (value < 0 || value + 1 != potential) continue;
                cell = to; orientation = dir; potential = value; connector.push_back(cell * 4 + orientation);
                moved = true; break;
            }
            if (!moved) throw std::logic_error("guide reconnection lost a finite descending action");
        }
    }

    template<class Heuristic, class Deadline>
    std::vector<int> search(int start, int goal, Heuristic heuristic, Deadline check, GuideRouteStats& stats) {
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
                const int forward = flow_[node.state], reverse = flow_[to * 4 + (dir + 2) % 4];
                const int64_t cost = options_.base_cost + int64_t(options_.opposite_cost) * reverse +
                                     int64_t(options_.load_cost) * (forward + reverse);
                relax(to * 4 + dir, node.state, node.g + cost);
            }
        }
        return {};
    }

    // A fixed local box contains the current-to-waypoint route (<= lookahead
    // actions) and every five-action candidate. Reverse unit BFS is complete
    // inside that box. Cache it until execution reaches/passes the waypoint.
    template<class Deadline>
    void build_window(Route& route, Deadline check) {
        const int target = route.states[route.waypoint], cell = target / 4;
        const int radius = options_.lookahead + 5;
        auto& w = route.window;
        w.x0 = std::max(0, cell % cols_ - radius); w.x1 = std::min(cols_ - 1, cell % cols_ + radius);
        w.y0 = std::max(0, cell / cols_ - radius); w.y1 = std::min(rows_ - 1, cell / cols_ + radius);
        w.width = w.x1 - w.x0 + 1;
        w.suffix = int(route.states.size() - route.waypoint - 1);
        w.dist.assign(size_t(w.width) * (w.y1 - w.y0 + 1) * 4, unreachable);
        queue_.clear();
        auto local = [&](int u, int d) { return ((u / cols_ - w.y0) * w.width + u % cols_ - w.x0) * 4 + d; };
        auto push = [&](int u, int d, int value) {
            if (u < 0 || !core_[u] || u % cols_ < w.x0 || u % cols_ > w.x1 || u / cols_ < w.y0 || u / cols_ > w.y1) return;
            const int index = local(u, d);
            if (w.dist[index] != unreachable) return;
            if (value >= unreachable) throw std::logic_error("guide window distance overflow");
            w.dist[index] = value; queue_.push_back(u * 4 + d);
        };
        if (route.waypoint + 1 == route.states.size()) for (int d = 0; d < 4; ++d) push(cell, d, 0);
        else push(cell, target % 4, 0);
        for (size_t head = 0; head < queue_.size(); ++head) {
            if (!(head % 256)) check();
            const int state = queue_[head], u = state / 4, dir = state % 4;
            const int value = w.dist[local(u, dir)] + 1;
            push(u, (dir + 1) % 4, value); push(u, (dir + 3) % 4, value);
            push(neighbor(u, (dir + 2) % 4), dir, value);
        }
    }
    int rows_ = 0, cols_ = 0, cursor_ = 0;
    uint32_t epoch_ = 0, join_epoch_ = 0;
    long long uses_ = 0;
    GuideRouteOptions options_;
    std::vector<char> core_;
    std::vector<int> flow_, parent_, queue_;
    std::vector<uint32_t> stamp_, join_stamp_;
    std::vector<size_t> join_index_;
    std::vector<int64_t> distance_;
    std::vector<Route> routes_;
};
}  // namespace cgar
