// Offline counterfactuals. The benchmark task stream stays in this executable;
// Engine sees only the ordinary revealed pool, exactly as in the simulator.
#include "engine.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stdexcept>
using namespace r05;
using json=nlohmann::json;
static json read(const std::string& file) {
    std::ifstream input(file);json result;input>>result;
    if(!input)throw std::runtime_error("cannot read "+file);return result;
}
static std::vector<std::vector<int>> tasks(const std::filesystem::path& path) {
    std::ifstream input(path);std::string line;std::getline(input,line);
    if(line.empty() || line[0]!='#')throw std::runtime_error("expected LoRR2024 task format");
    std::getline(input,line);int expected=std::stoi(line);std::vector<std::vector<int>> result;
    while(std::getline(input,line)) {
        if(line.empty())continue;
        std::stringstream fields(line);std::string item;std::vector<int> stops;
        while(std::getline(fields,item,','))stops.push_back(std::stoi(item));
        if(stops.empty())throw std::runtime_error("empty task");result.push_back(stops);
    }
    if(int(result.size())!=expected)throw std::runtime_error("task count mismatch");return result;
}
struct AdvanceResult { int completions=0,waypoints=0,forwards=0,rotations=0,waits=0; };
static AdvanceResult advance(Engine& engine,SharedEnvironment& env,
        const std::vector<Action>& actions,const std::vector<int>& schedule,
        const std::vector<std::vector<int>>& stream,int& next_task,int pool_size) {
    const auto& g=*engine.graph;int n=env.num_of_agents;AdvanceResult result;
    if(int(actions.size())!=n || int(schedule.size())!=n)throw std::runtime_error("short decision");
    std::set<int> chosen;std::vector<int> from(n),to(n);
    for(int a=0;a<n;++a) {
        int old=env.curr_task_schedule[a],id=schedule[a];
        if(old>=0 && env.task_pool.at(old).idx_next_loc>0 && old!=id)
            throw std::runtime_error("probe reassigned an opened task");
        if(id>=0 && (!env.task_pool.count(id) || !chosen.insert(id).second))
            throw std::runtime_error("probe invalid assignment");
        auto& state=env.curr_states[a];from[a]=g.from_grid.at(state.location);to[a]=from[a];
        switch(actions[a]) {
            case FW:to[a]=g.next.at(from[a]).at(state.orientation);++result.forwards;break;
            case CR:state.orientation=(state.orientation+1)%4;++result.rotations;break;
            case CCR:state.orientation=(state.orientation+3)%4;++result.rotations;break;
            case W:++result.waits;break;
            default:throw std::runtime_error("probe invalid action");
        }
    }
    Engine::certify(g,from,to);
    env.curr_task_schedule=schedule;env.new_freeagents.clear();env.new_tasks.clear();
    for(auto& item:env.task_pool)item.second.agent_assigned=-1;
    ++env.curr_timestep;
    for(int a=0;a<n;++a) {
        env.curr_states[a].location=g.to_grid.at(to[a]);env.curr_states[a].timestep=env.curr_timestep;
        int id=schedule[a];if(id<0)continue;
        auto& task=env.task_pool.at(id);task.agent_assigned=a;
        if(task.locations.at(task.idx_next_loc)==env.curr_states[a].location) {
            ++result.waypoints;
            if(++task.idx_next_loc==int(task.locations.size())) {
                ++result.completions;env.task_pool.erase(id);env.curr_task_schedule[a]=-1;
                env.new_freeagents.push_back(a);
            }
        }
    }
    while(int(env.task_pool.size())<pool_size) {
        Task task;task.task_id=next_task++;task.locations=stream.at(task.task_id%stream.size());
        task.t_revealed=env.curr_timestep;env.task_pool[task.task_id]=task;env.new_tasks.push_back(task.task_id);
    }
    return result;
}
int main(int argc,char** argv) {
    try {
        if(argc<6 || argc>7)throw std::invalid_argument(
            "usage: random05_decision_probe SNAPSHOT INPUT OUTPUT FUTURE_K STEPS [FUTURE_SEED]");
        auto snapshot=read(argv[1]),input=read(argv[2]);const auto& saved=snapshot.at("candidates");
        const int budget=std::stoi(argv[4]),steps=std::stoi(argv[5]),seed=argc==7?std::stoi(argv[6]):-1;
        if(budget<1 || steps<1 || steps>2000)throw std::invalid_argument("invalid probe budget or steps");
        SharedEnvironment env;env.trick_instance=snapshot.at("trick_instance");
        Config original=Config::environment(env);original.snapshot_interval=0;original.profile=false;original.policy_profile=false;
        Engine engine(original);engine.restore(snapshot,env);
        const auto stream=tasks(std::filesystem::path(argv[2]).parent_path()/input.at("taskFile").get<std::string>());
        const int pool_size=int(input.value("numTasksReveal",1.0)*env.num_of_agents);
        if(int(env.task_pool.size())!=pool_size)throw std::runtime_error("snapshot pool size mismatch");
        json output={{"snapshot",argv[1]},{"input",argv[2]},{"timestep",env.curr_timestep},
            {"future_k",budget},{"future_seed",seed},{"steps",steps},{"counterfactuals",json::array()},
            {"caveat","Reduced-budget downstream replanning, full task stream revealed only on completion. Diagnostic, not full-run throughput."}};
        for(const auto& candidate:saved) {
            engine.cfg=original;engine.restore(snapshot,env);int next_task=0;
            for(const auto& item:env.task_pool)next_task=std::max(next_task,item.first+1);
            std::vector<Action> plan;std::vector<int> schedule;json outcomes=json::array();
            int finished=0,waypoints=0,forwards=0,rotations=0,waits=0;
            for(int step=0;step<steps;++step) {
                engine.compute(&env,plan,schedule,step==0?candidate.at("candidate").get<int>():-1);
                if(step==0) {
                    auto after=engine.checkpoint(env);
                    if(plan!=candidate.at("actions").get<std::vector<Action>>() ||
                       schedule!=candidate.at("schedule").get<std::vector<int>>() ||
                       after.at("pending")!=candidate.at("pending"))
                        throw std::runtime_error("restored alternative does not reproduce its saved decision");
                    engine.cfg.futures=budget;engine.cfg.first_futures=0;
                    if(seed>=0)engine.cfg.seed=seed;
                }
                auto delta=advance(engine,env,plan,schedule,stream,next_task,pool_size);
                finished+=delta.completions;waypoints+=delta.waypoints;forwards+=delta.forwards;
                rotations+=delta.rotations;waits+=delta.waits;
                outcomes.push_back({{"step",step+1},{"finished",finished},{"waypoints",waypoints},
                    {"forwards",forwards},{"rotations",rotations},{"waits",waits}});
            }
            json record={{"candidate",candidate.at("candidate")},{"rank",candidate.at("rank")},
                {"score",candidate.at("score")},{"finished",finished},{"waypoints",waypoints},
                {"selected",candidate.at("candidate")==snapshot.at("selected_candidate")},
                {"outcomes",outcomes},{"reproduced_first_decision",true}};
            output["counterfactuals"].push_back(record);
            std::ofstream result(argv[3]);result<<output.dump(2)<<'\n';result.close();
            if(!result)throw std::runtime_error("probe output write failed");
            std::cout<<"snapshot="<<snapshot.at("timestep")<<" rank="<<candidate.at("rank")
                     <<" score="<<candidate.at("score")<<" completed="<<finished<<std::endl;
        }
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
