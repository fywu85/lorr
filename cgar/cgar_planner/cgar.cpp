#include "cgar.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iterator>
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
    (void)preprocess_ms;
    env_ = env;
    if (initialized_) return;  // the scheduler and the planner both call this
    initialized_ = true;
    n_ = env->num_of_agents;
    stall_limit_ = env_int("CGAR_STALL", 4);
    commit_limit_ = env_int("CGAR_COMMIT_AGE", 3);
    plan_tables_ = env_int("CGAR_PLAN_TABLES", 256);
    sched_tables_ = env_int("CGAR_SCHED_TABLES", 128);
    max_pairs_ = std::max(1, env_int("CGAR_MAX_PAIRS", 2000000));
    enable_txn_ = env_int("CGAR_TXN", 1) != 0;
    hrrn_ = env_int("CGAR_HRRN", 1) != 0;
    pickup_weight_ = std::max(1, std::min(16, env_int("CGAR_PICKUP_WEIGHT", 1)));
    refine_chain_costs_ = env_int("CGAR_REFINE_CHAIN_COSTS", 0) != 0;
    scheduler_cache_peek_ = env_int("CGAR_SCHEDULER_CACHE_PEEK", 0) != 0;
    stable_stall_basis_ = env_int("CGAR_STABLE_STALL_BASIS", 0) != 0;
    repair_fallback_ = env_int("CGAR_FALLBACK_REPAIR", 1) != 0;
    reassign_ = env_int("CGAR_REASSIGN", 0) != 0;
    fallback_samples_ = std::max(0, std::min(4096, env_int("CGAR_FALLBACK_SAMPLES", 64)));
    enable_locks_ = env_int("CGAR_CERT", 1) != 0;
    const size_t table_mb = static_cast<size_t>(env_int("CGAR_TABLE_MB", 2048));
    rng_.seed(static_cast<unsigned>(env_int("CGAR_SEED", 0)));

    const auto t0 = Clock::now();
    cert_ = build_certificate_feasible(env->map, env->rows, env->cols, enable_locks_ ? n_ : 0);
    if (!enable_locks_) {
        // Ablation: route over the whole connected floor with no pockets.
        std::fill(cert_.pocket.begin(), cert_.pocket.end(), -1);
        cert_.pocket_cells.clear();
    }
    const double secs = std::chrono::duration<double>(Clock::now() - t0).count();
    oracle_.init(&cert_, table_mb << 20);

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
    last_reassignment_.assign(n_, -20);

    size_t free_cells = std::count(cert_.free.begin(), cert_.free.end(), 1);
    size_t largest_pocket = 0;
    for (const auto& p : cert_.pocket_cells) largest_pocket = std::max(largest_pocket, p.size());
    std::printf("[cgar] certificate N=%d free=%zu core=%zu promotion_arcs=%zu iterations=%zu valid=%d "
                "capacity=%zu pockets=%zu largest_pocket=%zu secs=%.3f | stall=%d txn=%d cert=%d\n",
                n_, free_cells, cert_.core_size, cert_.promotion_arcs, cert_.iterations, cert_.valid, cert_.capacity,
                cert_.pocket_cells.size(), largest_pocket, secs, stall_limit_, enable_txn_, enable_locks_);
    std::fflush(stdout);
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

void Cgar::compute_order(int primary) {
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
        if (table_budget_ <= 0) { ++stats_.route_manhattan; return observe(oracle_.manhattan(cell, a.goal), ProgressBasis::Manhattan); }
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

bool Cgar::pibt(int i, int parent) {
    const int u = loc_[i];
    check_deadline(deadline_, "pibt");
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
    std::sort(cands, cands + m, [](const Cand& a, const Cand& b) {
        if (a.h != b.h) return a.h < b.h;
        if (a.occupied != b.occupied) return a.occupied < b.occupied;
        if (a.turns != b.turns) return a.turns < b.turns;
        return a.tie < b.tie;
    });
    for (int k = 0; k < m; ++k) {
        const int c = cands[k].cell;
        if (cands[k].h >= kInf) continue;
        const int r = reserved_[c];
        if (r != -1 && r != i) continue;
        if (parent >= 0 && c == loc_[parent]) continue;
        reserve(c, i);
        next_[i] = c;
        if (c == u) return true;
        const int j = occ_now_[c];
        if (j >= 0 && next_[j] == -1 && !pibt(j, i)) continue;
        return true;
    }
    next_[i] = u;
    reserve(u, i);
    return false;
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
    sync_agents();
    advance_txn();
    prepare_capacity_mode();
    update_locks();
    primary_ = select_primary();
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
    for (int i : order_) {
        if (next_[i] != -1) continue;
        check_deadline(deadline_, "action_planning");
        pibt(i, -1);
    }
    for (int i = 0; i < n_; ++i) {
        if (next_[i] == loc_[i]) continue;
        if (agents_[i].committed != next_[i]) agents_[i].commit_age = 0;
        agents_[i].committed = next_[i];
        actions[i] = action_toward(i, next_[i]);
    }
    std::vector<char> checked(n_, 0);
    for (int i = 0; i < n_; ++i) if (!checked[i] && actions[i] == Action::FW) move_check(i, checked, actions);
    make_safe(actions);
    if (env_->curr_timestep % 200 == 0) log_summary();
    check_deadline(deadline_, "planning_complete");
}

void Cgar::log_summary() {
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
                "pickup_estimate=%lld chain_estimate=%lld route_queries=%lld route_manhattan=%lld\n",
                env_->curr_timestep, repair_fallback_, fallback_samples_, pickup_weight_, stats_.schedule_calls,
                stats_.local_assignments, stats_.fallback_assignments, stats_.fair_assignments,
                stats_.candidate_searches, stats_.candidate_nodes, stats_.candidate_task_limits,
                stats_.candidate_node_limits, stats_.candidate_deadlines, stats_.empty_searches,
                stats_.skipped_empty_searches, stats_.sample_evaluations,
                stats_.sample_deadlines, stats_.improved_fallbacks, stats_.estimated_pickup_cost,
                stats_.estimated_chain_cost, stats_.route_queries, stats_.route_manhattan);
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
    for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k) {
        const auto* table = scheduler_cache_peek_ ? oracle_.peek(task.locations[k]) : oracle_.find(task.locations[k]);
        if (!table && table_budget_ > 0) {
            --table_budget_;
            table = oracle_.try_table(task.locations[k], distance_deadline_);
        }
        const int d = table ? oracle_.distance_from(*table, task.locations[k - 1])
                            : oracle_.manhattan(task.locations[k - 1], task.locations[k]);
        total += std::min(d, kFar);
    }
    const int cost = static_cast<int>(std::min<long long>(total, kFar));
    chain_cost_[task_id] = cost;
    return cost;
}

// A fixed-work swap pass over unopened, already assigned tasks. No task is
// dropped, and a task can be retargeted at most once before its first pickup.
void Cgar::reassign_unopened(std::vector<int>& proposed) {
    constexpr int interval = 10, source_limit = 256, local_limit = 16;
    constexpr int node_limit = 2048, global_samples = 16, cooldown = 20;
    const int now = env_->curr_timestep;
    if (!reassign_ || now % interval != 0) return;
    ++stats_.reassign_passes;
    auto prune = [&](std::unordered_set<int>& records) {
        for (auto it = records.begin(); it != records.end();) {
            const auto task = env_->task_pool.find(*it);
            it = task != env_->task_pool.end() && task->second.idx_next_loc == 0
                ? std::next(it) : records.erase(it);
        }
    };
    prune(reassigned_tasks_);
    prune(fair_tasks_);
    // Also preserve the next pending primary if the previous one just finished.
    int oldest = -1;
    for (int i = 0; i < n_; ++i) {
        const Agent& agent = agents_[i];
        const auto task = env_->task_pool.find(proposed[i]);
        if (parked_[i] || task == env_->task_pool.end() || task->second.idx_next_loc != 0 ||
            agent.task != proposed[i] || agent.stop != 0 || agent.ticket == kIdleTicket ||
            env_->curr_states[i].location == task->second.locations.front()) continue;
        if (oldest < 0 || agent.ticket < agents_[oldest].ticket) oldest = i;
    }
    std::vector<int> eligible;
    for (int i = 0; i < n_; ++i) {
        check_deadline(deadline_, "reassignment_eligibility");
        if (i == primary_ || i == oldest) { ++stats_.reassign_primary_protected; continue; }
        const Agent& agent = agents_[i];
        const int cell = env_->curr_states[i].location;
        if (agent.in_txn || std::binary_search(txn_cells_.begin(), txn_cells_.end(), cell)) {
            ++stats_.reassign_recovery_protected; continue;
        }
        if (parked_[i] || agent.lock >= 0 || !cert_.core[cell] ||
            (agent.committed >= 0 && agent.committed != cell) || now - last_reassignment_[i] < cooldown) continue;
        const auto found = env_->task_pool.find(proposed[i]);
        if (found == env_->task_pool.end()) continue;
        const Task& task = found->second;
        if (task.idx_next_loc != 0 || task.locations.empty() || !cert_.core[task.locations.front()] ||
            cell == task.locations.front() || !eligible_task(task)) continue;
        if (fair_tasks_.count(proposed[i])) { ++stats_.reassign_fair_protected; continue; }
        if (reassigned_tasks_.count(proposed[i])) continue;
        eligible.push_back(i);
    }
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

// Sparse whole-chain HRRN: nearby task candidates for every idle robot, plus an
// unpruned oldest-task admission. A bounded pair store never truncates the task
// pool by ID; unmatched robots get a fresh search over the remaining tasks.
void Cgar::schedule(SharedEnvironment* env, int time_limit_ms, std::vector<int>& proposed) {
    schedule(env, Clock::now() + std::chrono::milliseconds(time_limit_ms), proposed);
}

void Cgar::schedule(SharedEnvironment* env, Clock::time_point deadline, std::vector<int>& proposed) {
    check_deadline(deadline, "scheduling_start");
    ++stats_.schedule_calls;
    env_ = env;
    deadline_ = deadline;
    distance_deadline_ = deadline_;
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
    std::vector<int> robots;
    for (int i = 0; i < n_; ++i) if (proposed[i] == -1 && !parked_[i]) robots.push_back(i);
    if (robots.empty() || free_tasks_.empty()) {
        reassign_unopened(proposed);
        check_deadline(deadline_, "empty_schedule"); return;
    }
    std::rotate(robots.begin(), robots.begin() + scheduler_cursor_ % robots.size(), robots.end());
    ++scheduler_cursor_;

    struct TaskCost { int id, first, chain, revealed; };
    struct Pair { double score; int cost, task, robot, pickup; };
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
        tasks.push_back({id, first, task_chain_cost(id), task.t_revealed});
    }
    const int now = env->curr_timestep;
    auto pair_for = [&](int r, int t, int d) {
        const auto& task = tasks[t];
        const int cost = static_cast<int>(std::max<long long>(1, std::min<long long>(kInf - 1,
            static_cast<long long>(pickup_weight_) * d + task.chain)));
        return Pair{hrrn_ ? 1.0 + std::max(0, now - task.revealed) / static_cast<double>(cost) : 1.0,
                    cost, t, r, d};
    };
    auto better = [&](const Pair& a, const Pair& b) {
        if (a.score != b.score) return a.score > b.score;
        if (a.cost != b.cost) return a.cost < b.cost;
        if (tasks[a.task].id != tasks[b.task].id) return tasks[a.task].id < tasks[b.task].id;
        return a.robot < b.robot;
    };
    for (auto& bucket : at_cell) std::sort(bucket.begin(), bucket.end(), [&](int a, int b) {
        return better(pair_for(0, a, 0), pair_for(0, b, 0));
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
        stats_.estimated_pickup_cost += p.pickup;
        stats_.estimated_chain_cost += tasks[p.task].chain;
        ++stats_.assignments;
    };
    auto estimate = [&](int r, int t) {
        const int from = env->curr_states[r].location, goal = tasks[t].first;
        const auto* table = scheduler_cache_peek_ ? oracle_.peek(goal) : oracle_.find(goal);
        int d = table ? oracle_.value(*table, from) : oracle_.manhattan(from, goal);
        if (d >= kInf) {
            const int exit = cert_.exit_cell[from];
            d = exit >= 0 && table && oracle_.value(*table, exit) < kInf ? cert_.exit_dist[from] + oracle_.value(*table, exit) : kFar;
        }
        return pair_for(r, t, d);
    };
    auto fair_admission = [&]() {
        if (regular_admissions_ < n_) return;
        const int t = oldest_task();
        if (t < 0) return;
        Pair best{};
        bool found = false;
        for (int r : robots) if (!robot_used[r]) {
            const Pair p = estimate(r, t);
            if (!found || better(p, best)) { best = p; found = true; }
        }
        if (found) {
            assign(best);
            if (reassign_) fair_tasks_.insert(tasks[best.task].id);
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
                result.push_back(pair_for(r, t, distance[u]));
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
    std::vector<Pair> pairs;
    pairs.reserve(static_cast<size_t>(std::min<long long>(max_pairs_, robots.size() * per_robot)));
    for (int r : robots) {
        check_deadline(candidate_deadline, "candidate_generation");
        if (static_cast<long long>(pairs.size()) >= max_pairs_) break;
        auto local = candidates(r, std::min<int>(per_robot, static_cast<int>(max_pairs_ - pairs.size())));
        first_search_empty[r] = local.empty();
        pairs.insert(pairs.end(), local.begin(), local.end());
    }
    std::sort(pairs.begin(), pairs.end(), better);
    for (const Pair& p : pairs) {
        if (robot_used[p.robot] || task_used[p.task]) continue;
        fair_admission();
        if (robot_used[p.robot] || task_used[p.task]) continue;
        assign(p);
        ++stats_.local_assignments;
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
    check_deadline(deadline_, "scheduling_complete");
}

}  // namespace cgar
