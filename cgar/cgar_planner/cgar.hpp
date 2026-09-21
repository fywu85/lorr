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
#include "pibt_kernel.hpp"
#include "pickup_search.hpp"
#include "pickup_full.hpp"
#include "assignment_permutation.hpp"
#include "temporal_geometry.hpp"
#include "temporal_prepare.hpp"
#include "temporal_priorities.hpp"
#include "temporal_regions.hpp"
#include "temporal_transactions.hpp"
#include "temporal_warm_start.hpp"
#include "flow_guidance.hpp"
#include "tricks.hpp"
#include "game_fleet.hpp"
#include "horizon_bound.hpp"
#include "chain_potential.hpp"
#include "guide_routes.hpp"

#include <array>
#include <chrono>
#include <cstdint>
#include <list>
#include <map>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace cgar {

class Timeout : public std::runtime_error {
public:
    explicit Timeout(const char* stage) : std::runtime_error(stage) {}
};

inline void check_deadline(std::chrono::steady_clock::time_point deadline, const char* stage) {
    if (std::chrono::steady_clock::now() >= deadline) throw Timeout(stage);
}

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
    const std::vector<int>* peek(int goal) const;  // no build or LRU promotion
    int distance_from(const std::vector<int>& table, int from) const;
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

// Lossless optional packing. A table with any finite value >= 65535 stays
// 32-bit; unreachable entries retain kInf through a reserved 16-bit sentinel.
// Logical cache capacity still uses the original 32-bit table size, preserving
// admission and eviction decisions while reducing physical storage.
class TurnTable {
public:
    TurnTable() = default;
    explicit TurnTable(std::vector<int> values, bool compact) {
        bool fits = compact;
        if (fits) for (int value : values)
            if (value != kInf && (value < 0 || value >= 65535)) { fits = false; break; }
        if (fits) {
            narrow_.reserve(values.size());
            for (int value : values) narrow_.push_back(value == kInf ? 65535 : value);
        } else wide_ = std::move(values);
    }
    int operator[](size_t index) const {
        if (narrow_.empty()) return wide_[index];
        const int value = narrow_[index]; return value == 65535 ? kInf : value;
    }
    size_t size() const { return narrow_.empty() ? wide_.size() : narrow_.size(); }
    size_t storage_bytes() const { return wide_.size() * sizeof(int) + narrow_.size() * sizeof(uint16_t); }
    bool is_compact() const { return !narrow_.empty(); }
    bool operator!=(const TurnTable& other) const {
        if (size() != other.size()) return true;
        for (size_t i = 0; i < size(); ++i) if ((*this)[i] != other[i]) return true;
        return false;
    }
private:
    std::vector<int> wide_;
    std::vector<uint16_t> narrow_;
};

// Shortest guidance costs over (cell, orientation), with positive integer
// turn costs and optional learned forward costs. The certified spatial potential remains in
// DistanceOracle. Every cached reverse traversal is complete.
class TurnDistanceOracle {
public:
    void init(const Certificate* cert, size_t max_bytes, int turn_cost = 1, bool compact = false, int forward_base = 1, int cost_limit = 16);
    void prefetch(const std::vector<int>& goals, int threads, std::chrono::steady_clock::time_point deadline);
    void discard_prefetch();
    // Complete every free-cell goal in fixed batches, before tasks are revealed.
    // Requires sufficient logical capacity; timeout throws, never succeeds with
    // a partially warmed cache. Returns the actual bytes in resident tables.
    size_t prewarm_all(int threads, std::chrono::steady_clock::time_point deadline);
    void clear_tables();  // discard cached results while preserving the metric
    bool set_forward_costs(std::vector<uint8_t> costs);
    int forward_cost(int cell, int orientation) const {
        return forward_costs_.empty() ? forward_base_ : forward_costs_.at(size_t(cell) * 4 + orientation);
    }
    bool weighted_forward() const { return !forward_costs_.empty(); }
    long long prefetched_builds = 0, prefetched_hits = 0, prefetched_discarded = 0, wide_fallback_tables = 0;
    const TurnTable* find(int goal);
    const TurnTable* peek(int goal) const {
        const auto found = tables_.find(goal);
        return found == tables_.end() ? nullptr : &found->second.dist;
    }
    const TurnTable* table(int goal, std::chrono::steady_clock::time_point deadline);
    int value(const TurnTable& table, int cell, int orientation) const;
    bool has(int goal) const { return tables_.count(goal) != 0; }
    size_t capacity() const { return max_bytes_ / table_bytes_; }
    void retain(const std::unordered_set<int>& goals);
    void trim();
private:
    struct Entry { TurnTable dist; std::list<int>::iterator lru; };
    const Certificate* cert_ = nullptr;
    size_t max_bytes_ = 0, table_bytes_ = 1;
    std::vector<int> cells_, index_, queue_, backward_;
    int turn_cost_ = 1, max_edge_cost_ = 1, forward_base_ = 1, cost_limit_ = 16;
    bool compact_ = false;
    std::vector<uint8_t> forward_costs_;
    std::vector<std::vector<int>> buckets_;
    std::vector<int> compute(int goal, std::chrono::steady_clock::time_point deadline,
                             std::vector<int>& queue, std::vector<std::vector<int>>& buckets) const;
    std::unordered_map<int, std::vector<int>> prefetched_;
    std::list<int> lru_;
    std::unordered_map<int, Entry> tables_;
};

enum class ChainPriceStatus { Covered, MissingTable, OutsideDomain, Unreachable, Invalid };
struct ResidentChainPrice {
    int cost = -1;
    ChainPriceStatus status = ChainPriceStatus::MissingTable;
};
// Relax each known loaded leg's starting heading independently. Consult only
// complete current-metric resident tables, without admission or LRU mutations.
ResidentChainPrice resident_chain_price(const Task& task, const TurnDistanceOracle& oracle,
                                       const Certificate& cert);
int imputed_chain_price(int native, long long numerator, long long denominator);
int selected_chain_price(int mode, int native, const ResidentChainPrice& resident,
                         long long numerator, long long denominator);

// Retain table-derived scalar legs even if their full distance table is evicted.
// Previously approximated legs are refined only when a complete table is cached.
class ChainCostCache {
public:
    int estimate(const Task& task, DistanceOracle& oracle, int& table_budget,
                 std::chrono::steady_clock::time_point deadline, bool peek);
    void retain(const std::unordered_set<int>& task_ids);
    bool all_table_derived(int task_id) const;
    long long refined_legs = 0, changed_costs = 0, invalidations = 0;
    long long approximate_reads = 0, table_reads = 0;
private:
    struct Entry {
        int stop = -1;
        std::vector<int> locations, legs;
        std::vector<char> table_derived;
        int total = 0;
    };
    std::unordered_map<int, Entry> entries_;
};

enum class ProgressBasis { None, Manhattan, RouteTable, PocketExit };

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
    ProgressBasis progress_basis = ProgressBasis::None;
    // A different potential starts a new observation window, not a new ticket.
    bool observe_progress(int distance, ProgressBasis basis, bool stable_basis);
};

// Disjoint final actions and overlapping diagnostic contexts. Recording only
// reads cached distances and never changes table recency or consumes randomness.
struct MovementStats {
    long long actions[4] = {};  // FW, CR, CCR, W
    long long planned_wait = 0, blocked_forward = 0, safety_cancel = 0;
    long long turn_dependency = 0;
    long long forward_closer = 0, forward_farther = 0, forward_equal = 0, forward_unknown = 0;
    long long recovery = 0, primary = 0, commitment = 0, pocket = 0;
};

struct Stats {
    long long match_repeat_moves = 0, match_max_task_moves = 0;
    TemporalRegionPeaks regional_peaks;
    long long regional_peaks_restored = 0;
    long long chain_robot_steps = 0, chain_fallback_steps = 0, chain_scored_choices = 0;
    long long chain_service_choices = 0, chain_multi_service_choices = 0, chain_completed_choices = 0;
    MovementStats movement[3];  // idle, before pickup, after pickup
    long long expired_commitments = 0;
    long long oriented_builds = 0, oriented_guided = 0, oriented_fallback = 0;
    long long flow_freezes = 0, flow_penalized_edges = 0, flow_publications = 0, flow_cache_resets = 0, flow_cache_only_resets = 0;
    long long temporal_prepared_robots = 0, temporal_parallel_preparations = 0, temporal_conflict_audits = 0;
    long long temporal_next_known = 0, temporal_next_eligible = 0, temporal_next_unavailable = 0;
    long long temporal_next_arriving_choices = 0, temporal_next_changed_choices = 0;
    long long native_service_choices = 0, native_service_changed_choices = 0;
    long long temporal_transaction_calls = 0, temporal_transaction_roots = 0, temporal_transaction_candidates = 0;
    long long temporal_transaction_accepted = 0, temporal_transaction_rollbacks = 0, temporal_transaction_exhausted = 0;
    long long temporal_wait_seeds = 0, temporal_seed_rotations = 0, temporal_tied_seed_rotations = 0;
    long long temporal_planned_rotations = 0, temporal_protected_rotations = 0;
    long long temporal_warm_calls = 0, temporal_warm_retained = 0, temporal_warm_collision_resets = 0;
    long long temporal_rank_squared_robots = 0;
    long long temporal_promise_calls = 0, temporal_promise_retained = 0, temporal_promise_collision_resets = 0;
    long long temporal_warm_worker_runs = 0, temporal_cold_worker_runs = 0, temporal_selected_warm_runs = 0;
    long long guide_attempts = 0, guide_solved = 0, guide_robot_steps = 0, guide_expanded = 0, guide_reconnections = 0, guide_refinements = 0;
    long long txns = 0;
    long long txn_aborts = 0;
    long long txn_no_hole = 0;
    long long txn_moves = 0;
    long long lock_grants = 0;
    long long safety_waits = 0;
    long long assignments = 0;
    long long fair_assignments = 0;
    long long horizon_pairs = 0, horizon_impossible_pairs = 0, horizon_rank_changes = 0;
    long long horizon_assignments = 0, horizon_impossible_assignments = 0;
    long long horizon_first_rank_change = -1;
    long long horizon_margin_pairs = 0, horizon_margin_assignments = 0, horizon_margin_rank_changes = 0;
    long long horizon_margin_first_rank_change = -1;
    long long evacuations = 0;
    long long schedule_calls = 0, local_assignments = 0, fallback_assignments = 0;
    long long candidate_searches = 0, candidate_nodes = 0, candidate_task_limits = 0;
    long long candidate_node_limits = 0, candidate_deadlines = 0, empty_searches = 0;
    long long pickup_flow_searches = 0, pickup_flow_pops = 0, pickup_flow_states = 0;
    long long pickup_flow_cells = 0, pickup_flow_candidates = 0, pickup_flow_limits = 0;
    long long pickup_flow_cached_estimates = 0, pickup_flow_approximate_estimates = 0;
    long long pickup_flow_warmup_calls = 0, pickup_flow_snapshot_publication = 0;
    long long pickup_full_fields = 0, pickup_full_pops = 0, pickup_full_states = 0;
    long long pickup_full_searches = 0, pickup_full_scans = 0, pickup_full_candidates = 0;
    long long pickup_full_estimates = 0;
    long long chain_price_calls = 0, chain_price_observations = 0, chain_price_changed = 0;
    long long chain_price_assignments = 0, chain_price_assigned_covered = 0, chain_price_assigned_imputed = 0;
    std::array<long long, 4> chain_price_basis{};  // covered table/approx, uncovered table/approx
    long long chain_price_ratio_calls = 0;
    std::array<long long, 5> chain_price_outcomes{};
    long long chain_price_publication = 0, chain_price_numerator = 0, chain_price_denominator = 0;
    long long chain_shadow_queries = 0, chain_shadow_changed2 = 0, chain_shadow_changed3 = 0;
    long long chain_shadow_specific = 0, chain_shadow_small = 0;
    std::array<long long, 5> chain_shadow_choices{};  // same, mode2-only, mode3-only, both-same, both-different
    long long skipped_empty_searches = 0;
    long long sample_evaluations = 0, sample_deadlines = 0, improved_fallbacks = 0;
    long long global_evaluations = 0, global_assignments = 0;
    long long estimated_pickup_cost = 0, estimated_chain_cost = 0;
    long long route_queries = 0, route_manhattan = 0, progress_basis_resets = 0;
    long long reassign_passes = 0, reassign_eligible = 0, reassign_sources = 0, reassign_nodes = 0;
    long long reassign_pairs = 0, reassign_swaps = 0, reassign_saving = 0;
    long long reassign_table_pairs = 0, reassign_manhattan_pairs = 0;
    long long reassign_primary_protected = 0, reassign_recovery_protected = 0, reassign_fair_protected = 0;
    long long temporal_batch_passes = 0, temporal_batch_built = 0, temporal_batch_covered = 0;
    long long pool_passes = 0, pool_eligible = 0, pool_sources = 0, pool_nodes = 0, pool_pairs = 0;
    long long pool_exchanges = 0, pool_pickup_saving = 0, pool_chain_delta = 0, pool_total_saving = 0;
    long long pool_missing_pickup = 0, pool_missing_chain = 0, pool_short_pickup = 0;
    long long pool_primary_protected = 0, pool_recovery_protected = 0, pool_fair_protected = 0;
    long long match_passes = 0, match_eligible = 0, match_resident = 0, match_missing = 0, match_unreachable = 0;
    long long match_groups = 0, match_selected = 0, match_nodes = 0, match_matrix_entries = 0;
    long long match_cycles = 0, match_accepted_cycles = 0, match_moved = 0, match_saving = 0;
    long long match_anchors = 0, match_full_groups = 0;
    long long match_pickup_selected = 0;
    long long match_horizon_cycles = 0, match_horizon_rows = 0;
    long long match_horizon_rejected = 0, match_horizon_worse_rows = 0;
    long long match_primary_protected = 0, match_recovery_protected = 0, match_fair_protected = 0, match_budget_protected = 0;
};

// Diagnostic-only ledger. Accepted witnesses have disjoint task IDs across the
// entire run; these cost savings are not realized motion or throughput gains.
struct MatchBudgetShadowStats {
    Stats work;
    long long budget_cycles = 0, unprotected_cycles = 0, duplicate_cycles = 0;
    long long witness_cycles = 0, witness_rows = 0, witness_budget_rows = 0, witness_saving = 0;
    long long fully_protected_cycles = 0, fully_protected_rows = 0, fully_protected_saving = 0;
};

// Separate diagnostic ledger; none of these values governs real assignments.
struct FreshPickupShadowStats {
    long long passes = 0, eligible = 0, missing = 0, groups = 0, matrix_entries = 0;
    long long positive_cycles = 0, positive_saving = 0, accepted_cycles = 0, accepted_saving = 0;
    long long horizon_excluded_pairs = 0, guarded_cycles = 0, guarded_saving = 0;
    long long witness_cycles = 0, witness_rows = 0, witness_saving = 0, duplicate_cycles = 0;
    long long ordinary_witness_saving = 0, match_tick_witness_saving = 0, late_witness_saving = 0;
};

// Prospective duration-minus-bound means or exact empirical percentiles.
// Only a task whose first accepted
// holder never changes can train a bucket; dropped/retargeted tasks stay excluded.
class HorizonMargins {
public:
    struct Snapshot {
        std::array<long long, 5> count{}, excess{}, cutoff{};
        int percentile = 0;  // zero preserves the original rational mean
        static int bucket(long long bound) { return bound < 50 ? 0 : bound < 100 ? 1 : bound < 200 ? 2 : bound < 400 ? 3 : 4; }
        int tier(long long bound, long long remaining) const {
            if (bound > remaining) return 2;
            const int k = bucket(bound);
            if (percentile) return !count[k] || remaining - bound >= cutoff[k] ? 0 : 1;
            return !count[k] || static_cast<__int128>(remaining - bound) * count[k] >= excess[k] ? 0 : 1;
        }
    };
    void configure_percentile(int value) {
        if (value < 0 || value > 100) throw std::invalid_argument("horizon percentile must be in [0,100]");
        if (observed_tick_ >= 0 || !records_.empty()) throw std::logic_error("cannot change a trained horizon estimator");
        percentile_ = value;
    }
    Snapshot snapshot() const {
        Snapshot result = learned_;
        if (!percentile_) return result;
        result.percentile = percentile_;
        for (size_t k = 0; k < histogram_.size(); ++k) {
            if (!result.count[k]) continue;
            // Nearest-rank empirical quantile, with exact counts and no clipped
            // delays. A frozen five-value snapshot is shared by every pair in
            // this scheduling entry; comparisons never scan or mutate a histogram.
            const auto rank = static_cast<long long>((static_cast<__int128>(result.count[k]) * percentile_ + 99) / 100);
            long long cumulative = 0;
            bool found = false;
            for (const auto& entry : histogram_[k]) {
                cumulative += entry.second;
                if (cumulative >= rank) { result.cutoff[k] = entry.first; found = true; break; }
            }
            if (!found) throw std::logic_error("horizon percentile sample accounting mismatch");
        }
        return result;
    }
    long long invalidated = 0, excluded_completions = 0, bound_violations = 0;
    size_t tracked() const { return records_.size(); }

    void observe(const SharedEnvironment& env) {
        const int now = env.curr_timestep;
        if (now == observed_tick_) return;  // repeated observation cannot train twice
        if (now < observed_tick_) throw std::logic_error("horizon margin time moved backwards");
        const bool consecutive = observed_tick_ >= 0 && now == observed_tick_ + 1;
        for (auto it = records_.begin(); it != records_.end();) {
            auto& record = it->second;
            if (!consecutive && observed_tick_ >= 0) invalidate(record);
            const auto task = env.task_pool.find(it->first);
            if (task == env.task_pool.end()) {
                // TaskManager removes tasks only after completion, before the
                // next scheduling call. Require its free-holder/final-cell state
                // and a consecutive observation to bind the completion timestamp.
                const long long duration = static_cast<long long>(now) - record.admitted;
                if (record.single_holder && consecutive &&
                    env.curr_task_schedule.at(record.robot) == -1 &&
                    env.curr_states.at(record.robot).location == record.final_cell) {
                    if (duration < record.bound) {
                        ++bound_violations;
                        throw std::logic_error("completed horizon holder violated its physical bound");
                    }
                    const int k = Snapshot::bucket(record.bound);
                    ++learned_.count[k]; learned_.excess[k] += duration - record.bound;
                    if (percentile_) ++histogram_[k][duration - record.bound];
                } else ++excluded_completions;
                it = records_.erase(it);
            } else {
                if (task->second.agent_assigned != record.robot || env.curr_task_schedule.at(record.robot) != it->first)
                    invalidate(record);
                ++it;
            }
        }
        observed_tick_ = now;
    }

    template<class Bound>
    void proposed(const SharedEnvironment& env, const std::vector<int>& schedule, Bound bound) {
        for (auto& entry : records_)
            if (schedule.at(entry.second.robot) != entry.first) invalidate(entry.second);
        for (size_t robot = 0; robot < schedule.size(); ++robot) {
            const int id = schedule[robot];
            if (id < 0 || records_.count(id)) continue;
            const auto& task = env.task_pool.at(id);
            // Record the final proposal after all matching. An intermediate
            // proposal that never reaches TaskManager is not a holder change.
            const bool fresh = task.agent_assigned < 0 && task.idx_next_loc == 0 && !task.locations.empty();
            const long long value = fresh ? bound(static_cast<int>(robot), id) : -1;
            records_.emplace(id, Record{static_cast<int>(robot), env.curr_timestep,
                task.locations.empty() ? -1 : task.locations.back(), value, fresh && value >= 1});
        }
    }
private:
    struct Record { int robot, admitted, final_cell; long long bound; bool single_holder; };
    void invalidate(Record& record) { if (record.single_holder) { record.single_holder = false; ++invalidated; } }
    Snapshot learned_;
    int percentile_ = 0;
    std::array<std::map<long long, long long>, 5> histogram_;  // no nodes allocated in mean mode
    std::unordered_map<int, Record> records_;
    int observed_tick_ = -1;
};

class Cgar {
public:
    static Cgar& instance();

    void initialize(SharedEnvironment* env, int preprocess_ms);
    void schedule(SharedEnvironment* env, int time_limit_ms, std::vector<int>& proposed);
    void schedule(SharedEnvironment* env, std::chrono::steady_clock::time_point deadline, std::vector<int>& proposed);
    void plan(SharedEnvironment* env, int time_limit_ms, std::vector<Action>& actions);
    void plan(SharedEnvironment* env, std::chrono::steady_clock::time_point deadline, std::vector<Action>& actions);

    const GameFleetSelection& game_fleet_selection() const { return game_fleet_selection_; }
    int primary() const { return primary_; }
    int parked_count() const;
    bool active_certified() const { return active_certified_; }
    const Stats& stats() const { return stats_; }
    const HorizonMargins& horizon_margins() const { return horizon_margins_; }
    const MatchBudgetShadowStats& match_budget_shadow() const { return match_budget_shadow_; }
    const FreshPickupShadowStats& fresh_pickup_shadow() const { return fresh_pickup_shadow_; }
    // Destination proposals before the LoRR turn adapter; native-grid conformance only.
    const std::vector<int>& proposed_cells() const { return next_; }

private:
    using Clock = std::chrono::steady_clock;

    // step preparation
    void sync_agents();
    void update_locks();
    int select_primary();
    void compute_order(int primary);
    void refresh_orientation_cache();
    void prepare_temporal_tables(const std::vector<char>& pinned);
    void update_pibt_priorities();

    // routing
    int route_h(int i, int cell, ProgressBasis* basis = nullptr);
    bool allowed(int i, int cell) const;
    int turn_steps(int i, int cell) const;
    bool pibt(int i, int parent);
    PibtCandidates pibt_candidates(int i);
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
    void plan_temporal(std::vector<Action>& actions);

    int neighbor(int cell, int dir) const;
    bool adjacent_to_pocket(int cell, int pocket) const;
    int task_chain_cost(int task_id);
    struct UnopenedCandidates {
        std::vector<int> robots;
        long long primary = 0, recovery = 0, fair = 0, budget = 0;
    };
    void prune_reassignment_records();
    bool reassignment_budget_exhausted(int task) const;
    UnopenedCandidates unopened_candidates(const std::vector<int>& proposed, bool existing_only, bool include_budget = false) const;
    void audit_fresh_pickup(const std::vector<int>& proposed, const std::vector<int>& full_slots);
    void reassign_unopened(std::vector<int>& proposed);
    void exchange_unopened_with_pool(std::vector<int>& proposed);
    void match_unopened(std::vector<int>& proposed);
    void match_unopened_impl(std::vector<int>& proposed, bool shadow, Stats& observed);
    void log_summary();
    void record_movement(const std::vector<Action>& offered, const std::vector<Action>& actions,
                         const std::vector<char>& commitments);
    void log_movement() const;

    bool initialized_ = false;
    SharedEnvironment* env_ = nullptr;
    Certificate cert_;
    DistanceOracle oracle_;
    TurnDistanceOracle turn_oracle_;
    FlowGuidance flow_guidance_;
    bool static_trick_metric_ = false, native_trick_metric_ = false, native_neutral_tail_ = false;
    int guidance_cost_limit_ = 16;
    int flow_strength_ = 0, flow_cost_scale_ = 1;
    GuideRoutes guide_routes_;
    GuideRouteOptions guide_options_;
    bool guide_enabled_ = false;
    TemporalGeometry temporal_geometry_;
    TemporalWarmStart temporal_history_;
    bool temporal_warm_start_ = false, temporal_strict_wait_turns_ = false;
    bool temporal_promise_after_turn_ = false;
    bool temporal_mixed_start_ = false, temporal_next_errand_ = false;
    bool temporal_remaining_flow_ = false;
    ChainPotential chain_potential_;
    int temporal_chain_mode_ = 0, temporal_chain_mb_ = 512, temporal_chain_threads_ = 1;
    std::mt19937_64 temporal_rng_{0};
    TemporalPriorityPortfolio temporal_priority_portfolio_;
    int temporal_priority_noise_ = 0;
    bool temporal_ = false, temporal_equal_weight_ = false, temporal_rank_squared_ = false;
    int temporal_steps_ = 0, temporal_budget_ = 8192, temporal_order_ = 1, temporal_distance_scale_ = 50;
    int temporal_candidate_limit_ = 0, turn_cost_ = 1, turn_prefetch_threads_ = 0, temporal_conflict_audit_stride_ = 0;
    int temporal_group_snapshot_count_ = 0;
    int temporal_service_audit_stride_ = 0;
    int guidance_turn_cost_ = 1, turn_surcharge_ = 0;
    int temporal_workers_ = 1, temporal_threads_ = 1, temporal_prepare_threads_ = 1;
    bool temporal_regions_ = false;
    TemporalRegionOptions temporal_region_options_;
    TemporalTransactionOptions temporal_transaction_options_;
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
    bool pibt_reference_ = false;  // exact spatial PIBT policy, experimental
    bool pibt_tickets_ = false, pibt_commitments_ = false;
    bool pibt_priorities_ready_ = false;
    std::vector<int> pibt_elapsed_, pibt_initial_distance_, pibt_previous_goal_;
    std::vector<float> pibt_tie_;
    bool turn_first_ = false;
    int orientation_guidance_ = 0;  // 0 off, 1 LRU experiment, 2 demand-based admission
    std::unordered_set<int> oriented_goals_;
    int turn_table_budget_ = 0;
    bool diagnostics_ = false;
    bool enable_txn_ = true;
    bool enable_locks_ = true;
    bool hrrn_ = true;
    bool short_task_trick_ = false;
    int game_active_limit_ = 0;
    bool game_tabu_ = false, game_fleet_ready_ = false;
    GameFleetSelection game_fleet_selection_;
    int known_horizon_ = 0;
    bool known_horizon_passed_ = false;
    bool horizon_manhattan_ = false;
    bool horizon_margin_ = false;
    bool match_horizon_ = false;
    HorizonMargins horizon_margins_;
    void record_horizon_proposal(const std::vector<int>& proposed);
    long long geometric_task_bound(int robot, const Task& task) const;
    bool repair_fallback_ = true;
    bool refine_chain_costs_ = false;
    bool scheduler_cache_peek_ = false;
    bool stable_stall_basis_ = false;
    int fallback_samples_ = 64;
    int global_samples_ = 0;
    int pickup_weight_ = 1;
    bool pickup_flow_ = false;
    int chain_flow_pricing_ = 0;  // 0 native, 1 resident, 2 imputed, 3 ratio-only, 4 shadow
    int pickup_flow_nodes_ = 8192;
    OrientedPickupSearch pickup_search_;
    int pickup_full_robots_ = 0, pickup_full_threads_ = 4;
    bool pickup_full_cost_key_ = false;
    std::vector<CompletePickupSearch> pickup_full_workers_;
    std::vector<FullPickupField> pickup_full_fields_;
    int turn_build_limit_ = 32;
    int temporal_table_batch_ = 0, temporal_table_threads_ = 1;
    bool reassign_ = false, reassign_pool_ = false, reassign_match_ = false;
    int match_group_limit_ = 4;
    int match_task_budget_ = 1;
    std::unordered_map<int, int> match_task_moves_;
    int match_interval_ = 10;
    bool match_pickup_groups_ = false;
    int match_budget_audit_stride_ = 0;
    size_t match_budget_audit_cursor_ = 0;
    MatchBudgetShadowStats match_budget_shadow_;
    std::unordered_set<int> match_budget_audit_seen_tasks_;
    bool fresh_pickup_audit_ = false;
    FreshPickupShadowStats fresh_pickup_shadow_;
    std::unordered_set<int> fresh_pickup_seen_tasks_;
    int primary_ = -1;
    bool capacity_mode_ = false, parking_ready_ = false, active_certified_ = false;
    Clock::time_point deadline_, distance_deadline_;

    // scheduler state
    std::unordered_set<int> free_tasks_;
    std::unordered_map<int, int> chain_cost_;
    std::unordered_map<int, bool> chain_table_basis_;
    ChainCostCache refined_chain_cost_;
    long long regular_admissions_ = 0;
    size_t scheduler_cursor_ = 0, reassign_cursor_ = 0, pool_reassign_cursor_ = 0, match_cursor_ = 0;
    std::unordered_set<int> reassigned_tasks_, fair_tasks_;
    std::vector<int> last_reassignment_;
};

}  // namespace cgar
