#pragma once
#include "SharedEnv.h"
#include "ActionModel.h"
#include <array>
#include <cstdint>
#include <memory>
#include <random>
#include <unordered_map>

namespace r05 {
struct Config {
    int futures=16, depth=8, threads=1, seed=0, expansion_limit=100000;
    float noise=50, mutation=0.3, dispersion=0, push_price=0, loop_threshold=1;
    float length_weight=0.25, keep_bonus=2, turn_cost=2, wait_cost=2;
    bool matching=true, loops=true, deadends=true, guided_matching=false, intent_rotation=true;
    int flow_seed=1, flow_iterations=20;
    float flow_penalty=1.6;
    float flow_turn=0, flow_power=1, flow_alpha=1, flow_betweenness=0;
    bool flow_average=false, flow_normalize=false;
    int loop_extent=2;
    bool predict_matching=false, rollout_age=false, cost_cache=false, pocket_components=false;
    int local_trials=0, horizon=0, hungarian_limit=0;
    bool prospective_wait=false, chain_matching=false, random_by_step=false;
    int age_cap=0, pre_cycles=0;
    float pre_cycle_gain=0;
    float triage_scale=0.45;
    bool accept_equal=false;
    std::string guidance="none", weights;
    static Config environment(const SharedEnvironment& env);
};
struct Graph {
    int cells=0, states=0, rows=0, cols=0;
    std::vector<int> from_grid, to_grid, degree, pocket;
    std::vector<std::array<int,4>> next;
    std::vector<std::array<float,5>> weight;
    std::vector<std::vector<int>> cycles;
    std::vector<float> distance;
    std::vector<uint16_t> hops;
    Graph(const SharedEnvironment& env, const Config& cfg);
    float dist(int target, int source) const { return distance[size_t(target)*states+source]; }
    int hop(int target, int source) const { return hops[size_t(target)*cells+source]; }
    int direction(int a,int b) const;
};
struct Chain {
    std::vector<int> goals;
    std::vector<std::array<float,4>> tail;
    std::vector<std::vector<float>> values;
    Chain(const Graph& g, const Task& t,bool cache=false);
    float cost(const Graph& g,int stage,int cell,int direction) const;
};
struct Frame { std::vector<int> loc, dir, pending, stage, age; };
struct Rollout {
    double score=-1e100;
    Frame first;
    std::vector<Action> actions;
    std::vector<float> offsets;
    int moves=0;
    uint64_t expansions=0;
};
class Engine {
public:
    Config cfg;
    std::unique_ptr<Graph> graph;
    explicit Engine(Config config):cfg(std::move(config)){}
    void initialize(SharedEnvironment* env);
    void compute(SharedEnvironment* env, std::vector<Action>& plan, std::vector<int>& schedule);
    static void certify(const Graph& g,const std::vector<int>& from,const std::vector<int>& to);
    void match(SharedEnvironment* env,std::vector<int>& schedule);
    const std::vector<int>& ages() const { return age_; }
    int triaged() const { return triaged_; }
private:
    std::mt19937 rng_;
    uint64_t total_forward_=0,total_agent_steps_=0;
    int triaged_=0;
    std::unordered_map<int,std::shared_ptr<Chain>> chains_;
    std::vector<const Chain*> assigned_;
    std::vector<int> age_, previous_task_, previous_stage_, pending_;
    std::vector<float> best_offsets_;
    std::vector<int> predicted_loc_, predicted_dir_;
    Rollout rollout(Frame frame,const std::vector<float>& offsets) const;
    void advance(Frame& frame,const std::vector<float>& offsets,std::vector<Action>& actions,
                 uint64_t& expansions) const;
};
}
