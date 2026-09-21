#include "engine.hpp"
#include <sstream>
#include <stdexcept>

namespace r05 {
using json=nlohmann::json;
json Engine::checkpoint(const SharedEnvironment& env) const {
    json state={
        {"version",1},{"rows",env.rows},{"cols",env.cols},{"map",env.map},
        {"map_name",env.map_name},{"trick_instance",env.trick_instance},
        {"num_of_agents",env.num_of_agents},{"timestep",env.curr_timestep},
        {"schedule",env.curr_task_schedule},{"new_tasks",env.new_tasks},
        {"new_freeagents",env.new_freeagents},{"goal_locations",env.goal_locations},
        {"age",age_},{"previous_task",previous_task_},{"previous_stage",previous_stage_},
        {"pending",pending_},{"best_offsets",best_offsets_},{"past_offsets",past_offsets_},
        {"last_actions",last_actions_},{"predicted_loc",predicted_loc_},
        {"predicted_dir",predicted_dir_},{"operations",operations_},
        {"total_forward",total_forward_},{"total_agent_steps",total_agent_steps_},
        {"active_forward",active_forward_},{"active_agent_steps",active_agent_steps_}
    };
    if(cfg.window)state["window_paths"]=window_paths_;
    state["states"]=json::array();
    for(const auto& robot:env.curr_states)
        state["states"].push_back({robot.location,robot.timestep,robot.orientation});
    state["tasks"]=json::array();
    for(const auto& entry:env.task_pool) {
        const auto& t=entry.second;
        state["tasks"].push_back({{"id",t.task_id},{"locations",t.locations},
            {"stage",t.idx_next_loc},{"assigned",t.agent_assigned},
            {"revealed",t.t_revealed},{"completed",t.t_completed}});
    }
    std::sort(state["tasks"].begin(),state["tasks"].end(),[](const json& a,const json& b){
        return a["id"].get<int>()<b["id"].get<int>();
    });
    std::ostringstream random;random<<rng_;state["rng"]=random.str();
    return state;
}
void Engine::restore(const json& state,SharedEnvironment& env) {
    if(state.at("version")!=1)throw std::invalid_argument("unsupported checkpoint version");
    env.rows=state.at("rows");env.cols=state.at("cols");env.map=state.at("map").get<std::vector<int>>();
    env.map_name=state.at("map_name");env.trick_instance=state.at("trick_instance");
    env.num_of_agents=state.at("num_of_agents");env.curr_timestep=state.at("timestep");
    env.curr_task_schedule=state.at("schedule").get<std::vector<int>>();
    env.new_tasks=state.at("new_tasks").get<std::vector<int>>();
    env.new_freeagents=state.at("new_freeagents").get<std::vector<int>>();
    env.goal_locations=state.at("goal_locations").get<std::vector<std::vector<std::pair<int,int>>>>();
    env.curr_states.clear();
    for(const auto& robot:state.at("states"))env.curr_states.emplace_back(
        robot.at(0).get<int>(),robot.at(1).get<int>(),robot.at(2).get<int>());
    env.task_pool.clear();
    for(const auto& saved:state.at("tasks")) {
        Task t;t.task_id=saved.at("id");t.locations=saved.at("locations").get<std::vector<int>>();
        t.idx_next_loc=saved.at("stage");t.agent_assigned=saved.at("assigned");
        t.t_revealed=saved.at("revealed");t.t_completed=saved.at("completed");env.task_pool[t.task_id]=t;
    }
    if(!graph)initialize(&env);
    if(graph->rows!=env.rows || graph->cols!=env.cols)
        throw std::invalid_argument("checkpoint graph dimensions differ");
    for(size_t p=0;p<env.map.size();++p)if((graph->from_grid.at(p)<0)!=(env.map[p]!=0))
        throw std::invalid_argument("checkpoint obstacles differ");
    age_=state.at("age").get<std::vector<int>>();
    previous_task_=state.at("previous_task").get<std::vector<int>>();
    previous_stage_=state.at("previous_stage").get<std::vector<int>>();
    pending_=state.at("pending").get<std::vector<int>>();
    best_offsets_=state.at("best_offsets").get<std::vector<float>>();
    past_offsets_=state.at("past_offsets").get<std::vector<std::vector<float>>>();
    last_actions_=state.at("last_actions").get<std::vector<Action>>();
    predicted_loc_=state.at("predicted_loc").get<std::vector<int>>();
    predicted_dir_=state.at("predicted_dir").get<std::vector<int>>();
    operations_=state.at("operations").get<std::vector<int>>();
    window_paths_=state.value("window_paths",std::vector<std::vector<int>>{});
    total_forward_=state.at("total_forward");total_agent_steps_=state.at("total_agent_steps");
    active_forward_=state.value("active_forward",uint64_t(0));active_agent_steps_=state.value("active_agent_steps",uint64_t(0));
    std::istringstream random(state.at("rng").get<std::string>());random>>rng_;
    if(!random)throw std::invalid_argument("invalid checkpoint RNG state");
    // Cached pointers are implementation state, never part of a saved decision.
    // Invalidate before clearing shared tasks, even when restore is repeated.
    ++ranking_epoch_;chains_.clear();score_chains_.clear();
    assigned_.clear();score_assigned_.clear();future_tasks_.clear();future_plain_.clear();
    future_lengths_.clear();score_weights_.clear();triaged_=0;policy_profile_active_=false;
}
}
