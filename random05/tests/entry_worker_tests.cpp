#include "Entry.h"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include <thread>
using namespace r05;
void require(bool yes,const char* message){if(!yes)throw std::runtime_error(message);}
void lifecycle() {
    PersistentWorker worker;std::thread::id identity;int observed=0;
    for(int i=1;i<=64;++i) {
        std::thread caller([&]{worker.run([&]{
            thread_local int count=0;observed=++count;
            if(i==1)identity=std::this_thread::get_id();
            require(identity==std::this_thread::get_id(),"worker identity changed");
        });});caller.join();require(observed==i,"thread-local state was discarded");
    }
    bool caught=false;
    try{worker.run([]{throw std::runtime_error("expected");});}
    catch(const std::runtime_error& e){caught=std::string(e.what())=="expected";}
    require(caught,"worker exception was not delivered synchronously");
    worker.run([&]{observed=99;});require(observed==99,"worker stopped after a failed request");
}
uint64_t simulate(bool persistent,int threads,bool window) {
    setenv("R05_PERSISTENT_WORKER",persistent?"1":"0",1);
    setenv("R05_THREADS",std::to_string(threads).c_str(),1);
    setenv("R05_K","8",1);setenv("R05_DEPTH","6",1);setenv("R05_RANDOM_BY_STEP","1",1);
    setenv("R05_HUNGARIAN","1000",1);setenv("R05_MATCH_SKIP_ZERO","1",1);setenv("R05_MATCH_SIMD","1",1);
    setenv("R05_SCRATCH_REUSE","1",1);setenv("R05_CANDIDATE_CACHE","1",1);setenv("R05_COST_CACHE","1",1);
    setenv("R05_WINDOW",window?"8":"0",1);setenv("R05_WINDOW_KEEP","4",1);
    setenv("R05_WINDOW_ITERS","32",1);setenv("R05_WINDOW_ISLANDS","4",1);
    setenv("R05_WINDOW_NEIGHBORHOOD","4",1);setenv("R05_WINDOW_REUSE","1",1);
    setenv("R05_WINDOW_HEAP4","1",1);
    Entry entry;auto& e=*entry.env;
    const int n=24,cells=25;e.rows=5;e.cols=5;e.num_of_agents=n;e.map.assign(cells,0);
    e.curr_task_schedule.assign(n,-1);e.goal_locations.resize(n);
    for(int a=0;a<n;++a)e.curr_states.emplace_back(a,0,a%4);
    // Initialize with no tasks, exactly as the competition driver does.
    e.plan_start_time=std::chrono::steady_clock::now();
    std::thread init([&]{entry.initialize(30000);});init.join();
    for(int a=0;a<n+12;++a) {Task t;t.task_id=a;t.locations={(a+7)%cells,(a+17)%cells};e.task_pool[a]=t;}
    int next_task=n+12,completed=0;uint64_t signature=14695981039346656037ULL;
    for(int step=0;step<100;++step) {
        e.curr_timestep=step;auto old_schedule=e.curr_task_schedule;
        std::vector<Action> actions;std::vector<int> schedule;
        e.plan_start_time=std::chrono::steady_clock::now();
        std::thread caller([&]{entry.compute(30000,actions,schedule);});caller.join();
        require(entry.last_compute_seconds()>=0,"entry elapsed time was not recorded");
        require(actions.size()==size_t(n) && schedule.size()==size_t(n),"entry returned an incomplete decision");
        std::vector<int> from(n),to(n);std::set<int> occupied,tasks;
        for(int a=0;a<n;++a) {
            signature=(signature^uint64_t(actions[a]+1))*1099511628211ULL;
            signature=(signature^uint64_t(schedule[a]+1))*1099511628211ULL;
            const int old=old_schedule[a];
            require(old<0 || e.task_pool.at(old).idx_next_loc==0 || old==schedule[a],"opened order changed");
            auto& state=e.curr_states[a];from[a]=state.location;to[a]=from[a];
            if(actions[a]==FW) {
                const int direction=state.orientation;
                const int dr[]={0,1,0,-1},dc[]={1,0,-1,0};
                const int r=from[a]/5+dr[direction],c=from[a]%5+dc[direction];
                require(r>=0 && r<5 && c>=0 && c<5,"move left the grid");to[a]=r*5+c;
            } else if(actions[a]==CR)state.orientation=(state.orientation+1)%4;
            else if(actions[a]==CCR)state.orientation=(state.orientation+3)%4;
            else require(actions[a]==W,"unknown action");
            require(occupied.insert(to[a]).second,"vertex collision");
            if(schedule[a]>=0)require(tasks.insert(schedule[a]).second,"duplicate assignment");
            state.location=to[a];++state.timestep;
        }
        for(int a=0;a<n;++a)for(int b=0;b<a;++b)
            require(from[a]!=to[b] || from[b]!=to[a],"edge collision");
        for(int a=0;a<n;++a)if(schedule[a]>=0) {
            auto& task=e.task_pool.at(schedule[a]);task.agent_assigned=a;
            if(task.locations[task.idx_next_loc]==e.curr_states[a].location &&
               ++task.idx_next_loc==int(task.locations.size())) {
                ++completed;e.task_pool.erase(schedule[a]);e.curr_task_schedule[a]=-1;
                Task replacement;replacement.task_id=next_task++;
                replacement.locations={(step+a+9)%cells,(step+2*a+3)%cells};e.task_pool[replacement.task_id]=replacement;
            }
        }
    }
    require(completed>0,"adapter fixture had no task turnover");return signature;
}
int main() {
    lifecycle();
    for(bool window:{false,true}) {
        const auto reference=simulate(false,1,window);
        for(int threads:{1,3})for(bool worker:{false,true})
            require(simulate(worker,threads,window)==reference,"persistent adapter changed actions or schedules");
    }
    for(const char* value:{"-1","2"}) {
        setenv("R05_PERSISTENT_WORKER",value,1);SharedEnvironment e;bool caught=false;
        try{Config::environment(e);}catch(const std::invalid_argument&){caught=true;}
        require(caught,"persistent worker accepted nonboolean option");
    }
    std::cout<<"Persistent worker lifecycle, exceptions, action replay and task turnover passed\n";
}
