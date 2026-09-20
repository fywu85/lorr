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
void simulation() {
    auto e=environment(5,5,24);Config cfg;cfg.futures=4;cfg.depth=6;cfg.threads=2;
    Engine engine(cfg);engine.initialize(&e);
    for(int a=0;a<24;++a) {Task t;t.task_id=a;t.locations={(a+7)%25,(a+17)%25};e.task_pool[a]=t;}
    int next_task=24,total_moved=0;
    for(int step=0;step<150;++step) {
        e.curr_timestep=step;std::vector<Action> actions;std::vector<int> assignment;
        engine.compute(&e,actions,assignment);
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
int main(){validation();scheduling();simulation();triage_task_change();std::cout<<"All Random05 checks passed\n";}
