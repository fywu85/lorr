#include <sched.h>
#include "cgar.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <numeric>
#include <queue>
#include <string>

namespace cgar {

namespace {

constexpr int kFar = 1 << 20;  // above any grid distance, below kInf

using Adj = std::vector<std::vector<int>>;

int env_int(const char* name, int fallback) {
    const char* v = std::getenv(name);
    return v ? std::atoi(v) : fallback;
}

int grid_neighbor(int cell, int dir, int rows, int cols) {
    const int r = cell / cols, c = cell % cols;
    switch (dir) {
        case 0: return c + 1 < cols ? cell + 1 : -1;
        case 1: return r + 1 < rows ? cell + cols : -1;
        case 2: return c > 0 ? cell - 1 : -1;
        default: return r > 0 ? cell - cols : -1;
    }
}

int direction(int from, int to, int cols) {
    const int diff = to - from;
    if (diff == 1) return 0;
    if (diff == cols) return 1;
    if (diff == -1) return 2;
    return 3;
}

// Largest strongly connected component induced by `active` (iterative Tarjan).
std::vector<char> largest_scc(const Adj& adj, const std::vector<char>& active) {
    const size_t n = adj.size();
    std::vector<int> index(n, -1), low(n, 0), comp(n, -1);
    std::vector<char> on_stack(n, 0);
    std::vector<int> stack;
    std::vector<size_t> comp_size;
    std::vector<std::pair<int, size_t>> call;
    int next_index = 0;
    for (size_t s = 0; s < n; ++s) {
        if (!active[s] || index[s] >= 0) continue;
        call.push_back({static_cast<int>(s), 0});
        index[s] = low[s] = next_index++;
        stack.push_back(static_cast<int>(s));
        on_stack[s] = 1;
        while (!call.empty()) {
            const int u = call.back().first;
            size_t& i = call.back().second;
            if (i < adj[u].size()) {
                const int v = adj[u][i++];
                if (!active[v]) continue;
                if (index[v] < 0) {
                    index[v] = low[v] = next_index++;
                    stack.push_back(v);
                    on_stack[v] = 1;
                    call.push_back({v, 0});
                } else if (on_stack[v]) {
                    low[u] = std::min(low[u], index[v]);
                }
            } else {
                call.pop_back();
                if (!call.empty()) low[call.back().first] = std::min(low[call.back().first], low[u]);
                if (low[u] == index[u]) {
                    size_t size = 0;
                    for (;;) {
                        const int w = stack.back();
                        stack.pop_back();
                        on_stack[w] = 0;
                        comp[w] = static_cast<int>(comp_size.size());
                        ++size;
                        if (w == u) break;
                    }
                    comp_size.push_back(size);
                }
            }
        }
    }
    std::vector<char> out(n, 0);
    if (comp_size.empty()) return out;
    const int best = static_cast<int>(std::max_element(comp_size.begin(), comp_size.end()) - comp_size.begin());
    for (size_t v = 0; v < n; ++v) out[v] = (comp[v] == best);
    return out;
}

// For the symmetric graph induced by a connected `core`, the size of the component
// of (core - u) that contains v, for every arc u->v in adjacency order. This equals
// KNAPP's directed_reachable_count(H, core, v, u) when H is symmetric (checked
// against knapp::pibt_directed_capacity_fixed_point on the 2024 random map).
std::vector<std::vector<int>> piece_sizes(const Adj& adj, const std::vector<char>& core) {
    const size_t n = adj.size();
    std::vector<int> tin(n, -1), tout(n, -1), low(n, 0), parent(n, -1), sz(n, 1);
    std::vector<std::vector<int>> children(n);
    std::vector<std::vector<int>> out(n);
    int total = 0, root = -1;
    for (size_t v = 0; v < n; ++v) {
        if (!core[v]) continue;
        ++total;
        if (root < 0) root = static_cast<int>(v);
    }
    if (root < 0) return out;
    int timer = 0;
    std::vector<std::pair<int, size_t>> call{{root, 0}};
    tin[root] = low[root] = timer++;
    while (!call.empty()) {
        const int u = call.back().first;
        size_t& i = call.back().second;
        if (i < adj[u].size()) {
            const int v = adj[u][i++];
            if (!core[v]) continue;
            if (tin[v] < 0) {
                parent[v] = u;
                children[u].push_back(v);
                tin[v] = low[v] = timer++;
                call.push_back({v, 0});
            } else if (v != parent[u]) {
                low[u] = std::min(low[u], tin[v]);
            }
        } else {
            tout[u] = timer - 1;
            call.pop_back();
            if (!call.empty()) {
                const int p = call.back().first;
                low[p] = std::min(low[p], low[u]);
                sz[p] += sz[u];
            }
        }
    }
    for (size_t u = 0; u < n; ++u) {
        if (!core[u]) continue;
        const bool is_root = static_cast<int>(u) == root;
        int separated = 0;
        for (int c : children[u]) {
            if (is_root || low[c] >= tin[u]) separated += sz[c];
        }
        const int rest = total - 1 - separated;
        out[u].reserve(adj[u].size());
        for (int v : adj[u]) {
            if (!core[v]) {
                out[u].push_back(0);
                continue;
            }
            const bool descendant = tin[v] > tin[u] && tin[v] <= tout[u];
            if (!descendant) {
                out[u].push_back(rest);
                continue;
            }
            auto it = std::upper_bound(children[u].begin(), children[u].end(), v,
                                       [&](int a, int b) { return tin[a] < tin[b]; });
            const int c = *std::prev(it);
            out[u].push_back((is_root || low[c] >= tin[u]) ? sz[c] : rest);
        }
    }
    return out;
}

}  // namespace

// ─── certificate ──────────────────────────────────────────────────────────────

Certificate build_certificate(const std::vector<int>& map, int rows, int cols, size_t robots) {
    Certificate cert;
    cert.rows = rows;
    cert.cols = cols;
    cert.robots = robots;
    const size_t cells = static_cast<size_t>(rows) * cols;
    cert.free.assign(cells, 0);
    for (size_t i = 0; i < cells; ++i) cert.free[i] = (map[i] == 0);

    Adj adj(cells);
    for (size_t u = 0; u < cells; ++u) {
        if (!cert.free[u]) continue;
        for (int d = 0; d < 4; ++d) {
            const int v = grid_neighbor(static_cast<int>(u), d, rows, cols);
            if (v >= 0 && cert.free[v]) adj[u].push_back(v);
        }
    }

    std::vector<char> core = largest_scc(adj, cert.free);
    Adj promotion;
    for (;;) {
        const auto pieces = piece_sizes(adj, core);
        promotion.assign(cells, {});
        for (size_t u = 0; u < cells; ++u) {
            if (!core[u]) continue;
            for (size_t k = 0; k < adj[u].size(); ++k) {
                const int v = adj[u][k];
                if (core[v] && static_cast<size_t>(pieces[u][k]) >= robots) promotion[u].push_back(v);
            }
        }
        std::vector<char> next = largest_scc(promotion, core);
        ++cert.iterations;
        if (next == core) break;
        core = std::move(next);
    }
    cert.core_size = std::count(core.begin(), core.end(), 1);
    for (const auto& out : promotion) cert.promotion_arcs += out.size();
    // Premises 4-6 hold by construction of the fixed point; premise 7 is |B| > N.
    cert.valid = cert.core_size > robots && cert.promotion_arcs > 0;
    if (!cert.valid) {
        // Fail open for the competition: no liveness floor, the whole connected
        // floor is routable, and every arc may be a promoted step.
        core = largest_scc(adj, cert.free);
        cert.core_size = std::count(core.begin(), core.end(), 1);
        promotion = adj;
        cert.promotion_arcs = 0;
        for (size_t u = 0; u < cells; ++u) {
            if (!core[u]) promotion[u].clear();
            cert.promotion_arcs += promotion[u].size();
        }
    }
    cert.core = core;

    cert.promotion.assign(cells, 0);
    for (size_t u = 0; u < cells; ++u) {
        for (int v : promotion[u]) cert.promotion[u] |= static_cast<uint8_t>(1u << direction(static_cast<int>(u), v, cols));
    }

    // Pockets: connected free off-core regions.
    cert.pocket.assign(cells, -1);
    std::vector<int> queue;
    for (size_t s = 0; s < cells; ++s) {
        if (!cert.free[s] || cert.core[s] || cert.pocket[s] >= 0) continue;
        const int id = static_cast<int>(cert.pocket_cells.size());
        cert.pocket_cells.emplace_back();
        queue.assign(1, static_cast<int>(s));
        cert.pocket[s] = id;
        for (size_t head = 0; head < queue.size(); ++head) {
            const int u = queue[head];
            cert.pocket_cells[id].push_back(u);
            for (int v : adj[u]) {
                if (!cert.core[v] && cert.pocket[v] < 0) {
                    cert.pocket[v] = id;
                    queue.push_back(v);
                }
            }
        }
    }

    // Exit distances from off-core cells to the core, staying inside the pocket.
    cert.exit_dist.assign(cells, kInf);
    cert.exit_cell.assign(cells, -1);
    queue.clear();
    for (size_t u = 0; u < cells; ++u) {
        if (!cert.core[u]) continue;
        cert.exit_dist[u] = 0;
        cert.exit_cell[u] = static_cast<int>(u);
    }
    for (size_t u = 0; u < cells; ++u) {
        if (!cert.free[u] || cert.core[u]) continue;
        for (int v : adj[u]) {
            if (cert.core[v]) {
                cert.exit_dist[u] = 1;
                cert.exit_cell[u] = v;
                queue.push_back(static_cast<int>(u));
                break;
            }
        }
    }
    for (size_t head = 0; head < queue.size(); ++head) {
        const int u = queue[head];
        for (int v : adj[u]) {
            if (cert.core[v] || cert.exit_dist[v] <= cert.exit_dist[u] + 1) continue;
            cert.exit_dist[v] = cert.exit_dist[u] + 1;
            cert.exit_cell[v] = cert.exit_cell[u];
            queue.push_back(v);
        }
    }
    return cert;
}

Certificate build_certificate_feasible(const std::vector<int>& map, int rows, int cols, size_t robots) {
    Certificate cert = build_certificate(map, rows, cols, robots);
    cert.capacity = robots;
    if (cert.valid) return cert;
    // The fleet does not fit the certified core. Keep the structure anyway: binary
    // search the largest capacity that still yields a well-formed core, so pockets
    // and their locks exist. Without them a dead-end can deadlock the fleet.
    size_t low = 0, high = robots;
    Certificate best = build_certificate(map, rows, cols, 0);
    best.capacity = 0;
    while (low + 1 < high) {
        const size_t mid = low + (high - low) / 2;
        Certificate probe = build_certificate(map, rows, cols, mid);
        if (probe.valid) {
            probe.capacity = mid;
            best = std::move(probe);
            low = mid;
        } else {
            high = mid;
        }
    }
    best.valid = false;  // the real fleet is still uncertified
    return best;
}

// ─── distances ────────────────────────────────────────────────────────────────

void DistanceOracle::init(const Certificate* cert, size_t max_bytes) {
    cert_ = cert;
    max_bytes_ = max_bytes;
    index_.assign(cert->free.size(), -1);
    cells_.clear();
    for (size_t cell = 0; cell < cert->free.size(); ++cell) {
        if (!cert->free[cell]) continue;
        index_[cell] = static_cast<int>(cells_.size());
        cells_.push_back(static_cast<int>(cell));
    }
    table_bytes_ = std::max<size_t>(1, cells_.size() * sizeof(int));
    neighbors_.assign(cells_.size(), {});
    for (size_t u = 0; u < cells_.size(); ++u) {
        for (int d = 0; d < 4; ++d) {
            const int cell = grid_neighbor(cells_[u], d, cert->rows, cert->cols);
            if (cell >= 0 && index_[cell] >= 0) neighbors_[u].push_back(index_[cell]);
        }
    }
    tables_.clear();
    lru_.clear();
    queue_.reserve(cells_.size());
}

const std::vector<int>& DistanceOracle::table(int goal) {
    return *try_table(goal, std::chrono::steady_clock::time_point::max());
}

const std::vector<int>* DistanceOracle::try_table(int goal, std::chrono::steady_clock::time_point deadline) {
    check_deadline(deadline, "distance_table");
    if (const auto* cached = find(goal)) return cached;
    std::vector<int> dist(cells_.size(), kInf);
    const int goal_pocket = cert_->pocket[goal];
    const int start = index_[goal];
    queue_.assign(1, start);
    dist[start] = 0;
    for (size_t head = 0; head < queue_.size(); ++head) {
        if ((head & 1023) == 0) check_deadline(deadline, "distance_table");
        const int u = queue_[head];
        for (int v : neighbors_[u]) {
            if (dist[v] != kInf) continue;
            const int cell = cells_[v];
            if (!cert_->core[cell] && cert_->pocket[cell] != goal_pocket) continue;
            dist[v] = dist[u] + 1;
            queue_.push_back(v);
        }
    }
    check_deadline(deadline, "distance_table_complete");
    // Only complete tables enter the cache: partial BFS results cannot certify
    // a decreasing potential. Compact indices avoid caching obstacle cells.
    lru_.push_front(goal);
    auto result = tables_.emplace(goal, Entry{std::move(dist), lru_.begin()});
    return &result.first->second.dist;
}

int DistanceOracle::value(const std::vector<int>& table, int cell) const {
    return index_[cell] < 0 ? kInf : table[index_[cell]];
}

const std::vector<int>* DistanceOracle::find(int goal) {
    auto it = tables_.find(goal);
    if (it == tables_.end()) return nullptr;
    lru_.splice(lru_.begin(), lru_, it->second.lru);
    return &it->second.dist;
}

const std::vector<int>* DistanceOracle::peek(int goal) const {
    const auto it = tables_.find(goal);
    return it == tables_.end() ? nullptr : &it->second.dist;
}

int DistanceOracle::dist(int from, int goal) {
    return distance_from(table(goal), from);
}

int DistanceOracle::distance_from(const std::vector<int>& t, int from) const {
    if (value(t, from) < kInf) return value(t, from);
    const int exit = cert_->exit_cell[from];
    if (exit >= 0 && value(t, exit) < kInf) return cert_->exit_dist[from] + value(t, exit);
    return kInf;
}

int DistanceOracle::manhattan(int a, int b) const {
    return std::abs(a / cert_->cols - b / cert_->cols) + std::abs(a % cert_->cols - b % cert_->cols);
}

void DistanceOracle::trim() {
    while (!lru_.empty() && tables_.size() * table_bytes_ > max_bytes_) {
        tables_.erase(lru_.back());
        lru_.pop_back();
    }
}

void TurnDistanceOracle::init(const Certificate* cert, size_t max_bytes, int turn_cost, bool compact, int forward_base, int cost_limit) {
    if (!cert || cost_limit < 1 || cost_limit > 255 || turn_cost < 1 || turn_cost > cost_limit ||
        forward_base < 1 || forward_base > cost_limit || cert->free.size() > size_t((kInf - cost_limit) / (4 * cost_limit)))
        throw std::invalid_argument("turn/base guidance costs or graph exceed the explicit positive integer cost bound");
    cost_limit_ = cost_limit; wide_fallback_tables = 0;
    cert_ = cert; max_bytes_ = max_bytes; turn_cost_ = turn_cost; compact_ = compact; forward_base_ = forward_base;
    max_edge_cost_ = std::max(turn_cost_, forward_base_); forward_costs_.clear();
    buckets_.assign(max_edge_cost_ + 1, {});
    index_.assign(cert->free.size(), -1); cells_.clear();
    for (size_t u = 0; u < cert->free.size(); ++u) if (cert->free[u]) {
        index_[u] = static_cast<int>(cells_.size()); cells_.push_back(u);
    }
    // Reverse-forward topology is invariant across goals and guidance costs.
    // Cache compact predecessor states once; pocket eligibility remains a
    // per-goal check in compute(), exactly as in the direct neighbor traversal.
    backward_.assign(cells_.size() * 4, -1);
    for (size_t u = 0; u < cells_.size(); ++u) for (int d = 0; d < 4; ++d) {
        const int from = grid_neighbor(cells_[u], (d + 2) % 4, cert->rows, cert->cols);
        if (from >= 0 && index_[from] >= 0) backward_[u * 4 + d] = index_[from] * 4 + d;
    }
    table_bytes_ = std::max<size_t>(1, cells_.size() * 4 * sizeof(int));
    tables_.clear(); lru_.clear(); queue_.reserve(cells_.size() * 4);
    prefetched_.clear(); prefetched_builds = prefetched_hits = prefetched_discarded = 0;
}

void TurnDistanceOracle::clear_tables() {
    tables_.clear(); lru_.clear(); discard_prefetch();
}

bool TurnDistanceOracle::set_forward_costs(std::vector<uint8_t> costs) {
    if (!cert_ || costs.size() != cert_->free.size() * 4)
        throw std::invalid_argument("invalid forward guidance dimensions");
    int maximum = forward_base_;
    for (int cost : costs) {
        if (cost < forward_base_ || cost > cost_limit_) throw std::invalid_argument("forward guidance cost exceeds the explicit base/cost bound");
        maximum = std::max(maximum, cost);
    }
    if (maximum == forward_base_) costs.clear();
    if (costs == forward_costs_) return false;
    // No old-metric table or speculative result may survive a metric change.
    clear_tables();
    forward_costs_ = std::move(costs); max_edge_cost_ = std::max(turn_cost_, maximum);
    buckets_.assign(max_edge_cost_ + 1, {});
    return true;
}

const TurnTable* TurnDistanceOracle::find(int goal) {
    const auto it = tables_.find(goal);
    if (it == tables_.end()) return nullptr;
    lru_.splice(lru_.begin(), lru_, it->second.lru);
    return &it->second.dist;
}

std::vector<int> TurnDistanceOracle::compute(int goal, std::chrono::steady_clock::time_point deadline,
                                             std::vector<int>& queue, std::vector<std::vector<int>>& buckets) const {
    check_deadline(deadline, "turn_distance_table");
    std::vector<int> dist(cells_.size() * 4, kInf); queue.clear();
    const int gp = cert_->pocket[goal], root = index_.at(goal) * 4;
    auto predecessors = [&](int node) {
        const int d = node % 4, backward = backward_[node];
        std::array<int, 3> pred{node / 4 * 4 + (d + 1) % 4, node / 4 * 4 + (d + 3) % 4, -1};
        if (backward >= 0) {
            const int cell = cells_[backward / 4];
            if (cert_->core[cell] || cert_->pocket[cell] == gp) pred[2] = backward;
        }
        return pred;
    };
    if (turn_cost_ == forward_base_ && forward_costs_.empty()) {
        // Preserve uniform-cost traversal order, including scaled unit costs.
        for (int d = 0; d < 4; ++d) { dist[root + d] = 0; queue.push_back(root + d); }
        for (size_t head = 0; head < queue.size(); ++head) {
            if ((head & 1023) == 0) check_deadline(deadline, "turn_distance_table");
            const int node = queue[head];
            for (int v : predecessors(node)) if (v >= 0 && dist[v] == kInf) {
                dist[v] = dist[node] + forward_base_; queue.push_back(v);
            }
        }
    } else {
        // Dial's bounded-integer Dijkstra: no heap and no partially cached table.
        for (auto& bucket : buckets) bucket.clear();
        for (int d = 0; d < 4; ++d) { dist[root + d] = 0; buckets[0].push_back(root + d); }
        size_t pending = 4, popped = 0; int distance = 0;
        const int bucket_count = static_cast<int>(buckets.size());
        int current_bucket = 0;
        while (pending) {
            if ((popped & 1023) == 0) check_deadline(deadline, "turn_distance_table");
            auto& bucket = buckets[current_bucket];
            if (bucket.empty()) {
                ++distance;
                if (++current_bucket == bucket_count) current_bucket = 0;
                continue;
            }
            const int node = bucket.back(); bucket.pop_back(); --pending; ++popped;
            if (dist[node] != distance) continue;
            const auto pred = predecessors(node);
            for (int k = 0; k < 3; ++k) if (pred[k] >= 0) {
                const int edge = k == 2 ? forward_cost(cells_[pred[k] / 4], node % 4) : turn_cost_;
                const int next_distance = distance + edge;
                if (next_distance >= dist[pred[k]]) continue;
                dist[pred[k]] = next_distance;
                // Every positive edge is below bucket_count; one subtraction
                // preserves the modulo index and the exact existing pop order.
                int next_bucket = current_bucket + edge;
                if (next_bucket >= bucket_count) next_bucket -= bucket_count;
                buckets[next_bucket].push_back(pred[k]); ++pending;
            }
        }
    }
    check_deadline(deadline, "turn_distance_table_complete");
    return dist;
}

const TurnTable* TurnDistanceOracle::table(int goal, std::chrono::steady_clock::time_point deadline) {
    check_deadline(deadline, "turn_distance_table");
    if (const auto* cached = find(goal)) return cached;
    std::vector<int> dist;
    const auto prefetched = prefetched_.find(goal);
    if (prefetched != prefetched_.end()) {
        dist = std::move(prefetched->second); prefetched_.erase(prefetched); ++prefetched_hits;
    } else dist = compute(goal, deadline, queue_, buckets_);
    TurnTable stored(std::move(dist), compact_);
    check_deadline(deadline, "turn_distance_table_complete");
    if (compact_ && !stored.is_compact()) ++wide_fallback_tables;
    lru_.push_front(goal);
    auto added = tables_.emplace(goal, Entry{std::move(stored), lru_.begin()});
    return &added.first->second.dist;
}

void TurnDistanceOracle::discard_prefetch() {
    prefetched_discarded += prefetched_.size(); prefetched_.clear();
}

void TurnDistanceOracle::prefetch(const std::vector<int>& goals, int threads,
                                  std::chrono::steady_clock::time_point deadline) {
    if (threads < 1 || threads > 32 || goals.size() > 32)
        throw std::invalid_argument("invalid turn prefetch work limits");
    discard_prefetch(); check_deadline(deadline, "turn_prefetch_start");
    std::vector<int> worklist; std::unordered_set<int> seen;
    for (int goal : goals) if (!has(goal) && seen.insert(goal).second) worklist.push_back(goal);
    if (worklist.empty()) return;
    std::vector<std::vector<int>> results(worklist.size());
    std::vector<std::exception_ptr> errors(worklist.size());
    std::atomic<size_t> next{0};
    auto work = [&] {
        std::vector<int> queue; std::vector<std::vector<int>> buckets(max_edge_cost_ + 1);
        for (;;) {
            const size_t item = next.fetch_add(1);
            if (item >= worklist.size()) return;
            try { results[item] = compute(worklist[item], deadline, queue, buckets); }
            catch (...) { errors[item] = std::current_exception(); }
        }
    };
    std::vector<std::thread> workers;
    try {
        for (int t = 1; t < std::min<int>(threads, worklist.size()); ++t) workers.emplace_back(work);
    } catch (...) {
        for (auto& worker : workers) worker.join();
        throw;
    }
    work(); for (auto& worker : workers) worker.join();
    for (const auto& error : errors) if (error) std::rethrow_exception(error);
    check_deadline(deadline, "turn_prefetch_complete");
    // These complete speculative results are invisible to find/has/retain.
    // Only a normal table() demand admits one, in the original LRU order and
    // against the original fixed table budget. Unused results are discarded.
    for (size_t item = 0; item < worklist.size(); ++item)
        prefetched_.emplace(worklist[item], std::move(results[item]));
    prefetched_builds += worklist.size();
}

int TurnDistanceOracle::value(const TurnTable& table, int cell, int orientation) const {
    return index_.at(cell) < 0 ? kInf : table[index_[cell] * 4 + orientation];
}

void TurnDistanceOracle::retain(const std::unordered_set<int>& goals) {
    for (auto it = tables_.begin(); it != tables_.end();) {
        if (goals.count(it->first)) { ++it; continue; }
        lru_.erase(it->second.lru); it = tables_.erase(it);
    }
}

void TurnDistanceOracle::trim() {
    while (!lru_.empty() && tables_.size() * table_bytes_ > max_bytes_) {
        tables_.erase(lru_.back()); lru_.pop_back();
    }
}

ResidentChainPrice resident_chain_price(const Task& task, const TurnDistanceOracle& oracle,
                                       const Certificate& cert) {
    if (task.idx_next_loc < 0 || static_cast<size_t>(task.idx_next_loc) >= task.locations.size())
        return {-1, ChainPriceStatus::Invalid};
    // Validate every remaining stop before any indexed oracle access. Missing
    // tables must not conceal malformed or structurally excluded later legs.
    for (size_t k = task.idx_next_loc; k < task.locations.size(); ++k) {
        const int cell = task.locations[k];
        if (cell < 0 || static_cast<size_t>(cell) >= cert.free.size()) return {-1, ChainPriceStatus::Invalid};
        if (!cert.free[cell]) return {-1, ChainPriceStatus::OutsideDomain};
        if (k > static_cast<size_t>(task.idx_next_loc)) {
            const int from = task.locations[k - 1];
            if (from != cell && !cert.core[from] && cert.pocket[from] != cert.pocket[cell])
                return {-1, ChainPriceStatus::OutsideDomain};
        }
    }
    long long total = 0;
    for (size_t k = static_cast<size_t>(task.idx_next_loc) + 1; k < task.locations.size(); ++k) {
        const int from = task.locations[k - 1], goal = task.locations[k];
        if (from == goal) continue;
        const auto* table = oracle.peek(goal);
        if (!table) return {-1, ChainPriceStatus::MissingTable};
        int leg = kInf;
        for (int heading = 0; heading < 4; ++heading)
            leg = std::min(leg, oracle.value(*table, from, heading));
        if (leg >= kInf) return {-1, ChainPriceStatus::Unreachable};
        total = std::min<long long>(kInf - 1, total + leg);
    }
    return {static_cast<int>(total), ChainPriceStatus::Covered};
}

int imputed_chain_price(int native, long long numerator, long long denominator) {
    if (native < 0 || numerator < 0 || denominator < 0)
        throw std::invalid_argument("negative task-chain price or ratio");
    if (!denominator) return std::min(native, kInf - 1);
    const __int128 product = static_cast<__int128>(native) * numerator;
    return static_cast<int>(std::min<__int128>(kInf - 1, (product + denominator - 1) / denominator));
}

int selected_chain_price(int mode, int native, const ResidentChainPrice& resident,
                         long long numerator, long long denominator) {
    if (mode < 0 || mode > 4 || native < 0) throw std::invalid_argument("invalid task-chain pricing mode or cost");
    if (mode == 0 || mode == 4) return std::min(native, kInf - 1);
    // The ratio-only control never uses the individual resident quote.
    if (mode == 3) return imputed_chain_price(native, numerator, denominator);
    if (resident.status == ChainPriceStatus::Covered) return resident.cost;
    return mode == 2 ? imputed_chain_price(native, numerator, denominator) : std::min(native, kInf - 1);
}

bool ChainCostCache::all_table_derived(int task_id) const {
    const auto found = entries_.find(task_id);
    if (found == entries_.end()) return false;
    const auto& entry = found->second;
    for (size_t k = entry.stop + 1; k < entry.table_derived.size(); ++k)
        if (!entry.table_derived[k]) return false;
    return true;
}

int ChainCostCache::estimate(const Task& task, DistanceOracle& oracle, int& table_budget,
                             std::chrono::steady_clock::time_point deadline, bool peek) {
    auto result = entries_.try_emplace(task.task_id);
    Entry& entry = result.first->second;
    const bool fresh = result.second || entry.stop != task.idx_next_loc || entry.locations != task.locations;
    if (fresh) {
        if (!result.second) ++invalidations;
        entry.stop = task.idx_next_loc;
        entry.locations = task.locations;
        entry.legs.assign(task.locations.size(), 0);
        entry.table_derived.assign(task.locations.size(), 0);
    }
    const int previous = entry.total;
    long long total = 0;
    bool approximate = false;
    for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k) {
        if (!entry.table_derived[k]) {
            const int goal = task.locations[k];
            const auto* table = peek ? oracle.peek(goal) : oracle.find(goal);
            // Refinement never spends additional BFS work on an existing entry.
            if (!table && fresh && table_budget > 0) {
                --table_budget;
                table = oracle.try_table(goal, deadline);
            }
            if (table) {
                entry.legs[k] = std::min(oracle.distance_from(*table, task.locations[k - 1]), kFar);
                entry.table_derived[k] = 1;
                if (!fresh) ++refined_legs;
            } else if (fresh) {
                entry.legs[k] = std::min(oracle.manhattan(task.locations[k - 1], goal), kFar);
            }
        }
        total += entry.legs[k];
        approximate |= !entry.table_derived[k];
    }
    entry.total = static_cast<int>(std::min<long long>(total, kFar));
    if (!fresh && entry.total != previous) ++changed_costs;
    if (approximate) ++approximate_reads;
    else ++table_reads;
    return entry.total;
}

void ChainCostCache::retain(const std::unordered_set<int>& task_ids) {
    for (auto it = entries_.begin(); it != entries_.end();)
        it = task_ids.count(it->first) ? std::next(it) : entries_.erase(it);
}

bool Agent::observe_progress(int distance, ProgressBasis basis, bool stable_basis) {
    const bool changed = stable_basis && progress_basis != ProgressBasis::None && progress_basis != basis;
    if (changed) { best = kInf; stall = 0; }
    progress_basis = basis;
    if (distance < best) { best = distance; stall = 0; }
    else ++stall;
    return changed;
}

// ─── planner ─────────────────────────────────────────────────────────────────

Cgar& Cgar::instance() {
    static Cgar cgar;
    return cgar;
}

int Cgar::neighbor(int cell, int dir) const { return grid_neighbor(cell, dir, cert_.rows, cert_.cols); }

bool Cgar::adjacent_to_pocket(int cell, int pocket) const {
    for (int d = 0; d < 4; ++d) {
        const int v = neighbor(cell, d);
        if (v >= 0 && cert_.pocket[v] == pocket) return true;
    }
    return false;
}

void Cgar::initialize(SharedEnvironment* env, int preprocess_ms) {
    const auto preprocess_deadline = Clock::now() + std::chrono::milliseconds(preprocess_ms);
    env_ = env;
    if (initialized_) return;  // the scheduler and the planner both call this
    n_ = env->num_of_agents;
    const auto trick_options = tricks::options(env->trick_instance);
    static_trick_metric_ = trick_options.lanes;
    native_trick_metric_ = trick_options.native_metric;
    guidance_cost_limit_ = native_trick_metric_ ? (trick_options.native_bands ? 201 : 200) : 16;
    if ((trick_options.native_bands && !native_trick_metric_) ||
        (native_trick_metric_ && (!static_trick_metric_ || !trick_options.remaining_flow)))
        throw std::invalid_argument("native metric requires explicit static lanes and remaining-flow; native bands require native metric");
    short_task_trick_ = trick_options.short_tasks;
    known_horizon_ = trick_options.known_horizon;
    horizon_margin_ = trick_options.horizon_margin;
    if (!env->trick_instance.empty())
        tricks::validate_map(env->trick_instance, env->map, env->rows, env->cols);
    stall_limit_ = env_int("CGAR_STALL", 4);
    commit_limit_ = env_int("CGAR_COMMIT_AGE", 3);
    plan_tables_ = env_int("CGAR_PLAN_TABLES", 256);
    sched_tables_ = env_int("CGAR_SCHED_TABLES", 128);
    max_pairs_ = std::max(1, env_int("CGAR_MAX_PAIRS", 2000000));
    diagnostics_ = env_int("CGAR_DIAGNOSTICS", 0) != 0;
    turn_first_ = env_int("CGAR_TURN_FIRST", 0) != 0;
    orientation_guidance_ = std::max(0, std::min(2, env_int("CGAR_ORIENTATION_GUIDANCE", 0)));
    temporal_ = env_int("CGAR_TEMPORAL", 0) != 0;
    temporal_warm_start_ = env_int("CGAR_TEMPORAL_WARM_START", 0) != 0;
    const int strict_wait_turns = env_int("CGAR_TEMPORAL_STRICT_WAIT_TURNS", 0);
    if (strict_wait_turns < 0 || strict_wait_turns > 1 || (strict_wait_turns && !temporal_))
        throw std::invalid_argument("strict wait turns require temporal planning and a boolean setting");
    temporal_strict_wait_turns_ = strict_wait_turns != 0;
    temporal_conflict_audit_stride_ = env_int("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE", 0);
    if (temporal_conflict_audit_stride_ < 0 || temporal_conflict_audit_stride_ > 4096 ||
        (temporal_conflict_audit_stride_ && !temporal_))
        throw std::invalid_argument("temporal conflict audit requires temporal planning and stride 1-4096");
    temporal_group_snapshot_count_ = env_int("CGAR_TEMPORAL_GROUP_SNAPSHOT_COUNT", 0);
    if (temporal_group_snapshot_count_ < 0 || temporal_group_snapshot_count_ > 32 ||
        (temporal_group_snapshot_count_ && (!temporal_ || !diagnostics_ || !temporal_conflict_audit_stride_)))
        throw std::invalid_argument("temporal group snapshots require diagnostics, conflict audit and count 1-32");
    temporal_service_audit_stride_ = env_int("CGAR_TEMPORAL_SERVICE_AUDIT_STRIDE", 0);
    if (temporal_service_audit_stride_ < 0 || temporal_service_audit_stride_ > 4096 ||
        (temporal_service_audit_stride_ && !temporal_))
        throw std::invalid_argument("temporal service audit requires temporal planning and stride 1-4096");
    const int next_errand = env_int("CGAR_TEMPORAL_NEXT_ERRAND", 0);
    if (next_errand < 0 || next_errand > 1 || (next_errand && !temporal_))
        throw std::invalid_argument("next-errand scoring requires temporal planning and a boolean setting");
    temporal_next_errand_ = next_errand != 0;
    temporal_history_.clear();
    if (temporal_warm_start_ && !temporal_) throw std::invalid_argument("temporal warm start requires temporal planning");
    turn_prefetch_threads_ = env_int("CGAR_TURN_PREFETCH_THREADS", 0);
    if (turn_prefetch_threads_ < 0 || turn_prefetch_threads_ > 32 ||
        (turn_prefetch_threads_ && !orientation_guidance_))
        throw std::invalid_argument("turn prefetch requires orientation guidance and 1-32 threads");
    flow_strength_ = env_int("CGAR_FLOW_STRENGTH", 0);
    if (flow_strength_ < 0 || flow_strength_ > 8 || (flow_strength_ && !orientation_guidance_))
        throw std::invalid_argument("learned flow requires orientation guidance and strength 1-8");
    const int cache_only_refresh = env_int("CGAR_FLOW_CACHE_ONLY_REFRESH", 0);
    if (cache_only_refresh < 0 || cache_only_refresh > 1 ||
        (cache_only_refresh && (!flow_strength_ || !env_int("CGAR_FLOW_REFRESH_INTERVAL", 0))))
        throw std::invalid_argument("cache-only flow refresh requires enabled flow, a positive interval and a boolean setting");
    turn_build_limit_ = env_int("CGAR_TURN_BUILD_LIMIT", 32);
    if (turn_build_limit_ < 0 || turn_build_limit_ > 256)
        throw std::invalid_argument("CGAR_TURN_BUILD_LIMIT must be in [0,256]");
    turn_cost_ = env_int("CGAR_TURN_COST", 1);
    if (turn_cost_ < 1 || turn_cost_ > 16) throw std::invalid_argument("CGAR_TURN_COST must be in [1,16]");
    if (turn_cost_ != 1 && !orientation_guidance_) throw std::invalid_argument("weighted turns require orientation guidance");
    flow_cost_scale_ = env_int("CGAR_FLOW_COST_SCALE", 1);
    if (flow_cost_scale_ != 1 && flow_cost_scale_ != 2 && flow_cost_scale_ != 4 && flow_cost_scale_ != 8 &&
        !(native_trick_metric_ && flow_cost_scale_ == 20))
        throw std::invalid_argument("flow cost scale must be one of 1,2,4,8");
    if (flow_cost_scale_ != 1 && ((!flow_strength_ && !static_trick_metric_) || !temporal_ || turn_cost_ != 1))
        throw std::invalid_argument("scaled flow costs require temporal planning, enabled flow and unit physical turns");
    // A fractional surcharge is expressed in the existing scaled cost units.
    // Keep the physical slot and forward/base costs unchanged, and use this
    // single effective price in every oriented planning/scheduling calculation.
    turn_surcharge_ = env_int("CGAR_TURN_SURCHARGE", 0);
    if (turn_surcharge_ < 0 || turn_surcharge_ > 15 ||
        (turn_surcharge_ && (!flow_strength_ || !temporal_ || turn_cost_ != 1)))
        throw std::invalid_argument("turn surcharge requires temporal flow, unit physical turns and a value in [0,15]");
    guidance_turn_cost_ = native_trick_metric_ ? 1 : turn_cost_ * flow_cost_scale_ + turn_surcharge_;
    if (guidance_turn_cost_ > 16)
        throw std::invalid_argument("scaled turn cost plus surcharge must not exceed 16");
    guide_enabled_ = env_int("CGAR_GUIDE_ROUTES", 0) != 0;
    if (guide_enabled_ && (!temporal_ || turn_cost_ != 1 || flow_strength_))
        throw std::invalid_argument("guide routes require temporal planning, unit turns and frozen flow disabled");
    guide_options_.batch = env_int("CGAR_GUIDE_BATCH", 128);
    guide_options_.expansions = env_int("CGAR_GUIDE_EXPANSIONS", 4096);
    guide_options_.lookahead = env_int("CGAR_GUIDE_LOOKAHEAD", 8);
    guide_options_.base_cost = env_int("CGAR_GUIDE_BASE_COST", 16);
    guide_options_.opposite_cost = env_int("CGAR_GUIDE_OPPOSITE_COST", 1);
    guide_options_.load_cost = env_int("CGAR_GUIDE_LOAD_COST", 0);
    guide_options_.heuristic_weight = env_int("CGAR_GUIDE_HEURISTIC_WEIGHT", 1);
    guide_options_.reconnect_steps = env_int("CGAR_GUIDE_RECONNECT_STEPS", 0);
    guide_options_.refine_batch = env_int("CGAR_GUIDE_REFINE_BATCH", 0);
    temporal_transaction_options_.work = env_int("CGAR_TEMPORAL_BRANCH_WORK", 0);
    temporal_transaction_options_.max_owners = env_int("CGAR_TEMPORAL_BRANCH_OWNERS", 2);
    if (temporal_transaction_options_.work < 0 || temporal_transaction_options_.work > 2000000 ||
        temporal_transaction_options_.max_owners < 1 || temporal_transaction_options_.max_owners > 2 ||
        (temporal_transaction_options_.work && !temporal_))
        throw std::invalid_argument("temporal branching requires temporal planning, work in [0,2000000] and one or two owners");
    const int remaining_flow = env_int("CGAR_TEMPORAL_REMAINING_FLOW", 0);
    if (remaining_flow < 0 || remaining_flow > 1 ||
        (remaining_flow && (!flow_strength_ || !env->trick_instance.empty())) ||
        ((remaining_flow || trick_options.remaining_flow) &&
         (!temporal_ || !orientation_guidance_ || guide_enabled_ || temporal_next_errand_ ||
          temporal_service_audit_stride_ || temporal_conflict_audit_stride_ || temporal_transaction_options_.work)) ||
        (trick_options.remaining_flow && !static_trick_metric_))
        throw std::invalid_argument("remaining-flow scoring requires generic learned flow or explicit CGAR_TRICK_REMAINING_FLOW with static lanes only; incompatible with guide, next-errand, paid-progress audits or branching");
    temporal_remaining_flow_ = remaining_flow != 0 || trick_options.remaining_flow;
    if (temporal_remaining_flow_)
        std::printf("[cgar-temporal-score] remaining_flow=1 paid_forward_extra=0\n");
    temporal_distance_scale_ = env_int("CGAR_TEMPORAL_DISTANCE_SCALE", 50);
    if (temporal_distance_scale_ < 1 || temporal_distance_scale_ > 4096)
        throw std::invalid_argument("temporal distance scale must be in [1,4096]");
    if (native_trick_metric_ && (flow_cost_scale_ != 20 || turn_cost_ != 1 || turn_surcharge_ || temporal_distance_scale_ != 50))
        throw std::invalid_argument("native metric requires forward base20, physical turn1, no surcharge and raw distance scale50");
    temporal_transaction_options_.distance_scale = temporal_distance_scale_;
    temporal_transaction_options_.unit_cost = flow_cost_scale_;
    temporal_equal_weight_ = env_int("CGAR_TEMPORAL_EQUAL_WEIGHT", 0) != 0;
    temporal_workers_ = std::max(1, std::min(32, env_int("CGAR_TEMPORAL_WORKERS", 1)));
    temporal_threads_ = std::max(1, std::min(temporal_workers_, env_int("CGAR_TEMPORAL_THREADS", temporal_workers_)));
    const int mixed_start = env_int("CGAR_TEMPORAL_MIXED_START", 0);
    if (mixed_start < 0 || mixed_start > 1 ||
        (mixed_start && (!temporal_warm_start_ || temporal_workers_ < 2)))
        throw std::invalid_argument("mixed temporal starts require warm start, at least two workers and a boolean setting");
    temporal_mixed_start_ = mixed_start != 0;
    temporal_prepare_threads_ = env_int("CGAR_TEMPORAL_PREP_THREADS", 1);
    if (temporal_prepare_threads_ < 1 || temporal_prepare_threads_ > 32)
        throw std::invalid_argument("temporal preparation threads must be in [1,32]");
    temporal_table_batch_ = env_int("CGAR_TEMPORAL_TABLE_BATCH", 0);
    temporal_table_threads_ = env_int("CGAR_TEMPORAL_TABLE_THREADS", 1);
    if (temporal_table_batch_ < 0 || temporal_table_batch_ > 1024 ||
        temporal_table_threads_ < 1 || temporal_table_threads_ > 32 ||
        (temporal_table_batch_ && (!temporal_ || !orientation_guidance_)))
        throw std::invalid_argument("temporal table batches require temporal guidance, 0-1024 tables and 1-32 threads");
    temporal_regions_ = env_int("CGAR_TEMPORAL_REGIONS", 0) != 0;
    temporal_region_options_.parts = env_int("CGAR_TEMPORAL_REGIONS", 4);
    temporal_region_options_.rounds = env_int("CGAR_TEMPORAL_REGION_ROUNDS", 2);
    temporal_region_options_.steps = env_int("CGAR_TEMPORAL_REGION_STEPS", 25000);
    temporal_region_options_.temperature_ppm = env_int("CGAR_TEMPORAL_REGION_TEMPERATURE_PPM", 1000);
    temporal_region_options_.threads = env_int("CGAR_TEMPORAL_REGION_THREADS", temporal_region_options_.parts);
    const int region_peak_audit = env_int("CGAR_TEMPORAL_REGION_PEAK_AUDIT", 0);
    if (region_peak_audit < 0 || region_peak_audit > 1 || (region_peak_audit && !temporal_regions_))
        throw std::invalid_argument("regional peak audit requires enabled regions and a boolean setting");
    temporal_region_options_.audit_peaks = region_peak_audit != 0;
    if (temporal_regions_ && (!temporal_ || temporal_region_options_.parts < 1 || temporal_region_options_.parts > 32 ||
        temporal_region_options_.rounds < 1 || temporal_region_options_.rounds > 16 ||
        temporal_region_options_.steps < 1 || temporal_region_options_.steps > 1000000 ||
        temporal_region_options_.temperature_ppm < 0 || temporal_region_options_.temperature_ppm > 10000 ||
        temporal_region_options_.threads < 1 || temporal_region_options_.threads > temporal_region_options_.parts))
        throw std::invalid_argument("invalid CGAR temporal region configuration");
    temporal_candidate_limit_ = std::max(0, std::min(100000000, env_int("CGAR_TEMPORAL_CANDIDATE_LIMIT", 0)));
    temporal_steps_ = std::max(0, std::min(1000000, env_int("CGAR_TEMPORAL_STEPS", temporal_candidate_limit_ ? 1000000 : 0)));
    temporal_budget_ = std::max(1, std::min(32768, env_int("CGAR_TEMPORAL_BUDGET", 8192)));
    temporal_order_ = std::max(0, std::min(2, env_int("CGAR_TEMPORAL_ORDER", 1)));
    temporal_rng_.seed(static_cast<uint64_t>(env_int("CGAR_SEED", 0)));
    if (temporal_ && !orientation_guidance_) throw std::invalid_argument("temporal policy requires orientation guidance");
    pibt_reference_ = env_int("CGAR_PIBT_REFERENCE", 0) != 0;
    pibt_tickets_ = env_int("CGAR_PIBT_TICKETS", 0) != 0;
    pibt_commitments_ = env_int("CGAR_PIBT_COMMITMENTS", 0) != 0;
    enable_txn_ = env_int("CGAR_TXN", pibt_reference_ ? 0 : 1) != 0;
    hrrn_ = !short_task_trick_ && env_int("CGAR_HRRN", 1) != 0;
    pickup_weight_ = std::max(1, std::min(16, env_int("CGAR_PICKUP_WEIGHT", 1)));
    const int pickup_flow = env_int("CGAR_PICKUP_FLOW", 0);
    pickup_flow_nodes_ = env_int("CGAR_PICKUP_FLOW_NODES", 8192);
    if (pickup_flow < 0 || pickup_flow > 1 || pickup_flow_nodes_ < 1 || pickup_flow_nodes_ > 65536 ||
        (pickup_flow && !flow_strength_ && !static_trick_metric_))
        throw std::invalid_argument("pickup flow requires learned flow, a boolean setting and 1-65536 queue pops");
    pickup_flow_ = pickup_flow != 0;
    pickup_full_robots_ = env_int("CGAR_PICKUP_FULL_ROBOTS", 0);
    pickup_full_threads_ = env_int("CGAR_PICKUP_FULL_THREADS", 4);
    if (pickup_full_robots_ < 0 || pickup_full_robots_ > 256 ||
        pickup_full_threads_ < 1 || pickup_full_threads_ > 32 || (pickup_full_robots_ && !pickup_flow_))
        throw std::invalid_argument("complete pickup fields require pickup flow, 0-256 robots and 1-32 threads");
    pickup_full_workers_.resize(pickup_full_robots_ ? pickup_full_threads_ : 0);
    pickup_full_fields_.resize(pickup_full_robots_);
    const int pickup_full_cost_key = env_int("CGAR_PICKUP_FULL_COST_KEY", 0);
    if (pickup_full_cost_key < 0 || pickup_full_cost_key > 1 ||
        (pickup_full_cost_key && !pickup_full_robots_))
        throw std::invalid_argument("complete pickup cost shortlist requires enabled full fields and a boolean setting");
    pickup_full_cost_key_ = pickup_full_cost_key != 0;
    if (temporal_ || turn_prefetch_threads_ || pickup_full_robots_) {
        // The phases run sequentially. Complete pickup workers are additionally
        // bounded by the fixed field quota and the total robot count.
        const int pickup_threads = std::min({pickup_full_threads_, pickup_full_robots_, n_});
        const int required_threads = std::max({temporal_ ? temporal_threads_ : 1, temporal_ ? temporal_prepare_threads_ : 1, temporal_regions_ ? temporal_region_options_.threads : 1, turn_prefetch_threads_, temporal_table_batch_ ? temporal_table_threads_ : 1, pickup_threads});
        cpu_set_t affinity; CPU_ZERO(&affinity);
        if (sched_getaffinity(0, sizeof(affinity), &affinity) || CPU_COUNT(&affinity) < required_threads)
            throw std::invalid_argument("planner or pickup threads exceed the allowed logical CPU affinity");
        std::printf("[cgar-temporal-allocation] workers=%d threads=%d preparation_threads=%d region_threads=%d table_threads=%d pickup_threads=%d allowed_cpus=%d\n",
                    temporal_workers_, temporal_threads_, temporal_prepare_threads_, temporal_regions_ ? temporal_region_options_.threads : 0, temporal_table_batch_ ? temporal_table_threads_ : 0, pickup_threads, CPU_COUNT(&affinity));
    }
    refine_chain_costs_ = env_int("CGAR_REFINE_CHAIN_COSTS", 0) != 0;
    scheduler_cache_peek_ = env_int("CGAR_SCHEDULER_CACHE_PEEK", 0) != 0;
    stable_stall_basis_ = env_int("CGAR_STABLE_STALL_BASIS", 0) != 0;
    repair_fallback_ = env_int("CGAR_FALLBACK_REPAIR", 1) != 0;
    reassign_ = env_int("CGAR_REASSIGN", 0) != 0;
    const int reassign_pool = env_int("CGAR_REASSIGN_POOL", 0);
    if (reassign_pool < 0 || reassign_pool > 1) throw std::invalid_argument("pool reassignment must be boolean");
    reassign_pool_ = reassign_pool != 0;
    chain_flow_pricing_ = env_int("CGAR_CHAIN_FLOW_PRICING", 0);
    if (chain_flow_pricing_ < 0 || chain_flow_pricing_ > 4 ||
        (chain_flow_pricing_ && (!pickup_flow_ || !orientation_guidance_ || !flow_strength_ ||
         !env->trick_instance.empty() || temporal_remaining_flow_ || guide_enabled_ || reassign_ || reassign_pool_)))
        throw std::invalid_argument("chain flow pricing requires generic learned pickup flow and mode0..4; incompatible with tricks, remaining-flow score, guide routes or rematching");
    if (chain_flow_pricing_)
        std::printf("[cgar-chain-pricing] mode=%d shadow=%d resident_only=1 extra_tables=0\n", chain_flow_pricing_, chain_flow_pricing_ == 4);
    const int reassign_match = env_int("CGAR_REASSIGN_MATCH", 0);
    if (reassign_match < 0 || reassign_match > 1 ||
        (reassign_match && !env->trick_instance.empty()) ||
        ((reassign_match || trick_options.matching) &&
         (!temporal_ || !orientation_guidance_ || !pickup_flow_ || (!flow_strength_ && !static_trick_metric_) ||
          guide_enabled_ || reassign_ || reassign_pool_ || chain_flow_pricing_ ||
          (temporal_remaining_flow_ && !trick_options.remaining_flow))))
        throw std::invalid_argument("unopened pickup matching requires temporal/oriented pickup flow, a boolean selector and no other rematching, chain pricing, generic remaining-flow score or guides; under --trick use CGAR_TRICK_UNOPENED_MATCH");
    reassign_match_ = reassign_match != 0 || trick_options.matching;
    match_group_limit_ = env_int("CGAR_REASSIGN_MATCH_GROUPS", 4);
    if (match_group_limit_ < 1 || match_group_limit_ > 64 || (!reassign_match_ && match_group_limit_ != 4))
        throw std::invalid_argument("unopened matching group quota requires enabled matching and 1-64 groups (disabled default4)");
    const int match_pickup_groups = env_int("CGAR_REASSIGN_MATCH_PICKUP_GROUPS", 0);
    if (match_pickup_groups < 0 || match_pickup_groups > 1 || (match_pickup_groups && !reassign_match_))
        throw std::invalid_argument("pickup-neighborhood grouping requires enabled matching and a boolean selector");
    match_pickup_groups_ = match_pickup_groups != 0;
    match_budget_audit_stride_ = env_int("CGAR_MATCH_BUDGET_AUDIT_STRIDE", 0);
    if (match_budget_audit_stride_ < 0 || match_budget_audit_stride_ > 5000 ||
        (match_budget_audit_stride_ && (!reassign_match_ || !diagnostics_ || match_budget_audit_stride_ % 10)))
        throw std::invalid_argument("matching budget audit requires diagnostics, enabled matching and a stride divisible by10 in10..5000");
    if (match_budget_audit_stride_)
        std::printf("[cgar-match-budget-shadow-config] stride=%d read_only=1 after_real_match=1 resident_only=1 include_budget=1 cooldown=20 task_disjoint_witnesses=1\n", match_budget_audit_stride_);
    if (reassign_match_)
        std::printf("[cgar-unopened-match] enabled=1 groups=%d group_size=32 node_limit=2048 task_budget=1 cooldown=20 resident_only=1 extra_tables=0 local_pool=all_resident anchor_candidates=128 pickup_groups=%d\n", match_group_limit_, match_pickup_groups_);
    fallback_samples_ = std::max(0, std::min(4096, env_int("CGAR_FALLBACK_SAMPLES", 64)));
    global_samples_ = std::max(0, std::min(512, env_int("CGAR_GLOBAL_SAMPLES", 0)));
    enable_locks_ = env_int("CGAR_CERT", pibt_reference_ ? 0 : 1) != 0;
    const size_t table_mb = static_cast<size_t>(env_int("CGAR_TABLE_MB", 2048));
    rng_.seed(static_cast<unsigned>(env_int("CGAR_SEED", 0)));

    if (!env->trick_instance.empty() && (!temporal_ || !orientation_guidance_ || pibt_reference_ || guide_enabled_ || reassign_ || reassign_pool_))
        throw std::invalid_argument("--trick WAREHOUSE requires temporal/oriented CGAR without guide routes or rematching");
    if (static_trick_metric_ && (!temporal_ || !orientation_guidance_ || pibt_reference_ || guide_enabled_ ||
        flow_cost_scale_ != (native_trick_metric_ ? 20 : 4) || turn_cost_ != 1 || turn_surcharge_ != 0 || cache_only_refresh))
        throw std::invalid_argument("--trick WAREHOUSE requires temporal/oriented CGAR, the selected metric scale (legacy4/native20), unit physical turns, no turn surcharge or cache-only refresh");

    const auto t0 = Clock::now();
    if (pibt_reference_ && !enable_locks_) {
        // The reference domain includes every traversable component. No capacity
        // pruning or pocket policy is part of the native PIBT special case.
        cert_.rows = env->rows; cert_.cols = env->cols; cert_.robots = n_;
        for (int obstacle : env->map) cert_.free.push_back(!obstacle);
        cert_.core = cert_.free;
        cert_.core_size = std::count(cert_.free.begin(), cert_.free.end(), 1);
        cert_.pocket.assign(env->map.size(), -1);
        cert_.exit_dist.assign(env->map.size(), 0);
        cert_.exit_cell.assign(env->map.size(), -1);
        cert_.promotion.assign(env->map.size(), 0);
    } else cert_ = build_certificate_feasible(env->map, env->rows, env->cols, enable_locks_ ? n_ : 0);
    if (!enable_locks_) {
        // Ablation: route over the whole connected floor with no pockets.
        std::fill(cert_.pocket.begin(), cert_.pocket.end(), -1);
        cert_.pocket_cells.clear();
    }
    const double secs = std::chrono::duration<double>(Clock::now() - t0).count();
    // These spatial tables can exclude non-goal pockets. The configured-horizon
    // experiment is restricted to an unrestricted core so every cached distance
    // is a lower bound on physical travel. No native lane cost enters this bound.
    if (known_horizon_ && (cert_.core != cert_.free || refine_chain_costs_ || chain_flow_pricing_ || pickup_full_cost_key_))
        throw std::invalid_argument("known horizon requires a full core, original spatial chain estimates and ordinary shortlist ordering");
    if (horizon_margin_ && reassign_pool_)
        throw std::invalid_argument("horizon margin does not support pool exchanges");
    if (horizon_margin_)
        std::printf("[CGAR_TRICK_HORIZON_MARGIN] enabled=1 estimator=prospective_bucket_mean basis=admission_bound samples=single_holder tiers=margin_feasible_impossible fair=unchanged held=unchanged\n");
    if (known_horizon_)
        std::printf("[CGAR_TRICK_HORIZON] known_horizon=%d assumption=configured lower_bound=spatial_plus_service core=full assignments=new_only fair=unchanged held=unchanged all_impossible=assign after_horizon=ordinary\n", known_horizon_);
    oracle_.init(&cert_, table_mb << 20);
    if (orientation_guidance_) {
        const size_t mb = static_cast<size_t>(std::max(16, std::min(32768, env_int("CGAR_TURN_TABLE_MB", 512))));
        turn_oracle_.init(&cert_, mb << 20, guidance_turn_cost_, env_int("CGAR_TURN_COMPACT", 0) != 0, flow_cost_scale_, guidance_cost_limit_);
        if (static_trick_metric_) {
            auto field = native_trick_metric_ ?
                tricks::native_forward_costs(env->trick_instance, env->map, env->rows, env->cols, trick_options.native_bands) :
                tricks::forward_costs(env->trick_instance, env->map, env->rows, env->cols);
            const uint64_t installed_fingerprint = native_trick_metric_ ? tricks::validate_native_field(field, trick_options.native_bands) : 0;
            turn_oracle_.set_forward_costs(std::move(field));
            if (trick_options.remaining_flow && !turn_oracle_.weighted_forward())
                throw std::logic_error("static remaining-flow score requires active weighted forward costs");
            if (native_trick_metric_) {
                std::printf("[CGAR_TRICK] instance=%s provider=nms-native-metric forward_base=20 opposing=200 band=%d turn=1 score=pure_potential tie=raw field_sha256=%s installed_fnv1a64=%llu occupancy_sha256=%s learned_publications=disabled\n",
                    env->trick_instance.c_str(), trick_options.native_bands, tricks::native_field_hash(trick_options.native_bands),
                    static_cast<unsigned long long>(installed_fingerprint), tricks::warehouse_occupancy_sha256);
            } else {
                std::printf("[CGAR_TRICK] instance=%s provider=nms-lane-directions forward_base=4 opposing=16 turn=4 field_sha256=%s occupancy_sha256=%s learned_publications=disabled\n",
                    env->trick_instance.c_str(), tricks::warehouse_field_sha256, tricks::warehouse_occupancy_sha256);
            }
        }
        if (!env->trick_instance.empty()) {
            if (!static_trick_metric_)
                std::printf("[CGAR_TRICK] instance=%s provider=%s field_sha256=none learned_publications=enabled\n",
                    env->trick_instance.c_str(), short_task_trick_ ? "short-task-preference" : "ablation-control");
            std::printf("[CGAR_TRICK_COMPONENTS] instance=%s lanes=%d short_tasks=%d matching=%d remaining_flow=%d native_metric=%d native_bands=%d hrrn=%d oldest_admission=%d started_tasks=protected\n",
                env->trick_instance.c_str(), static_trick_metric_, short_task_trick_, trick_options.matching, trick_options.remaining_flow, native_trick_metric_, trick_options.native_bands, hrrn_, !short_task_trick_);
        }
        if (flow_strength_ && !static_trick_metric_) flow_guidance_.initialize(cert_.free, cert_.rows, cert_.cols,
            env_int("CGAR_FLOW_WARMUP", 128), flow_strength_, env_int("CGAR_FLOW_MIN_SAMPLES", 8),
            env_int("CGAR_FLOW_MIN_MARGIN_PERCENT", 0), env_int("CGAR_FLOW_REFRESH_INTERVAL", 0), flow_cost_scale_, cache_only_refresh != 0);
    }

    if (temporal_) temporal_geometry_.initialize(cert_.free, cert_.rows, cert_.cols,
        [&] { check_deadline(preprocess_deadline, "temporal_preprocess"); });

    if (guide_enabled_) guide_routes_.initialize(cert_.core, cert_.rows, cert_.cols, n_, guide_options_);

    const size_t cells = cert_.free.size();
    occ_now_.assign(cells, -1);
    reserved_.assign(cells, -1);
    touched_.clear();
    pocket_occ_.assign(cert_.pocket_cells.size(), 0);
    pocket_lock_.assign(cert_.pocket_cells.size(), -1);
    pocket_draining_.assign(cert_.pocket_cells.size(), 0);
    parked_.assign(n_, 0);
    capacity_mode_ = enable_locks_ && !cert_.valid && cert_.capacity > 0;
    active_certified_ = enable_locks_ && cert_.valid;
    agents_.assign(n_, Agent());
    loc_.assign(n_, -1);
    ori_.assign(n_, 0);
    free_tasks_.clear();
    chain_cost_.clear();
    chain_table_basis_.clear();
    last_reassignment_.assign(n_, -20);
    pool_reassign_cursor_ = 0;
    match_cursor_ = 0;
    if (pibt_reference_) {
        pibt_elapsed_.assign(n_, 0); pibt_initial_distance_.assign(n_, 0);
        pibt_previous_goal_.assign(n_, -1); pibt_tie_.resize(n_);
        std::uniform_real_distribution<float> uniform(0, 1);
        for (int i = 0; i < n_; ++i) pibt_tie_[i] = uniform(rng_);
        if (turn_first_ || orientation_guidance_)
            throw std::invalid_argument("PIBT reference policy requires unmodified spatial candidate ordering");
        std::printf("[cgar-pibt-reference] enabled=1 tickets=%d commitments=%d cert=%d txn=%d exact_distances=1\n",
                    pibt_tickets_, pibt_commitments_, enable_locks_, enable_txn_);
    }

    size_t free_cells = std::count(cert_.free.begin(), cert_.free.end(), 1);
    size_t largest_pocket = 0;
    for (const auto& p : cert_.pocket_cells) largest_pocket = std::max(largest_pocket, p.size());
    std::printf("[cgar] certificate N=%d free=%zu core=%zu promotion_arcs=%zu iterations=%zu valid=%d "
                "capacity=%zu pockets=%zu largest_pocket=%zu secs=%.3f | stall=%d txn=%d cert=%d\n",
                n_, free_cells, cert_.core_size, cert_.promotion_arcs, cert_.iterations, cert_.valid, cert_.capacity,
                cert_.pocket_cells.size(), largest_pocket, secs, stall_limit_, enable_txn_, enable_locks_);
    std::fflush(stdout);
    check_deadline(preprocess_deadline, "initialize_complete");
    initialized_ = true;
}

void Cgar::sync_agents() {
    if (static_cast<int>(agents_.size()) != n_) agents_.assign(n_, Agent());
    for (int i = 0; i < n_; ++i) {
        if (loc_[i] >= 0 && occ_now_[loc_[i]] == i) occ_now_[loc_[i]] = -1;
    }
    for (int i = 0; i < n_; ++i) {
        loc_[i] = env_->curr_states[i].location;
        ori_[i] = env_->curr_states[i].orientation;
        occ_now_[loc_[i]] = i;
    }
    for (int i = 0; i < n_; ++i) {
        Agent& a = agents_[i];
        const int goal = env_->goal_locations[i].empty() ? -1 : env_->goal_locations[i].front().first;
        const int task = i < static_cast<int>(env_->curr_task_schedule.size()) ? env_->curr_task_schedule[i] : -1;
        const auto found = env_->task_pool.find(task);
        const int stop = found == env_->task_pool.end() ? -1 : found->second.idx_next_loc;
        if (goal != a.goal || task != a.task || stop != a.stop) {
            a.goal = goal;
            a.task = task;
            a.stop = stop;
            a.ticket = goal < 0 ? kIdleTicket : next_ticket_++;
            a.best = kInf;
            a.stall = 0;
            a.progress_basis = ProgressBasis::None;
        }
        if (pibt_reference_ && !pibt_commitments_ && !a.in_txn) {
            a.committed = -1;
            a.commit_age = 0;
        }
        if (a.committed == loc_[i]) {
            a.committed = -1;
            a.commit_age = 0;
        } else if (a.committed >= 0) {
            // A commitment is always to a neighbouring cell; anything else is stale.
            bool adjacent = false;
            for (int d = 0; d < 4; ++d) adjacent = adjacent || neighbor(loc_[i], d) == a.committed;
            ++a.commit_age;
            // Release a commitment whose blocker did not clear: a robot that keeps its
            // target is neither replanned nor pushable, so chains of blocked robots
            // would freeze (KNAPP releases commitment when blockers are not expected
            // to clear; here a fixed age is the grid equivalent).
            if (!adjacent || (!a.in_txn && a.commit_age > commit_limit_)) {
                if (diagnostics_) ++stats_.expired_commitments;
                a.committed = -1;
                a.commit_age = 0;
                a.in_txn = false;
            }
        }
    }
}

void Cgar::update_locks() {
    std::fill(pocket_occ_.begin(), pocket_occ_.end(), 0);
    for (int i = 0; i < n_; ++i) {
        const int p = cert_.pocket[loc_[i]];
        if (p >= 0) ++pocket_occ_[p];
    }
    for (size_t p = 0; p < pocket_lock_.size(); ++p) {
        if (!capacity_mode_ && pocket_occ_[p] > 1) pocket_draining_[p] = 1;
        if (pocket_occ_[p] == 0) pocket_draining_[p] = 0;
        const int holder = pocket_lock_[p];
        if (holder < 0) continue;
        Agent& a = agents_[holder];
        const bool wants = a.goal >= 0 && cert_.pocket[a.goal] == static_cast<int>(p);
        const bool inside = cert_.pocket[loc_[holder]] == static_cast<int>(p);
        const bool near = inside || adjacent_to_pocket(loc_[holder], static_cast<int>(p));
        // A primary may reserve an empty service pocket before reaching its portal.
        // An exiting resident keeps the exclusion until it physically leaves.
        if (!inside && (!wants || (holder != primary_ && !near))) {
            pocket_lock_[p] = -1;
            a.lock = -1;
        }
    }
}

void Cgar::refresh_orientation_cache() {
    // Reserve the finite cache for currently requested goals. Retain resident
    // entries on equal demand, avoiding cyclic LRU churn when the working set
    // exceeds capacity. This policy depends on live requests, never map names.
    std::unordered_map<int, int> demand;
    for (int i = 0; i < n_; ++i) if (agents_[i].goal >= 0 && !parked_[i]) ++demand[agents_[i].goal];
    struct Goal { int goal, count; bool resident; };
    std::vector<Goal> ranked; ranked.reserve(demand.size());
    for (const auto& item : demand) ranked.push_back({item.first, item.second, turn_oracle_.has(item.first)});
    std::sort(ranked.begin(), ranked.end(), [](const Goal& a, const Goal& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.resident != b.resident) return a.resident > b.resident;
        return a.goal < b.goal;
    });
    oriented_goals_.clear();
    const size_t capacity = std::min(ranked.size(), turn_oracle_.capacity());
    for (size_t k = 0; k < capacity; ++k) oriented_goals_.insert(ranked[k].goal);
    turn_oracle_.retain(oriented_goals_);
    check_deadline(deadline_, "orientation_cache_admission");
}

// Admit a fixed batch of complete current-goal tables before temporal scoring.
// This is a distinct admission policy from demand-triggered PIBT construction;
// all selected results are admitted, in a deterministic order, after workers join.
void Cgar::prepare_temporal_tables(const std::vector<char>& pinned) {
    if (!temporal_table_batch_) return;
    const auto started = Clock::now();
    struct Goal { int goal = -1, count = 0; long long ticket = kIdleTicket; };
    std::unordered_map<int, Goal> missing;
    for (int r = 0; r < n_; ++r) {
        if ((r & 63) == 0) check_deadline(deadline_, "temporal_table_batch_candidates");
        const auto& agent = agents_[r];
        if (pinned[r] || agent.goal < 0 || turn_oracle_.has(agent.goal) ||
            (orientation_guidance_ == 2 && !oriented_goals_.count(agent.goal))) continue;
        auto& goal = missing[agent.goal]; goal.goal = agent.goal; ++goal.count;
        goal.ticket = std::min(goal.ticket, agent.ticket);
    }
    std::vector<Goal> ranked; ranked.reserve(missing.size());
    for (const auto& entry : missing) ranked.push_back(entry.second);
    std::sort(ranked.begin(), ranked.end(), [](const Goal& a, const Goal& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.ticket != b.ticket) return a.ticket < b.ticket;
        return a.goal < b.goal;
    });
    const int count = std::min<int>(temporal_table_batch_, ranked.size());
    int covered = 0;
    // The existing oracle bounds parallel scratch to32tables. Complete chunks
    // are admitted before proceeding; a later exception still fails the entry.
    for (int begin = 0; begin < count; begin += 32) {
        check_deadline(deadline_, "temporal_table_batch_build");
        std::vector<int> goals;
        for (int k = begin; k < std::min(count, begin + 32); ++k) goals.push_back(ranked[k].goal);
        turn_oracle_.prefetch(goals, temporal_table_threads_, deadline_);
        for (int k = begin; k < std::min(count, begin + 32); ++k) {
            turn_oracle_.table(ranked[k].goal, deadline_); covered += ranked[k].count;
        }
    }
    check_deadline(deadline_, "temporal_table_batch_complete");
    ++stats_.temporal_batch_passes; stats_.temporal_batch_built += count;
    stats_.temporal_batch_covered += covered; stats_.oriented_builds += count;
    if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
        std::printf("[cgar-temporal-table-batch] step=%d limit=%d threads=%d missing_goals=%zu built=%d covered=%d seconds=%.6f\n",
            env_->curr_timestep + 1, temporal_table_batch_, temporal_table_threads_, ranked.size(), count, covered,
            std::chrono::duration<double>(Clock::now() - started).count());
}

void Cgar::update_pibt_priorities() {
    // Update from observed movement, so turns count as elapsed physical steps.
    // Fixed goals reproduce pibt2. A replacement/retired LoRR goal starts a new
    // priority episode; that extension is not part of upstream's MAPF driver.
    for (int i = 0; i < n_; ++i) {
        const int goal = agents_[i].goal;
        if (pibt_priorities_ready_)
            pibt_elapsed_[i] = pibt_previous_goal_[i] < 0 || loc_[i] == pibt_previous_goal_[i]
                                  ? 0 : pibt_elapsed_[i] + 1;
        if (!pibt_priorities_ready_ || goal != pibt_previous_goal_[i]) {
            pibt_elapsed_[i] = 0;
            pibt_initial_distance_[i] = goal < 0 ? 0 : route_h(i, loc_[i]);
        }
        pibt_previous_goal_[i] = goal;
    }
    pibt_priorities_ready_ = true;
}

void Cgar::compute_order(int primary) {
    if (pibt_reference_ && !pibt_tickets_) {
        if (order_.size() != static_cast<size_t>(n_)) {
            order_.resize(n_);
            for (int i = 0; i < n_; ++i) order_[i] = i;
        }
        // Preserve the previous order on subsequent sorts, as upstream does.
        std::sort(order_.begin(), order_.end(), [&](int x, int y) {
            if (pibt_elapsed_[x] != pibt_elapsed_[y]) return pibt_elapsed_[x] > pibt_elapsed_[y];
            if (pibt_initial_distance_[x] != pibt_initial_distance_[y])
                return pibt_initial_distance_[x] > pibt_initial_distance_[y];
            return pibt_tie_[x] > pibt_tie_[y];
        });
        return;
    }
    order_.resize(n_);
    for (int i = 0; i < n_; ++i) order_[i] = i;
    auto cls = [&](int i) {
        const Agent& a = agents_[i];
        if (i == primary) return 0;
        const int p = cert_.pocket[loc_[i]];
        if (p >= 0 && (a.goal < 0 || cert_.pocket[a.goal] != p)) return 1;  // must leave a pocket
        return a.goal >= 0 ? 2 : 3;
    };
    std::vector<int> classes(n_);
    for (int i = 0; i < n_; ++i) classes[i] = cls(i);
    std::sort(order_.begin(), order_.end(), [&](int x, int y) {
        if (classes[x] != classes[y]) return classes[x] < classes[y];
        if (agents_[x].ticket != agents_[y].ticket) return agents_[x].ticket < agents_[y].ticket;
        return x < y;
    });
}

int Cgar::select_primary() {
    auto pending = [&](int i) {
        return i >= 0 && agents_[i].goal >= 0 && loc_[i] != agents_[i].goal && !parked_[i];
    };
    if (pending(primary_) && agents_[primary_].ticket == primary_ticket_) return primary_;
    int best = -1;
    for (int i = 0; i < n_; ++i) {
        if (pending(i) && (best < 0 || agents_[i].ticket < agents_[best].ticket)) best = i;
    }
    primary_ticket_ = best < 0 ? kIdleTicket : agents_[best].ticket;
    return best;
}

int Cgar::route_h(int i, int cell, ProgressBasis* basis) {
    ++stats_.route_queries;
    auto observe = [&](int distance, ProgressBasis kind) {
        if (basis) *basis = kind;
        return distance;
    };
    const Agent& a = agents_[i];
    if (pibt_reference_ && !enable_locks_) {
        if (a.goal < 0) return observe(cell == loc_[i] ? 0 : 1, ProgressBasis::None);
        const auto* table = oracle_.try_table(a.goal, distance_deadline_);
        return observe(oracle_.value(*table, cell), ProgressBasis::RouteTable);
    }
    const int pocket = cert_.pocket[cell];
    const int source_pocket = cert_.pocket[loc_[i]];
    if (source_pocket >= 0 && pocket_draining_[source_pocket])
        return observe(pocket < 0 ? 0 : kFar + cert_.exit_dist[cell], ProgressBasis::PocketExit);
    if (a.goal < 0) return observe(pocket < 0 ? 0 : kFar + cert_.exit_dist[cell], ProgressBasis::PocketExit);
    const std::vector<int>* t = oracle_.find(a.goal);
    if (t == nullptr) {
        // Building a table is a full-map BFS. Cap how many a single step may build,
        // otherwise a map with thousands of distinct goals blows the time budget;
        // robots that miss out steer by straight-line distance until a later step.
        if (table_budget_ <= 0 && !pibt_reference_) { ++stats_.route_manhattan; return observe(oracle_.manhattan(cell, a.goal), ProgressBasis::Manhattan); }
        --table_budget_;
        t = oracle_.try_table(a.goal, distance_deadline_);
    }
    if (oracle_.value(*t, cell) < kInf) return observe(oracle_.value(*t, cell), ProgressBasis::RouteTable);
    if (pocket >= 0) return observe(kFar + cert_.exit_dist[cell], ProgressBasis::PocketExit);
    return observe(kInf, ProgressBasis::RouteTable);
}

bool Cgar::allowed(int i, int cell) const {
    if (!cert_.free[cell]) return false;
    if (capacity_mode_) return !parked_[i] && cert_.core[cell];
    if (cert_.core[cell]) return true;
    const int p = cert_.pocket[cell];
    if (p < 0) return false;  // disconnected floor that is not a pocket (ablation)
    return p == cert_.pocket[loc_[i]] || (!pocket_draining_[p] && pocket_lock_[p] == i);
}

int Cgar::turn_steps(int i, int cell) const {
    if (cell == loc_[i]) return 0;
    const int diff = (direction(loc_[i], cell, cert_.cols) - ori_[i] + 4) % 4;
    return diff == 0 ? 0 : (diff == 2 ? 2 : 1);
}

void Cgar::reserve(int cell, int who) {
    if (reserved_[cell] == -1) touched_.push_back(cell);
    reserved_[cell] = who;
}

PibtCandidates Cgar::pibt_candidates(int i) {
    const int u = loc_[i];
    check_deadline(deadline_, "pibt");
    if (pibt_reference_) {
        PibtCandidates result;
        // Match grid-pathfinding's west, east, north, south, then wait order.
        for (int d : {2, 0, 3, 1}) {
            const int v = neighbor(u, d);
            if (v >= 0 && allowed(i, v)) result.cells[result.size++] = v;
        }
        result.cells[result.size++] = u;
        std::shuffle(result.cells.begin(), result.cells.begin() + result.size, rng_);
        std::sort(result.cells.begin(), result.cells.begin() + result.size, [&](int v, int w) {
            const int dv = route_h(i, v), dw = route_h(i, w);
            if (dv != dw) return dv < dw;
            return (occ_now_[v] >= 0) < (occ_now_[w] >= 0);
        });
        return result;
    }
    struct Cand {
        int cell, h, occupied, turns;
        unsigned tie;
    };
    Cand cands[5];
    int m = 0;
    cands[m++] = {u, route_h(i, u), 0, 0, static_cast<unsigned>(rng_())};
    for (int d = 0; d < 4; ++d) {
        const int v = neighbor(u, d);
        if (v < 0 || !allowed(i, v)) continue;
        cands[m++] = {v, route_h(i, v), occ_now_[v] >= 0 ? 1 : 0, turn_steps(i, v), static_cast<unsigned>(rng_())};
    }
    if (orientation_guidance_ && agents_[i].goal >= 0 &&
        !(cert_.pocket[u] >= 0 && pocket_draining_[cert_.pocket[u]])) {
        const int goal = agents_[i].goal;
        const auto* table = turn_oracle_.find(goal);
        if (!table && turn_table_budget_ > 0 && (orientation_guidance_ == 1 || oriented_goals_.count(goal))) {
            --turn_table_budget_;
            table = turn_oracle_.table(goal, deadline_);
            ++stats_.oriented_builds;
        }
        if (table && turn_oracle_.value(*table, u, ori_[i]) < kInf) {
            ++stats_.oriented_guided;
            // Compare complete turn-then-forward macros, including a wait
            // in the same metric units. Every robot uses one cost basis.
            for (int k = 0; k < m; ++k) {
                const int v = cands[k].cell;
                const int d = v == u ? ori_[i] : direction(u, v, cert_.cols);
                const int remaining = turn_oracle_.value(*table, v, d);
                cands[k].h = remaining >= kInf ? kInf : remaining + (v == u ? flow_cost_scale_ : turn_oracle_.forward_cost(u, d)) + guidance_turn_cost_ * cands[k].turns;
            }
        } else ++stats_.oriented_fallback;
    }
    std::sort(cands, cands + m, [&](const Cand& a, const Cand& b) {
        if (a.h != b.h) return a.h < b.h;
        if (turn_first_ && a.turns != b.turns) return a.turns < b.turns;
        if (a.occupied != b.occupied) return a.occupied < b.occupied;
        if (a.turns != b.turns) return a.turns < b.turns;
        return a.tie < b.tie;
    });
    PibtCandidates result;
    for (int k = 0; k < m; ++k) if (cands[k].h < kInf) result.cells[result.size++] = cands[k].cell;
    return result;
}

bool Cgar::pibt(int i, int parent) {
    auto candidates = [&](int robot) { return pibt_candidates(robot); };
    auto reservation = [&](int cell, int robot) { reserve(cell, robot); };
    return assign_pibt(i, parent, loc_, occ_now_, reserved_, next_, candidates, reservation);
}

void Cgar::abort_txn() {
    for (Agent& a : agents_) {
        if (!a.in_txn) continue;
        a.in_txn = false;
        a.committed = -1;
        a.commit_age = 0;
    }
    txn_cells_.clear();
    txn_moves_.clear();
    txn_cursor_ = txn_wave_end_ = 0;
    txn_age_ = 0;
    ++stats_.txn_aborts;
}

// Construct a serial hole shift against a private placement. Nothing is admitted
// unless the entire requested witness succeeds; every appended move has an empty
// destination. The executor can safely commute distinct robots into one wave.
bool Cgar::clear_cell(int cell, int forbidden, const std::vector<char>& region,
                      std::vector<int>& occupancy, std::vector<int>& positions,
                      std::vector<Shift>& witness) {
    if (occupancy[cell] < 0) return true;
    std::vector<int> prev(region.size(), -1), queue{cell};
    prev[cell] = cell;
    int hole = -1;
    for (size_t head = 0; head < queue.size(); ++head) {
        if ((head & 255) == 0) check_deadline(deadline_, "recovery_search");
        const int u = queue[head];
        if (occupancy[u] < 0) { hole = u; break; }
        for (int d = 0; d < 4; ++d) {
            const int v = neighbor(u, d);
            if (v < 0 || v == forbidden || !region[v] || prev[v] >= 0) continue;
            prev[v] = u;
            queue.push_back(v);
        }
    }
    if (hole < 0) return false;
    while (hole != cell) {
        const int from = prev[hole], robot = occupancy[from];
        if (robot < 0) return false;
        witness.push_back({robot, from, hole});
        occupancy[hole] = robot;
        occupancy[from] = -1;
        positions[robot] = hole;
        hole = from;
    }
    return true;
}

void Cgar::install_txn(std::vector<Shift> witness) {
    if (witness.empty()) return;
    // Independently replay the complete certificate before changing live state.
    auto occupancy = occ_now_;
    for (const Shift& m : witness) {
        bool adjacent = false;
        for (int d = 0; d < 4; ++d) adjacent |= neighbor(m.from, d) == m.to;
        if (!adjacent || !cert_.free[m.to] || occupancy[m.from] != m.robot || occupancy[m.to] >= 0)
            return;
        occupancy[m.from] = -1;
        occupancy[m.to] = m.robot;
    }
    txn_moves_ = std::move(witness);
    txn_cursor_ = txn_wave_end_ = 0;
    txn_age_ = 0;
    txn_cells_.clear();
    for (const Shift& m : txn_moves_) {
        txn_cells_.push_back(m.from);
        txn_cells_.push_back(m.to);
        agents_[m.robot].in_txn = true;
    }
    std::sort(txn_cells_.begin(), txn_cells_.end());
    txn_cells_.erase(std::unique(txn_cells_.begin(), txn_cells_.end()), txn_cells_.end());
    for (Agent& a : agents_) {
        if (a.in_txn || (a.committed >= 0 && std::binary_search(txn_cells_.begin(), txn_cells_.end(), a.committed))) {
            a.committed = -1;
            a.commit_age = 0;
        }
    }
    ++stats_.txns;
    stats_.txn_moves += txn_moves_.size();
    advance_txn();
}

void Cgar::advance_txn() {
    if (txn_moves_.empty()) return;
    ++txn_age_;
    bool complete = txn_wave_end_ > txn_cursor_;
    for (size_t k = txn_cursor_; k < txn_wave_end_; ++k) {
        const Shift& m = txn_moves_[k];
        if (loc_[m.robot] != m.from && loc_[m.robot] != m.to) {
            abort_txn();  // Only an external placement change invalidates a witness.
            return;
        }
        complete &= loc_[m.robot] == m.to;
    }
    if (complete) {
        for (size_t k = txn_cursor_; k < txn_wave_end_; ++k) agents_[txn_moves_[k].robot].committed = -1;
        txn_cursor_ = txn_wave_end_;
    }
    if (txn_cursor_ == txn_moves_.size()) {
        for (Agent& a : agents_) if (a.in_txn) {
            a.in_txn = false;
            a.committed = -1;
            a.commit_age = 0;
        }
        txn_moves_.clear();
        txn_cells_.clear();
        txn_cursor_ = txn_wave_end_ = 0;
        return;
    }
    if (txn_cursor_ == txn_wave_end_) {
        std::vector<char> used(n_, 0);
        for (; txn_wave_end_ < txn_moves_.size(); ++txn_wave_end_) {
            const Shift& m = txn_moves_[txn_wave_end_];
            if (used[m.robot]) break;
            if (loc_[m.robot] != m.from) { abort_txn(); return; }
            used[m.robot] = 1;
        }
    }
    for (size_t k = txn_cursor_; k < txn_wave_end_; ++k) {
        const Shift& m = txn_moves_[k];
        agents_[m.robot].committed = loc_[m.robot] == m.to ? -1 : m.to;
    }
}

// Remove every occupant from a contested pocket. The closest resident has an
// unoccupied path to a core portal; a core vacancy shift clears that portal.
// Repetition constructs a complete normalization suffix, including root backoff.
bool Cgar::evacuate_pocket(int pocket) {
    auto occupancy = occ_now_;
    auto positions = loc_;
    std::vector<Shift> witness;
    for (;;) {
        int robot = -1;
        for (int i = 0; i < n_; ++i) {
            if (cert_.pocket[positions[i]] != pocket) continue;
            if (robot < 0 || cert_.exit_dist[positions[i]] < cert_.exit_dist[positions[robot]]) robot = i;
        }
        if (robot < 0) break;
        check_deadline(deadline_, "pocket_evacuation");
        while (!cert_.core[positions[robot]]) {
            const int u = positions[robot];
            int v = -1;
            for (int d = 0; d < 4; ++d) {
                const int w = neighbor(u, d);
                if (w >= 0 && cert_.free[w] && cert_.exit_dist[w] < cert_.exit_dist[u]) { v = w; break; }
            }
            if (v < 0) return false;
            if (cert_.core[v] && !clear_cell(v, -1, cert_.core, occupancy, positions, witness)) return false;
            if (occupancy[v] >= 0) return false;
            witness.push_back({robot, u, v});
            occupancy[u] = -1;
            occupancy[v] = robot;
            positions[robot] = v;
        }
    }
    if (witness.empty()) return false;
    install_txn(std::move(witness));
    if (txn_moves_.empty()) return false;
    ++stats_.evacuations;
    return true;
}

int Cgar::parked_count() const {
    return std::count(parked_.begin(), parked_.end(), 1);
}

void Cgar::prepare_capacity_mode() {
    if (!capacity_mode_ || parking_ready_) return;
    if (env_->curr_states.size() != static_cast<size_t>(n_)) return;
    size_t active = 0;
    for (int i = 0; i < n_; ++i) {
        parked_[i] = !cert_.core[env_->curr_states[i].location];
        active += !parked_[i];
    }
    if (txn_moves_.empty() && active <= cert_.capacity && active < cert_.core_size) {
        parking_ready_ = active_certified_ = true;
        std::printf("[cgar] capacity_mode active=%zu parked=%d capacity=%zu core_tasks_only=1\n",
                    active, parked_count(), cert_.capacity);
    }
}

bool Cgar::eligible_task(const Task& task) const {
    if (!capacity_mode_) return true;
    for (size_t k = task.idx_next_loc; k < task.locations.size(); ++k)
        if (!cert_.core[task.locations[k]]) return false;
    return true;
}

void Cgar::try_install_txn(int primary) {
    const int u = loc_[primary];
    const int hu = route_h(primary, u);
    int v = -1, hv = kInf;
    for (int d = 0; d < 4; ++d) {
        const int w = neighbor(u, d);
        if (w < 0 || !allowed(primary, w)) continue;
        if (cert_.core[u] && cert_.core[w] && !(cert_.promotion[u] >> d & 1)) continue;
        const int h = route_h(primary, w);
        if (h >= hu) continue;
        if (v < 0 || h < hv || (h == hv && turn_steps(primary, w) < turn_steps(primary, v))) { v = w; hv = h; }
    }
    if (v < 0) return;
    auto occupancy = occ_now_;
    auto positions = loc_;
    std::vector<Shift> witness;
    if (!cert_.core[v]) {
        // Exclusivity is not a vacancy certificate: an occupied portal must be
        // normalized first, even if the primary is already inside that pocket.
        if (occupancy[v] >= 0) {
            pocket_draining_[cert_.pocket[v]] = 1;
            evacuate_pocket(cert_.pocket[v]);
            return;
        }
    } else if (!clear_cell(v, u, cert_.core, occupancy, positions, witness)) {
        ++stats_.txn_no_hole;
        return;
    }
    witness.push_back({primary, u, v});
    install_txn(std::move(witness));
}

Action Cgar::action_toward(int i, int target) const {
    const int d = direction(loc_[i], target, cert_.cols);
    const int o = ori_[i];
    if (d == o) return Action::FW;
    if (d == (o + 1) % 4) return Action::CR;
    if (d == (o + 3) % 4) return Action::CCR;
    return Action::CR;
}

bool Cgar::move_check(int i, std::vector<char>& checked, std::vector<Action>& actions) {
    if (checked[i] && actions[i] == Action::FW) return true;
    checked[i] = 1;
    if (actions[i] != Action::FW) return false;
    const int j = occ_now_[next_[i]];
    if (j < 0) return true;
    if (move_check(j, checked, actions)) return true;
    actions[i] = Action::W;
    return false;
}

// Final guard: resolve any vertex or swap conflict by turning moves into waits.
void Cgar::make_safe(std::vector<Action>& actions) {
    static thread_local std::vector<int> owner;
    if (owner.size() != cert_.free.size()) owner.assign(cert_.free.size(), -1);
    std::vector<int> nxt(n_);
    for (bool changed = true; changed;) {
        changed = false;
        for (int i = 0; i < n_; ++i) nxt[i] = actions[i] == Action::FW ? next_[i] : loc_[i];
        for (int i = 0; i < n_; ++i) {
            int& o = owner[nxt[i]];
            if (o < 0) {
                o = i;
                continue;
            }
            for (int x : {i, o}) {
                if (actions[x] == Action::FW) {
                    actions[x] = Action::W;
                    changed = true;
                    ++stats_.safety_waits;
                }
            }
        }
        for (int i = 0; i < n_; ++i) {
            if (actions[i] != Action::FW) continue;
            const int j = occ_now_[nxt[i]];
            if (j >= 0 && actions[j] == Action::FW && nxt[j] == loc_[i]) {
                actions[i] = actions[j] = Action::W;
                changed = true;
                ++stats_.safety_waits;
            }
        }
        for (int i = 0; i < n_; ++i) owner[nxt[i]] = -1;
    }
}

void Cgar::plan(SharedEnvironment* env, int time_limit_ms, std::vector<Action>& actions) {
    plan(env, Clock::now() + std::chrono::milliseconds(time_limit_ms), actions);
}

void Cgar::plan(SharedEnvironment* env, Clock::time_point deadline, std::vector<Action>& actions) {
    check_deadline(deadline, "planning_start");
    env_ = env;
    actions.assign(n_, Action::W);
    deadline_ = deadline;
    distance_deadline_ = deadline_;
    oracle_.trim();
    table_budget_ = plan_tables_;
    turn_table_budget_ = turn_build_limit_;
    if (orientation_guidance_) turn_oracle_.trim();
    sync_agents();
    if (flow_strength_ && !static_trick_metric_ && flow_guidance_.observe(env_->curr_timestep, loc_)) {
        const bool changed = turn_oracle_.set_forward_costs(flow_guidance_.costs());
        const bool cache_only = flow_guidance_.cache_only_refresh() && flow_guidance_.publications() > 1;
        if (cache_only && !changed) turn_oracle_.clear_tables();
        const bool reset = changed || cache_only;
        ++stats_.flow_publications; stats_.flow_cache_resets += reset; stats_.flow_cache_only_resets += cache_only;
        stats_.flow_freezes += flow_guidance_.frozen(); stats_.flow_penalized_edges = flow_guidance_.penalized_edges();
        if (diagnostics_) std::printf("[cgar-flow] step=%d samples=%d moves=%llu strength=%d margin_percent=%d penalized_edges=%d cache_reset=%d frozen=%d publications=%d refresh_interval=%d cost_scale=%d metric_changed=%d cache_only_refresh=%d\n",
            env_->curr_timestep, flow_guidance_.samples(), static_cast<unsigned long long>(flow_guidance_.moves()),
            flow_strength_, flow_guidance_.minimum_margin_percent(), flow_guidance_.penalized_edges(), reset,
            int(flow_guidance_.frozen()), flow_guidance_.publications(), flow_guidance_.refresh_interval(), flow_cost_scale_, changed, int(flow_guidance_.cache_only_refresh()));
        check_deadline(deadline_, "flow_guidance_published");
    }
    advance_txn();
    prepare_capacity_mode();
    update_locks();
    if (orientation_guidance_ == 2 && (env_->curr_timestep % 32 == 0 || oriented_goals_.empty()))
        refresh_orientation_cache();
    primary_ = select_primary();
    if (temporal_) {
        for (int i = 0; i < n_; ++i) {
            auto& a = agents_[i];
            if (i == primary_ || a.in_txn || parked_[i] || a.lock >= 0 || !cert_.core[loc_[i]]) continue;
            a.committed = -1; a.commit_age = 0;
        }
    }
    // The persistent primary gets an exact potential before optional routing work.
    if (primary_ >= 0) oracle_.try_table(agents_[primary_].goal, distance_deadline_);

    if (capacity_mode_ && !parking_ready_ && txn_moves_.empty()) {
        // A fully occupied core needs one more resident parked. Exclude existing
        // core holes from the search, so a successful shift adds a new core hole.
        auto region = cert_.free;
        for (size_t c = 0; c < region.size(); ++c) if (cert_.core[c] && occ_now_[c] < 0) region[c] = 0;
        for (int i = 0; i < n_; ++i) {
            check_deadline(deadline_, "capacity_bootstrap");
            if (!cert_.core[loc_[i]]) continue;
            bool portal = false;
            for (int d = 0; d < 4; ++d) {
                const int v = neighbor(loc_[i], d);
                portal |= v >= 0 && cert_.free[v] && !cert_.core[v];
            }
            if (!portal) continue;
            auto occupancy = occ_now_;
            auto positions = loc_;
            std::vector<Shift> witness;
            if (clear_cell(loc_[i], -1, region, occupancy, positions, witness)) {
                install_txn(std::move(witness));
                break;
            }
        }
    }

    int urgent_pocket = -1;
    if (primary_ >= 0 && !capacity_mode_) {
        const int gp = cert_.pocket[agents_[primary_].goal];
        if (gp >= 0) {
            const int resident = cert_.pocket[loc_[primary_]] == gp ? 1 : 0;
            if (pocket_occ_[gp] > resident) pocket_draining_[gp] = 1;
            pocket_lock_[gp] = primary_;
            agents_[primary_].lock = gp;
            if (pocket_draining_[gp]) urgent_pocket = gp;
        }
        const int here = cert_.pocket[loc_[primary_]];
        if (here >= 0 && pocket_draining_[here]) urgent_pocket = here;
    }
    if (enable_txn_ && active_certified_ && txn_moves_.empty() && !capacity_mode_) {
        if (urgent_pocket < 0) {
            for (size_t p = 0; p < pocket_draining_.size(); ++p)
                if (pocket_draining_[p] && pocket_occ_[p] > 0) { urgent_pocket = static_cast<int>(p); break; }
        }
        if (urgent_pocket >= 0) evacuate_pocket(urgent_pocket);
    }

    if (pibt_reference_) update_pibt_priorities();
    compute_order(primary_);
    for (int i : order_) {
        Agent& a = agents_[i];
        if (a.goal < 0 || !enable_locks_ || capacity_mode_) continue;
        const int gp = cert_.pocket[a.goal];
        if (gp < 0 || cert_.pocket[loc_[i]] >= 0 || pocket_draining_[gp]) continue;
        if (pocket_lock_[gp] >= 0 || pocket_occ_[gp] > 0 || !adjacent_to_pocket(loc_[i], gp)) continue;
        pocket_lock_[gp] = i;
        a.lock = gp;
        ++stats_.lock_grants;
    }
    for (int i = 0; i < n_; ++i) {
        Agent& a = agents_[i];
        if (a.goal < 0 || parked_[i]) continue;
        ProgressBasis basis;
        const int h = route_h(i, loc_[i], &basis);
        stats_.progress_basis_resets += a.observe_progress(h, basis, stable_stall_basis_);
    }
    if (enable_txn_ && active_certified_ && txn_moves_.empty() && primary_ >= 0 &&
        agents_[primary_].stall >= stall_limit_) {
        check_deadline(deadline_, "primary_recovery");
        // A promotion must use a complete, exact distance table.
        if (oracle_.has(agents_[primary_].goal)) try_install_txn(primary_);
    }

    std::vector<char> diagnostic_commitments;
    if (diagnostics_) {
        diagnostic_commitments.resize(n_);
        for (int i = 0; i < n_; ++i) diagnostic_commitments[i] = agents_[i].committed >= 0;
    }
    for (int c : touched_) reserved_[c] = -1;
    touched_.clear();
    next_.assign(n_, -1);
    for (int c : txn_cells_) reserve(c, -2);
    for (int i = 0; i < n_; ++i) {
        Agent& a = agents_[i];
        if (parked_[i] || a.in_txn || reserved_[loc_[i]] == -2 || (capacity_mode_ && !parking_ready_)) {
            next_[i] = loc_[i];
            reserve(loc_[i], i);
        } else if (a.committed >= 0) {
            if (reserved_[a.committed] == -1 && allowed(i, a.committed)) {
                reserve(a.committed, i);
                next_[i] = a.committed;
            } else a.committed = -1;
        }
    }
    // The complete witness has been replay-validated; the current wave contains
    // distinct robots. Its dependency chain may move together after turns finish.
    for (size_t k = txn_cursor_; k < txn_wave_end_; ++k) {
        const Shift& m = txn_moves_[k];
        next_[m.robot] = m.to;
        reserve(m.to, m.robot);
    }
    if (turn_prefetch_threads_) {
        std::vector<int> goals; std::unordered_set<int> seen;
        for (int i : order_) {
            const int goal = agents_[i].goal;
            if (next_[i] != -1 || goal < 0 || turn_oracle_.has(goal) ||
                (orientation_guidance_ == 2 && !oriented_goals_.count(goal)) || !seen.insert(goal).second) continue;
            goals.push_back(goal); if (goals.size() == 32) break;
        }
        turn_oracle_.prefetch(goals, turn_prefetch_threads_, deadline_);
    }
    for (int i : order_) {
        if (next_[i] != -1) continue;
        check_deadline(deadline_, "action_planning");
        pibt(i, -1);
    }
    if (turn_prefetch_threads_) {
        turn_oracle_.discard_prefetch();
        if (diagnostics_ && (env_->curr_timestep + 1) % 200 == 0)
            std::printf("[cgar-turn-prefetch] step=%d threads=%d built=%lld used=%lld discarded=%lld\n",
                        env_->curr_timestep + 1, turn_prefetch_threads_, turn_oracle_.prefetched_builds,
                        turn_oracle_.prefetched_hits, turn_oracle_.prefetched_discarded);
        check_deadline(deadline_, "turn_prefetch_demand_complete");
    }
    for (int i = 0; i < n_; ++i) {
        if (next_[i] == loc_[i]) continue;
        if (agents_[i].committed != next_[i]) agents_[i].commit_age = 0;
        if (!pibt_reference_ || pibt_commitments_ || agents_[i].in_txn)
            agents_[i].committed = next_[i];
        actions[i] = action_toward(i, next_[i]);
    }
    auto offered = diagnostics_ ? actions : std::vector<Action>();
    std::vector<char> checked(n_, 0);
    for (int i = 0; i < n_; ++i) if (!checked[i] && actions[i] == Action::FW) move_check(i, checked, actions);
    make_safe(actions);
    if (temporal_) {
        plan_temporal(actions);
        if (diagnostics_) offered = actions;
    }
    if (diagnostics_) {
        record_movement(offered, actions, diagnostic_commitments);
        if ((env_->curr_timestep + 1) % 200 == 0) log_movement();
    }
    if (env_->curr_timestep % 200 == 0) log_summary();
    check_deadline(deadline_, "planning_complete");
}

void Cgar::record_movement(const std::vector<Action>& offered, const std::vector<Action>& actions,
                           const std::vector<char>& commitments) {
    // Memoize the terminal offered action in each dependency chain. A cycle of
    // forward moves has no stationary terminal; final safety rejection is separate.
    std::vector<int> terminal(n_, -1), seen(n_, -1), chain;
    for (int root = 0; root < n_; ++root) {
        if (terminal[root] >= 0) continue;
        chain.clear();
        int j = root, kind = 4;
        while (j >= 0 && terminal[j] < 0 && seen[j] != root) {
            seen[j] = root; chain.push_back(j);
            if (offered[j] != Action::FW) { kind = static_cast<int>(offered[j]); break; }
            j = occ_now_[next_[j]];
        }
        if (j >= 0 && terminal[j] >= 0) kind = terminal[j];
        for (int k : chain) terminal[k] = kind;
    }
    for (int i = 0; i < n_; ++i) {
        const Agent& a = agents_[i];
        auto& m = stats_.movement[a.goal < 0 ? 0 : (a.stop <= 0 ? 1 : 2)];
        ++m.actions[static_cast<int>(actions[i])];
        m.recovery += a.in_txn;
        m.primary += i == primary_;
        m.commitment += commitments[i];
        m.pocket += cert_.pocket[loc_[i]] >= 0;
        if (actions[i] == Action::W) {
            if (offered[i] == Action::W) ++m.planned_wait;
            else if (terminal[i] == Action::CR || terminal[i] == Action::CCR || terminal[i] == Action::W) {
                ++m.blocked_forward;
                m.turn_dependency += terminal[i] == Action::CR || terminal[i] == Action::CCR;
            } else ++m.safety_cancel;
        }
        if (actions[i] != Action::FW) continue;
        const auto* table = a.goal >= 0 ? oracle_.peek(a.goal) : nullptr;
        const int before = table ? oracle_.value(*table, loc_[i]) : kInf;
        const int after = table ? oracle_.value(*table, next_[i]) : kInf;
        if (before >= kInf || after >= kInf) ++m.forward_unknown;
        else if (after < before) ++m.forward_closer;
        else if (after > before) ++m.forward_farther;
        else ++m.forward_equal;
    }
}

void Cgar::log_movement() const {
    std::printf("[cgar-orientation] steps=%d enabled=%d turn_first=%d build_limit=%d builds=%lld guided=%lld fallback=%lld\n",
                env_->curr_timestep + 1, orientation_guidance_, turn_first_, turn_build_limit_, stats_.oriented_builds,
                stats_.oriented_guided, stats_.oriented_fallback);
    for (int phase = 0; phase < 3; ++phase) {
        const auto& m = stats_.movement[phase];
        std::printf("[cgar-movement] steps=%d phase=%d fw=%lld cr=%lld ccr=%lld wait=%lld "
                    "planned_wait=%lld blocked_forward=%lld safety_cancel=%lld turn_dependency=%lld "
                    "closer=%lld farther=%lld equal=%lld unknown=%lld recovery=%lld primary=%lld "
                    "commitment=%lld pocket=%lld expired_commitments=%lld\n",
                    env_->curr_timestep + 1, phase, m.actions[0], m.actions[1], m.actions[2], m.actions[3],
                    m.planned_wait, m.blocked_forward, m.safety_cancel, m.turn_dependency,
                    m.forward_closer, m.forward_farther, m.forward_equal, m.forward_unknown,
                    m.recovery, m.primary, m.commitment, m.pocket, stats_.expired_commitments);
    }
    // The competition driver uses _exit on normal shutdown, so the final
    // periodic diagnostic sample must not remain in the C stdout buffer.
    std::fflush(stdout);
}

void Cgar::log_summary() {
    if (native_trick_metric_) std::printf("[cgar-native-metric] t=%d forward_base=20 turn=1 cost_limit=%d wide_fallback_tables=%lld\n",
        env_->curr_timestep, guidance_cost_limit_, turn_oracle_.wide_fallback_tables);
    int in_txn = 0, locks = 0, idle = 0;
    for (const Agent& a : agents_) { in_txn += a.in_txn; idle += a.goal < 0; }
    for (int h : pocket_lock_) locks += h >= 0;
    std::printf("[cgar] t=%d primary=%d stall=%d txns=%lld moves=%lld aborts=%lld no_hole=%lld in_txn=%d "
                "locks=%d grants=%lld safety_waits=%lld idle=%d assigned=%lld fair=%lld tables=%s "
                "evacuations=%lld parked=%d active_certified=%d\n",
                env_->curr_timestep, primary_, primary_ >= 0 ? agents_[primary_].stall : -1, stats_.txns,
                stats_.txn_moves, stats_.txn_aborts, stats_.txn_no_hole, in_txn, locks, stats_.lock_grants,
                stats_.safety_waits, idle, stats_.assignments, stats_.fair_assignments,
                oracle_.full() ? "full" : "ok", stats_.evacuations, parked_count(), active_certified_);
    std::printf("[cgar-scheduler] t=%d repair=%d samples=%d pickup_weight=%d calls=%lld local=%lld fallback=%lld fair=%lld "
                "searches=%lld nodes=%lld task_limits=%lld node_limits=%lld deadlines=%lld empty=%lld "
                "skipped_empty=%lld sampled=%lld sample_deadlines=%lld improved=%lld "
                "pickup_estimate=%lld chain_estimate=%lld route_queries=%lld route_manhattan=%lld "
                "global_samples=%d global_evaluations=%lld global_assignments=%lld\n",
                env_->curr_timestep, repair_fallback_, fallback_samples_, pickup_weight_, stats_.schedule_calls,
                stats_.local_assignments, stats_.fallback_assignments, stats_.fair_assignments,
                stats_.candidate_searches, stats_.candidate_nodes, stats_.candidate_task_limits,
                stats_.candidate_node_limits, stats_.candidate_deadlines, stats_.empty_searches,
                stats_.skipped_empty_searches, stats_.sample_evaluations,
                stats_.sample_deadlines, stats_.improved_fallbacks, stats_.estimated_pickup_cost,
                stats_.estimated_chain_cost, stats_.route_queries, stats_.route_manhattan,
                global_samples_, stats_.global_evaluations, stats_.global_assignments);
    if (pickup_flow_)
        std::printf("[cgar-pickup-flow] t=%d enabled=1 node_limit=%d snapshot_publication=%lld searches=%lld pops=%lld states=%lld cells=%lld candidates=%lld limits=%lld cached_estimates=%lld approximate_estimates=%lld warmup_calls=%lld\n",
            env_->curr_timestep, pickup_flow_nodes_, stats_.pickup_flow_snapshot_publication,
            stats_.pickup_flow_searches, stats_.pickup_flow_pops, stats_.pickup_flow_states,
            stats_.pickup_flow_cells, stats_.pickup_flow_candidates, stats_.pickup_flow_limits,
            stats_.pickup_flow_cached_estimates, stats_.pickup_flow_approximate_estimates, stats_.pickup_flow_warmup_calls);
    if (chain_flow_pricing_)
        std::printf("[cgar-chain-price] t=%d mode=%d calls=%lld observations=%lld covered=%lld missing=%lld outside=%lld unreachable=%lld invalid=%lld changed=%lld active_assignments=%lld assigned_covered=%lld assigned_imputed=%lld shadow_queries=%lld shadow_changed2=%lld shadow_changed3=%lld shadow_specific=%lld\n",
            env_->curr_timestep, chain_flow_pricing_, stats_.chain_price_calls, stats_.chain_price_observations,
            stats_.chain_price_outcomes[0], stats_.chain_price_outcomes[1], stats_.chain_price_outcomes[2],
            stats_.chain_price_outcomes[3], stats_.chain_price_outcomes[4], stats_.chain_price_changed,
            stats_.chain_price_assignments, stats_.chain_price_assigned_covered, stats_.chain_price_assigned_imputed, stats_.chain_shadow_queries,
            stats_.chain_shadow_changed2, stats_.chain_shadow_changed3, stats_.chain_shadow_specific);
    if (pickup_full_robots_)
        std::printf("[cgar-pickup-full] t=%d robot_limit=%d threads=%d cost_key=%d fields=%lld pops=%lld states=%lld searches=%lld scans=%lld candidates=%lld estimates=%lld\n",
            env_->curr_timestep, pickup_full_robots_, pickup_full_threads_, pickup_full_cost_key_, stats_.pickup_full_fields,
            stats_.pickup_full_pops, stats_.pickup_full_states, stats_.pickup_full_searches,
            stats_.pickup_full_scans, stats_.pickup_full_candidates, stats_.pickup_full_estimates);
    std::printf("[cgar-estimates] t=%d refine=%d peek=%d stable_stall=%d refined_legs=%lld "
                "changed_costs=%lld invalidations=%lld approximate_reads=%lld table_reads=%lld basis_resets=%lld\n",
                env_->curr_timestep, refine_chain_costs_, scheduler_cache_peek_, stable_stall_basis_,
                refined_chain_cost_.refined_legs, refined_chain_cost_.changed_costs,
                refined_chain_cost_.invalidations, refined_chain_cost_.approximate_reads,
                refined_chain_cost_.table_reads, stats_.progress_basis_resets);
    std::printf("[cgar-reassignment] t=%d enabled=%d passes=%lld eligible=%lld sources=%lld nodes=%lld "
                "pairs=%lld swaps=%lld estimated_saving=%lld table_pairs=%lld manhattan_pairs=%lld "
                "primary_protected=%lld recovery_protected=%lld fair_protected=%lld\n",
                env_->curr_timestep, reassign_, stats_.reassign_passes, stats_.reassign_eligible,
                stats_.reassign_sources, stats_.reassign_nodes, stats_.reassign_pairs, stats_.reassign_swaps,
                stats_.reassign_saving, stats_.reassign_table_pairs, stats_.reassign_manhattan_pairs,
                stats_.reassign_primary_protected, stats_.reassign_recovery_protected, stats_.reassign_fair_protected);
    std::printf("[cgar-pool-exchange] t=%d enabled=%d passes=%lld eligible=%lld sources=%lld nodes=%lld pairs=%lld exchanges=%lld pickup_saving=%lld chain_delta=%lld total_saving=%lld missing_pickup=%lld missing_chain=%lld short_pickup=%lld primary_protected=%lld recovery_protected=%lld fair_protected=%lld\n",
        env_->curr_timestep, reassign_pool_, stats_.pool_passes, stats_.pool_eligible, stats_.pool_sources,
        stats_.pool_nodes, stats_.pool_pairs, stats_.pool_exchanges, stats_.pool_pickup_saving,
        stats_.pool_chain_delta, stats_.pool_total_saving, stats_.pool_missing_pickup, stats_.pool_missing_chain,
        stats_.pool_short_pickup, stats_.pool_primary_protected, stats_.pool_recovery_protected, stats_.pool_fair_protected);
    std::printf("[cgar-unopened-match] t=%d enabled=%d passes=%lld eligible=%lld resident=%lld missing=%lld unreachable=%lld groups=%lld selected=%lld anchors=%lld full_groups=%lld nodes=%lld matrix_entries=%lld cycles=%lld accepted_cycles=%lld moved=%lld saving=%lld primary_protected=%lld recovery_protected=%lld fair_protected=%lld budget_protected=%lld pickup_selected=%lld\n",
        env_->curr_timestep, reassign_match_, stats_.match_passes, stats_.match_eligible, stats_.match_resident,
        stats_.match_missing, stats_.match_unreachable, stats_.match_groups, stats_.match_selected,
        stats_.match_anchors, stats_.match_full_groups, stats_.match_nodes, stats_.match_matrix_entries, stats_.match_cycles, stats_.match_accepted_cycles,
        stats_.match_moved, stats_.match_saving, stats_.match_primary_protected,
        stats_.match_recovery_protected, stats_.match_fair_protected, stats_.match_budget_protected, stats_.match_pickup_selected);
    if (known_horizon_)
        std::printf("[cgar-horizon] t=%d known_horizon=%d pairs=%lld impossible_pairs=%lld rank_changes=%lld first_rank_change=%lld assignments=%lld impossible_assignments=%lld assumption=configured\n",
            env_->curr_timestep, known_horizon_, stats_.horizon_pairs, stats_.horizon_impossible_pairs,
            stats_.horizon_rank_changes, stats_.horizon_first_rank_change, stats_.horizon_assignments,
            stats_.horizon_impossible_assignments);
    if (horizon_margin_) {
        const auto model = horizon_margins_.snapshot();
        std::printf("[cgar-horizon-margin] t=%d risky_pairs=%lld risky_assignments=%lld rank_changes=%lld first_rank_change=%lld tracked=%zu invalidated=%lld excluded_completions=%lld n0=%lld n1=%lld n2=%lld n3=%lld n4=%lld sum0=%lld sum1=%lld sum2=%lld sum3=%lld sum4=%lld\n",
            env_->curr_timestep, stats_.horizon_margin_pairs, stats_.horizon_margin_assignments,
            stats_.horizon_margin_rank_changes, stats_.horizon_margin_first_rank_change, horizon_margins_.tracked(),
            horizon_margins_.invalidated, horizon_margins_.excluded_completions,
            model.count[0], model.count[1], model.count[2], model.count[3], model.count[4],
            model.excess[0], model.excess[1], model.excess[2], model.excess[3], model.excess[4]);
    }
    std::fflush(stdout);
}

// ─── scheduler ───────────────────────────────────────────────────────────────

int Cgar::task_chain_cost(int task_id) {
    if (refine_chain_costs_)
        return refined_chain_cost_.estimate(env_->task_pool.at(task_id), oracle_, table_budget_,
                                            distance_deadline_, scheduler_cache_peek_);
    auto it = chain_cost_.find(task_id);
    if (it != chain_cost_.end()) return it->second;
    const Task& task = env_->task_pool.at(task_id);
    long long total = 0;
    bool all_table = true;
    for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k) {
        const auto* table = scheduler_cache_peek_ ? oracle_.peek(task.locations[k]) : oracle_.find(task.locations[k]);
        if (!table && table_budget_ > 0) {
            --table_budget_;
            table = oracle_.try_table(task.locations[k], distance_deadline_);
        }
        const int d = table ? oracle_.distance_from(*table, task.locations[k - 1])
                            : oracle_.manhattan(task.locations[k - 1], task.locations[k]);
        total += std::min(d, kFar);
        all_table &= table != nullptr;
    }
    const int cost = static_cast<int>(std::min<long long>(total, kFar));
    chain_cost_[task_id] = cost;
    if (chain_flow_pricing_) chain_table_basis_[task_id] = all_table;
    return cost;
}

void Cgar::prune_reassignment_records() {
    auto prune = [&](std::unordered_set<int>& records) {
        for (auto it = records.begin(); it != records.end();) {
            const auto task = env_->task_pool.find(*it);
            it = task != env_->task_pool.end() && task->second.idx_next_loc == 0
                ? std::next(it) : records.erase(it);
        }
    };
    prune(reassigned_tasks_);
    prune(fair_tasks_);
}

Cgar::UnopenedCandidates Cgar::unopened_candidates(const std::vector<int>& proposed, bool existing_only, bool include_budget) const {
    constexpr int cooldown = 20;
    const int now = env_->curr_timestep;
    // Also preserve the next pending primary if the previous one just finished.
    // A fresh assignment still has its old episode ticket until sync_agents;
    // it cannot nominate the oldest holder, but can join the candidate scan below.
    int oldest = -1;
    for (int i = 0; i < n_; ++i) {
        const Agent& agent = agents_[i];
        const auto task = env_->task_pool.find(proposed[i]);
        if (parked_[i] || task == env_->task_pool.end() || task->second.idx_next_loc != 0 ||
            agent.task != proposed[i] || agent.stop != 0 || agent.ticket == kIdleTicket ||
            task->second.locations.empty() ||
            env_->curr_states[i].location == task->second.locations.front()) continue;
        if (oldest < 0 || agent.ticket < agents_[oldest].ticket) oldest = i;
    }
    UnopenedCandidates result;
    for (int i = 0; i < n_; ++i) {
        check_deadline(deadline_, "reassignment_eligibility");
        if (i == primary_ || i == oldest) { ++result.primary; continue; }
        const Agent& agent = agents_[i];
        const int cell = env_->curr_states[i].location;
        if (agent.in_txn || std::binary_search(txn_cells_.begin(), txn_cells_.end(), cell)) {
            ++result.recovery; continue;
        }
        if (parked_[i] || agent.lock >= 0 || !cert_.core[cell] ||
            (agent.committed >= 0 && agent.committed != cell) || now - last_reassignment_[i] < cooldown) continue;
        const auto found = env_->task_pool.find(proposed[i]);
        if (found == env_->task_pool.end()) continue;
        const Task& task = found->second;
        if (task.idx_next_loc != 0 || task.locations.empty() || !cert_.core[task.locations.front()] ||
            cell == task.locations.front() || !eligible_task(task)) continue;
        if (fair_tasks_.count(proposed[i])) { ++result.fair; continue; }
        if (reassigned_tasks_.count(proposed[i])) { ++result.budget; if (!include_budget) continue; }
        if (existing_only && (agent.task != proposed[i] || agent.stop != 0 || agent.ticket == kIdleTicket)) continue;
        result.robots.push_back(i);
    }
    return result;
}

// A fixed-work swap pass over unopened, already assigned tasks. No task is
// dropped, and a task can be retargeted at most once before its first pickup.
void Cgar::reassign_unopened(std::vector<int>& proposed) {
    constexpr int interval = 10, source_limit = 256, local_limit = 16;
    constexpr int node_limit = 2048, global_samples = 16, cooldown = 20;
    const int now = env_->curr_timestep;
    if (!reassign_ || now % interval != 0) return;
    ++stats_.reassign_passes;
    prune_reassignment_records();
    auto candidates = unopened_candidates(proposed, false);
    stats_.reassign_primary_protected += candidates.primary;
    stats_.reassign_recovery_protected += candidates.recovery;
    stats_.reassign_fair_protected += candidates.fair;
    const auto& eligible = candidates.robots;
    stats_.reassign_eligible += eligible.size();
    if (eligible.size() < 2) { check_deadline(deadline_, "reassignment_empty"); return; }
    // Index pickup locations with linked lists, avoiding per-cell heap allocation.
    std::vector<int> head(cert_.free.size(), -1), link(n_, -1), seen(cert_.free.size(), 0), queue;
    for (auto it = eligible.rbegin(); it != eligible.rend(); ++it) {
        const int r = *it, cell = env_->task_pool.at(proposed[r]).locations.front();
        link[r] = head[cell]; head[cell] = r;
    }
    std::vector<char> used(n_, 0);
    int generation = 0;
    const size_t start = reassign_cursor_ % eligible.size();
    const size_t sources = std::min<size_t>(source_limit, eligible.size());
    reassign_cursor_ += sources;
    for (size_t offset = 0; offset < sources; ++offset) {
        const int r = eligible[(start + offset) % eligible.size()];
        if (used[r]) continue;
        check_deadline(deadline_, "reassignment_candidates");
        ++stats_.reassign_sources;
        std::vector<int> partners;
        auto add = [&](int other) {
            if (other != r && !used[other] && std::find(partners.begin(), partners.end(), other) == partners.end())
                partners.push_back(other);
        };
        queue.assign(1, env_->curr_states[r].location);
        seen[queue.front()] = ++generation;
        for (size_t pos = 0; pos < queue.size() && pos < node_limit && partners.size() < local_limit; ++pos) {
            if ((pos & 63) == 0) check_deadline(deadline_, "reassignment_search");
            const int cell = queue[pos];
            ++stats_.reassign_nodes;
            for (int other = head[cell]; other >= 0 && partners.size() < local_limit; other = link[other]) add(other);
            for (int d = 0; d < 4; ++d) {
                const int next = neighbor(cell, d);
                if (next < 0 || !cert_.core[next] || seen[next] == generation) continue;
                seen[next] = generation; queue.push_back(next);
            }
        }
        const uint64_t base = (static_cast<uint64_t>(r) + 1) * 2654435761ULL +
                              (static_cast<uint64_t>(now) + 1) * 2246822519ULL;
        for (size_t k = 0; k < std::min<size_t>(global_samples, eligible.size()); ++k)
            add(eligible[(base + k * 3266489917ULL) % eligible.size()]);
        int best = -1, best_saving = 0;
        for (int other : partners) {
            check_deadline(deadline_, "reassignment_cost");
            ++stats_.reassign_pairs;
            const int a = env_->task_pool.at(proposed[r]).locations.front();
            const int b = env_->task_pool.at(proposed[other]).locations.front();
            if (a == b) continue;
            const auto* ta = oracle_.peek(a);
            const auto* tb = oracle_.peek(b);
            // Compare all four distances on one basis. Do not build tables or
            // alter routing LRU order merely to evaluate a speculative swap.
            const bool tables = ta && tb;
            if (tables) ++stats_.reassign_table_pairs;
            else ++stats_.reassign_manhattan_pairs;
            auto distance = [&](int robot, int goal, const std::vector<int>* table) {
                return tables ? oracle_.distance_from(*table, env_->curr_states[robot].location)
                              : oracle_.manhattan(env_->curr_states[robot].location, goal);
            };
            const int aa = distance(r, a, ta), bb = distance(other, b, tb);
            const int ab = distance(r, b, tb), ba = distance(other, a, ta);
            if (std::max({aa, bb, ab, ba}) >= kFar) continue;
            const int before = aa + bb, saving = before - ab - ba;
            // Task-chain terms cancel under a swap; require both an absolute
            // four-step gain and a ten-percent reduction in total pickup travel.
            if (saving < 4 || static_cast<long long>(saving) * 100 < static_cast<long long>(before) * 10) continue;
            if (saving > best_saving || (saving == best_saving && other < best)) {
                best = other; best_saving = saving;
            }
        }
        if (best < 0) continue;
        reassigned_tasks_.insert(proposed[r]);
        reassigned_tasks_.insert(proposed[best]);
        std::swap(proposed[r], proposed[best]);
        for (int robot : {r, best}) {
            used[robot] = 1;
            last_reassignment_[robot] = now;
            // Eligibility guarantees any former movement commitment has arrived.
            agents_[robot].committed = -1;
            agents_[robot].commit_age = 0;
        }
        ++stats_.reassign_swaps;
        stats_.reassign_saving += best_saving;
    }
    check_deadline(deadline_, "reassignment_complete");
}

// Exchange an existing unopened assignment with an available task. Pickup
// savings are required independently of chain length; released tasks retain
// their original metadata and remain available to ordinary scheduling.
void Cgar::exchange_unopened_with_pool(std::vector<int>& proposed) {
    constexpr int interval = 10, source_limit = 256, local_limit = 16;
    constexpr int node_limit = 2048, exchange_limit = 32;
    const int now = env_->curr_timestep;
    if (!reassign_pool_ || now % interval != 0) return;
    ++stats_.pool_passes;
    prune_reassignment_records();
    const auto candidates = unopened_candidates(proposed, true);
    stats_.pool_primary_protected += candidates.primary;
    stats_.pool_recovery_protected += candidates.recovery;
    stats_.pool_fair_protected += candidates.fair;
    const auto& eligible = candidates.robots;
    stats_.pool_eligible += eligible.size();
    if (eligible.empty()) { check_deadline(deadline_, "pool_exchange_empty"); return; }

    // Scheduling may already have assigned tasks this call. Proposed, rather
    // than the simulator's previous agent_assigned fields, defines availability.
    std::unordered_set<int> assigned(proposed.begin(), proposed.end());
    std::vector<int> available;
    for (const auto& [id, task] : env_->task_pool) {
        check_deadline(deadline_, "pool_exchange_index");
        if (!assigned.count(id) && !reassigned_tasks_.count(id) && task.idx_next_loc == 0 &&
            !task.locations.empty() && cert_.core[task.locations.front()] && eligible_task(task))
            available.push_back(id);
    }
    std::sort(available.begin(), available.end());
    if (available.empty()) { check_deadline(deadline_, "pool_exchange_no_tasks"); return; }
    std::vector<int> head(cert_.free.size(), -1), link(available.size(), -1);
    for (int k = int(available.size()) - 1; k >= 0; --k) {
        const int cell = env_->task_pool.at(available[k]).locations.front();
        link[k] = head[cell]; head[cell] = k;
    }

    // Read only complete cached tables: no speculative table construction,
    // partial BFS chain estimates, mixed Manhattan scores or LRU promotion.
    std::unordered_map<int, int> chains;
    auto chain = [&](int id) {
        const auto found = chains.find(id);
        if (found != chains.end()) return found->second;
        const auto& locations = env_->task_pool.at(id).locations;
        long long total = 0;
        for (size_t k = 1; k < locations.size(); ++k) {
            check_deadline(deadline_, "pool_exchange_chain");
            const auto* table = oracle_.peek(locations[k]);
            if (!table) { total = kFar; break; }
            const int distance = oracle_.distance_from(*table, locations[k - 1]);
            if (distance >= kFar) { total = kFar; break; }
            total += distance;
            if (total >= kFar) { total = kFar; break; }
        }
        return chains.emplace(id, int(total)).first->second;
    };
    struct Pair { int robot, task, pickup_saving, old_chain, new_chain; long long new_total; };
    std::vector<Pair> pairs;
    std::vector<int> seen(cert_.free.size(), 0), distances(cert_.free.size(), 0), queue;
    queue.reserve(node_limit * 2);
    int generation = 0;
    const size_t start = pool_reassign_cursor_ % eligible.size();
    const size_t sources = std::min<size_t>(source_limit, eligible.size());
    pool_reassign_cursor_ += sources;
    for (size_t offset = 0; offset < sources; ++offset) {
        check_deadline(deadline_, "pool_exchange_source");
        const int r = eligible[(start + offset) % eligible.size()];
        ++stats_.pool_sources;
        const auto& old = env_->task_pool.at(proposed[r]);
        const auto* pickup_table = oracle_.peek(old.locations.front());
        if (!pickup_table) { ++stats_.pool_missing_pickup; continue; }
        const int from = env_->curr_states[r].location;
        const int old_pickup = oracle_.distance_from(*pickup_table, from);
        if (old_pickup >= kFar) { ++stats_.pool_missing_pickup; continue; }
        const int margin = std::max(4, (old_pickup + 9) / 10);
        if (old_pickup < margin) { ++stats_.pool_short_pickup; continue; }
        const int old_chain = chain(proposed[r]);
        if (old_chain >= kFar) { ++stats_.pool_missing_chain; continue; }
        const int radius = old_pickup - margin;
        int considered = 0;
        queue.assign(1, from); seen[from] = ++generation; distances[from] = 0;
        for (size_t pos = 0; pos < queue.size() && pos < node_limit && considered < local_limit; ++pos) {
            if ((pos & 63) == 0) check_deadline(deadline_, "pool_exchange_search");
            const int cell = queue[pos], distance = distances[cell];
            ++stats_.pool_nodes;
            for (int k = head[cell]; k >= 0 && considered < local_limit; k = link[k]) {
                ++considered; ++stats_.pool_pairs;
                const int id = available[k], new_chain = chain(id);
                if (new_chain >= kFar) { ++stats_.pool_missing_chain; continue; }
                const long long new_total = static_cast<long long>(distance) + new_chain;
                if (new_total > static_cast<long long>(old_pickup) + old_chain) continue;
                pairs.push_back({r, k, old_pickup - distance, old_chain, new_chain, new_total});
            }
            if (distance == radius) continue;
            for (int d = 0; d < 4; ++d) {
                const int next = neighbor(cell, d);
                // Exactly mirror DistanceOracle's domain for a CORE goal,
                // including unclassified floor (pocket == -1). FIFO discovery
                // gives exact distances to reached candidates despite the cap.
                if (next < 0 || !cert_.free[next] || seen[next] == generation ||
                    (!cert_.core[next] && cert_.pocket[next] != -1)) continue;
                seen[next] = generation; distances[next] = distance + 1; queue.push_back(next);
            }
        }
    }
    std::sort(pairs.begin(), pairs.end(), [](const Pair& a, const Pair& b) {
        if (a.pickup_saving != b.pickup_saving) return a.pickup_saving > b.pickup_saving;
        if (a.new_total != b.new_total) return a.new_total < b.new_total;
        if (a.task != b.task) return a.task < b.task;
        return a.robot < b.robot;
    });
    std::vector<char> used_robots(n_, false), used_tasks(available.size(), false);
    int accepted = 0;
    for (const Pair& pair : pairs) {
        check_deadline(deadline_, "pool_exchange_commit");
        if (accepted == exchange_limit) break;
        if (used_robots[pair.robot] || used_tasks[pair.task]) continue;
        const int old_id = proposed[pair.robot], new_id = available[pair.task];
        // Both sides have a finite retarget limit. The released task is not
        // inserted into this call's candidate snapshot; it stays in task_pool.
        reassigned_tasks_.insert(old_id); reassigned_tasks_.insert(new_id);
        proposed[pair.robot] = new_id;
        last_reassignment_[pair.robot] = now;
        agents_[pair.robot].committed = -1; agents_[pair.robot].commit_age = 0;
        used_robots[pair.robot] = used_tasks[pair.task] = true;
        ++accepted; ++stats_.pool_exchanges;
        stats_.pool_pickup_saving += pair.pickup_saving;
        stats_.pool_chain_delta += pair.new_chain - pair.old_chain;
        stats_.pool_total_saving += pair.pickup_saving + pair.old_chain - pair.new_chain;
    }
    check_deadline(deadline_, "pool_exchange_complete");
}

// A fixed-work, resident-only permutation pass over unopened assignments. It
// changes only the current proposal: task metadata and the free pool are never
// touched. Every group is analysed completely before any accepted cycle is
// committed, so a deadline can only fail the whole entry.
void Cgar::match_unopened(std::vector<int>& proposed) {
    match_unopened_impl(proposed, false, stats_);
    const int now = env_->curr_timestep;
    if (!match_budget_audit_stride_ || now % match_budget_audit_stride_) return;
    // The real pass always runs first. The shadow owns its cursor, counters and
    // witness ledger, and uses only const table peeks and a copied proposal.
    auto unchanged = proposed;
    match_unopened_impl(unchanged, true, match_budget_shadow_.work);
    if (unchanged != proposed) throw std::logic_error("matching budget shadow changed a proposal");
    const auto& a = match_budget_shadow_; const auto& w = a.work;
    std::printf("[cgar-match-budget-shadow] t=%d passes=%lld eligible=%lld resident=%lld missing=%lld groups=%lld selected=%lld nodes=%lld matrix_entries=%lld accepted_cycles=%lld budget_cycles=%lld unprotected_cycles=%lld duplicate_cycles=%lld witness_cycles=%lld witness_rows=%lld witness_budget_rows=%lld witness_saving=%lld fully_protected_cycles=%lld fully_protected_rows=%lld fully_protected_saving=%lld unit=%d unique_tasks=%zu primary_protected=%lld recovery_protected=%lld fair_protected=%lld budget_protected=%lld real_moved=%lld real_saving=%lld assignments=%lld read_only=1\n",
        now, w.match_passes, w.match_eligible, w.match_resident, w.match_missing,
        w.match_groups, w.match_selected, w.match_nodes, w.match_matrix_entries, w.match_accepted_cycles,
        a.budget_cycles, a.unprotected_cycles, a.duplicate_cycles, a.witness_cycles, a.witness_rows,
        a.witness_budget_rows, a.witness_saving, a.fully_protected_cycles, a.fully_protected_rows,
        a.fully_protected_saving, flow_cost_scale_, match_budget_audit_seen_tasks_.size(),
        w.match_primary_protected, w.match_recovery_protected, w.match_fair_protected,
        w.match_budget_protected, stats_.match_moved, stats_.match_saving, stats_.assignments);
    check_deadline(deadline_, "unopened_match_shadow_report_complete");
}

void Cgar::match_unopened_impl(std::vector<int>& proposed, bool shadow, Stats& observed) {
    constexpr int interval = 10, anchor_limit = 128;
    constexpr int group_size = 32, node_limit = 2048;
    const int now = env_->curr_timestep;
    if (!reassign_match_ || now % interval != 0) return;
    ++observed.match_passes;
    if (!shadow) prune_reassignment_records();
    // Match the scheduler's metric lifecycle: static trick quotes are ready
    // after tick0 even though learned-flow publication is disabled for them.
    if (!((static_trick_metric_ && now > 0) || flow_guidance_.publications() > 0)) {
        check_deadline(deadline_, "unopened_match_waiting_for_publication");
        return;
    }

    const auto candidates = unopened_candidates(proposed, false, shadow);
    observed.match_primary_protected += candidates.primary;
    observed.match_recovery_protected += candidates.recovery;
    observed.match_fair_protected += candidates.fair;
    observed.match_budget_protected += candidates.budget;
    observed.match_eligible += candidates.robots.size();
    if (candidates.robots.size() < 2) {
        check_deadline(deadline_, "unopened_match_empty");
        return;
    }

    // Read only complete resident tables. The diagonal check excludes a holder
    // whose own current pickup is unreachable under this immutable metric.
    std::vector<int> resident;
    resident.reserve(candidates.robots.size());
    std::vector<const TurnTable*> tables(n_, nullptr);
    std::unordered_set<int> task_ids;
    for (int robot : candidates.robots) {
        check_deadline(deadline_, "unopened_match_resident_index");
        const auto found = env_->task_pool.find(proposed[robot]);
        if (found == env_->task_pool.end() || found->second.locations.empty()) continue;
        if (!task_ids.insert(found->first).second) continue;
        const int goal = found->second.locations.front();
        const auto* table = turn_oracle_.peek(goal);
        if (!table) {
            ++observed.match_missing;
            continue;
        }
        const int own = turn_oracle_.value(*table, env_->curr_states[robot].location,
                                           env_->curr_states[robot].orientation);
        if (own >= kInf) {
            ++observed.match_unreachable;
            continue;
        }
        tables[robot] = table;
        resident.push_back(robot);
        ++observed.match_resident;
    }
    if (resident.size() < 2) {
        check_deadline(deadline_, "unopened_match_no_resident_group");
        return;
    }

    // Rotate a bounded list of anchors, then collect nearby holders from ALL
    // resident candidates. Prefiltering that spatial pool to the anchor quota
    // scatters it across a large map and leaves local groups mostly empty.
    // The configured group quota and32-holder cap bound matrix participants.
    // Rebuild the index from current states, without touching planner occupancy.
    auto& cursor = shadow ? match_budget_audit_cursor_ : match_cursor_;
    const size_t start = cursor % resident.size();
    const size_t anchor_count = std::min<size_t>(anchor_limit, resident.size());
    cursor += anchor_count;
    std::vector<int> anchors;
    anchors.reserve(anchor_count);
    for (size_t k = 0; k < anchor_count; ++k)
        anchors.push_back(resident[(start + k) % resident.size()]);

    std::vector<int> head(cert_.free.size(), -1), link(n_, -1);
    for (auto it = resident.rbegin(); it != resident.rend(); ++it) {
        const int robot = *it;
        const int cell = env_->curr_states[robot].location;
        if (cell >= 0 && cell < static_cast<int>(head.size())) {
            link[robot] = head[cell];
            head[cell] = robot;
        }
    }

    // The optional second index contains the same eligible resident holders,
    // keyed by their pickup location instead of their current robot location.
    // This can admit a distant holder whose pickup is near the current anchor.
    std::vector<int> pickup_head, pickup_link;
    if (match_pickup_groups_) {
        pickup_head.assign(cert_.free.size(), -1);
        pickup_link.assign(n_, -1);
        for (auto it = resident.rbegin(); it != resident.rend(); ++it) {
            const int robot = *it;
            const int cell = env_->task_pool.at(proposed[robot]).locations.front();
            if (cell >= 0 && cell < static_cast<int>(pickup_head.size())) {
                pickup_link[robot] = pickup_head[cell];
                pickup_head[cell] = robot;
            }
        }
    }

    std::vector<char> used(n_, 0), anchor_seen(n_, 0);
    std::vector<int> seen(cert_.free.size(), 0), queue;
    int generation = 0;
    struct Planned {
        std::vector<int> robots;
        AssignmentPermutation permutation;
        std::vector<PickupPermutationCycle> cycles;
    };
    std::vector<Planned> planned;

    for (int group_number = 0; group_number < match_group_limit_; ++group_number) {
        check_deadline(deadline_, "unopened_match_group_start");
        int anchor = -1;
        for (int robot : anchors) {
            if (!anchor_seen[robot] && !used[robot]) {
                anchor = robot;
                anchor_seen[robot] = 1;
                break;
            }
        }
        if (anchor < 0) break;
        ++observed.match_anchors;

        queue.clear();
        const int start_cell = env_->curr_states[anchor].location;
        if (start_cell < 0 || start_cell >= static_cast<int>(cert_.free.size()) ||
            !cert_.free[start_cell]) continue;
        queue.push_back(start_cell);
        seen[start_cell] = ++generation;
        std::vector<int> group;
        const int location_limit = match_pickup_groups_ ? group_size / 2 : group_size;
        int location_count = 0, pickup_count = 0;
        for (size_t pos = 0; pos < queue.size() && pos < node_limit && group.size() < group_size; ++pos) {
            if ((pos & 63) == 0) check_deadline(deadline_, "unopened_match_group_search");
            const int cell = queue[pos];
            ++observed.match_nodes;
            for (int robot = head[cell]; robot >= 0 && location_count < location_limit; robot = link[robot]) {
                if (!used[robot]) {
                    used[robot] = 1;
                    group.push_back(robot);
                    ++location_count;
                }
            }
            if (match_pickup_groups_) {
                for (int robot = pickup_head[cell]; robot >= 0 && pickup_count < group_size / 2; robot = pickup_link[robot]) {
                    if (!used[robot]) {
                        used[robot] = 1;
                        group.push_back(robot);
                        ++pickup_count;
                    }
                }
            }
            for (int direction = 0; direction < 4; ++direction) {
                const int next = neighbor(cell, direction);
                if (next < 0 || !cert_.core[next] || seen[next] == generation) continue;
                seen[next] = generation;
                queue.push_back(next);
            }
        }
        if (group.size() < 2) {
            for (int robot : group) used[robot] = 0;
            continue;
        }

        ++observed.match_groups;
        observed.match_pickup_selected += pickup_count;
        observed.match_selected += group.size();
        observed.match_full_groups += group.size() == group_size;
        const int n = static_cast<int>(group.size());
        std::vector<int> costs(static_cast<size_t>(n) * n, kInf);
        for (int row = 0; row < n; ++row) {
            const int robot = group[row];
            for (int column = 0; column < n; ++column) {
                check_deadline(deadline_, "unopened_match_matrix");
                const int value = turn_oracle_.value(*tables[group[column]],
                    env_->curr_states[robot].location, env_->curr_states[robot].orientation);
                costs[static_cast<size_t>(row) * n + column] = value;
                ++observed.match_matrix_entries;
                if (value >= kInf) ++observed.match_unreachable;
            }
        }
        const auto permutation = minimum_pickup_permutation(costs, n, kInf,
            [&] { check_deadline(deadline_, "unopened_match_hungarian"); });
        auto cycles = pickup_permutation_cycles(costs, permutation, flow_cost_scale_,
            [&] { check_deadline(deadline_, "unopened_match_cycles"); }, native_trick_metric_ ? 20 : 16);
        observed.match_cycles += cycles.size();
        for (const auto& cycle : cycles) observed.match_accepted_cycles += cycle.accepted;
        planned.push_back({std::move(group), permutation, std::move(cycles)});
    }

    if (shadow) {
        for (const Planned& item : planned) for (const auto& cycle : item.cycles) if (cycle.accepted) {
            check_deadline(deadline_, "unopened_match_shadow_witness");
            int budget_rows = 0; bool repeated = false;
            for (int row : cycle.rows) {
                const int task = proposed[item.robots[row]];
                budget_rows += reassigned_tasks_.count(task) != 0;
                repeated |= match_budget_audit_seen_tasks_.count(task) != 0;
            }
            if (!budget_rows) { ++match_budget_shadow_.unprotected_cycles; continue; }
            ++match_budget_shadow_.budget_cycles;
            // Reject the whole overlapping witness, then mark EVERY participating
            // task (budget-protected or not). No task contributes twice to S2.
            if (repeated) { ++match_budget_shadow_.duplicate_cycles; continue; }
            ++match_budget_shadow_.witness_cycles;
            match_budget_shadow_.witness_rows += cycle.rows.size();
            match_budget_shadow_.witness_budget_rows += budget_rows;
            match_budget_shadow_.witness_saving += cycle.before - cycle.after;
            // A mixed cycle's complete saving is not attributable solely to the
            // protected rows. Report the fully protected subset separately.
            if (budget_rows == int(cycle.rows.size())) {
                ++match_budget_shadow_.fully_protected_cycles;
                match_budget_shadow_.fully_protected_rows += cycle.rows.size();
                match_budget_shadow_.fully_protected_saving += cycle.before - cycle.after;
            }
            for (int row : cycle.rows) match_budget_audit_seen_tasks_.insert(proposed[item.robots[row]]);
        }
        check_deadline(deadline_, "unopened_match_shadow_complete");
        return;  // Never reach assignment, cooldown, commitment or real-budget writes.
    }

    // Commit only after every selected group has a complete finite solution.
    check_deadline(deadline_, "unopened_match_commit_start");
    for (const Planned& item : planned) {
        std::vector<int> replacement(item.robots.size());
        for (size_t i = 0; i < item.robots.size(); ++i)
            replacement[i] = proposed[item.robots[i]];
        for (const auto& cycle : item.cycles) if (cycle.accepted) {
            const long long saving = cycle.before - cycle.after;
            observed.match_moved += cycle.rows.size();
            observed.match_saving += saving;
            for (int row : cycle.rows) {
                const int robot = item.robots[row];
                reassigned_tasks_.insert(proposed[robot]);
                replacement[row] = proposed[item.robots[item.permutation.column[row]]];
                last_reassignment_[robot] = now;
                agents_[robot].committed = -1;
                agents_[robot].commit_age = 0;
            }
        }
        for (size_t i = 0; i < item.robots.size(); ++i)
            proposed[item.robots[i]] = replacement[i];
    }
    check_deadline(deadline_, "unopened_match_complete");
}

void Cgar::record_horizon_proposal(const std::vector<int>& proposed) {
    if (!horizon_margin_) return;
    horizon_margins_.proposed(*env_, proposed, [&](int robot, int id) -> long long {
        const auto chain = chain_cost_.find(id);
        if (chain == chain_cost_.end()) return -1;  // unknown first admission cannot train
        const auto& task = env_->task_pool.at(id);
        const int from = env_->curr_states[robot].location, goal = task.locations.front();
        const auto* table = oracle_.peek(goal);
        int pickup = table ? oracle_.value(*table, from) : oracle_.manhattan(from, goal);
        if (pickup >= kInf) pickup = oracle_.manhattan(from, goal);
        long long bound = std::max(1, pickup) + static_cast<long long>(chain->second);
        for (size_t k = 1; k < task.locations.size(); ++k) bound += task.locations[k] == task.locations[k - 1];
        return bound;
    });
    check_deadline(deadline_, "horizon_margin_proposal");
}

// Sparse whole-chain HRRN: nearby task candidates for every idle robot, plus an
// unpruned oldest-task admission. A bounded pair store never truncates the task
// pool by ID; unmatched robots get a fresh search over the remaining tasks.
void Cgar::schedule(SharedEnvironment* env, int time_limit_ms, std::vector<int>& proposed) {
    schedule(env, Clock::now() + std::chrono::milliseconds(time_limit_ms), proposed);
}

void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vector<int>& proposed) {
    check_deadline(deadline, "scheduling_start");
    ++stats_.schedule_calls;
    if (known_horizon_ && env->curr_timestep >= known_horizon_ && !known_horizon_passed_) {
        known_horizon_passed_ = true;
        std::printf("[cgar-horizon-passed] t=%d known_horizon=%d policy=ordinary assumption=configured\n", env->curr_timestep, known_horizon_);
    }
    env_ = env;
    deadline_ = deadline;
    distance_deadline_ = deadline_;
    if (horizon_margin_) { horizon_margins_.observe(*env); check_deadline(deadline_, "horizon_margin_observe"); }
    proposed = env->curr_task_schedule;
    proposed.resize(n_, -1);
    prepare_capacity_mode();
    if (capacity_mode_ && !parking_ready_) { check_deadline(deadline_, "capacity_schedule"); return; }
    table_budget_ = sched_tables_;
    free_tasks_.clear();
    for (const auto& entry : env->task_pool)
        if (entry.second.agent_assigned == -1 && eligible_task(entry.second)) free_tasks_.insert(entry.first);
    for (auto it = chain_cost_.begin(); it != chain_cost_.end();)
        it = free_tasks_.count(it->first) ? std::next(it) : chain_cost_.erase(it);
    if (refine_chain_costs_) refined_chain_cost_.retain(free_tasks_);
    if (chain_flow_pricing_) for (auto it = chain_table_basis_.begin(); it != chain_table_basis_.end();)
        it = free_tasks_.count(it->first) ? std::next(it) : chain_table_basis_.erase(it);
    std::vector<int> robots;
    for (int i = 0; i < n_; ++i) if (proposed[i] == -1 && !parked_[i]) robots.push_back(i);
    if (robots.empty() || free_tasks_.empty()) {
        reassign_unopened(proposed);
        exchange_unopened_with_pool(proposed);
        match_unopened(proposed);
        record_horizon_proposal(proposed);
        check_deadline(deadline_, "empty_schedule"); return;
    }
    std::rotate(robots.begin(), robots.begin() + scheduler_cursor_ % robots.size(), robots.end());
    ++scheduler_cursor_;

    // Scheduling precedes the current plan's observation/publication. Use the
    // last complete published metric without changing that lifecycle. Before
    // its first publication the original scheduler is preserved exactly.
    // An explicitly requested static trick is complete at initialization.
    // Keep the generic mass dispatch at tick0: weighted searches for all
    // initially free robots exceed the entry budget. Later pickup work uses
    // the static field under the same fixed quotas. This is a fixed startup
    // rule, never an elapsed-time fallback. No environment variable activates it.
    const bool pickup_metric = pickup_flow_ &&
        ((static_trick_metric_ && env->curr_timestep > 0) || flow_guidance_.publications() > 0);
    const int pickup_scale = pickup_metric ? flow_cost_scale_ : 1;
    if (pickup_flow_) {
        stats_.pickup_flow_snapshot_publication = flow_guidance_.publications();
        stats_.pickup_flow_warmup_calls += !pickup_metric;
    }
    const bool chain_metric = chain_flow_pricing_ && pickup_metric;
    struct TaskCost { int id, first, chain, revealed; int native = 0, price = 0; ResidentChainPrice resident; bool all_table = false; int repeated_stops = 0; };
    long long ratio_numerator = 0, ratio_denominator = 0;
    struct Pair { double score; int cost, task, robot, pickup; bool global = false; bool horizon_impossible = false; int horizon_tier = 0; };
    std::vector<int> ids(free_tasks_.begin(), free_tasks_.end());
    std::sort(ids.begin(), ids.end());
    std::vector<TaskCost> tasks;
    std::vector<std::vector<int>> at_cell(cert_.free.size());
    for (int id : ids) {
        const Task& task = env->task_pool.at(id);
        // The table-count limit is fixed work policy; elapsed time never changes
        // which estimates are computed. A missed deadline raises Timeout.
        check_deadline(deadline_, "task_metadata");
        const int first = task.locations.at(task.idx_next_loc);
        at_cell[first].push_back(static_cast<int>(tasks.size()));
        const int chain = task_chain_cost(id);  // Preserve spatial admission/recency work in every mode.
        TaskCost item{id, first, chain, task.t_revealed};
        if (known_horizon_ && env->curr_timestep < known_horizon_)
            for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k)
                item.repeated_stops += task.locations[k] == task.locations[k - 1];
        if (chain_metric) {
            item.native = static_cast<int>(std::min<long long>(kInf - 1, static_cast<long long>(pickup_scale) * chain));
            item.resident = resident_chain_price(task, turn_oracle_, cert_);
            item.all_table = refine_chain_costs_ ? refined_chain_cost_.all_table_derived(id) : chain_table_basis_.at(id);
            if (item.resident.status == ChainPriceStatus::Covered && item.native > 0 && item.chain < kFar) {
                ratio_numerator += item.resident.cost;
                ratio_denominator += item.native;
            }
        }
        tasks.push_back(item);
    }
    if (chain_metric) {
        ++stats_.chain_price_calls;
        stats_.chain_price_publication = flow_guidance_.publications();
        stats_.chain_price_numerator = ratio_numerator; stats_.chain_price_denominator = ratio_denominator;
        std::array<long long, 5> outcomes{};
        std::array<long long, 4> basis{};  // covered table/approx, uncovered table/approx
        long long covered_nonzero = 0, nonzero = 0, clipped_native = 0, covered_clipped = 0, ratio_eligible = 0;
        for (auto& task : tasks) {
            check_deadline(deadline_, "chain_price_snapshot");
            const bool covered = task.resident.status == ChainPriceStatus::Covered;
            ++outcomes[static_cast<size_t>(task.resident.status)];
            ++basis[(covered ? 0 : 2) + !task.all_table];
            nonzero += task.native > 0; covered_nonzero += covered && task.native > 0;
            clipped_native += task.chain >= kFar;
            covered_clipped += covered && task.chain >= kFar;
            ratio_eligible += covered && task.native > 0 && task.chain < kFar;
            task.price = selected_chain_price(chain_flow_pricing_, task.native, task.resident, ratio_numerator, ratio_denominator);
            stats_.chain_price_changed += task.price != task.native;
        }
        stats_.chain_price_observations += tasks.size();
        stats_.chain_price_ratio_calls += ratio_denominator > 0;
        for (size_t k = 0; k < basis.size(); ++k) stats_.chain_price_basis[k] += basis[k];
        for (size_t k = 0; k < outcomes.size(); ++k) stats_.chain_price_outcomes[k] += outcomes[k];
        if (diagnostics_)
            std::printf("[cgar-chain-snapshot] t=%d mode=%d publication=%lld tasks=%zu nonzero=%lld covered_nonzero=%lld covered=%lld missing=%lld outside=%lld unreachable=%lld invalid=%lld covered_table=%lld covered_approx=%lld uncovered_table=%lld uncovered_approx=%lld clipped_native=%lld covered_clipped=%lld ratio_eligible=%lld ratio_numerator=%lld ratio_denominator=%lld\n",
                env_->curr_timestep, chain_flow_pricing_, stats_.chain_price_publication, tasks.size(), nonzero, covered_nonzero,
                outcomes[0], outcomes[1], outcomes[2], outcomes[3], outcomes[4], basis[0], basis[1], basis[2], basis[3],
                clipped_native, covered_clipped, ratio_eligible, ratio_numerator, ratio_denominator);
    }
    // Fixed robot quota in the existing rotating order. All selected fields
    // finish against this immutable published metric before any is consulted.
    std::vector<int> full_pickup_slot;
    if (pickup_metric && pickup_full_robots_) {
        full_pickup_slot.assign(n_, -1);
        std::vector<int> selected;
        for (int r : robots) {
            const int from = env->curr_states[r].location;
            if (capacity_mode_ && !cert_.core[from]) continue;
            selected.push_back(r);
            if (static_cast<int>(selected.size()) == pickup_full_robots_) break;
        }
        if (!selected.empty()) {
            const int threads = std::min<int>(pickup_full_threads_, selected.size());
            run_temporal_preparation(threads, [&](int worker) {
                for (int k = worker; k < static_cast<int>(selected.size()); k += threads) {
                    const auto& state = env->curr_states[selected[k]];
                    pickup_full_workers_[worker].run(static_cast<int>(cert_.free.size()),
                        state.location, state.orientation, guidance_turn_cost_, kInf,
                        [&](int cell, int dir) { return neighbor(cell, dir); },
                        [&](int cell) { return cert_.free[cell] && (!capacity_mode_ || cert_.core[cell]); },
                        [&](int cell, int dir) { return turn_oracle_.forward_cost(cell, dir); },
                        [&] { check_deadline(deadline_, "pickup_full_distance"); }, pickup_full_fields_[k], guidance_cost_limit_);
                }
            });
            check_deadline(deadline_, "pickup_full_fields_complete");
            for (size_t k = 0; k < selected.size(); ++k) {
                full_pickup_slot[selected[k]] = static_cast<int>(k);
                ++stats_.pickup_full_fields;
                stats_.pickup_full_pops += pickup_full_fields_[k].pops;
                stats_.pickup_full_states += pickup_full_fields_[k].states;
            }
        }
    }
    auto full_field = [&](int r) -> const FullPickupField* {
        if (full_pickup_slot.empty() || full_pickup_slot[r] < 0) return nullptr;
        return &pickup_full_fields_[full_pickup_slot[r]];
    };
    const int now = env->curr_timestep;
    const bool horizon_active = known_horizon_ && now < known_horizon_;
    const auto horizon_model = horizon_margins_.snapshot();  // immutable for this whole scheduling entry
    auto pair_for = [&](int r, int t, int d, bool assess_horizon = true) {
        const auto& task = tasks[t];
        const int cost = static_cast<int>(std::max<long long>(pickup_scale, std::min<long long>(kInf - 1,
            static_cast<long long>(pickup_weight_) * d + (chain_metric && chain_flow_pricing_ != 4 ?
                task.price : static_cast<long long>(pickup_scale) * task.chain))));
        Pair result{hrrn_ ? 1.0 + (static_cast<double>(std::max(0, now - task.revealed)) * pickup_scale) / cost : 1.0,
                    cost, t, r, d};
        if (horizon_active && assess_horizon) {
            const int from = env->curr_states[r].location;
            const auto* table = oracle_.peek(task.first);  // no builds or LRU mutation
            int spatial = table ? oracle_.value(*table, from) : oracle_.manhattan(from, task.first);
            if (spatial >= kInf) spatial = oracle_.manhattan(from, task.first);
            const long long bound = std::max(1, spatial) + static_cast<long long>(task.chain) + task.repeated_stops;
            const long long remaining = static_cast<long long>(known_horizon_) - now;
            result.horizon_impossible = bound > remaining;
            result.horizon_tier = horizon_margin_ ? horizon_model.tier(bound, remaining) : (result.horizon_impossible ? 2 : 0);
            stats_.horizon_margin_pairs += result.horizon_tier == 1;
            ++stats_.horizon_pairs; stats_.horizon_impossible_pairs += result.horizon_impossible;
        }
        return result;
    };
    auto ordinary_better = [&](const Pair& a, const Pair& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.cost != b.cost) return a.cost < b.cost;
        if (tasks[a.task].id != tasks[b.task].id) return tasks[a.task].id < tasks[b.task].id;
        return a.robot < b.robot;
    };
    auto better = [&](const Pair& a, const Pair& b) {
        const bool ordinary = ordinary_better(a, b);
        const bool minimal = a.horizon_impossible != b.horizon_impossible ? !a.horizon_impossible : ordinary;
        const bool selected = a.horizon_tier != b.horizon_tier ? a.horizon_tier < b.horizon_tier : ordinary;
        if (selected != ordinary) {
            ++stats_.horizon_rank_changes;
            if (stats_.horizon_first_rank_change < 0) {
                stats_.horizon_first_rank_change = now;
                std::printf("[cgar-horizon-first-rank-change] t=%d known_horizon=%d assumption=configured\n", now, known_horizon_);
            }
        }
        if (selected != minimal) {
            ++stats_.horizon_margin_rank_changes;
            if (stats_.horizon_margin_first_rank_change < 0) {
                stats_.horizon_margin_first_rank_change = now;
                std::printf("[cgar-horizon-margin-first-rank-change] t=%d known_horizon=%d assumption=configured\n", now, known_horizon_);
            }
        }
        return selected;
    };
    // Optional cost-only discovery retains a broader low-cost shortlist while
    // leaving age-weighted assignment ranking and fair admission unchanged.
    auto retain_better = [&](const Pair& a, const Pair& b) {
        if (!pickup_full_cost_key_ || a.horizon_impossible != b.horizon_impossible) return better(a, b);
        if (a.cost != b.cost) return a.cost < b.cost;
        if (tasks[a.task].id != tasks[b.task].id) return tasks[a.task].id < tasks[b.task].id;
        return a.robot < b.robot;
    };
    for (auto& bucket : at_cell) std::sort(bucket.begin(), bucket.end(), [&](int a, int b) {
        // Endpoint bucket discovery is independent of a particular robot's
        // remaining travel. Preserve its original order in both experiment arms.
        return ordinary_better(pair_for(0, a, 0, false), pair_for(0, b, 0, false));
    });
    std::vector<int> by_age(tasks.size());
    for (size_t t = 0; t < tasks.size(); ++t) by_age[t] = static_cast<int>(t);
    std::sort(by_age.begin(), by_age.end(), [&](int a, int b) {
        if (tasks[a].revealed != tasks[b].revealed) return tasks[a].revealed < tasks[b].revealed;
        return tasks[a].id < tasks[b].id;
    });
    std::vector<char> robot_used(n_, 0), task_used(tasks.size(), 0);
    // Keep unused task indices compact so global samples never scan assigned tasks.
    // Maintain this in both modes for matched ablations with the same bookkeeping.
    std::vector<int> available(tasks.size()), available_position(tasks.size());
    for (size_t t = 0; t < tasks.size(); ++t)
        available[t] = available_position[t] = static_cast<int>(t);
    size_t oldest = 0;
    auto oldest_task = [&]() {
        while (oldest < by_age.size() && task_used[by_age[oldest]]) ++oldest;
        return oldest < by_age.size() ? by_age[oldest] : -1;
    };
    auto assign = [&](const Pair& p) {
        robot_used[p.robot] = 1;
        task_used[p.task] = 1;
        const int slot = available_position[p.task], last = available.back();
        available[slot] = last;
        available_position[last] = slot;
        available.pop_back();
        available_position[p.task] = -1;
        proposed[p.robot] = tasks[p.task].id;
        stats_.estimated_pickup_cost += (static_cast<long long>(p.pickup) + pickup_scale - 1) / pickup_scale;
        stats_.estimated_chain_cost += tasks[p.task].chain;
        if (chain_metric) {
            const bool covered = tasks[p.task].resident.status == ChainPriceStatus::Covered;
            ++stats_.chain_price_assignments;
            stats_.chain_price_assigned_covered += covered;
            stats_.chain_price_assigned_imputed += (chain_flow_pricing_ == 3 || (chain_flow_pricing_ == 2 && !covered)) &&
                tasks[p.task].native > 0 && ratio_denominator > 0;
        }
        ++stats_.assignments;
        if (horizon_active) {
            ++stats_.horizon_assignments;
            stats_.horizon_impossible_assignments += p.horizon_impossible;
            stats_.horizon_margin_assignments += p.horizon_tier == 1;
        }
        stats_.global_assignments += p.global;
    };
    auto estimate = [&](int r, int t) {
        const int from = env->curr_states[r].location, goal = tasks[t].first;
        const auto* table = scheduler_cache_peek_ ? oracle_.peek(goal) : oracle_.find(goal);
        int d = table ? oracle_.value(*table, from) : oracle_.manhattan(from, goal);
        if (d >= kInf) {
            const int exit = cert_.exit_cell[from];
            d = exit >= 0 && table && oracle_.value(*table, exit) < kInf ? cert_.exit_dist[from] + oracle_.value(*table, exit) : kFar;
        }
        if (pickup_metric) {
            if (const auto* field = full_field(r)) {
                const int value = field->distance[goal];
                if (value < kInf) { ++stats_.pickup_full_estimates; return pair_for(r, t, value); }
            }
            // Complete cached metrics improve fallback estimates without new
            // table construction or LRU mutation. Missing/unreachable entries
            // retain the previous approximate estimate in the same base units.
            const auto* oriented = turn_oracle_.peek(goal);
            const int value = oriented ? turn_oracle_.value(*oriented, from, env->curr_states[r].orientation) : kInf;
            if (value < kInf) {
                ++stats_.pickup_flow_cached_estimates;
                return pair_for(r, t, value);
            }
            ++stats_.pickup_flow_approximate_estimates;
        }
        return pair_for(r, t, static_cast<int>(std::min<long long>(kInf - 1, static_cast<long long>(d) * pickup_scale)));
    };
    auto fair_admission = [&]() {
        // Explicit competition-objective ablation: long unpicked tasks may wait
        // indefinitely. Started assignments and motion protection are unchanged.
        if (short_task_trick_ || regular_admissions_ < n_) return;
        const int t = oldest_task();
        if (t < 0) return;
        Pair best{};
        bool found = false;
        for (int r : robots) if (!robot_used[r]) {
            const Pair p = estimate(r, t);
            if (!found || ordinary_better(p, best)) { best = p; found = true; }
        }
        if (found) {
            assign(best);
            if (reassign_ || reassign_pool_ || reassign_match_) fair_tasks_.insert(tasks[best.task].id);
            ++stats_.fair_assignments;
            regular_admissions_ = 0;
        }
    };

    std::vector<int> seen(cert_.free.size(), 0), distance(cert_.free.size(), 0), queue;
    int generation = 0;
    const auto candidate_deadline = deadline_;
    std::vector<char> first_search_empty(n_, 0);
    auto candidates = [&](int r, int limit) {
        ++stats_.candidate_searches;
        std::vector<Pair> result;
        const int from = env->curr_states[r].location;
        if (const auto* field = full_field(r)) {
            ++stats_.pickup_full_searches;
            for (int t = 0; t < static_cast<int>(tasks.size()); ++t) {
                if ((t & 127) == 0) check_deadline(candidate_deadline, "pickup_full_candidates");
                ++stats_.pickup_full_scans;
                if (task_used[t] || field->distance[tasks[t].first] >= kInf) continue;
                Pair candidate = pair_for(r, t, field->distance[tasks[t].first]);
                // The heap root is the worst retained discovery candidate.
                if (static_cast<int>(result.size()) < limit) {
                    result.push_back(candidate); std::push_heap(result.begin(), result.end(), retain_better);
                } else if (retain_better(candidate, result.front())) {
                    std::pop_heap(result.begin(), result.end(), retain_better);
                    result.back() = candidate; std::push_heap(result.begin(), result.end(), retain_better);
                }
            }
            std::sort(result.begin(), result.end(), better);
            stats_.pickup_full_candidates += result.size();
            if (result.empty()) ++stats_.empty_searches;
            return result;
        }
        if (pickup_metric && (!capacity_mode_ || cert_.core[from])) {
            const auto work = pickup_search_.run(static_cast<int>(cert_.free.size()), from,
                env->curr_states[r].orientation, pickup_flow_nodes_, guidance_turn_cost_,
                [&](int cell, int dir) { return neighbor(cell, dir); },
                [&](int cell) { return cert_.free[cell] && (!capacity_mode_ || cert_.core[cell]); },
                [&](int cell, int dir) { return turn_oracle_.forward_cost(cell, dir); },
                [&](int cell, int distance) {
                    for (int t : at_cell[cell]) if (!task_used[t]) {
                        result.push_back(pair_for(r, t, distance));
                        if (static_cast<int>(result.size()) >= limit) return true;
                    }
                    return false;
                },
                [&] {
                    if (Clock::now() >= candidate_deadline) {
                        ++stats_.candidate_deadlines; throw Timeout("pickup_flow_candidates");
                    }
                }, guidance_cost_limit_);
            ++stats_.pickup_flow_searches; stats_.pickup_flow_pops += work.pops;
            stats_.pickup_flow_states += work.states; stats_.pickup_flow_cells += work.cells;
            stats_.pickup_flow_candidates += result.size(); stats_.pickup_flow_limits += work.limited;
            stats_.candidate_nodes += work.pops; stats_.candidate_task_limits += work.stopped;
            stats_.candidate_node_limits += work.limited;
            if (result.empty()) ++stats_.empty_searches;
            return result;
        }
        queue.assign(1, from);
        seen[from] = ++generation;
        distance[from] = 0;
        // The search is local and bounded; the fallback below still covers every
        // remaining robot when sparse endpoints exhaust its fixed work limit.
        size_t head = 0;
        for (; head < queue.size() && head < 2048; ++head) {
            if ((head & 63) == 0 && Clock::now() >= candidate_deadline) {
                ++stats_.candidate_deadlines;
                throw Timeout("candidate_search");
            }
            ++stats_.candidate_nodes;
            const int u = queue[head];
            for (int t : at_cell[u]) if (!task_used[t]) {
                result.push_back(pair_for(r, t, distance[u] * pickup_scale));
                if (static_cast<int>(result.size()) >= limit) {
                    ++stats_.candidate_task_limits;
                    return result;
                }
            }
            for (int d = 0; d < 4; ++d) {
                const int v = neighbor(u, d);
                if (v < 0 || !cert_.free[v] || seen[v] == generation || (capacity_mode_ && !cert_.core[v])) continue;
                seen[v] = generation;
                distance[v] = distance[u] + 1;
                queue.push_back(v);
            }
        }
        if (head >= 2048 && head < queue.size()) ++stats_.candidate_node_limits;
        if (result.empty()) ++stats_.empty_searches;
        return result;
    };
    const int per_robot = static_cast<int>(std::max<long long>(1, std::min<long long>(16, max_pairs_ / robots.size())));
    const int global_per_robot = std::min(global_samples_, static_cast<int>(
        std::max<long long>(0, max_pairs_ / robots.size() - per_robot)));
    std::vector<int> candidate_seen(global_per_robot ? tasks.size() : 0, -1);
    // A coprime stride visits distinct task indices without favoring low IDs.
    // This stream is independent of the planner RNG and does not consult time.
    size_t global_stride = 3266489917ULL % tasks.size();
    if (global_per_robot) while (std::gcd(global_stride, tasks.size()) != 1) ++global_stride;
    std::vector<Pair> pairs;
    pairs.reserve(static_cast<size_t>(std::min<long long>(max_pairs_, robots.size() * (per_robot + global_per_robot))));
    std::array<long long, 5> shadow_choices{};
    long long shadow_small = 0;
    for (int r : robots) {
        check_deadline(candidate_deadline, "candidate_generation");
        if (static_cast<long long>(pairs.size()) >= max_pairs_) break;
        auto local = candidates(r, std::min<int>(per_robot, static_cast<int>(max_pairs_ - pairs.size())));
        first_search_empty[r] = local.empty();
        if (chain_metric && chain_flow_pricing_ == 4 && local.size() < 2) ++shadow_small;
        if (chain_metric && chain_flow_pricing_ == 4 && local.size() > 1) {
            // Describe a fixed retained shortlist, not candidates the alternative
            // policy might have discovered. No extra lookup or cache mutation.
            const Pair* native_best = &*std::min_element(local.begin(), local.end(), better);
            int alternative[2];
            for (int mode : {2, 3}) {
                Pair best{}; bool found = false;
                for (const auto& p : local) {
                    const auto& task = tasks[p.task];
                    const int chain = selected_chain_price(mode, task.native, task.resident, ratio_numerator, ratio_denominator);
                    const int cost = static_cast<int>(std::max<long long>(pickup_scale, std::min<long long>(kInf - 1,
                        static_cast<long long>(pickup_weight_) * p.pickup + chain)));
                    Pair value{hrrn_ ? 1.0 + static_cast<double>(std::max(0, now - task.revealed)) * pickup_scale / cost : 1.0,
                               cost, p.task, p.robot, p.pickup};
                    if (!found || better(value, best)) { found = true; best = value; }
                }
                alternative[mode - 2] = best.task;
            }
            const bool changed2 = alternative[0] != native_best->task;
            const bool changed3 = alternative[1] != native_best->task;
            const int category = !changed2 && !changed3 ? 0 : changed2 && !changed3 ? 1 :
                !changed2 && changed3 ? 2 : alternative[0] == alternative[1] ? 3 : 4;
            ++shadow_choices[category];
            ++stats_.chain_shadow_queries;
            stats_.chain_shadow_changed2 += changed2;
            stats_.chain_shadow_changed3 += changed3;
            stats_.chain_shadow_specific += alternative[0] != alternative[1];
        }
        pairs.insert(pairs.end(), local.begin(), local.end());
        if (global_per_robot) {
            for (const auto& candidate : local) candidate_seen[candidate.task] = r;
            const uint64_t base = (static_cast<uint64_t>(r) + 1) * 2654435761ULL +
                                  (static_cast<uint64_t>(now) + 1) * 2246822519ULL;
            const int samples = std::min<int>(global_per_robot, static_cast<int>(tasks.size()));
            for (int k = 0; k < samples; ++k) {
                check_deadline(deadline_, "global_task_candidates");
                const int t = (base + static_cast<uint64_t>(k) * global_stride) % tasks.size();
                if (candidate_seen[t] == r) continue;
                candidate_seen[t] = r;
                Pair candidate = estimate(r, t); candidate.global = true;
                pairs.push_back(candidate); ++stats_.global_evaluations;
            }
        }
    }
    if (chain_metric && chain_flow_pricing_ == 4) {
        stats_.chain_shadow_small += shadow_small;
        for (size_t k = 0; k < shadow_choices.size(); ++k) stats_.chain_shadow_choices[k] += shadow_choices[k];
        if (diagnostics_)
            std::printf("[cgar-chain-shadow] t=%d publication=%lld small=%lld same=%lld mode2_only=%lld mode3_only=%lld both_same=%lld both_different=%lld\n",
                env_->curr_timestep, stats_.chain_price_publication, shadow_small,
                shadow_choices[0], shadow_choices[1], shadow_choices[2], shadow_choices[3], shadow_choices[4]);
    }
    std::sort(pairs.begin(), pairs.end(), better);
    for (const Pair& p : pairs) {
        if (robot_used[p.robot] || task_used[p.task]) continue;
        fair_admission();
        if (robot_used[p.robot] || task_used[p.task]) continue;
        assign(p);
        if (!p.global) ++stats_.local_assignments;
        ++regular_admissions_;
    }
    for (int r : robots) {
        if (robot_used[r]) continue;
        fair_admission();
        if (robot_used[r]) continue;
        const int fallback = oldest_task();
        if (fallback < 0) break;
        std::vector<Pair> local;
        if (repair_fallback_ && first_search_empty[r]) {
            // Assignments only remove tasks within this call; an identical search
            // cannot recover from an earlier empty result under the same bounds.
            ++stats_.skipped_empty_searches;
        } else {
            // Candidates may have been taken by other robots, so a previously
            // nonempty search still needs replenishment.
            local = candidates(r, 8);
        }
        Pair best = local.empty() ? estimate(r, fallback) : *std::min_element(local.begin(), local.end(), better);
        if (local.empty()) {
            ++stats_.fallback_assignments;
            if (repair_fallback_) {
                // A separate deterministic sequence preserves PIBT's random stream.
                // The oldest task remains a candidate; fair admission is independent.
                const uint64_t base = (static_cast<uint64_t>(r) + 1) * 2654435761ULL +
                    (static_cast<uint64_t>(now) + 1) * 2246822519ULL;
                const int samples = std::min<int>(fallback_samples_, static_cast<int>(available.size()));
                for (int k = 0; k < samples; ++k) {
                    if (Clock::now() >= deadline_) { ++stats_.sample_deadlines; throw Timeout("fallback_sampling"); }
                    const int t = available[(base + static_cast<uint64_t>(k) * 3266489917ULL) % available.size()];
                    const Pair candidate = estimate(r, t);
                    ++stats_.sample_evaluations;
                    if (better(candidate, best)) best = candidate;
                }
                if (best.task != fallback) ++stats_.improved_fallbacks;
            }
        } else {
            ++stats_.local_assignments;
        }
        assign(best);
        ++regular_admissions_;
    }
    reassign_unopened(proposed);
    exchange_unopened_with_pool(proposed);
    match_unopened(proposed);
    record_horizon_proposal(proposed);
    check_deadline(deadline_, "scheduling_complete");
}

}  // namespace cgar

#include "temporal_adapter.hpp"
