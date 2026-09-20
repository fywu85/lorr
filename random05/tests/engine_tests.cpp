#include "engine.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
using namespace r05;
void require(bool c,const char* m){if(!c)throw std::runtime_error(m);}
SharedEnvironment environment(int rows,int cols,int agents) {
    SharedEnvironment e;e.rows=rows;e.cols=cols;e.map.assign(rows*cols,0);e.num_of_agents=agents;
    e.curr_task_schedule.assign(agents,-1);e.goal_locations.resize(agents);
    for(int a=0;a<agents;++a)e.curr_states.emplace_back(a,0,a%4);
    return e;
}
void validation() {
    auto e=environment(2,2,4);Config cfg;Graph g(e,cfg);
    Engine::certify(g,{0,1,3,2},{1,3,2,0});
    for(auto to:std::vector<std::vector<int>>{{1,0,3,2},{1,1,3,2},{3,1,2,0}}) {
        bool threw=false;try{Engine::certify(g,{0,1,3,2},to);}catch(const std::exception&){threw=true;}
        require(threw,"illegal transition accepted");
    }
    Task t;t.locations={1,3};Chain chain(g,t);
    require(chain.cost(g,0,0,0)==6,"orientation-aware chained distance is wrong");
    Chain cached(g,t,true);
    for(int k=0;k<=int(t.locations.size());++k)for(int v=0;v<g.cells;++v)for(int o=0;o<4;++o)
        require(cached.cost(g,k,v,o)==chain.cost(g,k,v,o),"cached cost differs from exact oriented chain cost");
}
void scheduling() {
    auto e=environment(3,3,2);Config cfg;Engine engine(cfg);engine.initialize(&e);
    Task t;t.task_id=7;t.locations={8,0};t.idx_next_loc=1;t.agent_assigned=0;e.task_pool[7]=t;
    t.task_id=8;t.locations={1,2};t.idx_next_loc=0;t.agent_assigned=1;e.task_pool[8]=t;
    e.curr_task_schedule={7,8};
    std::vector<int> assignment;engine.match(&e,assignment);
    require(assignment[0]==7,"started task reassigned");
    require(assignment[1]==8,"eligible task missing");
}
uint64_t simulation(int threads=2,bool step_rng=false,int age_cap=0,int pre_cycles=0,int generations=1,float plain_score=0,int intent_mode=0,float reverse_penalty=0,int mutation_radius=0,bool cost_cache=false) {
    auto e=environment(5,5,24);Config cfg;cfg.futures=4;cfg.depth=6;cfg.threads=threads;cfg.random_by_step=step_rng;cfg.age_cap=age_cap;cfg.pre_cycles=pre_cycles;cfg.generations=generations;cfg.plain_score=plain_score;cfg.intent_mode=intent_mode;cfg.reverse_penalty=reverse_penalty;cfg.mutation_radius=mutation_radius;cfg.cost_cache=cost_cache;
    uint64_t signature=14695981039346656037ULL;
    Engine engine(cfg);engine.initialize(&e);
    for(int a=0;a<24;++a) {Task t;t.task_id=a;t.locations={(a+7)%25,(a+17)%25};e.task_pool[a]=t;}
    int next_task=24,total_moved=0;
    for(int step=0;step<150;++step) {
        e.curr_timestep=step;std::vector<Action> actions;std::vector<int> assignment;
        engine.compute(&e,actions,assignment);
        for(int a=0;a<24;++a) {
            signature=(signature^uint64_t(actions[a]+1))*1099511628211ULL;
            signature=(signature^uint64_t(assignment[a]+1))*1099511628211ULL;
        }
        std::vector<int> from(24),to(24);
        for(int a=0;a<24;++a) {
            auto& s=e.curr_states[a];from[a]=s.location;to[a]=s.location;
            if(actions[a]==FW) {to[a]=engine.graph->next[s.location][s.orientation];++total_moved;}
            else if(actions[a]==CR)s.orientation=(s.orientation+1)%4;
            else if(actions[a]==CCR)s.orientation=(s.orientation+3)%4;
            s.location=to[a];s.timestep++;
            int old=e.curr_task_schedule[a];
            require(old<0 || e.task_pool.at(old).idx_next_loc==0 || old==assignment[a],"opened task changed");
        }
        Engine::certify(*engine.graph,from,to);
        auto sorted=assignment;std::sort(sorted.begin(),sorted.end());
        for(int a=1;a<24;++a)require(sorted[a]<0 || sorted[a]!=sorted[a-1],"duplicate task assignment");
        e.curr_task_schedule=assignment;
        for(int a=0;a<24;++a)if(assignment[a]>=0) {
            auto& task=e.task_pool.at(assignment[a]);task.agent_assigned=a;
            if(task.locations[task.idx_next_loc]==e.curr_states[a].location) {
                if(++task.idx_next_loc==int(task.locations.size())) {
                    e.task_pool.erase(assignment[a]);e.curr_task_schedule[a]=-1;
                    Task t;t.task_id=next_task++;t.locations={(step+a+9)%25,(step+2*a+3)%25};e.task_pool[t.task_id]=t;
                }
            }
        }
    }
    require(total_moved>150,"dense rollout is immobile");
    std::cout<<"dense simulation moves="<<total_moved<<"\n";
    return signature;
}
void triage_task_change() {
    auto e=environment(3,3,1);e.curr_states[0].location=4;
    Config cfg;cfg.futures=1;cfg.horizon=3;Engine engine(cfg);engine.initialize(&e);
    Task t;t.task_id=1;t.locations={8,0};t.idx_next_loc=1;e.task_pool[1]=t;e.curr_task_schedule={1};e.curr_timestep=2;
    std::vector<Action> plan;std::vector<int> schedule;engine.compute(&e,plan,schedule);
    require(engine.triaged()==1,"long task was not triaged");
    e.task_pool.clear();t.task_id=2;t.locations={4,5};t.idx_next_loc=0;e.task_pool[2]=t;e.curr_task_schedule={-1};
    engine.compute(&e,plan,schedule);
    require(engine.triaged()==0,"triage state leaked across task replacement");
}
void occupied_ring() {
    auto e=environment(3,3,8);e.map[4]=1;
    int ring[]={0,1,2,5,8,7,6,3};int dirs[]={0,0,1,1,2,2,3,3};
    for(int a=0;a<8;++a) {
        e.curr_states[a].location=ring[a];e.curr_states[a].orientation=dirs[a];
        Task t;t.task_id=a;t.locations={ring[(a+1)%8]};e.task_pool[a]=t;e.curr_task_schedule[a]=a;
    }
    Config cfg;cfg.futures=1;cfg.depth=3;cfg.matching=false;cfg.loop_extent=3;
    Engine engine(cfg);engine.initialize(&e);
    std::vector<Action> plan;std::vector<int> schedule;
    engine.compute(&e,plan,schedule);e.curr_task_schedule=schedule;e.curr_timestep=1;
    engine.compute(&e,plan,schedule);
    require(std::count(plan.begin(),plan.end(),FW)==8,"occupied 8-cycle failed to move simultaneously");
}
void exact_matching() {
    auto e=environment(1,5,2);e.curr_states[0].location=2;e.curr_states[1].location=0;
    Task t;t.task_id=10;t.locations={1};e.task_pool[10]=t;t.task_id=11;t.locations={4};e.task_pool[11]=t;
    Config cfg;cfg.hungarian_limit=2;Engine engine(cfg);engine.initialize(&e);
    std::vector<int> schedule;engine.match(&e,schedule);
    require(schedule[0]==11 && schedule[1]==10,"joint minimum-cost matching failed the greedy trap");
}
void idle_pocket_eviction() {
    auto e=environment(3,4,1);e.map[3]=e.map[11]=1;
    e.curr_states[0].location=7;e.curr_states[0].orientation=0;
    Config cfg;cfg.futures=1;cfg.wait_cost=0.5;cfg.idle_eviction=4;
    Engine engine(cfg);engine.initialize(&e);
    require(engine.graph->pocket_depth[engine.graph->from_grid[7]]==1,"tree pocket exit was not identified");
    for(int step=0;step<5;++step) {
        e.curr_timestep=step;std::vector<Action> actions;std::vector<int> schedule;
        engine.compute(&e,actions,schedule);e.curr_task_schedule=schedule;
        auto& state=e.curr_states[0];
        if(actions[0]==CR)state.orientation=(state.orientation+1)%4;
        else if(actions[0]==CCR)state.orientation=(state.orientation+3)%4;
        else if(actions[0]==FW)state.location=engine.graph->to_grid[
            engine.graph->next[engine.graph->from_grid[state.location]][state.orientation]];
    }
    require(e.curr_states[0].location!=7,"goal-less robot kept reserving the dead-end doorway");
}
void initial_task_length_preference() {
    auto e=environment(1,9,1);
    Task long_task;long_task.task_id=0;long_task.locations={0,8};e.task_pool[0]=long_task;
    Task short_task;short_task.task_id=1;short_task.locations={2,3};e.task_pool[1]=short_task;
    Config cfg;cfg.initial_length_weight=1;cfg.initial_length_steps=10;
    Engine engine(cfg);engine.initialize(&e);std::vector<int> schedule;
    engine.match(&e,schedule);
    require(schedule[0]==1,"initial preference failed to choose the shorter available task");
    e.curr_timestep=10;engine.match(&e,schedule);
    require(schedule[0]==0,"initial preference leaked past the configured phase");
    e.curr_task_schedule[0]=1;e.task_pool[1].idx_next_loc=1;
    engine.match(&e,schedule);
    require(schedule[0]==1,"phase change reassigned a started task");
}
void guidance_scale_reference() {
    auto e=environment(4,4,1);Config cfg;
    cfg.guidance="flow";cfg.flow_iterations=3;cfg.flow_average=true;
    cfg.flow_normalize=true;cfg.flow_betweenness=0.75;cfg.flow_output_penalty=2.4;
    Graph original(e,cfg);
    cfg.flow_normalize_ref=2.4;Graph reference(e,cfg);
    require(original.weight==reference.weight,"equal reference changed normalized guidance");
    cfg.flow_output_penalty=4.8;Graph stronger(e,cfg);bool changed=false;
    for(int v=0;v<original.cells;++v)for(int d=0;d<4;++d) {
        int u=original.next[v][d];if(u<0)continue;
        if(original.weight[v][d]<=original.weight[u][(d+2)%4])
            require(stronger.weight[v][d]==original.weight[v][d],"preferred-direction cost changed with fixed reference");
        else {require(stronger.weight[v][d]>original.weight[v][d],"opposing cost did not increase");changed=true;}
    }
    require(changed,"guidance test did not contain opposing traffic");
}
int main(){initial_task_length_preference();require(simulation(1,true)==simulation(1,true,0,0,1,0,0,0,0,true),"cached active rows changed the task-replacement trajectory");require(simulation(1,true,0,0,1,0,0,0,2)==simulation(2,true,0,0,1,0,0,0,2),"regional mutation changed with worker count");require(simulation(1,true,0,0,1,0,0,0.2)==simulation(2,true,0,0,1,0,0,0.2),"reverse-turn scoring changed with worker count");guidance_scale_reference();idle_pocket_eviction();validation();scheduling();simulation();simulation(2,true,100,1);simulation(2,true,100,2);simulation(2,true,100,3);require(simulation(1,true,100)==simulation(2,true,100),"worker count changed fixed-work trajectory");require(simulation(1,true,0,0,3)==simulation(2,true,0,0,3),"multi-generation worker count changed trajectory");require(simulation(1,true)==simulation(1,true,0,0,1,1),"plain evaluation changed an already unit-cost policy");require(simulation(1,true,0,0,1,0.5)==simulation(2,true,0,0,1,0.5),"blended score changed with worker count");require(simulation(1,true,0,0,1,0,1)==simulation(2,true,0,0,1,0,1),"component policy changed with worker count");require(simulation(1,true,0,0,1,0,2)==simulation(2,true,0,0,1,0,2),"pinned component policy changed with worker count");triage_task_change();occupied_ring();exact_matching();std::cout<<"All Random05 checks passed\n";}
