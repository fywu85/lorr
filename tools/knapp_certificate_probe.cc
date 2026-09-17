// Probe whether KNAPP's abstract PIBT ground-progress certificate
// (knapp/src/core/pibt_progress_certificate.h) can hold on a LoRR 2024 instance.
//
// On a LoRR grid a berth is a free cell with a canonical heading, and a support
// macro u->v is "turn to face v, FW, turn to the canonical heading". It occupies
// only u and v, so KNAPP's geometric premises 1-3 hold trivially and the support
// graph H is the symmetric 4-connected free-cell graph.
//
// usage: knapp_certificate_probe <instance.json> [--exact] [--max-tasks K]
//   --exact  use knapp::pibt_directed_capacity_fixed_point and
//            knapp::certify_pibt_directed_core (O(V*E) per iteration; small maps only)
//   default  an equivalent block-cut computation valid for symmetric H, O(V+E) per iteration
#include "core/pibt_progress_certificate.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

using knapp::NodeId;
using knapp::PibtAdjacency;

namespace {

struct Grid {
    int rows = 0, cols = 0;
    std::vector<char> free;  // per linear cell index
};

std::string dir_of(const std::string& path) {
    auto slash = path.find_last_of('/');
    return slash == std::string::npos ? "." : path.substr(0, slash);
}

// Minimal extraction of "key": "value" / "key": number from the flat instance JSON.
std::string json_field(const std::string& text, const std::string& key) {
    auto at = text.find("\"" + key + "\"");
    if (at == std::string::npos) return "";
    at = text.find(':', at) + 1;
    while (text[at] == ' ' || text[at] == '"') ++at;
    auto end = text.find_first_of("\",\n}", at);
    return text.substr(at, end - at);
}

Grid read_map(const std::string& path) {
    std::ifstream in(path);
    std::string line;
    Grid g;
    std::getline(in, line);  // type octile
    std::getline(in, line); g.rows = std::stoi(line.substr(line.find(' ') + 1));
    std::getline(in, line); g.cols = std::stoi(line.substr(line.find(' ') + 1));
    std::getline(in, line);  // map
    g.free.assign(static_cast<size_t>(g.rows) * g.cols, 0);
    for (int r = 0; r < g.rows && std::getline(in, line); ++r) {
        for (int c = 0; c < g.cols && c < static_cast<int>(line.size()); ++c) {
            // Same rule as the start-kit Grid.cpp: '@' and 'T' are obstacles.
            g.free[r * g.cols + c] = (line[c] != '@' && line[c] != 'T');
        }
    }
    return g;
}

std::vector<int> read_agents(const std::string& path) {
    std::ifstream in(path);
    std::string line;
    std::vector<int> locs;
    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        locs.push_back(std::stoi(line));
    }
    locs.erase(locs.begin());  // leading count
    return locs;
}

std::vector<int> read_task_errands(const std::string& path, size_t max_tasks) {
    std::ifstream in(path);
    std::string line;
    std::vector<int> errands;
    bool counted = false;
    size_t tasks = 0;
    while (std::getline(in, line) && tasks < max_tasks) {
        if (line.empty() || line[0] == '#') continue;
        if (!counted) { counted = true; continue; }
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) errands.push_back(std::stoi(cell));
        ++tasks;
    }
    return errands;
}

PibtAdjacency grid_adjacency(const Grid& g) {
    PibtAdjacency adj(g.free.size());
    for (int r = 0; r < g.rows; ++r) {
        for (int c = 0; c < g.cols; ++c) {
            const int u = r * g.cols + c;
            if (!g.free[u]) continue;
            const int dr[4] = {0, 1, 0, -1}, dc[4] = {1, 0, -1, 0};
            for (int k = 0; k < 4; ++k) {
                int rr = r + dr[k], cc = c + dc[k];
                if (rr < 0 || cc < 0 || rr >= g.rows || cc >= g.cols) continue;
                const int v = rr * g.cols + cc;
                if (g.free[v]) adj[u].push_back(static_cast<NodeId>(v));
            }
        }
    }
    return adj;
}

// Largest SCC of the directed graph induced by `active` (iterative Tarjan).
std::vector<char> largest_scc(const PibtAdjacency& adj, const std::vector<char>& active) {
    const size_t n = adj.size();
    std::vector<int> index(n, -1), low(n, 0), comp(n, -1);
    std::vector<char> on_stack(n, 0);
    std::vector<NodeId> stack;
    int next_index = 0, comps = 0;
    std::vector<size_t> comp_size;
    std::vector<std::pair<NodeId, size_t>> call;
    for (NodeId s = 0; s < n; ++s) {
        if (!active[s] || index[s] >= 0) continue;
        call.push_back({s, 0});
        index[s] = low[s] = next_index++;
        stack.push_back(s); on_stack[s] = 1;
        while (!call.empty()) {
            auto& [u, i] = call.back();
            if (i < adj[u].size()) {
                NodeId v = adj[u][i++];
                if (!active[v]) continue;
                if (index[v] < 0) {
                    index[v] = low[v] = next_index++;
                    stack.push_back(v); on_stack[v] = 1;
                    call.push_back({v, 0});
                } else if (on_stack[v]) {
                    low[u] = std::min(low[u], index[v]);
                }
            } else {
                NodeId done = u;
                call.pop_back();
                if (!call.empty()) low[call.back().first] = std::min(low[call.back().first], low[done]);
                if (low[done] == index[done]) {
                    size_t size = 0;
                    for (;;) {
                        NodeId w = stack.back(); stack.pop_back(); on_stack[w] = 0;
                        comp[w] = comps; ++size;
                        if (w == done) break;
                    }
                    comp_size.push_back(size);
                    ++comps;
                }
            }
        }
    }
    std::vector<char> out(n, 0);
    if (comps == 0) return out;
    const int best = static_cast<int>(std::max_element(comp_size.begin(), comp_size.end()) - comp_size.begin());
    for (size_t v = 0; v < n; ++v) out[v] = (comp[v] == best);
    return out;
}

// For a symmetric graph induced by `core` (connected), return for every arc u->v
// (in adj order) the size of the component of (core - u) that contains v.
std::vector<std::vector<size_t>> piece_sizes(const PibtAdjacency& adj, const std::vector<char>& core) {
    const size_t n = adj.size();
    std::vector<int> tin(n, -1), tout(n, -1), low(n, 0), parent(n, -1);
    std::vector<size_t> sz(n, 1);
    std::vector<std::vector<NodeId>> children(n);
    size_t total = 0;
    NodeId root = knapp::kInvalidNode;
    for (NodeId v = 0; v < n; ++v) if (core[v]) { ++total; if (root == knapp::kInvalidNode) root = v; }
    std::vector<std::vector<size_t>> out(n);
    if (root == knapp::kInvalidNode) return out;

    int timer = 0;
    std::vector<std::pair<NodeId, size_t>> call{{root, 0}};
    tin[root] = low[root] = timer++;
    while (!call.empty()) {
        auto& [u, i] = call.back();
        if (i < adj[u].size()) {
            NodeId v = adj[u][i++];
            if (!core[v]) continue;
            if (tin[v] < 0) {
                parent[v] = static_cast<int>(u);
                children[u].push_back(v);
                tin[v] = low[v] = timer++;
                call.push_back({v, 0});
            } else if (static_cast<int>(v) != parent[u]) {
                low[u] = std::min(low[u], tin[v]);
            }
        } else {
            NodeId done = u;
            tout[done] = timer - 1;
            call.pop_back();
            if (!call.empty()) {
                NodeId p = call.back().first;
                low[p] = std::min(low[p], low[done]);
                sz[p] += sz[done];
            }
        }
    }
    for (NodeId u = 0; u < n; ++u) {
        if (!core[u]) continue;
        size_t separated = 0;
        for (NodeId c : children[u]) if (u == root || low[c] >= tin[u]) separated += sz[c];
        const size_t rest = total - 1 - separated;
        out[u].reserve(adj[u].size());
        for (NodeId v : adj[u]) {
            if (!core[v]) { out[u].push_back(0); continue; }
            const bool descendant = tin[v] > tin[u] && tin[v] <= tout[u];
            if (!descendant) { out[u].push_back(rest); continue; }
            // children[u] is in increasing tin order; find the subtree holding v.
            auto it = std::upper_bound(children[u].begin(), children[u].end(), v,
                                       [&](NodeId a, NodeId b) { return tin[a] < tin[b]; });
            NodeId c = *std::prev(it);
            out[u].push_back((u == root || low[c] >= tin[u]) ? sz[c] : rest);
        }
    }
    return out;
}

struct FastFixedPoint {
    std::vector<char> core;
    PibtAdjacency promotion;
    size_t iterations = 0;
};

// Same fixed point as knapp::pibt_directed_capacity_fixed_point, specialised to a
// symmetric support graph.
FastFixedPoint fast_fixed_point(const PibtAdjacency& adj, const std::vector<char>& active, size_t robots) {
    FastFixedPoint r;
    r.core = largest_scc(adj, active);
    for (;;) {
        const auto pieces = piece_sizes(adj, r.core);
        PibtAdjacency promotion(adj.size());
        for (NodeId u = 0; u < adj.size(); ++u) {
            if (!r.core[u]) continue;
            for (size_t k = 0; k < adj[u].size(); ++k) {
                if (r.core[adj[u][k]] && pieces[u][k] >= robots) promotion[u].push_back(adj[u][k]);
            }
        }
        auto next = largest_scc(promotion, r.core);
        ++r.iterations;
        if (next == r.core) { r.promotion = std::move(promotion); break; }
        r.core = std::move(next);
    }
    return r;
}

size_t count_in(const std::vector<char>& core, const std::vector<int>& cells) {
    size_t k = 0;
    for (int c : cells) k += (c >= 0 && static_cast<size_t>(c) < core.size() && core[c]);
    return k;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) { std::fprintf(stderr, "usage: %s <instance.json> [--exact] [--max-tasks K]\n", argv[0]); return 2; }
    std::string instance = argv[1];
    bool exact = false;
    size_t max_tasks = 100000;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--exact") exact = true;
        else if (a == "--max-tasks" && i + 1 < argc) max_tasks = std::stoul(argv[++i]);
    }
    std::ifstream in(instance);
    std::string text((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const std::string base = dir_of(instance) + "/";
    const Grid g = read_map(base + json_field(text, "mapFile"));
    const size_t robots = std::stoul(json_field(text, "teamSize"));
    std::vector<int> agents = read_agents(base + json_field(text, "agentFile"));
    agents.resize(std::min(agents.size(), robots));
    const std::vector<int> errands = read_task_errands(base + json_field(text, "taskFile"), max_tasks);

    const PibtAdjacency adj = grid_adjacency(g);
    const size_t free_cells = std::count(g.free.begin(), g.free.end(), 1);
    const auto t0 = std::chrono::steady_clock::now();

    std::vector<char> core;
    size_t iterations = 0, promotion_arcs = 0;
    bool valid = false;
    std::string detail;
    if (exact) {
        auto fp = knapp::pibt_directed_capacity_fixed_point(adj, g.free, robots);
        core = fp.core;
        iterations = fp.iterations;
        const auto& c = fp.certificate;
        promotion_arcs = c.arcs;
        valid = c.valid();
        detail = " knapp_certificate{support_sc=" + std::to_string(c.support_strongly_connected) +
                 " promotion_sc=" + std::to_string(c.strongly_connected) +
                 " reach_capacity=" + std::to_string(c.reach_capacity) +
                 " min_reach_after_root_removal=" + std::to_string(c.min_reachable_after_root_removal) +
                 " holes=" + std::to_string(c.holes) + "}";
    } else {
        auto fp = fast_fixed_point(adj, g.free, robots);
        core = fp.core;
        iterations = fp.iterations;
        for (const auto& out : fp.promotion) promotion_arcs += out.size();
        const size_t core_size = std::count(core.begin(), core.end(), 1);
        // Premises 4-6 hold by construction of the fixed point; premise 7 is |B| > N.
        valid = core_size > robots && promotion_arcs > 0;
    }
    const double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
    const size_t core_size = std::count(core.begin(), core.end(), 1);
    std::printf("%s N=%zu free=%zu core=%zu (%.1f%% of free) holes=%lld promotion_arcs=%zu iterations=%zu "
                "valid=%d agents_in_core=%zu/%zu errands_in_core=%zu/%zu (%.1f%%) mode=%s secs=%.2f%s\n",
                instance.substr(instance.find_last_of('/') + 1).c_str(), robots, free_cells, core_size,
                100.0 * core_size / free_cells, static_cast<long long>(core_size) - static_cast<long long>(robots),
                promotion_arcs, iterations, valid, count_in(core, agents), agents.size(),
                count_in(core, errands), errands.size(), 100.0 * count_in(core, errands) / errands.size(),
                exact ? "exact" : "fast", secs, detail.c_str());
    if (const char* dump = std::getenv("PROBE_DUMP_CORE")) {
        std::ofstream out(dump);
        for (size_t v = 0; v < core.size(); ++v) if (core[v]) out << v << '\n';
    }
    return 0;
}
