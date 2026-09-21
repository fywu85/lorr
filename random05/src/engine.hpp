#pragma once
#include "SharedEnv.h"
#include "ActionModel.h"
#include <array>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <random>
#include <unordered_map>

namespace r05 {
struct MoveCandidate { int v,d;float score; };
struct CachedMove {
    float score=0;
    uint16_t v=0;
    uint8_t d=0, padding=0;
};
struct alignas(64) PreparedRanking {
    float base_cost=0;
    uint8_t idle_heading=0,count=0,kinematic_mask=0;
    std::array<CachedMove,5> candidates{};
    void load(std::array<MoveCandidate,5>& target) const {
        for(int k=0;k<count;++k)target[k]={int(candidates[k].v),int(candidates[k].d),candidates[k].score};
    }
};
static_assert(sizeof(PreparedRanking)==64,"unexpected prepared ranking size");
struct Config {
    int futures=16, first_futures=0, depth=8, threads=1, seed=0, expansion_limit=100000, generations=1, elites=1, persist_elites=1;
    int continuations=1, continuation_start=1, cache_slots=64, branch_diagnostics=0;
    int screen_branches=0, screen_keep=4;
    int component_trials=0, component_rounds=2, component_parents=8, component_min_agents=1;
    int replan_roots=0, replan_futures=1, replan_k=32, replan_steps=8, replan_continuations=4, replan_start=0;
    int replan_threads=1;
    bool replan_policy=false;
    int score_rank_steps=0;
    int window=0, window_keep=6, window_islands=32, window_iterations=24, window_neighborhood=8, window_expansions=20000;
    int window_starts=1, window_rounds=1, window_first_iterations=0, window_initial_steps=1, window_blocker_rotation=0;
    bool window_reuse=false, window_fast_groups=false, window_heap4=false, window_merge=false;
    float window_temperature=0;
    bool window_blockers=false, window_equal=false;
    int rescore_roots=0, rescore_branches=64;
    float rescore_blend=0, rescore_static_weight=-1;
    int snapshot_interval=0, snapshot_candidates=8;
    std::string snapshot_directory="snapshots";
    float future_mutation=0.3, future_elite_blend=0, continuation_risk=0;
    bool share_prefix=false, packed_order=false, fast_dispersion=false, scratch_reuse=false, profile=false, goal_cache=false, policy_profile=false, radix_order=false, candidate_cache=false, kinematic_mask=false, cycle_mask=false;
    bool fuse_cache_hits=false, lazy_cost_rows=false;
    int shared_rankings_mb=0;
    int restart_period=4;
    int blocker_mutation_size=0, blocker_mutation_period=2, blocker_mutation_edges=1;
    float elite_decision_distance=0, priority_remaining_weight=0;
    int priority_remaining_steps=0, early_root_period=0;
    float noise=50, mutation=0.3, mutation_decay=1, dispersion=0, push_price=0, loop_threshold=1;
    float move_bias=0, move_bias_fraction=0.25f;
    int move_bias_mode=0;
    float length_weight=0.25, keep_bonus=2, turn_cost=2, wait_cost=2, destination_load=0;
    float initial_length_weight=-1, match_horizon_weight=0;
    int initial_length_steps=250, active_task_cap=0, active_cap_steps=0;
    bool matching=true, loops=true, deadends=true, guided_matching=false, intent_rotation=true;
    int flow_seed=1, flow_iterations=20, flow_flips=0, flow_flip_seed=1;
    float flow_penalty=1.6, flow_output_penalty=-1, flow_normalize_ref=-1;
    float flow_turn=0, flow_power=1, flow_alpha=1, flow_betweenness=0, flow_confidence_power=0;
    bool flow_average=false, flow_normalize=false, flow_reverse=false;
    int loop_extent=2;
    bool predict_matching=false, rollout_age=false, rollout_match=false, cost_cache=false, pocket_components=false;
    int local_trials=0, horizon=0, hungarian_limit=0, mutation_radius=0;
    bool prospective_wait=false, chain_matching=false, random_by_step=false;
    int age_cap=0, pre_cycles=0, intent_mode=0;
    float pre_cycle_gain=0, idle_eviction=0;
    bool cycle_portfolio=false, early_fill=false;
    float early_fill_gain=0;
    int operation_depth=0, operation_revisits=4;
    bool operation_inherit=true, operation_moving=false, operation_protect=false, operation_finish_move=false;
    float operation_cost_weight=0;
    float progress_discount=1, flow_turn_load=0, plain_score=0, guidance_distance_mix=0, guidance_edge_mix=0, reverse_penalty=0, completion_bonus=0;
    float triage_scale=0.45, triage_guided_mix=0, waypoint_age_retain=0, score_rank_power=0;
    bool accept_equal=false;
    std::string guidance="none", weights;
    static Config environment(const SharedEnvironment& env);
};
double weighted_static_future_score(const std::vector<double>& scores,double static_weight);
std::vector<double> rank_progress_weights(const std::vector<float>& remaining,float power);
std::vector<int> priority_order(const std::vector<float>& priorities,bool packed,bool radix=false);
struct CycleWordMask { size_t word;uint64_t bits; };
struct Graph {
    int cells=0, states=0, rows=0, cols=0;
    std::vector<int> from_grid, to_grid, degree, pocket, pocket_depth;
    std::vector<std::array<int,4>> next;
    std::vector<std::array<float,5>> weight;
    std::vector<std::vector<int>> cycles, nearby;
    std::vector<std::vector<CycleWordMask>> cycle_masks;
    int all_nearby_pairs=0;
    std::vector<float> distance, any_heading_distance;
    std::vector<uint16_t> hops;
    Graph(const SharedEnvironment& env, const Config& cfg);
    float dist(int target, int source) const { return distance[size_t(target)*states+source]; }
    int hop(int target, int source) const { return hops[size_t(target)*cells+source]; }
    float approach(int target,int source) const;
    void blend_distances(const Graph& other,float fraction,int threads);
    int direction(int a,int b) const;
    int nearby_pairs(const std::vector<int>& locations) const;
};
struct Chain {
    std::vector<int> goals;
    std::vector<std::array<float,4>> tail;
    std::vector<std::vector<float>> values;
    mutable std::vector<PreparedRanking> rankings;
    Chain(const Graph& g, const Task& t,bool cache=false);
    const float* cached_row(const Graph& g,int stage) const;
    float cost(const Graph& g,int stage,int cell,int direction) const;
};
struct Frame {
    std::vector<int> loc, dir, pending, stage, age;
    std::vector<Action> last_actions;
    int reverse_turns=0;
    std::vector<int> operations;
    std::vector<const Chain*> active_chains, plain_chains;
    std::vector<unsigned char> free_tasks;
};
// Undirected interactions through the occupied cells on preferred goal routes.
std::vector<std::vector<int>> priority_dependencies(const Graph& graph,const Frame& frame,
    const std::vector<const Chain*>& assigned,int preferred_edges);
std::vector<int> dependency_neighborhood(const std::vector<std::vector<int>>& dependencies,
    int center,int limit);
struct PriorityChange { int agent;float offset; };
using Continuation = std::vector<std::vector<PriorityChange>>;
struct Rollout {
    double score=-1e100;
    Frame first;
    std::vector<Action> actions;
    std::vector<float> offsets;
    int moves=0;
    bool cycle_moves=true, early_moves=false, fully_evaluated=true;
    int evaluated_branches=1;
    uint64_t expansions=0;
};
std::vector<int> select_rollout_elites(const std::vector<Rollout>& results,int used,
                                      int best,int limit,bool accept_equal,float decision_distance=0);
// Connected by either proposed motion or a shared destination, including holes.
std::vector<std::vector<int>> decision_components(const Graph& graph,
                                                  const Rollout& left,const Rollout& right);
std::vector<std::vector<int>> window_conflict_components(const Graph& graph,
    const std::vector<std::vector<int>>& left,const std::vector<std::vector<int>>& right);
struct RolloutPrefix {
    Frame frame, first;
    std::vector<Action> actions;
    int time=0,completions=0;
    double initial=0,previous=0,progress=0,discounted=0,weight=1,weight_sum=0;
};
struct ScreenedRollout {
    Rollout result;
    RolloutPrefix prefix;
    double sum=0,mean=0,variance_sum=0;
    int count=0;
};
struct OperationModel {
    static constexpr int horizon=3, count=64, waiting=63;
    std::vector<std::array<int,horizon>> paths;
    std::vector<std::vector<std::vector<uint8_t>>> groups;
    explicit OperationModel(const Graph& graph);
    const std::array<int,horizon>& path(int state,int code) const {return paths[size_t(state)*count+code];}
};
struct alignas(64) CachedRanking {
    // The graph has at most4096 cells, so destination and heading fit exactly.
    // A hit, including all five candidates, occupies one64-byte cache line.
    uint32_t epoch=0,key=0;
    const Chain* chain=nullptr;
    float base_cost=0;
    uint8_t idle_heading=0,count=0,kinematic_mask=0;
    std::array<CachedMove,5> candidates{};
    void load(std::array<MoveCandidate,5>& target) const {
        for(int k=0;k<count;++k)target[k]={int(candidates[k].v),int(candidates[k].d),candidates[k].score};
    }
    void save(const std::array<MoveCandidate,5>& source) {
        for(int k=0;k<count;++k)candidates[k]={source[k].score,uint16_t(source[k].v),uint8_t(source[k].d),0};
    }
};
static_assert(sizeof(CachedMove)==8,"unexpected compact candidate layout");
static_assert(offsetof(CachedRanking,candidates)==24,"ranking header exceeds24 bytes");
static_assert(sizeof(CachedRanking)==64,"ranking cache must fit one line");
struct alignas(64) PolicyTiming {
    uint64_t calls=0,samples=0;
    std::array<uint64_t,7> nanoseconds{};
};
struct RescoreStats {
    int roots=0, branches=0, evaluations=0, selected_rank=0;
};
struct ReplanStats {
    int roots=0, futures=0, steps=0, decisions=0, selected_rank=0;
    int pool_before=0, min_pool_after=0, max_completed=0;
    uint64_t branch_evaluations=0;
};
class Engine {
public:
    Config cfg;
    std::shared_ptr<const Graph> graph;
    explicit Engine(Config config):cfg(std::move(config)){}
    void initialize(SharedEnvironment* env);
    void compute(SharedEnvironment* env, std::vector<Action>& plan, std::vector<int>& schedule,
                 int forced_candidate=-1);
    nlohmann::json checkpoint(const SharedEnvironment& env) const;
    void restore(const nlohmann::json& snapshot, SharedEnvironment& env);
    static void certify(const Graph& g,const std::vector<int>& from,const std::vector<int>& to);
    void match(SharedEnvironment* env,std::vector<int>& schedule);
    const std::vector<int>& ages() const { return age_; }
    int triaged() const { return triaged_; }
    const ReplanStats& replan_stats() const { return replan_stats_; }
    const RescoreStats& rescore_stats() const { return rescore_stats_; }
private:
    std::mt19937 rng_;
    bool quiet_=false;
    ReplanStats replan_stats_;
    RescoreStats rescore_stats_;
    int rescore_candidates(const SharedEnvironment& env,const Frame& frame,
                           const std::vector<Rollout>& results,int best);
    std::vector<std::unique_ptr<Engine>> replan_engines_;
    int rank_replanned(const SharedEnvironment& env,const std::vector<int>& schedule,
                       const std::vector<Rollout>& results,int best);
    void copy_replan_state(Engine& target,const Config& config) const;
    bool policy_profile_active_=false;
    float priority_remaining_scale_=0;
    mutable std::vector<PolicyTiming> policy_timings_;
    uint64_t ranking_epoch_=0;
    float shared_wait_cost_=-1;
    bool shared_intent_rotation_=false,shared_prospective_wait_=false;
    void prepare_shared_rankings(int timestep);
    mutable std::vector<std::vector<CachedRanking>> candidate_rankings_;
    uint64_t total_forward_=0,total_agent_steps_=0;
    int triaged_=0;
    std::unordered_map<int,std::shared_ptr<Chain>> chains_, score_chains_;
    std::unique_ptr<Graph> score_graph_;
    std::unique_ptr<OperationModel> operation_model_;
    std::vector<int> operations_;
    std::vector<std::vector<int>> window_paths_;
    void window_plan(const Frame& frame,const SharedEnvironment& env,std::vector<Action>& plan);
    std::vector<const Chain*> assigned_, score_assigned_, future_tasks_, future_plain_;
    std::vector<float> future_lengths_;
    std::vector<double> score_weights_;
    std::vector<int> age_, previous_task_, previous_stage_, pending_;
    std::vector<float> best_offsets_;
    std::vector<std::vector<float>> past_offsets_;
    std::vector<Action> last_actions_;
    std::vector<int> predicted_loc_, predicted_dir_;
    void match_future(Frame& frame) const;
    Rollout rollout(Frame frame,const std::vector<float>& offsets,bool cycle_moves=true,
                    const Continuation* continuation=nullptr,RolloutPrefix* save=nullptr,
                    const RolloutPrefix* resume=nullptr,const Rollout* forced_first=nullptr,
                    bool early_moves=false) const;
    Rollout evaluate(const Frame& frame,const std::vector<float>& offsets,
                     const std::vector<Continuation>& continuations,bool cycle_moves,
                     std::vector<double>* branch_scores=nullptr,const Rollout* forced_first=nullptr,
                     bool early_moves=false) const;
    void evaluate_until(const Frame& frame,const std::vector<float>& offsets,
                        const std::vector<Continuation>& continuations,bool cycle_moves,
                        int branches,ScreenedRollout& state,bool early_moves=false) const;
    void advance_operations(Frame& frame,const std::vector<float>& offsets,std::vector<Action>& actions,
                            uint64_t& expansions) const;
    void fill_ready_moves(const Frame& frame, const std::vector<float>& offsets, std::vector<int>& to) const;
    void advance(Frame& frame,const std::vector<float>& offsets,std::vector<Action>& actions,
                 uint64_t& expansions,bool cycle_moves,bool early_moves=false) const;
};
}
