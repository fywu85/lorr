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
    table_bytes_ = std::max<size_t>(1, cert->free.size() * sizeof(int));
    tables_.clear();
    lru_.clear();
}

const std::vector<int>& DistanceOracle::table(int goal) {
    auto it = tables_.find(goal);
    if (it != tables_.end()) {
        lru_.splice(lru_.begin(), lru_, it->second.lru);
        return it->second.dist;
    }
    lru_.push_front(goal);
    Entry& entry = tables_[goal];
    entry.lru = lru_.begin();
    std::vector<int>& dist = entry.dist;
    dist.assign(cert_->free.size(), kInf);
    const int goal_pocket = cert_->pocket[goal];
    queue_.assign(1, goal);
    dist[goal] = 0;
    for (size_t head = 0; head < queue_.size(); ++head) {
        const int u = queue_[head];
        for (int d = 0; d < 4; ++d) {
            const int v = grid_neighbor(u, d, cert_->rows, cert_->cols);
            if (v < 0 || !cert_->free[v] || dist[v] != kInf) continue;
            if (!cert_->core[v] && cert_->pocket[v] != goal_pocket) continue;
            dist[v] = dist[u] + 1;
            queue_.push_back(v);
        }
    }
    return dist;
}

const std::vector<int>* DistanceOracle::find(int goal) {
    auto it = tables_.find(goal);
    if (it == tables_.end()) return nullptr;
    lru_.splice(lru_.begin(), lru_, it->second.lru);
    return &it->second.dist;
}

int DistanceOracle::dist(int from, int goal) {
    const auto& t = table(goal);
    if (t[from] < kInf) return t[from];
    const int exit = cert_->exit_cell[from];
    if (exit >= 0 && t[exit] < kInf) return cert_->exit_dist[from] + t[exit];
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
    primary_patience_ = env_int("CGAR_PRIMARY_PATIENCE", 40);
    plan_tables_ = env_int("CGAR_PLAN_TABLES", 256);
    sched_tables_ = env_int("CGAR_SCHED_TABLES", 128);
    max_pairs_ = env_int("CGAR_MAX_PAIRS", 2000000);
    txn_max_age_ = env_int("CGAR_TXN_MAX_AGE", 12);
    enable_txn_ = env_int("CGAR_TXN", 1) != 0;
    hrrn_ = env_int("CGAR_HRRN", 1) != 0;
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
    agents_.assign(n_, Agent());
    loc_.assign(n_, -1);
    ori_.assign(n_, 0);
    free_tasks_.clear();
    chain_cost_.clear();

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
        if (goal != a.goal) {
            a.goal = goal;
            a.ticket = goal < 0 ? kIdleTicket : next_ticket_++;
            a.best = kInf;
            a.stall = 0;
        }
        if (a.committed == loc_[i]) {
            a.committed = -1;
            a.commit_age = 0;
            a.in_txn = false;
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
        const int holder = pocket_lock_[p];
        if (holder < 0) continue;
        Agent& a = agents_[holder];
        const bool wants = a.goal >= 0 && cert_.pocket[a.goal] == static_cast<int>(p);
        const bool near = cert_.pocket[loc_[holder]] == static_cast<int>(p) ||
                          adjacent_to_pocket(loc_[holder], static_cast<int>(p));
        if (!wants || !near) {
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
    int best = -1;
    for (int i = 0; i < n_; ++i) {
        const Agent& a = agents_[i];
        if (a.goal < 0) continue;
        // A primary that has stalled far past what a witness can repair yields the
        // floor, so one trapped robot cannot freeze the whole fleet.
        if (a.stall > primary_patience_) continue;
        const int gp = cert_.pocket[a.goal];
        const bool schedulable = gp < 0 || cert_.pocket[loc_[i]] == gp || pocket_lock_[gp] == i;
        if (schedulable && (best < 0 || a.ticket < agents_[best].ticket)) best = i;
    }
    return best;
}

int Cgar::route_h(int i, int cell) {
    const Agent& a = agents_[i];
    const int pocket = cert_.pocket[cell];
    if (a.goal < 0) return pocket < 0 ? 0 : kFar + cert_.exit_dist[cell];
    const std::vector<int>* t = oracle_.find(a.goal);
    if (t == nullptr) {
        // Building a table is a full-map BFS. Cap how many a single step may build,
        // otherwise a map with thousands of distinct goals blows the time budget;
        // robots that miss out steer by straight-line distance until a later step.
        if (table_budget_ <= 0) return oracle_.manhattan(cell, a.goal);
        --table_budget_;
        t = &oracle_.table(a.goal);
    }
    if ((*t)[cell] < kInf) return (*t)[cell];
    if (pocket >= 0) return kFar + cert_.exit_dist[cell];
    return kInf;
}

bool Cgar::allowed(int i, int cell) const {
    if (!cert_.free[cell]) return false;
    if (cert_.core[cell]) return true;
    const int p = cert_.pocket[cell];
    if (p < 0) return false;  // disconnected floor that is not a pocket (ablation)
    return p == cert_.pocket[loc_[i]] || pocket_lock_[p] == i;
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
    txn_age_ = 0;
    ++stats_.txn_aborts;
}

// KNAPP's weighted hole-to-target witness on the grid: the cheapest chain of
// occupied core berths from the primary's next promotion berth v to a hole,
// avoiding the primary's own berth u. Occupants shift one berth toward the hole,
// then the primary is promoted into v.
void Cgar::try_install_txn(int primary) {
    const int u = loc_[primary];
    const Agent& pa = agents_[primary];
    (void)pa;
    const int hu = route_h(primary, u);
    int v = -1, hv = kInf;
    for (int d = 0; d < 4; ++d) {
        const int w = neighbor(u, d);
        if (w < 0 || !allowed(primary, w)) continue;
        // A core-to-core step of the promoted root must be a certified promotion arc;
        // leaving or entering a pocket is a portal move, which the lock makes exclusive.
        if (cert_.core[u] && cert_.core[w] && !(cert_.promotion[u] >> d & 1)) continue;
        const int h = route_h(primary, w);
        if (h >= hu) continue;
        if (v < 0 || h < hv || (h == hv && turn_steps(primary, w) < turn_steps(primary, v))) {
            v = w;
            hv = h;
        }
    }
    if (v < 0) return;
    if (occ_now_[v] >= 0 && agents_[occ_now_[v]].in_txn) return;
    if (!cert_.core[v]) {
        // Portal step into the primary's own pocket: exclusive, so no hole is needed.
        Agent& p = agents_[primary];
        p.committed = v;
        p.commit_age = 0;
        p.in_txn = true;
        txn_cells_.assign({u, v});
        txn_age_ = 0;
        ++stats_.txns;
        ++stats_.txn_moves;
        return;
    }

    static thread_local std::vector<int> cost, prev;
    static thread_local std::vector<int> seen;
    const size_t cells = cert_.free.size();
    if (cost.size() != cells) {
        cost.assign(cells, kInf);
        prev.assign(cells, -1);
    }
    for (int c : seen) {
        cost[c] = kInf;
        prev[c] = -1;
    }
    seen.clear();

    using Item = std::pair<int, int>;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> open;
    cost[v] = 0;
    seen.push_back(v);
    open.push({0, v});
    int hole = -1;
    while (!open.empty()) {
        const auto [g, q] = open.top();
        open.pop();
        if (g != cost[q]) continue;
        const int o = occ_now_[q];
        if (o < 0) {
            hole = q;
            break;
        }
        if (agents_[o].in_txn || o == primary) continue;
        const int hq = route_h(o, q);
        for (int d = 0; d < 4; ++d) {
            const int q2 = neighbor(q, d);
            if (q2 < 0 || q2 == u || !cert_.core[q2]) continue;
            const int o2 = occ_now_[q2];
            if (o2 >= 0 && agents_[o2].in_txn) continue;
            const int hq2 = route_h(o, q2);
            const int regret = (hq >= kFar || hq2 >= kFar) ? 0 : std::max(0, hq2 - hq);
            const int step = 1 + turn_steps(o, q2) + regret;
            if (g + step < cost[q2]) {
                if (cost[q2] == kInf) seen.push_back(q2);
                cost[q2] = g + step;
                prev[q2] = q;
                open.push({cost[q2], q2});
            }
        }
    }
    if (hole < 0) {
        ++stats_.txn_no_hole;
        return;
    }

    std::vector<int> path;  // hole ... v
    for (int c = hole; c >= 0; c = prev[c]) path.push_back(c);
    txn_cells_.assign(path.begin(), path.end());
    txn_cells_.push_back(u);
    std::unordered_set<int> cells_set(txn_cells_.begin(), txn_cells_.end());
    for (int i = 0; i < n_; ++i) {
        Agent& a = agents_[i];
        if (!a.in_txn && a.committed >= 0 && cells_set.count(a.committed)) a.committed = -1;
    }
    // path[k] -> path[k-1]: the occupant of each berth shifts toward the hole.
    for (size_t k = 1; k < path.size(); ++k) {
        Agent& a = agents_[occ_now_[path[k]]];
        a.committed = path[k - 1];
        a.commit_age = 0;
        a.in_txn = true;
    }
    Agent& p = agents_[primary];
    p.committed = v;
    p.commit_age = 0;
    p.in_txn = true;
    txn_age_ = 0;
    ++stats_.txns;
    stats_.txn_moves += static_cast<long long>(path.size());
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
    env_ = env;
    const auto deadline = Clock::now() + std::chrono::milliseconds(std::max(1, time_limit_ms));
    oracle_.trim();
    table_budget_ = plan_tables_;
    sync_agents();
    update_locks();

    // Grant pocket locks in ticket order to robots at a portal whose goal lies inside.
    compute_order(-1);
    for (int i : order_) {
        Agent& a = agents_[i];
        if (a.goal < 0 || !enable_locks_) continue;
        const int gp = cert_.pocket[a.goal];
        if (gp < 0 || cert_.pocket[loc_[i]] >= 0) continue;
        if (pocket_lock_[gp] >= 0 || pocket_occ_[gp] > 0) continue;
        const auto& t = oracle_.table(a.goal);
        for (int d = 0; d < 4; ++d) {
            const int v = neighbor(loc_[i], d);
            if (v >= 0 && cert_.pocket[v] == gp && t[v] < t[loc_[i]]) {
                pocket_lock_[gp] = i;
                a.lock = gp;
                ++stats_.lock_grants;
                break;
            }
        }
    }
    primary_ = select_primary();
    compute_order(primary_);

    for (int i = 0; i < n_; ++i) {
        Agent& a = agents_[i];
        if (a.goal < 0) continue;
        const int h = route_h(i, loc_[i]);
        if (h < a.best) {
            a.best = h;
            a.stall = 0;
        } else {
            ++a.stall;
        }
    }

    bool txn_active = false;
    for (const Agent& a : agents_) txn_active = txn_active || a.in_txn;
    if (txn_active && ++txn_age_ > txn_max_age_) {
        abort_txn();
        txn_active = false;
    }
    if (!txn_active) txn_cells_.clear();
    if (enable_txn_ && cert_.valid && !txn_active && primary_ >= 0 &&
        agents_[primary_].stall >= stall_limit_) {
        try_install_txn(primary_);
    }

    for (int c : touched_) reserved_[c] = -1;
    touched_.clear();
    next_.assign(n_, -1);
    for (int c : txn_cells_) reserve(c, -2);
    for (int pass = 1; pass >= 0; --pass) {
        for (int i = 0; i < n_; ++i) {
            Agent& a = agents_[i];
            if (a.committed < 0 || a.in_txn != (pass == 1)) continue;
            const int c = a.committed;
            if (pass == 1) {
                reserve(c, i);
                next_[i] = c;
            } else if (reserved_[c] == -1 && allowed(i, c)) {
                reserve(c, i);
                next_[i] = c;
            } else {
                a.committed = -1;
            }
        }
    }

    for (int i : order_) {
        if (next_[i] != -1) continue;
        if (Clock::now() > deadline) {
            next_[i] = loc_[i];
            continue;
        }
        pibt(i, -1);
    }

    actions.assign(n_, Action::W);
    for (int i = 0; i < n_; ++i) {
        if (next_[i] == loc_[i]) continue;
        if (agents_[i].committed != next_[i]) agents_[i].commit_age = 0;
        agents_[i].committed = next_[i];
        actions[i] = action_toward(i, next_[i]);
    }
    std::vector<char> checked(n_, 0);
    for (int i = 0; i < n_; ++i) {
        if (!checked[i] && actions[i] == Action::FW) move_check(i, checked, actions);
    }
    make_safe(actions);

    if (env_->curr_timestep % 200 == 0) log_summary();
}

void Cgar::log_summary() {
    int in_txn = 0, locks = 0, idle = 0;
    for (const Agent& a : agents_) {
        in_txn += a.in_txn;
        idle += a.goal < 0;
    }
    for (int h : pocket_lock_) locks += h >= 0;
    std::printf("[cgar] t=%d primary=%d stall=%d txns=%lld moves=%lld aborts=%lld no_hole=%lld in_txn=%d "
                "locks=%d grants=%lld safety_waits=%lld idle=%d assigned=%lld fair=%lld tables=%s\n",
                env_->curr_timestep, primary_, primary_ >= 0 ? agents_[primary_].stall : -1, stats_.txns,
                stats_.txn_moves, stats_.txn_aborts, stats_.txn_no_hole, in_txn, locks, stats_.lock_grants,
                stats_.safety_waits, idle, stats_.assignments, stats_.fair_assignments,
                oracle_.full() ? "full" : "ok");
    std::fflush(stdout);
}

// ─── scheduler ───────────────────────────────────────────────────────────────

int Cgar::task_chain_cost(int task_id) {
    auto it = chain_cost_.find(task_id);
    if (it != chain_cost_.end()) return it->second;
    const Task& task = env_->task_pool.at(task_id);
    long long total = 0;
    for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k) {
        int d;
        if (oracle_.find(task.locations[k]) == nullptr) {
            if (table_budget_ > 0) {
                --table_budget_;
                d = oracle_.dist(task.locations[k - 1], task.locations[k]);
            } else {
                d = oracle_.manhattan(task.locations[k - 1], task.locations[k]);
            }
        } else {
            d = oracle_.dist(task.locations[k - 1], task.locations[k]);
        }
        total += std::min(d, kFar);
    }
    const int cost = static_cast<int>(std::min<long long>(total, kFar));
    chain_cost_[task_id] = cost;
    return cost;
}

// Whole-chain HRRN (knapp pibt-greedy without route-load pricing):
// score = 1 + wait / (distance to the first errand + remaining chain cost),
// best pairs first, and after every fleet-width of admissions one slot for the
// oldest open task.
void Cgar::schedule(SharedEnvironment* env, int time_limit_ms, std::vector<int>& proposed) {
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
}

}  // namespace cgar
