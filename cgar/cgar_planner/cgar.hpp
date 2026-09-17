#pragma once
// CGAR for the League of Robot Runners 2024 start-kit.
//
// A grid re-implementation of the KNAPP CGAR ground-progress method
// (knapp/docs/pibt-progress-certificate.md). On a LoRR grid a robot occupies one
// cell and every robot acts in the same synchronous step, so a KNAPP berth is a free
// cell and a support macro u->v is "turn to face v, then FW". The pieces kept here:
//
//  * the directed capacity certificate: core berths B, support arcs H, promotion
//    arcs P with |Reach_(H-u)(v)| >= N, and |B| > N;
//  * constructive pocket evacuation followed by exclusive service;
//  * dense synchronous PIBT whose priority is the goal-ticket age, with the oldest
//    schedulable ticket pinned first as the fair primary;
//  * durable vacancy/evacuation witnesses, executed in compatible synchronous waves;
//  * whole-chain HRRN task dispatch with a fair admission wave.
//
// If the whole fleet exceeds the core capacity, outside robots park and only
// core tasks are admitted. The certificate then covers the active fleet.
//
// Orientation follows the start-kit: 0 east (+1), 1 south (+cols), 2 west (-1),
// 3 north (-cols).

#include "ActionModel.h"
#include "SharedEnv.h"

#include <chrono>
#include <cstdint>
#include <list>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cgar {

constexpr int kInf = 1 << 29;
constexpr long long kIdleTicket = (1LL << 62);

struct Certificate {
    int rows = 0, cols = 0;
    size_t robots = 0;
    std::vector<char> free;
    std::vector<char> core;
    std::vector<uint8_t> promotion;  // bit d: cell -> neighbour in direction d is a promotion arc
    std::vector<int> pocket;         // pocket id of a free off-core cell, else -1
    std::vector<std::vector<int>> pocket_cells;
    std::vector<int> exit_dist;      // off-core cell: steps to the nearest core cell within its pocket
    std::vector<int> exit_cell;      // off-core cell: that core cell
    size_t core_size = 0;
    size_t promotion_arcs = 0;
    size_t iterations = 0;
    size_t capacity = 0;  // fleet size the pruning actually used
    bool valid = false;   // premises hold for the real fleet size
};

Certificate build_certificate(const std::vector<int>& map, int rows, int cols, size_t robots);
// As above, but when the fleet does not fit, fall back to the largest capacity whose
// fixed point is well formed, so dead-end pockets still exist (they are what stops a
// dead-end deadlock). `valid` still reports whether the real fleet size is certified.
Certificate build_certificate_feasible(const std::vector<int>& map, int rows, int cols, size_t robots);

// BFS tables toward a goal through core cells and the goal's own pocket. Tables are
// only evicted by trim(), so references stay valid for the rest of a planning step.
class DistanceOracle {
public:
    void init(const Certificate* cert, size_t max_bytes);
    const std::vector<int>& table(int goal);
    const std::vector<int>* try_table(int goal, std::chrono::steady_clock::time_point deadline);
    int value(const std::vector<int>& table, int cell) const;
    const std::vector<int>* find(int goal);  // no build
    bool has(int goal) const { return tables_.count(goal) != 0; }
    bool full() const { return tables_.size() * table_bytes_ >= max_bytes_; }
    int dist(int from, int goal);
    int manhattan(int a, int b) const;
    void trim();

private:
    struct Entry {
        std::vector<int> dist;
        std::list<int>::iterator lru;
    };
    const Certificate* cert_ = nullptr;
    size_t max_bytes_ = 0;
    size_t table_bytes_ = 1;
    std::list<int> lru_;
    std::unordered_map<int, Entry> tables_;
    std::vector<int> queue_, index_, cells_;
    std::vector<std::vector<int>> neighbors_;
};

struct Agent {
    int goal = -1;
    int task = -1, stop = -1;
    long long ticket = kIdleTicket;
    int committed = -1;  // next cell this robot is turning toward or moving into
    int commit_age = 0;  // steps spent holding that commitment without arriving
    bool in_txn = false;
    int best = kInf;     // best route distance reached under the current ticket
    int stall = 0;       // steps since `best` last improved
    int lock = -1;       // pocket lock held
};

struct Stats {
    long long txns = 0;
    long long txn_aborts = 0;
    long long txn_no_hole = 0;
    long long txn_moves = 0;
    long long lock_grants = 0;
    long long safety_waits = 0;
    long long assignments = 0;
    long long fair_assignments = 0;
    long long evacuations = 0;
};

class Cgar {
public:
    static Cgar& instance();

    void initialize(SharedEnvironment* env, int preprocess_ms);
    void schedule(SharedEnvironment* env, int time_limit_ms, std::vector<int>& proposed);
    void plan(SharedEnvironment* env, int time_limit_ms, std::vector<Action>& actions);

    int primary() const { return primary_; }
    int parked_count() const;
    bool active_certified() const { return active_certified_; }

private:
    using Clock = std::chrono::steady_clock;

    // step preparation
    void sync_agents();
    void update_locks();
    int select_primary();
    void compute_order(int primary);

    // routing
    int route_h(int i, int cell);
    bool allowed(int i, int cell) const;
    int turn_steps(int i, int cell) const;
    bool pibt(int i, int parent);
    void reserve(int cell, int who);

    // liveness floor
    void try_install_txn(int primary);
    void abort_txn();
    struct Shift { int robot, from, to; };
    bool clear_cell(int cell, int forbidden, const std::vector<char>& region,
                    std::vector<int>& occupancy, std::vector<int>& positions,
                    std::vector<Shift>& witness);
    void install_txn(std::vector<Shift> witness);
    void advance_txn();
    bool evacuate_pocket(int pocket);
    void prepare_capacity_mode();
    bool eligible_task(const Task& task) const;

    // action synthesis
    Action action_toward(int i, int target) const;
    bool move_check(int i, std::vector<char>& checked, std::vector<Action>& actions);
    void make_safe(std::vector<Action>& actions);

    int neighbor(int cell, int dir) const;
    bool adjacent_to_pocket(int cell, int pocket) const;
    int task_chain_cost(int task_id);
    void log_summary();

    bool initialized_ = false;
    SharedEnvironment* env_ = nullptr;
    Certificate cert_;
    DistanceOracle oracle_;
    Stats stats_;
    std::mt19937 rng_{0};

    int n_ = 0;
    std::vector<Agent> agents_;
    std::vector<int> loc_, ori_;
    std::vector<int> occ_now_;     // cell -> robot now
    std::vector<int> reserved_;    // cell -> robot planned there next, -2 for transaction cells
    std::vector<int> touched_;
    std::vector<int> next_;
    std::vector<int> order_;
    std::vector<int> pocket_occ_, pocket_lock_;
    std::vector<char> pocket_draining_, parked_;
    long long next_ticket_ = 0;

    std::vector<int> txn_cells_;
    std::vector<Shift> txn_moves_;
    size_t txn_cursor_ = 0, txn_wave_end_ = 0;
    int txn_age_ = 0;
    int stall_limit_ = 4;
    int commit_limit_ = 3;  // a commitment covers 2 turns plus the move
    long long primary_ticket_ = kIdleTicket;
    int table_budget_ = 0;       // new BFS tables allowed in the current call
    int plan_tables_ = 256;      // per-step budget for the planner
    int sched_tables_ = 128;     // per-step budget for the scheduler
    long long max_pairs_ = 2000000;
    bool enable_txn_ = true;
    bool enable_locks_ = true;
    bool hrrn_ = true;
    int primary_ = -1;
    bool capacity_mode_ = false, parking_ready_ = false, active_certified_ = false;
    Clock::time_point deadline_, distance_deadline_;

    // scheduler state
    std::unordered_set<int> free_tasks_;
    std::unordered_map<int, int> chain_cost_;
    long long regular_admissions_ = 0;
    size_t scheduler_cursor_ = 0;
};

}  // namespace cgar
