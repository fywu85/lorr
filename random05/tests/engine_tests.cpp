#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
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
uint64_t simulate(Config cfg,int spare_tasks=0,int rows=5,int cols=5) {
    const int cells=rows*cols,n=cells-1;
    auto e=environment(rows,cols,n);
    uint64_t signature=14695981039346656037ULL;
    Engine engine(cfg);engine.initialize(&e);
    for(int a=0;a<n+spare_tasks;++a) {Task t;t.task_id=a;t.locations={(a+7)%cells,(a+17)%cells};e.task_pool[a]=t;}
    int next_task=n+spare_tasks,total_moved=0;
    for(int step=0;step<150;++step) {
        e.curr_timestep=step;std::vector<Action> actions;std::vector<int> assignment;
        engine.compute(&e,actions,assignment);
        for(int a=0;a<n;++a) {
            signature=(signature^uint64_t(actions[a]+1))*1099511628211ULL;
            signature=(signature^uint64_t(assignment[a]+1))*1099511628211ULL;
        }
        std::vector<int> from(n),to(n);
        for(int a=0;a<n;++a) {
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
        for(int a=1;a<n;++a)require(sorted[a]<0 || sorted[a]!=sorted[a-1],"duplicate task assignment");
        e.curr_task_schedule=assignment;
        for(int a=0;a<n;++a)if(assignment[a]>=0) {
            auto& task=e.task_pool.at(assignment[a]);task.agent_assigned=a;
            if(task.locations[task.idx_next_loc]==e.curr_states[a].location) {
                if(++task.idx_next_loc==int(task.locations.size())) {
                    e.task_pool.erase(assignment[a]);e.curr_task_schedule[a]=-1;
                    Task t;t.task_id=next_task++;t.locations={(step+a+9)%cells,(step+2*a+3)%cells};e.task_pool[t.task_id]=t;
                }
            }
        }
    }
    require(total_moved>150,"dense rollout is immobile");
    std::cout<<"dense simulation moves="<<total_moved<<"\n";
    return signature;
}
uint64_t simulation(int threads=2,bool step_rng=false,int age_cap=0,int pre_cycles=0,int generations=1,float plain_score=0,int intent_mode=0,float reverse_penalty=0,int mutation_radius=0,bool cost_cache=false,int operations=0,bool operation_inherit=true,int revisits=4,bool operation_moving=false,bool rollout_match=false,int spare_tasks=0,bool early_fill=false,int continuations=1,float future_mutation=0.3,int futures=4) {
    Config cfg;cfg.futures=futures;cfg.continuations=continuations;cfg.future_mutation=future_mutation;cfg.depth=6;cfg.threads=threads;cfg.random_by_step=step_rng;cfg.age_cap=age_cap;cfg.pre_cycles=pre_cycles;cfg.generations=generations;cfg.plain_score=plain_score;cfg.intent_mode=intent_mode;cfg.reverse_penalty=reverse_penalty;cfg.mutation_radius=mutation_radius;cfg.cost_cache=cost_cache;cfg.operation_depth=operations;cfg.operation_inherit=operation_inherit;cfg.operation_revisits=revisits;cfg.operation_moving=operation_moving;cfg.rollout_match=rollout_match;cfg.early_fill=early_fill;
    return simulate(cfg,spare_tasks);
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
    cfg.flow_output_penalty=2.4;cfg.flow_flips=2;cfg.flow_flip_seed=7;
    Graph flipped(e,cfg),repeat(e,cfg);int flips=0;
    require(flipped.next==original.next,"guidance mutation changed physical connectivity");
    require(flipped.weight==repeat.weight,"guidance mutation is not reproducible");
    for(int v=0;v<original.cells;++v) {
        require(flipped.weight[v][4]==original.weight[v][4],"guidance mutation changed turn costs");
        for(int d=0;d<4;++d) {
            int u=original.next[v][d];if(u<=v)continue;
            int back=(d+2)%4;
            if(flipped.weight[v][d]!=original.weight[v][d]) {
                require(flipped.weight[v][d]==original.weight[u][back] &&
                        flipped.weight[u][back]==original.weight[v][d],"guidance flip changed pair costs");
                ++flips;
            }
        }
    }
    require(flips==2,"guidance mutation count differs from requested count");
    cfg.flow_flips=0;cfg.flow_normalize=false;cfg.flow_confidence_power=0;
    Graph hard(e,cfg);cfg.flow_confidence_power=1;Graph soft(e,cfg);
    bool softened=false;
    for(int v=0;v<hard.cells;++v) {
        require(soft.weight[v][4]==hard.weight[v][4],"flow confidence changed turn costs");
        for(int d=0;d<4;++d) {
            int u=hard.next[v][d];if(u<0)continue;
            const float before=hard.weight[v][d],after=soft.weight[v][d];
            require(std::isfinite(after) && after>0,"flow confidence created an invalid edge cost");
            if(before<=hard.weight[u][(d+2)%4])
                require(after==before,"flow confidence changed a preferred edge");
            else {
                require(after>=soft.weight[u][(d+2)%4] && after<=before,
                        "flow confidence reversed a direction or increased its penalty");
                softened=softened || after<before;
            }
        }
    }
    require(softened,"confidence test did not soften any opposing edges");
}
void early_forward_fill() {
    auto e=environment(2,2,4);int cells[]={0,1,3,2},heading[]={0,1,2,3};
    for(int a=0;a<4;++a) {
        e.curr_states[a].location=cells[a];e.curr_states[a].orientation=heading[a];
        Task t;t.task_id=a;t.locations={cells[(a+1)%4]};e.task_pool[a]=t;e.curr_task_schedule[a]=a;
    }
    Config cfg;cfg.early_fill=true;cfg.futures=1;cfg.depth=1;cfg.matching=false;
    Engine engine(cfg);engine.initialize(&e);
    std::vector<Action> plan;std::vector<int> schedule;engine.compute(&e,plan,schedule);
    require(std::all_of(plan.begin(),plan.end(),[](Action a){return a==FW;}),
            "already aligned idle cycle waited for another pipeline step");
    Engine::certify(*engine.graph,{0,1,3,2},{1,3,2,0});
    auto blocked=environment(1,2,2);blocked.curr_states[1].orientation=2;
    for(int a=0;a<2;++a){Task t;t.task_id=a;t.locations={1-a};blocked.task_pool[a]=t;blocked.curr_task_schedule[a]=a;}
    Engine other(cfg);other.initialize(&blocked);other.compute(&blocked,plan,schedule);
    require(plan[0]==W && plan[1]==W,"early fill admitted a two-agent edge swap");
}

void completion_objective() {
    // Two robots compete for the same first destination. Only robot0 finishes
    // its chain there; robot1 would still owe another stop. A terminal reward
    // should select the finishing agent even with task matching disabled.
    auto e=environment(1,3,2);e.curr_states[1].location=2;e.curr_states[1].orientation=2;
    Task a;a.task_id=0;a.locations={1};e.task_pool[0]=a;
    Task b;b.task_id=1;b.locations={1,0};e.task_pool[1]=b;e.curr_task_schedule={0,1};
    Config cfg;cfg.futures=64;cfg.depth=2;cfg.matching=false;cfg.completion_bonus=20;
    Engine engine(cfg);engine.initialize(&e);
    std::vector<Action> plan;std::vector<int> schedule;
    engine.compute(&e,plan,schedule);
    for(int i=0;i<2;++i) {
        require(plan[i]!=FW,"first pipeline action moved before its promise");
        if(plan[i]==CR)e.curr_states[i].orientation=(e.curr_states[i].orientation+1)%4;
        if(plan[i]==CCR)e.curr_states[i].orientation=(e.curr_states[i].orientation+3)%4;
    }
    e.curr_timestep=1;engine.compute(&e,plan,schedule);
    require(plan[0]==FW && plan[1]!=FW,"completion reward did not select the finishing task");
    Engine::certify(*engine.graph,{0,2},{1,2});
}
void operation_geometry() {
    auto e=environment(3,3,1);e.map[4]=1;Config cfg;Graph g(e,cfg);OperationModel model(g);
    for(int state=0;state<g.states;++state)for(int code=0;code<OperationModel::count;++code) {
        const auto& path=model.path(state,code);if(path[0]<0)continue;
        const auto& suffix=model.path(path[0],(code>>2)|(int(W)<<4));
        require(suffix[0]==path[1] && suffix[1]==path[2] && suffix[2]==path[2],
                "operation inheritance does not preserve the valid suffix");
    }
}
void operation_dense_cycle() {
    auto e=environment(2,2,4);int cells[]={0,1,3,2},heading[]={0,1,2,3};
    for(int a=0;a<4;++a) {
        e.curr_states[a].location=cells[a];e.curr_states[a].orientation=(heading[a]+2)%4;
        Task t;t.task_id=a;t.locations={cells[(a+1)%4]};e.task_pool[a]=t;e.curr_task_schedule[a]=a;
    }
    Config cfg;cfg.operation_depth=3;cfg.futures=1;cfg.depth=3;cfg.matching=false;
    Engine engine(cfg);engine.initialize(&e);
    for(int t=0;t<3;++t) {
        e.curr_timestep=t;std::vector<Action> plan;std::vector<int> schedule,from(4),to(4);
        engine.compute(&e,plan,schedule);
        for(int a=0;a<4;++a) {
            auto& state=e.curr_states[a];from[a]=state.location;
            if(plan[a]==FW)state.location=engine.graph->next[state.location][state.orientation];
            else if(plan[a]==CR)state.orientation=(state.orientation+1)%4;
            else if(plan[a]==CCR)state.orientation=(state.orientation+3)%4;
            to[a]=state.location;
        }
        Engine::certify(*engine.graph,from,to);
    }
    for(int a=0;a<4;++a)require(e.curr_states[a].location==cells[(a+1)%4],
        "fully occupied cycle did not coordinate half-turns followed by movement");
}
void operation_swap_rejection() {
    auto e=environment(1,2,2);e.curr_states[0].orientation=0;e.curr_states[1].orientation=2;
    for(int a=0;a<2;++a){Task t;t.task_id=a;t.locations={1-a};e.task_pool[a]=t;e.curr_task_schedule[a]=a;}
    Config cfg;cfg.operation_depth=3;cfg.futures=2;cfg.depth=5;cfg.matching=false;
    Engine engine(cfg);engine.initialize(&e);
    for(int t=0;t<8;++t) {
        e.curr_timestep=t;std::vector<Action> plan;std::vector<int> schedule;
        engine.compute(&e,plan,schedule);
        for(int a=0;a<2;++a) {
            require(plan[a]==W,"immovable aligned agents attempted a swap or redundant rotations");
            auto& state=e.curr_states[a];
            if(plan[a]==CR)state.orientation=(state.orientation+1)%4;
            else if(plan[a]==CCR)state.orientation=(state.orientation+3)%4;
        }
    }
}
void exact_hot_paths() {
    std::mt19937 random(9354);
    for(int n:{0,1,2,24,800,8193})for(int trial=0;trial<30;++trial) {
        std::vector<float> priorities(n);
        for(float& p:priorities) {
            p=std::uniform_real_distribution<float>(-200,200)(random);
            if(trial%3==0)p=std::round(p); // many exact ties
            if(trial%3==1)p+=1000000; // rounded pocket-priority values
        }
        if(n>=2){priorities[0]=-0.0f;priorities[1]=0.0f;}
        require(priority_order(priorities,false)==priority_order(priorities,true),
                "packed priorities changed descending value/agent-ID order");
        require(priority_order(priorities,false)==priority_order(priorities,false,true),
                "radix priorities changed descending value/agent-ID order");
    }
    std::vector<float> limits={0,-0.0f,std::numeric_limits<float>::denorm_min(),
        -std::numeric_limits<float>::denorm_min(),1e20f,-1e20f,
        std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity()};
    require(priority_order(limits,false)==priority_order(limits,true),
            "packed priorities changed nonfinite fallback ordering");
    require(priority_order(limits,false)==priority_order(limits,true,true),
            "radix priorities changed nonfinite fallback ordering");
    limits.resize(limits.size()-2); // subnormal/extreme finite values take the radix path
    require(priority_order(limits,false)==priority_order(limits,true,true),
            "radix priorities changed extreme finite ordering");
    auto e=environment(7,9,0);
    for(int p:{0,4,8,12,21,32,37,46,60})e.map[p]=1;
    Config cfg;Graph graph(e,cfg);
    std::vector<int> vertices(graph.cells);std::iota(vertices.begin(),vertices.end(),0);
    for(int count=0;count<=graph.cells;++count) {
        std::shuffle(vertices.begin(),vertices.end(),random);
        std::vector<int> locations(vertices.begin(),vertices.begin()+count);
        int expected=0;
        for(int i=0;i<count;++i)for(int j=i+1;j<count;++j) {
            int a=graph.to_grid[locations[i]],b=graph.to_grid[locations[j]];
            expected+=std::abs(a%graph.cols-b%graph.cols)<=2 &&
                      std::abs(a/graph.cols-b/graph.cols)<=2;
        }
        require(graph.nearby_pairs(locations)==expected,
                "sparse/hole dispersion disagrees with geometric pair counting");
    }
    cfg.futures=16;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.random_by_step=true;cfg.cost_cache=true;cfg.dispersion=0.8;
    cfg.share_prefix=true;cfg.rollout_match=true;cfg.local_trials=4;
    const auto control=simulate(cfg,12);
    for(int flags=1;flags<=7;++flags) {
        cfg.packed_order=flags&1;cfg.fast_dispersion=flags&2;cfg.scratch_reuse=flags&4;
        require(control==simulate(cfg,12),"hot-path optimization changed dense task-turnover decisions");
    }
    cfg.radix_order=true;
    require(control==simulate(cfg,12),"radix sorting changed dense task-turnover decisions");
    cfg.threads=2;
    require(control==simulate(cfg,12),"hot-path optimization changed with worker count");
    for(int mode:{0,1,2})for(int cycles:{0,3}) {
        Config branch;branch.futures=4;branch.depth=6;branch.random_by_step=true;
        branch.intent_mode=mode;branch.pre_cycles=cycles;
        const auto reference=simulate(branch,12);
        branch.scratch_reuse=true;
        require(reference==simulate(branch,12),"scratch reuse leaked into intent/cycle policy branches");
    }
}

void elite_parents() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.share_prefix=true;cfg.random_by_step=true;cfg.cost_cache=true;
    cfg.goal_cache=true;cfg.candidate_cache=true;cfg.radix_order=true;cfg.scratch_reuse=true;
    cfg.dispersion=0.8;cfg.fast_dispersion=true;cfg.rollout_match=true;
    const auto single_generation=simulate(cfg,12);cfg.elites=4;
    require(single_generation==simulate(cfg,12),"elite setting changed a single-generation search");
    cfg.generations=4;
    for(int count:{1,2,4,8}) {
        cfg.elites=count;cfg.threads=1;
        const auto serial=simulate(cfg,12);cfg.threads=2;
        require(serial==simulate(cfg,12),"elite-parent search changed with worker count");
    }
    // Duplicated anchors and a degenerate mutation distribution must still
    // produce a nonempty parent set and a valid collision-free decision.
    cfg.noise=0;cfg.mutation=0;cfg.elites=8;cfg.threads=1;
    const auto duplicates=simulate(cfg,12);cfg.threads=2;
    require(duplicates==simulate(cfg,12),"duplicate elite handling changed with worker count");
}




void initial_search_budget() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=4;cfg.elites=4;cfg.persist_elites=4;cfg.share_prefix=true;
    cfg.random_by_step=true;cfg.cost_cache=true;cfg.candidate_cache=true;
    cfg.radix_order=true;cfg.scratch_reuse=true;cfg.rollout_match=true;
    const auto baseline=simulate(cfg,12);cfg.first_futures=cfg.futures;
    require(baseline==simulate(cfg,12),"equal first-step budget changed the trajectory");
    cfg.first_futures=32;cfg.threads=1;
    const auto serial=simulate(cfg,12);cfg.threads=2;
    require(serial==simulate(cfg,12),"changing declared work after startup broke determinism");
}

void annealed_mutation() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.share_prefix=true;cfg.random_by_step=true;cfg.cost_cache=true;
    cfg.candidate_cache=true;cfg.radix_order=true;cfg.scratch_reuse=true;
    cfg.dispersion=0.8;cfg.fast_dispersion=true;cfg.rollout_match=true;
    // The first generation has the original mutation rate at every real step.
    const auto single_generation=simulate(cfg,12);cfg.mutation_decay=0.25;
    require(single_generation==simulate(cfg,12),"mutation decay changed a single generation");
    cfg.generations=3;cfg.elites=4;cfg.persist_elites=4;
    for(float decay:{0.25f,0.5f}) {
        cfg.mutation_decay=decay;cfg.threads=1;
        const auto serial=simulate(cfg,12);cfg.threads=2;
        require(serial==simulate(cfg,12),"annealed search changed with worker count");
    }
}

void persistent_elites() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=4;cfg.elites=4;cfg.share_prefix=true;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.goal_cache=true;cfg.candidate_cache=true;
    cfg.radix_order=true;cfg.scratch_reuse=true;cfg.dispersion=0.8;cfg.fast_dispersion=true;
    // Task replacements and reassignment change the state underlying saved
    // vectors. Certifying each real action in simulate catches stale decisions;
    // both worker counts must select the same newly evaluated trajectory.
    for(int count:{2,8})for(bool future_tasks:{false,true}) {
        cfg.persist_elites=count;cfg.rollout_match=future_tasks;cfg.threads=1;
        const auto serial=simulate(cfg,12);cfg.threads=2;
        require(serial==simulate(cfg,12),"persistent elites changed with worker count or reused stale decisions");
    }
    cfg.persist_elites=16;cfg.noise=0;cfg.mutation=0;cfg.threads=1;
    const auto duplicates=simulate(cfg,12);cfg.threads=2;
    require(duplicates==simulate(cfg,12),"degenerate persistent elites changed with worker count");
}

void elite_continuations() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=4;cfg.elites=4;cfg.persist_elites=4;cfg.share_prefix=true;
    cfg.random_by_step=true;cfg.cost_cache=true;cfg.candidate_cache=true;
    cfg.radix_order=true;cfg.scratch_reuse=true;cfg.rollout_match=true;
    // With no future perturbations, retained-control blending cannot affect
    // actions, scores or subsequent random streams through dense task turnover.
    cfg.future_mutation=0;const auto unchanged=simulate(cfg,12);
    for(float blend:{0.5f,1.0f}) {
        cfg.future_elite_blend=blend;
        require(unchanged==simulate(cfg,12),"elite futures changed an unperturbed continuation");
    }
    cfg.future_mutation=0.3;
    for(float blend:{0.5f,1.0f})for(bool future_tasks:{false,true}) {
        cfg.future_elite_blend=blend;cfg.rollout_match=future_tasks;cfg.threads=1;
        const auto serial=simulate(cfg,12);cfg.threads=2;
        require(serial==simulate(cfg,12),"elite continuations changed with worker count or reused stale actions");
    }
}

void continuation_risk() {
    Config cfg;cfg.futures=16;cfg.continuations=4;cfg.future_mutation=0;cfg.depth=6;
    cfg.share_prefix=true;cfg.random_by_step=true;cfg.cost_cache=true;
    cfg.packed_order=true;cfg.fast_dispersion=true;cfg.dispersion=0.8;
    const auto identical=simulate(cfg,12);
    for(float coefficient:{-1.0f,1.0f}) {
        cfg.continuation_risk=coefficient;
        require(identical==simulate(cfg,12),"zero-variance continuations changed under signed coefficient");
    }
    cfg.future_mutation=0.3;
    for(float coefficient:{-0.5f,0.5f}) {
        cfg.continuation_risk=coefficient;cfg.threads=1;
        const auto serial=simulate(cfg,12);cfg.threads=2;
        require(serial==simulate(cfg,12),"signed continuation score changed with worker count");
    }
}

void cycle_word_masks() {
    auto e=environment(9,17,0);
    for(int cell:{3,21,35,64,70,89,121,144})e.map[cell]=1;
    Config geometry;geometry.cycle_mask=true;geometry.loop_extent=8;
    Graph graph(e,geometry);
    require(graph.cells>128 && graph.cycle_masks.size()==graph.cycles.size(),"missing multiword cycle geometry");
    for(size_t r=0;r<graph.cycles.size();++r) {
        std::vector<int> decoded;
        for(const auto& mask:graph.cycle_masks[r]) {
            uint64_t bits=mask.bits;
            while(bits){decoded.push_back(int(mask.word*64+__builtin_ctzll(bits)));bits&=bits-1;}
        }
        auto original=graph.cycles[r];std::sort(decoded.begin(),decoded.end());std::sort(original.begin(),original.end());
        require(decoded==original,"cycle mask omitted, duplicated or added a perimeter vertex");
    }
    for(int variant=0;variant<4;++variant) {
        Config cfg;cfg.guidance="lanes";cfg.futures=16;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
        cfg.cost_cache=true;cfg.goal_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;
        cfg.radix_order=true;cfg.rollout_match=true;cfg.random_by_step=true;
        cfg.candidate_cache=true;cfg.cache_slots=8;cfg.kinematic_mask=true;
        if(variant)cfg.loop_extent=3;
        if(variant==2)cfg.pre_cycles=3;
        if(variant==3){cfg.pre_cycles=2;cfg.intent_mode=2;}
        // Dense turnover spans word boundaries; larger and preselected rings
        // exercise overlap removal and robots with a forced future heading.
        const auto original=simulate(cfg,12,9,10);cfg.cycle_mask=true;
        require(original==simulate(cfg,12,9,10),"cycle mask changed dense movement or overlapping cycle order");
        cfg.threads=2;
        require(original==simulate(cfg,12,9,10),"cycle mask changed with worker count");
    }
}

void cached_kinematic_masks() {
    for(int variant=0;variant<6;++variant) {
        Config cfg;cfg.guidance="lanes";cfg.futures=16;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
        cfg.cost_cache=true;cfg.goal_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;
        cfg.radix_order=true;cfg.rollout_match=true;cfg.random_by_step=true;
        cfg.candidate_cache=variant!=0;cfg.cache_slots=8; // frequent evictions
        if(variant==1)cfg.prospective_wait=true;
        if(variant==2)cfg.intent_rotation=false;
        if(variant==3)cfg.push_price=2; // dynamic costs bypass ranking cache
        if(variant==4)cfg.expansion_limit=20; // preserve failure/fallback order
        if(variant==5){cfg.intent_mode=2;cfg.pre_cycles=3;}
        const auto original=simulate(cfg,12);cfg.kinematic_mask=true;
        require(original==simulate(cfg,12),"kinematic mask changed candidate order or collision resolution");
        cfg.threads=2;
        require(original==simulate(cfg,12),"kinematic mask changed across workers or task turnover");
    }
}

void cached_candidate_rankings() {
    for(int variant=0;variant<4;++variant) {
        Config cfg;cfg.guidance="lanes";cfg.futures=16;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
        cfg.cost_cache=true;cfg.goal_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;
        cfg.radix_order=true;cfg.fast_dispersion=true;cfg.dispersion=0.8;
        cfg.guided_matching=true;cfg.hungarian_limit=1000;cfg.rollout_match=true;cfg.random_by_step=true;
        if(variant==1)cfg.prospective_wait=true;
        if(variant==2)cfg.intent_rotation=false;
        if(variant==3)cfg.push_price=2; // neighbor-dependent costs must bypass the cache
        const auto reference=simulate(cfg,12);cfg.candidate_cache=true;
        require(reference==simulate(cfg,12),"ranking cache changed dense task turnover/policy choices");
        cfg.threads=2;
        require(reference==simulate(cfg,12),"ranking cache changed with worker count");
        if(variant==0)for(int slots:{8,128,256}) {
            // A tiny direct-mapped cache forces eviction; larger tables must
            // retain exactly the same rankings through chained task turnover.
            cfg.cache_slots=slots;
            require(reference==simulate(cfg,12),"ranking cache capacity changed the trajectory");
        }
    }
}

void shared_goal_costs() {
    auto e=environment(4,5,1);e.map[6]=1;
    Config cfg;cfg.guidance="flow";cfg.flow_iterations=3;cfg.turn_cost=0.6;
    Graph original(e,cfg);cfg.goal_cache=true;Graph cached(e,cfg);
    for(int goal=0;goal<original.cells;++goal)for(int state=0;state<original.states;++state)
        require(original.approach(goal,state)==cached.approach(goal,state),
                "shared goal row changed oriented approach cost");
    Task task;task.locations={4,16,2,19};Chain first(original,task,true),second(cached,task,true),uncached(cached,task);
    for(int stage=0;stage<=int(task.locations.size());++stage)
        for(int cell=0;cell<original.cells;++cell)for(int d=0;d<4;++d) {
            require(first.cost(original,stage,cell,d)==second.cost(cached,stage,cell,d),
                    "shared terminal row changed chained task cost");
            require(first.cost(original,stage,cell,d)==uncached.cost(cached,stage,cell,d),
                    "shared goal row changed uncached task cost");
        }
    cfg.guidance="lanes";cfg.futures=16;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.packed_order=true;
    cfg.fast_dispersion=true;cfg.dispersion=0.8;cfg.guided_matching=true;cfg.hungarian_limit=1000;
    cfg.rollout_match=true;cfg.random_by_step=true;cfg.goal_cache=false;
    const auto reference=simulate(cfg,12);cfg.goal_cache=true;
    require(reference==simulate(cfg,12),"shared goal cache changed dense matching/task turnover");
    cfg.threads=2;
    require(reference==simulate(cfg,12),"shared goal cache changed with worker count");
}

void branch_diagnostics() {
    Config cfg;cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.share_prefix=true;cfg.rollout_match=true;cfg.cost_cache=true;
    cfg.scratch_reuse=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;
    for(float risk:{0.0f,0.5f}) {
        cfg.continuation_risk=risk;cfg.branch_diagnostics=0;cfg.threads=1;
        const auto reference=simulate(cfg,12);
        cfg.branch_diagnostics=100;
        require(reference==simulate(cfg,12),"branch diagnostics changed decisions or random draws");
        cfg.threads=2;
        require(reference==simulate(cfg,12),"branch diagnostics changed with worker count");
    }
}

void staged_continuations() {
    for(bool shared:{false,true}) {
        Config cfg;cfg.futures=160;cfg.continuations=10;cfg.continuation_start=2;cfg.depth=6;
        cfg.generations=2;cfg.random_by_step=true;cfg.future_mutation=0;
        cfg.share_prefix=shared;cfg.rollout_match=true;cfg.cost_cache=true;
        cfg.scratch_reuse=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;
        cfg.accept_equal=true;cfg.first_futures=80;
        // Identical continuations make screening exact. Both searches generate
        // 16 roots (8 at the first step); only work on rejected roots differs.
        const auto exhaustive=simulate(cfg,12);
        cfg.screen_branches=2;cfg.screen_keep=4;cfg.futures=64;cfg.first_futures=32;
        require(exhaustive==simulate(cfg,12),"staged search lost an exact winner with identical futures");
        cfg.future_mutation=.3;cfg.elites=2;cfg.persist_elites=2;cfg.continuation_risk=.5;
        const auto serial=simulate(cfg,12);cfg.threads=2;
        require(serial==simulate(cfg,12),"staged search changed across workers or reused stale task state");
    }
}

int main() {
    staged_continuations();
    branch_diagnostics();
    cycle_word_masks();
    cached_kinematic_masks();
    elite_continuations();
    initial_search_budget();
    annealed_mutation();
    persistent_elites();
    elite_parents();
    cached_candidate_rankings();
    shared_goal_costs();
    Config measured;measured.futures=4;measured.depth=6;measured.random_by_step=true;
    const auto unmeasured=simulate(measured,12);measured.profile=true;measured.policy_profile=true;
    require(unmeasured==simulate(measured,12),"phase profiling changed the trajectory");
    continuation_risk();
    exact_hot_paths();
    for(int prefix:{1,2}) {
        Config cfg;cfg.futures=16;cfg.continuations=4;cfg.continuation_start=prefix;cfg.depth=6;
        cfg.random_by_step=true;cfg.cost_cache=true;cfg.rollout_match=true;
        cfg.completion_bonus=8;cfg.progress_discount=0.9;cfg.reverse_penalty=0.1;
        cfg.dispersion=0.8;cfg.local_trials=4;cfg.threads=1;
        const auto control=simulate(cfg,12);
        cfg.share_prefix=true;
        require(control==simulate(cfg,12),"shared prefix changed predicted task-turnover/scoring decisions");
        cfg.threads=2;
        require(control==simulate(cfg,12),"shared prefix changed with worker count");
    }
    for(int flags=1;flags<=3;++flags) {
        Config cfg;cfg.futures=4;cfg.depth=6;cfg.random_by_step=true;cfg.cost_cache=true;
        cfg.operation_depth=3;cfg.operation_protect=flags&1;cfg.operation_finish_move=flags&2;
        cfg.threads=1;const auto reference=simulate(cfg,12);
        cfg.threads=2;require(reference==simulate(cfg,12),
                             "operation protection/terminal-turn policy changed with worker count");
    }
    Config moving;moving.futures=4;moving.depth=6;moving.operation_depth=3;
    moving.operation_protect=true;moving.operation_finish_move=true;moving.operation_moving=true;
    simulate(moving,12);
    // Four repeated identical branches must preserve the trajectory of the
    // same root portfolio; only their work count changes.
    require(simulation(1,true,0,0,1,0,0,0,0,true,0,true,4,false,false,12,false,4,0,16)==
            simulation(1,true,0,0,1,0,0,0,0,true,0,true,4,false,false,12,false,1,0,4),
            "identical continuations changed root selection");
    require(simulation(1,true,0,0,1,0,0,0,0,true,0,true,4,false,false,12,false,4,0.3,16)==
            simulation(2,true,0,0,1,0,0,0,0,true,0,true,4,false,false,12,false,4,0.3,16),
            "continuation averaging changed with worker count");
    early_forward_fill();
    require(simulation(1,true,0,0,1,0,0,0,0,true,0,true,4,false,false,12,true)==
            simulation(2,true,0,0,1,0,0,0,0,true,0,true,4,false,false,12,true),
            "early-fill dense task turnover changed with worker count");
    require(simulation(1,true)==simulation(1,true,0,0,1,0,0,0,0,false,0,true,4,false,true),
            "virtual matching changed a rollout without spare tasks");
    require(simulation(1,true,0,0,1,0,0,0,0,true,0,true,4,false,true,12)==
            simulation(2,true,0,0,1,0,0,0,0,true,0,true,4,false,true,12),
            "virtual task turnover changed with worker count");
    simulation(2,true,0,0,1,0,0,0,0,true,3,true,4,true);
    completion_objective();
    operation_geometry();
    operation_dense_cycle();
    operation_swap_rejection();
    require(simulation(1,true,0,0,1,0,0,0,0,true,3)==simulation(2,true,0,0,1,0,0,0,0,true,3),
            "operation policy changed with worker count");
    simulation(2,true,0,0,1,0,0,0,0,true,3,false);
    // A one-visit ablation stalled; mobility is required of the selected four-visit policy.
    initial_task_length_preference();
    require(simulation(1,true)==simulation(1,true,0,0,1,0,0,0,0,true),"cached active rows changed the task-replacement trajectory");
    require(simulation(1,true,0,0,1,0,0,0,2)==simulation(2,true,0,0,1,0,0,0,2),"regional mutation changed with worker count");
    require(simulation(1,true,0,0,1,0,0,0.2)==simulation(2,true,0,0,1,0,0,0.2),"reverse-turn scoring changed with worker count");
    guidance_scale_reference();
    idle_pocket_eviction();
    validation();
    scheduling();
    simulation();
    simulation(2,true,100,1);
    simulation(2,true,100,2);
    simulation(2,true,100,3);
    require(simulation(1,true,100)==simulation(2,true,100),"worker count changed fixed-work trajectory");
    require(simulation(1,true,0,0,3)==simulation(2,true,0,0,3),"multi-generation worker count changed trajectory");
    require(simulation(1,true)==simulation(1,true,0,0,1,1),"plain evaluation changed an already unit-cost policy");
    require(simulation(1,true,0,0,1,0.5)==simulation(2,true,0,0,1,0.5),"blended score changed with worker count");
    require(simulation(1,true,0,0,1,0,1)==simulation(2,true,0,0,1,0,1),"component policy changed with worker count");
    require(simulation(1,true,0,0,1,0,2)==simulation(2,true,0,0,1,0,2),"pinned component policy changed with worker count");
    triage_task_change();
    occupied_ring();
    exact_matching();
    std::cout<<"All Random05 checks passed\n";
}
