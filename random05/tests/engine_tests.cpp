#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
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
void local_goal_guidance() {
    auto env=environment(5,5,1);Config cfg;cfg.guidance="lanes";cfg.goal_cache=true;
    cfg.goal_local_radius=2;cfg.goal_local_mix=1;Graph g(env,cfg);
    bool softened=false,unchanged=false;
    for(int goal=0;goal<g.cells;++goal)for(int cell=0;cell<g.cells;++cell)
        for(int direction=0;direction<4;++direction)if(g.next[cell][direction]>=0) {
            float w=g.forward_weight(goal,cell,direction);
            require(w>0,"local guidance made a nonpositive edge");
            if(g.hop(goal,cell)<=1) {
                require(w==2,"immediate goal approach did not reach physical edge cost");
                softened=softened || g.weight[cell][direction]!=2;
            }
            if(g.hop(goal,cell)>cfg.goal_local_radius) {
                require(w==g.weight[cell][direction],"local guidance changed a distant edge");unchanged=true;
            }
        }
    require(softened && unchanged,"local guidance fixture did not cover both regions");
    // Verify shortest-path Bellman equations independently over every target
    // heading. Chained tails and policy lookups must share these edge prices.
    for(int target=0;target<g.states;++target)for(int source=0;source<g.states;++source) {
        if(target==source){require(g.dist(target,source)==0,"goal cost is not zero");continue;}
        int cell=source/4,dir=source%4,v=g.next[cell][dir];
        float best=std::min(g.dist(target,cell*4+(dir+1)%4),g.dist(target,cell*4+(dir+3)%4))+g.weight[cell][4];
        if(v>=0)best=std::min(best,g.dist(target,v*4+dir)+g.forward_weight(target/4,cell,dir));
        require(std::abs(g.dist(target,source)-best)<1e-4,"goal-local distance and move prices disagree");
    }
    Task task;task.locations={24,2,20};Chain ordinary(g,task),cached(g,task,true);
    for(int stage=0;stage<3;++stage)for(int cell=0;cell<g.cells;++cell)for(int dir=0;dir<4;++dir)
        require(ordinary.cost(g,stage,cell,dir)==cached.cost(g,stage,cell,dir),"goal-local chained cache mismatch");
    struct Setting {
        const char* key;bool present;std::string old;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);setenv(k,v,1);
        }
        ~Setting(){if(present)setenv(key,old.c_str(),1);else unsetenv(key);}
    };
    Setting radius("R05_GOAL_LOCAL_RADIUS","2"),mix("R05_GOAL_LOCAL_MIX",".5");
    bool rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"goal-local guidance escaped the explicit trick gate");
    env.trick_instance="RANDOM-04";
    require(Config::environment(env).goal_local_mix==.5f,"valid goal-local guidance was rejected");
    setenv("R05_GOAL_LOCAL_RADIUS","0",1);rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"goal-local guidance accepted zero radius");
    setenv("R05_GOAL_LOCAL_RADIUS","2",1);setenv("R05_GOAL_LOCAL_MIX","1.5",1);rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"goal-local guidance accepted an extrapolated mixture");
    setenv("R05_GOAL_LOCAL_MIX",".5",1);Setting window("R05_WINDOW","8");
    require(Config::environment(env).window==8,"consistent window cost model rejected goal-local guidance");
    env.trick_instance.clear();rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"window goal-local guidance escaped the explicit trick gate");

}

void idle_displacement_price() {
    // An opened order needs to pass an idle robot. Charging that idle robot's
    // nonexistent goal loss makes the parent wait; the free-idle variant yields.
    for(bool free_idle:{false,true}) {
        auto env=environment(3,3,2);env.curr_states[0].location=3;env.curr_states[1].location=4;
        env.curr_states[0].orientation=env.curr_states[1].orientation=0;
        Task task;task.task_id=1;task.locations={5,5};task.idx_next_loc=1;task.agent_assigned=0;
        env.task_pool[1]=task;env.curr_task_schedule={1,-1};
        Config cfg;cfg.futures=1;cfg.depth=1;cfg.loops=false;cfg.wait_cost=.5;
        cfg.push_price=2;cfg.fast_push=true;cfg.push_idle_free=free_idle;
        Engine engine(cfg);engine.initialize(&env);std::vector<Action> actions;std::vector<int> schedule;
        engine.compute(&env,actions,schedule);
        const auto pending=engine.checkpoint(env).at("pending").get<std::vector<int>>();
        require(schedule[0]==1,"displacement policy reassigned an opened order");
        require(pending[0]==(free_idle?4:3),"idle displacement pricing ignored the declared goal loss");
    }
}

void arrival_priority_semantics() {
    auto env=environment(3,3,1);Config cfg;Graph g(env,cfg);
    require(arrival_priority_bonus(g,4,0,false,5,50)==50,"facing immediate goal lost its bonus");
    require(arrival_priority_bonus(g,4,0,false,1,50)==50,"one-turn arrival lost its bonus");
    require(arrival_priority_bonus(g,4,0,true,1,50)==0,"moving robot received an impossible turn bonus");
    require(arrival_priority_bonus(g,4,0,false,3,50)==0,"half-turn arrival received a bonus");
    require(arrival_priority_bonus(g,4,0,false,8,50)==0,"distant waypoint received immediate-arrival priority");
    require(arrival_priority_bonus(g,4,0,false,4,50)==50,"repeated current waypoint lost its wait opportunity");
    struct Setting {
        const char* key;bool present;std::string old;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);setenv(k,v,1);
        }
        ~Setting(){if(present)setenv(key,old.c_str(),1);else unsetenv(key);}
    };
    Setting option("R05_ARRIVAL_PRIORITY","50");bool rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"arrival priority escaped its explicit trick gate");
    env.trick_instance="RANDOM-04";require(Config::environment(env).arrival_priority==50,"valid arrival priority rejected");
    Setting period("R05_ARRIVAL_ROOT_PERIOD","2");
    require(Config::environment(env).arrival_root_period==2,"arrival portfolio period lost");
    {Setting bonus("R05_ARRIVAL_PRIORITY","0");rejected=false;
     try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
     require(rejected,"arrival portfolio accepted a missing bonus");}
    {Setting components("R05_COMPONENT_TRIALS","4");rejected=false;
     try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
     require(rejected,"arrival portfolio accepted unsupported component search");}

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
uint64_t simulate(Config cfg,int spare_tasks=0,int rows=5,int cols=5,bool check_restore=false,bool replan_control=false) {
    const int cells=rows*cols,n=cells-1;
    auto e=environment(rows,cols,n);
    uint64_t signature=14695981039346656037ULL;
    Engine engine(cfg);engine.initialize(&e);
    std::unique_ptr<Engine> control;
    if(replan_control) {auto base=cfg;base.replan_roots=0;base.rescore_roots=0;control=std::make_unique<Engine>(base);control->initialize(&e);}
    for(int a=0;a<n+spare_tasks;++a) {Task t;t.task_id=a;t.locations={(a+7)%cells,(a+17)%cells};e.task_pool[a]=t;}
    int next_task=n+spare_tasks,total_moved=0;
    for(int step=0;step<150;++step) {
        e.curr_timestep=step;std::vector<Action> actions;std::vector<int> assignment;
        const auto before=check_restore && step%17==0?engine.checkpoint(e):nlohmann::json();
        engine.compute(&e,actions,assignment);
        if(cfg.rescore_roots) {
            const auto stats=engine.rescore_stats();
            require(stats.roots>0 && stats.roots<=cfg.rescore_roots &&
                    stats.branches==cfg.rescore_branches && stats.evaluations==stats.roots*stats.branches,
                    "independent candidate rescoring skipped declared work");
        }
        if(control) {
            std::vector<Action> base_actions;std::vector<int> base_schedule;
            control->compute(&e,base_actions,base_schedule);
            require(actions==base_actions && assignment==base_schedule,"single-root forecast changed the decision");
            require(engine.checkpoint(e)==control->checkpoint(e),"forecast mutated persistent solver state");
        }
        if(!before.is_null()) {
            const auto after=engine.checkpoint(e);
            engine.restore(before,e);std::vector<Action> replay_actions;std::vector<int> replay_schedule;
            engine.compute(&e,replay_actions,replay_schedule);
            require(actions==replay_actions && assignment==replay_schedule,
                    "checkpoint restore changed the selected decision");
            require(after==engine.checkpoint(e),"checkpoint restore changed persistent solver state");
            if(cfg.priority_remaining_steps>0) {
                auto reference_cfg=cfg;reference_cfg.priority_remaining_steps=0;
                if(step>=cfg.priority_remaining_steps)reference_cfg.priority_remaining_weight=0;
                auto reference_env=e;Engine reference(reference_cfg);reference.initialize(&reference_env);reference.restore(before,reference_env);
                std::vector<Action> reference_actions;std::vector<int> reference_schedule;
                reference.compute(&reference_env,reference_actions,reference_schedule);
                require(actions==reference_actions && assignment==reference_schedule,
                        "startup priority did not match its active or expired policy");
                require(after==reference.checkpoint(reference_env),"startup priority changed unrelated persistent state");
            }
            if(cfg.score_rank_steps>0) {
                // At the same saved state, a startup-weighted decision must
                // equal the unlimited weighted solver before the boundary and
                // the ordinary solver at/after it. This also checks stale weights.
                auto reference_cfg=cfg;reference_cfg.score_rank_steps=0;
                if(step>=cfg.score_rank_steps)reference_cfg.score_rank_power=0;
                auto reference_env=e;Engine reference(reference_cfg);
                reference.initialize(&reference_env);reference.restore(before,reference_env);
                std::vector<Action> reference_actions;std::vector<int> reference_schedule;
                reference.compute(&reference_env,reference_actions,reference_schedule);
                require(actions==reference_actions && assignment==reference_schedule,
                        "startup score window disagrees with the corresponding full policy");
                require(after==reference.checkpoint(reference_env),
                        "startup score window changed unrelated persistent state");
            }
        }
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
    if(total_moved<=150)std::cerr<<"immobile: moves="<<total_moved<<" window="<<cfg.window
        <<" starts="<<cfg.window_starts<<" iterations="<<cfg.window_iterations
        <<" expansions="<<cfg.window_expansions<<"\n";
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
void nonlinear_matching() {
    // Linear matching retains two moderate trips because of a small keep bonus.
    // Concavity instead gives one robot an immediate pickup and the other the
    // longer trip. Both choices preserve exactly one assignment per task.
    auto env=environment(1,20,2);env.curr_states[0].location=0;env.curr_states[1].location=4;
    Task a;a.task_id=10;a.locations={4,4};env.task_pool[10]=a;
    Task b;b.task_id=11;b.locations={19,19};env.task_pool[11]=b;
    env.curr_task_schedule={10,11};
    Config cfg;cfg.hungarian_limit=1000;cfg.guided_matching=false;
    cfg.length_weight=0;cfg.keep_bonus=.25;std::vector<int> schedule;
    Engine linear(cfg);linear.initialize(&env);linear.match(&env,schedule);
    require(schedule==std::vector<int>({10,11}),"nonlinear fixture lacks its linear baseline");
    cfg.match_power=.5;Engine concave(cfg);concave.initialize(&env);concave.match(&env,schedule);
    require(schedule==std::vector<int>({11,10}),"concave matching did not favor the immediate pickup");
    env.curr_task_schedule=schedule;env.task_pool[10].idx_next_loc=1;
    concave.match(&env,schedule);
    require(schedule[1]==10,"nonlinear matching reassigned an opened task");
    // Transform optional idle prices as well: without a keep bonus, the same
    // monotone one-pair threshold must hold for concave and convex costs.
    for(float power:{.5f,1.f,2.f})for(int limit:{0,1000}) {
        auto e=environment(1,6,1);Task task;task.task_id=0;task.locations={3,3};e.task_pool[0]=task;
        Config c;c.match_power=power;c.keep_bonus=0;c.length_weight=0;c.hungarian_limit=limit;
        c.admission_price=2.5;Engine refuse(c);refuse.initialize(&e);refuse.match(&e,schedule);
        require(schedule==std::vector<int>({-1}),"nonlinear pricing changed the optional-idle threshold below cost");
        c.admission_price=3.5;Engine admit(c);admit.initialize(&e);admit.match(&e,schedule);
        require(schedule==std::vector<int>({0}),"nonlinear pricing changed the optional-idle threshold above cost");
    }
    for(float power:{.5f,2.f}) {
        cfg=Config{};cfg.match_power=power;cfg.active_task_cap=18;cfg.admission_price=4;
        cfg.hungarian_limit=1000;cfg.futures=8;cfg.depth=6;cfg.random_by_step=true;
        cfg.cost_cache=true;cfg.threads=1;
        const auto reference=simulate(cfg,12,5,5,true);
        cfg.fast_admission=true;cfg.threads=3;cfg.candidate_cache=true;
        require(reference==simulate(cfg,12),"nonlinear matching depends on dummy-prefix optimization or workers");
        require(reference==simulate(cfg,12,5,5,true),"nonlinear matching changed after checkpoint restoration");
    }
    const char* old=std::getenv("R05_MATCH_POWER");const bool present=old;
    const std::string saved=old?old:"";setenv("R05_MATCH_POWER",".5",1);
    auto gate=environment(3,3,2);bool rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"nonlinear matching escaped the explicit trick gate");
    gate.trick_instance="RANDOM-01";
    require(Config::environment(gate).match_power==.5f,"nonlinear matching lost its declared curvature");
    for(const char* invalid:{"0","-1","3","nan"}) {
        setenv("R05_MATCH_POWER",invalid,1);rejected=false;
        try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
        require(rejected,"nonlinear matching accepted invalid curvature");
    }
    if(present)setenv("R05_MATCH_POWER",saved.c_str(),1);else unsetenv("R05_MATCH_POWER");
}

void retained_guidance_flips() {
    auto e=environment(4,4,4);Config cfg;cfg.guidance="flow";cfg.flow_iterations=3;
    cfg.flow_average=true;cfg.flow_normalize=true;cfg.flow_output_penalty=2.4;cfg.threads=1;
    Graph unflipped(e,cfg);cfg.flow_flips=2;cfg.flow_flip_seed=7;Graph base(e,cfg);
    cfg.flow_extra_flips=1;cfg.flow_extra_flip_seed=11;Graph extended(e,cfg),repeat(e,cfg);
    require(extended.next==base.next && extended.weight==repeat.weight,
            "independent field mutation changed topology or reproducibility");
    int retained=0,additional=0;
    for(int v=0;v<base.cells;++v)for(int d=0;d<4;++d) {
        const int u=base.next[v][d];if(u<=v)continue;const int opposite=(d+2)%4;
        if(base.weight[v][d]!=unflipped.weight[v][d]) {
            ++retained;require(extended.weight[v][d]==base.weight[v][d] &&
                extended.weight[u][opposite]==base.weight[u][opposite],"extra mutation undid a retained edge");
        } else if(extended.weight[v][d]!=base.weight[v][d]) {
            ++additional;require(extended.weight[v][d]==base.weight[u][opposite] &&
                extended.weight[u][opposite]==base.weight[v][d],"extra mutation changed edge prices instead of swapping them");
        }
    }
    require(retained==2 && additional==1,"field mutation lost its exact distinct-edge budget");
    cfg.futures=8;cfg.depth=6;cfg.random_by_step=true;cfg.cost_cache=true;
    const auto serial=simulate(cfg,12,5,5,true);cfg.threads=2;
    require(serial==simulate(cfg,12),"independent field mutations depend on workers");
    struct Setting {
        const char* key;bool present;std::string old;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);setenv(k,v,1);
        }
        ~Setting(){if(present)setenv(key,old.c_str(),1);else unsetenv(key);}
    };
    Setting guidance("R05_GUIDANCE","flow"),flips("R05_FLOW_FLIPS","1"),extra("R05_FLOW_EXTRA_FLIPS","1");
    auto gate=environment(3,3,2);gate.trick_instance="RANDOM-03";
    require(Config::environment(gate).flow_extra_flips==1,"extra field-flip budget was not parsed");
    gate.trick_instance.clear();bool rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"extra field flips escaped the explicit trick flag");
    gate.trick_instance="RANDOM-03";setenv("R05_FLOW_FLIPS","0",1);rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"extra field flips accepted a missing retained prefix");
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
        cfg.fuse_cache_hits=true;
        require(original==simulate(cfg,12,5,5,true),"fused ranking hits changed turnover, workers or checkpoint replay");
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
        cfg.fuse_cache_hits=true;
        require(reference==simulate(cfg,12),"fused ranking lookup changed the uncached policy");
        if(variant==0)for(int slots:{8,128,256}) {
            // A tiny direct-mapped cache forces eviction; larger tables must
            // retain exactly the same rankings through chained task turnover.
            cfg.cache_slots=slots;
            require(reference==simulate(cfg,12),"ranking cache capacity changed the trajectory");
        }
    }
}

void shared_task_rankings() {
    for(int variant=0;variant<5;++variant) {
        Config cfg;cfg.guidance="lanes";cfg.futures=16;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
        cfg.cost_cache=variant!=0;cfg.goal_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;
        cfg.radix_order=true;cfg.random_by_step=true;cfg.kinematic_mask=true;
        cfg.candidate_cache=true;cfg.cache_slots=8;cfg.move_bias=variant==4?3:0;
        if(variant==1)cfg.prospective_wait=true;
        if(variant==2)cfg.intent_rotation=false;
        if(variant==3)cfg.push_price=2;
        const auto reference=simulate(cfg,12,7,7);
        cfg.shared_rankings_mb=1; // not every two-stop task table can fit
        require(reference==simulate(cfg,12,7,7,true),"shared task rankings changed bounded-cache turnover or checkpoint replay");
        cfg.shared_rankings_mb=4;cfg.threads=2;cfg.fuse_cache_hits=true;
        require(reference==simulate(cfg,12,7,7),"shared rankings changed with cache capacity or worker scheduling");
        cfg.lazy_cost_rows=true;
        require(reference==simulate(cfg,12,7,7,true),"lazy cost rows changed shared rankings or checkpoint replay");
        cfg.shared_orders=true;cfg.shared_rankings_mb=1;
        require(reference==simulate(cfg,12,7,7,true),"order-only rankings changed policy or biased-routing fallback");
        cfg.shared_rankings_mb=0;cfg.threads=1;
        require(reference==simulate(cfg,12,7,7),"lazy cost rows changed the private-cache fallback");
    }
}

void bounded_order_rankings() {
    Config cfg;cfg.futures=4;cfg.depth=4;cfg.cost_cache=true;cfg.random_by_step=true;
    cfg.scratch_reuse=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;
    const auto baseline=simulate(cfg,12,10,10);
    // 99 two-errand chains need at least1,267,200 bytes even at8 bytes/entry.
    // The1MiB budget therefore exercises exact fallback for uncached chains.
    cfg.shared_orders=true;cfg.shared_rankings_mb=1;cfg.threads=3;
    require(baseline==simulate(cfg,12,10,10,true),"bounded order cache changed task turnover or fallback");
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
    {
        Config cfg;cfg.futures=32;cfg.continuations=10;cfg.generations=2;
        cfg.screen_branches=2;cfg.screen_keep=4;
        auto env=environment(3,3,2);Engine engine(cfg);engine.initialize(&env);
        std::vector<Action> actions;std::vector<int> schedule;bool rejected=false;
        try { engine.compute(&env,actions,schedule); }
        catch(const std::invalid_argument&) { rejected=true; }
        require(rejected,"screening silently spent its only finalist slot on the anchor");
    }
    for(bool shared:{false,true}) {
        Config cfg;cfg.futures=320;cfg.continuations=10;cfg.continuation_start=2;cfg.depth=6;
        cfg.generations=2;cfg.random_by_step=true;cfg.future_mutation=0;
        cfg.share_prefix=shared;cfg.rollout_match=true;cfg.cost_cache=true;
        cfg.scratch_reuse=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;
        cfg.accept_equal=true;cfg.first_futures=160;
        // Identical continuations make screening exact. Both searches generate
        // 32 roots (16 at the first step); only work on rejected roots differs.
        const auto exhaustive=simulate(cfg,12);
        cfg.screen_branches=2;cfg.screen_keep=4;cfg.futures=128;cfg.first_futures=64;
        require(exhaustive==simulate(cfg,12),"staged search lost an exact winner with identical futures");
        cfg.future_mutation=.3;cfg.elites=2;cfg.persist_elites=2;cfg.continuation_risk=.5;
        const auto serial=simulate(cfg,12);cfg.threads=2;
        require(serial==simulate(cfg,12),"staged search changed across workers or reused stale task state");
    }
}

void ranked_task_progress() {
    const auto weights=rank_progress_weights({10,10,50,-1},2);
    require(weights[0]==weights[1] && weights[0]>weights[2] && weights[2]>0 && weights[3]==0,
            "task progress weights break tied ranks or active ordering");
    require(std::abs(std::accumulate(weights.begin(),weights.end(),0.0)-3)<1e-12,
            "task progress weights changed the mean cost scale");
    require(rank_progress_weights({10,10,50,-1},0)==std::vector<double>({1,1,1,1}),
            "zero rank power changed baseline weights");
    require(rank_progress_weights({-1,-1},2)==std::vector<double>({0,0}),"inactive score weights are not zero");
    // Equal physical progress, but only one agent can complete its task.
    auto env=environment(1,3,2);env.curr_states[1].location=2;env.curr_states[1].orientation=2;
    Task a;a.task_id=0;a.locations={1};env.task_pool[0]=a;
    Task b;b.task_id=1;b.locations={1,0};env.task_pool[1]=b;env.curr_task_schedule={0,1};
    Config cfg;cfg.futures=64;cfg.depth=2;cfg.matching=false;cfg.score_rank_power=2;
    Engine engine(cfg);engine.initialize(&env);std::vector<Action> plan;std::vector<int> schedule;
    engine.compute(&env,plan,schedule);
    for(int i=0;i<2;++i) {
        require(plan[i]!=FW,"ranked score moved before its first promise");
        if(plan[i]==CR)env.curr_states[i].orientation=(env.curr_states[i].orientation+1)%4;
        if(plan[i]==CCR)env.curr_states[i].orientation=(env.curr_states[i].orientation+3)%4;
    }
    env.curr_timestep=1;engine.compute(&env,plan,schedule);
    require(plan[0]==FW && plan[1]!=FW,"ranked progress did not select the completing task");
    cfg=Config{};cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.score_rank_power=.5;cfg.share_prefix=true;cfg.cost_cache=true;cfg.hungarian_limit=1000;
    const auto serial=simulate(cfg,12);cfg.threads=2;
    require(serial==simulate(cfg,12),"rank-weighted scoring changed across worker counts");
    require(serial==simulate(cfg,12,5,5,true),"rank-weighted scoring failed checkpoint replay");
    cfg.score_rank_steps=51;cfg.threads=1;
    const auto startup=simulate(cfg,12,5,5,true);cfg.threads=2;
    require(startup==simulate(cfg,12),"startup rank scoring changed across worker counts");
}

void independent_candidate_rescoring() {
    const std::vector<double> scores{-10,-4,-8};
    require(weighted_static_future_score(scores,0)==-6 && weighted_static_future_score(scores,1)==-10 &&
            weighted_static_future_score(scores,.5)==-8,"static future mixture has wrong endpoint/scale semantics");
    // Duplicating random samples must not dilute the separately weighted anchor.
    require(weighted_static_future_score(scores,.25)==weighted_static_future_score({-10,-4,-8,-4,-8},.25),
            "static future weight depends on random sample count");
    Config cfg;cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.share_prefix=true;cfg.cost_cache=true;cfg.candidate_cache=true;
    const auto ordinary=simulate(cfg,12);
    cfg.rescore_roots=1;cfg.rescore_branches=8;
    require(ordinary==simulate(cfg,12,5,5,true,true),"single-finalist rescore changed the trajectory");
    cfg.rescore_roots=4;cfg.rescore_blend=1;
    require(ordinary==simulate(cfg,12,5,5,false,true),"original-score rescore control changed the trajectory");
    cfg.rescore_blend=0;
    const auto serial=simulate(cfg,12,5,5,true);cfg.threads=2;
    require(serial==simulate(cfg,12),"independent rescoring changed across worker counts");
    cfg.rescore_static_weight=.1;cfg.threads=1;
    const auto weighted=simulate(cfg,12,5,5,true);cfg.threads=2;
    require(weighted==simulate(cfg,12),"weighted independent futures changed across workers");
    cfg.rescore_roots=1;cfg.rescore_static_weight=.25;cfg.threads=1;
    require(ordinary==simulate(cfg,12,5,5,true,true),"one weighted finalist changed the live decision or state");
    cfg.rescore_roots=4;cfg.rescore_blend=.5;cfg.score_rank_power=.5;cfg.score_rank_steps=51;
    simulate(cfg,12,5,5,true);
}

void waypoint_priority_retention() {
    for(float retained:{0.0f,0.5f,1.0f}) {
        auto env=environment(1,3,1);Task task;task.task_id=7;task.locations={0,0};env.task_pool[7]=task;
        Config cfg;cfg.futures=1;cfg.waypoint_age_retain=retained;
        Engine engine(cfg);engine.initialize(&env);
        std::vector<Action> actions;std::vector<int> schedule;
        for(int step=0;step<3;++step) {
            engine.compute(&env,actions,schedule);
            const int expected=step==0?1:step==1?int(2*retained):0;
            require(engine.ages()[0]==expected,"waypoint age retention/reset is incorrect");
            auto& state=env.curr_states[0];
            if(actions[0]==FW)state.location=engine.graph->next[state.location][state.orientation];
            if(actions[0]==CR)state.orientation=(state.orientation+1)%4;
            if(actions[0]==CCR)state.orientation=(state.orientation+3)%4;
            state.timestep=++env.curr_timestep;env.curr_task_schedule=schedule;
            if(schedule[0]>=0) {
                auto& current=env.task_pool.at(schedule[0]);
                if(current.locations.at(current.idx_next_loc)==state.location &&
                   ++current.idx_next_loc==int(current.locations.size())) {
                    env.task_pool.erase(schedule[0]);env.curr_task_schedule[0]=-1;
                }
            }
        }
    }
    Config cfg;cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.waypoint_age_retain=.5;cfg.share_prefix=true;cfg.cost_cache=true;
    const auto serial=simulate(cfg,12);cfg.threads=2;
    require(serial==simulate(cfg,12),"priority retention changed across worker counts");
    require(serial==simulate(cfg,12,5,5,true),"priority retention failed checkpoint replay");
    cfg.rollout_age=true;simulate(cfg,12);
}

void replanning_forecast() {
    // A fully evaluated one-root forecast may spend work, but must leave every
    // selected decision, RNG stream and persistent solver vector unchanged.
    Config cfg;cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.depth=6;cfg.threads=2;
    cfg.cost_cache=true;cfg.goal_cache=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;
    cfg.scratch_reuse=true;cfg.share_prefix=true;cfg.hungarian_limit=1000;cfg.guided_matching=true;
    cfg.replan_roots=1;cfg.replan_k=4;cfg.replan_continuations=2;cfg.replan_steps=4;
    simulate(cfg,12,5,5,false,true);
    cfg.replan_roots=4;cfg.replan_futures=2;cfg.random_by_step=true;cfg.threads=1;
    const auto serial=simulate(cfg,12);cfg.threads=2;
    require(serial==simulate(cfg,12),"replanning forecast changed across worker counts");
    require(serial==simulate(cfg,12,5,5,true),"replanning forecast changed after checkpoint restoration");
    cfg.threads=4;cfg.replan_threads=2;
    require(serial==simulate(cfg,12),"nested forecast workers changed fixed-work decisions");
    cfg.shared_rankings_mb=1;
    require(serial==simulate(cfg,12,5,5,true),"nested forecasts failed to bypass shared mutable table construction");
    cfg.replan_policy=true;
    const auto faithful=simulate(cfg,12);cfg.replan_threads=1;
    require(faithful==simulate(cfg,12),"multi-generation forecast changed with nested worker count");
    // Finish the only visible task in a forecast. Do not invent a replacement,
    // and do not mutate the live task or advance the actual environment.
    auto env=environment(1,3,1);Task task;task.task_id=7;task.locations={0};env.task_pool[7]=task;
    cfg=Config{};cfg.futures=1;cfg.replan_roots=1;cfg.replan_steps=4;
    cfg.replan_k=4;cfg.replan_continuations=2;Engine engine(cfg);engine.initialize(&env);
    std::vector<Action> actions;std::vector<int> schedule;engine.compute(&env,actions,schedule);
    const auto stats=engine.replan_stats();
    require(stats.pool_before==1 && stats.min_pool_after==0 && stats.max_completed==1,
            "closed-loop forecast failed visible-task depletion");
    require(stats.decisions==3 && stats.branch_evaluations==12,"forecast changed its declared work budget");
    require(env.curr_timestep==0 && env.task_pool.size()==1 && env.task_pool.at(7).idx_next_loc==0 &&
            env.curr_task_schedule[0]==-1,"forecast modified the real task environment");
}

void normalized_directional_triage() {
    // Same one-cell distance, but the second robot must turn around first.
    // Pool normalization keeps total estimated distance2, assigning .5/1.5.
    auto env=environment(1,5,2);env.curr_states[0]=State(0,0,0);env.curr_states[1]=State(4,0,0);
    for(int a=0;a<2;++a) {Task task;task.task_id=a;task.locations={a?3:1};env.task_pool[a]=task;}
    env.curr_task_schedule={0,1};Config cfg;cfg.futures=1;cfg.matching=false;
    cfg.horizon=2;cfg.triage_scale=1;
    std::vector<Action> plan;std::vector<int> schedule;
    Engine baseline(cfg);baseline.initialize(&env);baseline.compute(&env,plan,schedule);
    require(baseline.triaged()==0,"hop triage fixture unexpectedly discarded a task");
    cfg.triage_guided_mix=1;Engine directed(cfg);directed.initialize(&env);directed.compute(&env,plan,schedule);
    require(directed.triaged()==1,"directional triage failed to distinguish the required half-turn");
    require(schedule==std::vector<int>({0,1}),"triage illegally changed assignments");
    // Dense task turnover still obeys locks, movement rules and deterministic workers.
    cfg=Config{};cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.share_prefix=true;cfg.cost_cache=true;cfg.horizon=150;cfg.triage_scale=.8;cfg.triage_guided_mix=.5;
    const auto serial=simulate(cfg,12);cfg.threads=2;
    require(serial==simulate(cfg,12),"directional triage changed with worker count");
}

void motion_component_search() {
    // Two legal parents compete for the same hole. A partial substitution
    // would collide; the dependency decomposition must join both robots.
    auto env=environment(1,3,2);Config plain;Graph graph(env,plain);
    Rollout a,b;a.first.loc=b.first.loc={0,2};a.first.pending={1,2};b.first.pending={0,1};
    a.actions={FW,W};b.actions={W,FW};
    auto groups=decision_components(graph,a,b);
    require(groups.size()==1 && groups[0]==std::vector<int>({0,1}),"shared empty destination split between components");
    // Independent rotations have no spatial dependency.
    a.first.pending=b.first.pending={0,2};a.actions={CR,W};b.actions={W,CCR};
    groups=decision_components(graph,a,b);
    require(groups.size()==2 && groups[0].size()==1 && groups[1].size()==1,"independent turns were coupled");
    Config cfg;cfg.futures=128;cfg.continuations=10;cfg.continuation_start=2;cfg.depth=6;
    cfg.screen_branches=2;cfg.screen_keep=4;cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;
    cfg.component_trials=8;cfg.component_rounds=2;cfg.component_parents=4;
    cfg.cost_cache=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.scratch_reuse=true;
    cfg.rollout_match=true;cfg.reverse_penalty=.1;cfg.completion_bonus=4;cfg.progress_discount=.9;
    cfg.random_by_step=true;cfg.share_prefix=false;
    const auto reference=simulate(cfg,12);
    cfg.share_prefix=true;
    require(reference==simulate(cfg,12),"component forced decisions changed under shared prefixes");
    cfg.threads=2;
    require(reference==simulate(cfg,12),"component search changed across worker counts");
    require(reference==simulate(cfg,12,5,5,true),"component search changed after checkpoint restoration");
}

void checkpoint_replay() {
    Config cfg;cfg.futures=128;cfg.continuations=10;cfg.continuation_start=2;
    cfg.screen_branches=2;cfg.screen_keep=4;cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;
    cfg.share_prefix=true;cfg.cost_cache=true;cfg.candidate_cache=true;cfg.goal_cache=true;
    cfg.kinematic_mask=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;cfg.guided_matching=true;
    for(bool step_rng:{false,true}) {
        cfg.random_by_step=step_rng;
        const auto reference=simulate(cfg,12);
        require(reference==simulate(cfg,12,5,5,true),"restoring changed the dense trajectory");
    }
}

void move_proposal_bias() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.threads=1;
    cfg.hungarian_limit=1000;cfg.guided_matching=true;
    const auto ordinary=simulate(cfg,12);
    cfg.move_bias=.75;
    const auto biased=simulate(cfg,12);
    require(biased!=ordinary,"dense fixture did not exercise move proposal bias");
    cfg.candidate_cache=true;cfg.kinematic_mask=true;
    require(biased==simulate(cfg,12,5,5,true),"cached ranking or restore changed move proposal bias");
    cfg.threads=3;
    require(biased==simulate(cfg,12),"move proposal bias changed with worker count");
    cfg.move_bias_fraction=0;
    require(ordinary==simulate(cfg,12),"zero proposal fraction changed the original policy");
    cfg.move_bias=0;
    require(ordinary==simulate(cfg,12),"zero move proposal bias changed the original policy");
    cfg.move_bias=.75;
    for(float fraction:{.125f,.5f,1.0f}) {
        cfg.move_bias_fraction=fraction;cfg.threads=1;
        cfg.candidate_cache=false;cfg.kinematic_mask=false;
        const auto changed=simulate(cfg,12);
        require(changed!=ordinary,"proposal fraction did not exercise alternative routing");
        cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.threads=3;
        require(changed==simulate(cfg,12,5,5,true),"proposal fraction changed with cache, workers or restoration");
    }
}

void feasible_move_proposals() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    cfg.move_bias=.75f;cfg.move_bias_fraction=.5f;
    const auto compass=simulate(cfg,12);
    for(int mode:{1,2}) {
        cfg.move_bias_mode=mode;cfg.threads=1;cfg.candidate_cache=false;cfg.kinematic_mask=false;
        const auto signature=simulate(cfg,12,5,5,true);
        require(signature!=compass,"available-edge proposal fixture did not change routing");
        cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.threads=3;
        require(signature==simulate(cfg,12),"available-edge proposals changed with cache or workers");
    }
}

void rollout_elite_diversity() {
    std::vector<Rollout> r(5);
    for(int i=0;i<5;++i){r[i].score=10-i;r[i].offsets={float(i)};r[i].first.pending={0,1,2,3};}
    r[1].first.pending={1,1,2,3};
    r[2].first.pending={1,0,2,3};
    r[3].first.pending={1,0,3,2};
    r[4].score=100;r[4].fully_evaluated=false;
    require(select_rollout_elites(r,5,0,3,false)==std::vector<int>({0,1,2}),
            "default elite ordering changed or admitted an unfinished rollout");
    require(select_rollout_elites(r,5,0,3,false,.5f)==std::vector<int>({0,2,3}),
            "elite decision separation failed to preserve the incumbent and distinct alternatives");
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=4;cfg.persist_elites=4;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    const auto ordinary=simulate(cfg,12);
    for(int period:{0,2,8}) {
        cfg.restart_period=period;cfg.elite_decision_distance=.1f;cfg.threads=1;
        const auto altered=simulate(cfg,12,5,5,true);
        require(altered!=ordinary,"restart/diversity fixture did not exercise changed proposals");
        cfg.threads=3;
        require(altered==simulate(cfg,12),"proposal diversity changed with worker scheduling");
    }
}

void compact_prepared_rankings() {
    // Preserve every score bit and ranking position, including waiting with
    // each possible idle heading and arbitrary nonconsecutive vertex IDs.
    const std::array<int,4> neighbors{73,12,805,301};
    for(int heading=0;heading<4;++heading)for(int shift=0;shift<5;++shift) {
        PreparedRanking entry;entry.count=5;entry.idle_heading=uint8_t(heading);
        std::array<MoveCandidate,5> source{},restored{};
        for(int k=0;k<5;++k) {
            const int d=(k+shift)%5;
            source[k]={d==4?91:neighbors[d],d==4?heading:d,(k-2)*1.234567f};
        }
        entry.save(source,91);entry.load(restored,91,neighbors);
        for(int k=0;k<5;++k)
            require(source[k].v==restored[k].v && source[k].d==restored[k].d &&
                    source[k].score==restored[k].score,"compact prepared ranking changed an ordered candidate");
        for(int count=1;count<=5;++count)for(unsigned mask=0;mask<32;++mask) {
            entry.count=uint8_t(count);entry.kinematic_mask=uint8_t(mask);entry.base_cost=12.34567f;
            PreparedOrder order;order.save(entry);order.load(restored,91,neighbors);
            require(order.base_cost==entry.base_cost && order.count()==count && order.kinematic_mask()==mask &&
                    order.idle_heading()==heading,"order-only ranking changed its cost or kinematic metadata");
            for(int k=0;k<count;++k)
                require(source[k].v==restored[k].v && source[k].d==restored[k].d,
                        "order-only ranking changed an ordered destination");
        }
    }
}

void active_travel_calibration() {
    auto env=environment(3,3,2);env.curr_states[0].location=4;env.curr_states[1].location=8;
    Task task;task.task_id=7;task.locations={4,5};task.idx_next_loc=1;
    env.task_pool[7]=task;env.curr_task_schedule={7,-1};env.curr_timestep=4;
    Config cfg;cfg.futures=1;cfg.depth=3;cfg.horizon=13;cfg.triage_scale=1;
    Engine ordinary(cfg);ordinary.initialize(&env);
    auto snapshot=ordinary.checkpoint(env);
    snapshot["total_agent_steps"]=1000;snapshot["total_forward"]=100;
    snapshot["active_agent_steps"]=500;snapshot["active_forward"]=100;
    ordinary.restore(snapshot,env);std::vector<Action> actions;std::vector<int> schedule;
    ordinary.compute(&env,actions,schedule);
    require(ordinary.triaged()==1,"full-fleet calibration fixture did not suppress the late task");
    cfg.active_travel_rate=true;Engine calibrated(cfg);calibrated.initialize(&env);
    calibrated.restore(snapshot,env);calibrated.compute(&env,actions,schedule);
    require(calibrated.triaged()==0,"active travel calibration still counted goal-less robot time");
    auto after=calibrated.checkpoint(env);
    require(after.at("active_agent_steps")==501 && after.at("total_agent_steps")==1002,
            "active travel accounting counted an idle robot or lost total robot time");
    cfg.active_task_cap=22;cfg.hungarian_limit=1000;cfg.horizon=300;cfg.triage_scale=1;
    cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.cost_cache=true;cfg.random_by_step=true;
    const auto reference=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(reference==simulate(cfg,12),"active travel calibration changed with worker scheduling");
    cfg.replan_roots=1;cfg.replan_futures=1;cfg.replan_steps=2;cfg.replan_k=1;cfg.replan_continuations=1;
    require(reference==simulate(cfg,12,5,5,true,true),"single-root forecast changed active travel accounting");
}

void observed_progress_triage() {
    // Equal remaining work with fast versus stalled observed progress must
    // redistribute time, preserve total predicted work, and survive negatives.
    const std::vector<double> work{4,4,0},rates{2,0,-100};
    const std::vector<int> mature{32,32,32};
    const auto factors=progress_time_factors(work,rates,mature,32,1);
    require(factors[0]<1 && factors[1]>1 && std::abs(4*factors[0]+4*factors[1]-8)<1e-12,
            "progress calibration did not redistribute a fixed work total");
    require(progress_time_factors(work,rates,{0,32,32},32,1)==std::vector<double>({1,1,1}),
            "immature progress history affected the forecast");
    require(progress_time_factors(work,{-2,-1,0},mature,32,1)==std::vector<double>({1,1,1}),
            "nonpositive fleet progress lost its global fallback");
    require(progress_time_factors(work,rates,mature,32,0)==std::vector<double>({1,1,1}),
            "zero progress mix was not an exact identity");
    auto env=environment(3,3,2);env.curr_states[0].location=0;env.curr_states[1].location=6;
    for(auto& state:env.curr_states)state.orientation=0;
    for(int a=0;a<2;++a) {
        Task task;task.task_id=7+a;task.locations={a*6,a*6+2};task.idx_next_loc=1;
        env.task_pool[task.task_id]=task;env.curr_task_schedule[a]=task.task_id;
    }
    env.curr_timestep=40;Config cfg;cfg.futures=1;cfg.depth=3;cfg.horizon=45;cfg.triage_scale=1;
    Engine ordinary(cfg);ordinary.initialize(&env);auto snapshot=ordinary.checkpoint(env);
    snapshot["previous_task"]={7,8};snapshot["previous_stage"]={1,1};
    snapshot["progress_timestep"]=39;snapshot["progress_remaining"]={4,4};
    snapshot["progress_rates"]={2,0};snapshot["progress_samples"]={32,32};
    std::vector<Action> plan;std::vector<int> schedule;
    ordinary.restore(snapshot,env);ordinary.compute(&env,plan,schedule);
    require(ordinary.triaged()==0,"global forecast fixture unexpectedly deferred a task");
    cfg.triage_progress_mix=1;Engine calibrated(cfg);calibrated.initialize(&env);
    calibrated.restore(snapshot,env);calibrated.compute(&env,plan,schedule);
    require(calibrated.triaged()==1 && schedule==std::vector<int>({7,8}),
            "observed progress did not distinguish the stalled task or changed opened assignments");
    const auto tracked=calibrated.checkpoint(env);
    require(tracked.at("progress_timestep")==40 && tracked.at("progress_samples").at(0)==32,
            "progress history did not advance with a real timestep");
    auto replacement=snapshot;replacement["schedule"]={9,8};replacement["tasks"][0]["id"]=9;
    calibrated.restore(replacement,env);calibrated.compute(&env,plan,schedule);
    const auto reset=calibrated.checkpoint(env);
    require(reset.at("progress_samples").at(0)==0 && reset.at("progress_rates").at(0)==0,
            "task reassignment inherited the previous task's speed");
    auto legacy=snapshot;
    for(const char* key:{"progress_timestep","progress_remaining","progress_rates","progress_samples"})legacy.erase(key);
    calibrated.restore(legacy,env);calibrated.compute(&env,plan,schedule);
    require(calibrated.triaged()==0,"legacy checkpoint did not start with the global progress prior");
    cfg=Config{};cfg.horizon=150;cfg.triage_scale=.8;cfg.triage_progress_mix=.5;cfg.triage_progress_window=8;
    cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;cfg.hungarian_limit=1000;
    cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.cost_cache=true;cfg.random_by_step=true;
    for(float guided:{0.f,.75f}) {
        cfg.triage_guided_mix=guided;cfg.threads=1;cfg.candidate_cache=false;
        const auto serial=simulate(cfg,12,5,5,true);
        cfg.threads=3;cfg.candidate_cache=true;
        require(serial==simulate(cfg,12),"progress-calibrated triage changed with workers or cached policy");
        cfg.replan_roots=1;cfg.replan_futures=1;cfg.replan_steps=2;cfg.replan_k=1;cfg.replan_continuations=1;
        require(serial==simulate(cfg,12,5,5,true,true),"shadow forecasting changed observed-progress history");
        cfg.replan_roots=0;
    }
    struct Setting {
        const char* key;bool present;std::string old;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);setenv(k,v,1);
        }
        ~Setting(){if(present)setenv(key,old.c_str(),1);else unsetenv(key);}
    };
    Setting horizon("R05_HORIZON","150"),mix("R05_TRIAGE_PROGRESS_MIX",".5"),span("R05_TRIAGE_PROGRESS_WINDOW","32");
    auto gate=environment(3,3,2);gate.trick_instance="RANDOM-04";
    require(Config::environment(gate).triage_progress_mix==.5f,"valid progress calibration was rejected");
    gate.trick_instance.clear();bool refused=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){refused=true;}
    require(refused,"progress-calibrated horizon bypassed the trick gate");gate.trick_instance="RANDOM-04";
    for(const char* value:{"-0.1","1.1","nan"}) {
        Setting invalid("R05_TRIAGE_PROGRESS_MIX",value);refused=false;
        try{Config::environment(gate);}catch(const std::invalid_argument&){refused=true;}
        require(refused,"invalid observed-progress mix was accepted");
    }
    for(const char* value:{"7","257"}) {
        Setting invalid("R05_TRIAGE_PROGRESS_WINDOW",value);refused=false;
        try{Config::environment(gate);}catch(const std::invalid_argument&){refused=true;}
        require(refused,"invalid observed-progress span was accepted");
    }
    Setting unknown_horizon("R05_HORIZON","0");refused=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){refused=true;}
    require(refused,"observed-progress cutoff accepted an unknown end");
}

void rectangular_matching_optimality() {
    // Independent enumeration of injective assignments checks the optimum,
    // including rectangular pools, negative keep bonuses and extensive ties.
    std::mt19937 random(624917);
    for(int rows=1;rows<=5;++rows)for(int extra:{0,2})for(int trial=0;trial<20;++trial) {
        const int columns=rows+extra;
        std::vector<float> matrix(size_t(rows)*columns);
        for(auto& value:matrix)value=(int(random()%17)-8)*.25f;
        double optimum=1e30;
        auto enumerate=[&](auto&& self,int row,unsigned used,double cost)->void {
            if(row==rows){optimum=std::min(optimum,cost);return;}
            for(int column=0;column<columns;++column)if(!(used&(1u<<column)))
                self(self,row+1,used|(1u<<column),cost+matrix[size_t(row)*columns+column]);
        };
        enumerate(enumerate,0,0,0);
        const auto chosen=hungarian_assignment(matrix,rows,columns);
        require(int(chosen.size())==rows,"rectangular matching lost a row");
        unsigned used=0;double actual=0;
        for(int row=0;row<rows;++row) {
            const int column=chosen[row];
            require(column>=0 && column<columns && !(used&(1u<<column)),"rectangular matching reused or missed a column");
            used|=1u<<column;actual+=matrix[size_t(row)*columns+column];
        }
        require(actual==optimum,"rectangular matching differs from exhaustive optimum");
    }
    require(hungarian_assignment({},0,0).empty(),"empty matching was not empty");
    require(hungarian_assignment(std::vector<float>(15,0),3,5)==std::vector<int>({0,1,2}),
            "equal-price matching changed its stable column ties");
}

void exact_dummy_prefix() {
    std::mt19937 random(194837);
    int comparisons=0;
    for(int rows=1;rows<=12;++rows)for(int extra:{0,3})for(int dummies=0;dummies<=rows;++dummies) {
        const int columns=rows+extra,real=columns-dummies;
        for(int trial=0;trial<20;++trial) {
            std::vector<float> matrix(size_t(rows)*columns);float minimum=0;
            for(int row=0;row<rows;++row)for(int j=0;j<real;++j) {
                // Ties, negative keep bonuses, and nonintegral costs all occur
                // in real task matching. Compare exact assignments, not cost.
                float value=int(random()%13)-6;
                if(trial%2)value*=.37f;
                matrix[size_t(row)*columns+j]=value;minimum=std::min(minimum,value);
            }
            for(int row=0;row<rows;++row)for(int j=real;j<columns;++j)
                matrix[size_t(row)*columns+j]=minimum-1;
            require(hungarian_assignment(matrix,rows,columns,dummies,false)==
                    hungarian_assignment(matrix,rows,columns,dummies,true),
                    "dummy-prefix shortcut changed an exact assignment");
            ++comparisons;
        }
    }
    std::vector<float> malformed{0,1,0,1};bool rejected=false;
    try{hungarian_assignment(malformed,2,2,1,true);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"dummy prefix accepted a non-cheaper dummy column");
    std::cout<<"exact dummy-prefix assignments="<<comparisons<<"\n";
}

void active_task_admission() {
    auto env=environment(3,3,4);env.curr_states[3].location=8;
    for(int j=0;j<4;++j){Task t;t.task_id=j;t.locations={8,8};env.task_pool[j]=t;}
    Config cfg;cfg.active_task_cap=1;cfg.hungarian_limit=1000;cfg.keep_bonus=0;
    Engine engine(cfg);engine.initialize(&env);std::vector<int> schedule;
    engine.match(&env,schedule);
    require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==1 && schedule[3]>=0,
            "cardinality-constrained matching did not choose the closest robot");
    env.curr_task_schedule=schedule;env.task_pool.at(schedule[3]).idx_next_loc=1;
    engine.match(&env,schedule);
    require(schedule==env.curr_task_schedule,"admission cap changed an opened task or admitted an extra task");
    env.curr_task_schedule[0]=0;env.task_pool[0].idx_next_loc=1;
    if(env.curr_task_schedule[3]==0){env.curr_task_schedule[0]=1;env.task_pool[1].idx_next_loc=1;}
    engine.match(&env,schedule);
    require(schedule==env.curr_task_schedule,"admission cap dropped an opened task above the cap");
    env.curr_task_schedule.assign(4,-1);for(auto& kv:env.task_pool)kv.second.idx_next_loc=0;
    for(int limit:{0,1000}) {
        cfg.hungarian_limit=limit;cfg.active_cap_steps=10;
        Engine boundary(cfg);boundary.initialize(&env);env.curr_timestep=9;boundary.match(&env,schedule);
        require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==1,
                "greedy/exact admission did not enforce its active cap");
        env.curr_timestep=10;boundary.match(&env,schedule);
        require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==4,
                "startup admission cap failed to expire");
    }
    cfg.active_task_cap=22;cfg.active_cap_steps=0;cfg.futures=32;cfg.continuations=4;
    cfg.continuation_start=2;cfg.depth=6;cfg.random_by_step=true;cfg.share_prefix=true;
    cfg.scratch_reuse=true;cfg.cost_cache=true;cfg.candidate_cache=true;
    const auto signature=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(signature==simulate(cfg,12),"task admission changed with worker scheduling");
    cfg.fast_admission=true;
    require(signature==simulate(cfg,12,5,5,true),"fast dummy prefix changed the admission trajectory");
    const char* old=std::getenv("R05_ACTIVE_TASK_CAP");const bool present=old;
    const std::string saved=old?old:"";setenv("R05_ACTIVE_TASK_CAP","1",1);
    SharedEnvironment gate;bool rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"active admission did not require the trick flag");
    gate.trick_instance="RANDOM-04";require(Config::environment(gate).active_task_cap==1,"explicit admission trick was rejected");
    if(present)setenv("R05_ACTIVE_TASK_CAP",saved.c_str(),1);else unsetenv("R05_ACTIVE_TASK_CAP");
}

void deferred_task_admission_credit() {
    auto initial=environment(3,3,3);
    Task a;a.task_id=7;a.locations={0,8};a.idx_next_loc=1;initial.task_pool[7]=a;
    Task b;b.task_id=8;b.locations={1,6};b.idx_next_loc=1;initial.task_pool[8]=b;
    Task c;c.task_id=9;c.locations={2,2};initial.task_pool[9]=c;
    initial.curr_task_schedule={7,8,-1};
    Config cfg;cfg.active_task_cap=2;cfg.horizon=2;cfg.triage_scale=1;
    cfg.hungarian_limit=1000;cfg.futures=1;cfg.depth=2;
    for(float fraction:{0.f,.5f,1.f}) {
        cfg.active_cap_triage_credit=fraction;auto env=initial;
        Engine engine(cfg);engine.initialize(&env);std::vector<Action> plan;std::vector<int> schedule;
        engine.compute(&env,plan,schedule);
        require(schedule==std::vector<int>({7,8,-1}),"credit guessed deferred work before planning it");
        require(engine.triaged()==2,"credit fixture did not defer both opened orders");
        env.curr_task_schedule=schedule;
        for(int i=0;i<3;++i) {
            if(plan[i]==FW)env.curr_states[i].location=engine.graph->next[env.curr_states[i].location][env.curr_states[i].orientation];
            if(plan[i]==CR)env.curr_states[i].orientation=(env.curr_states[i].orientation+1)%4;
            if(plan[i]==CCR)env.curr_states[i].orientation=(env.curr_states[i].orientation+3)%4;
            ++env.curr_states[i].timestep;
        }
        env.curr_timestep=1;engine.match(&env,schedule);
        require(schedule[0]==7 && schedule[1]==8,"admission credit released an opened order");
        require(schedule[2]==(fraction>0?9:-1),"deferred orders did not release the expected admission slot");
        const auto checkpoint=engine.checkpoint(env);auto restored_env=env;
        Engine restored(cfg);restored.restore(checkpoint,restored_env);std::vector<int> again;
        restored.match(&restored_env,again);require(schedule==again,"admission credit disappeared after checkpoint restore");
        if(fraction>0) {
            auto replaced=env;replaced.task_pool[17]=replaced.task_pool[7];replaced.task_pool[17].task_id=17;
            replaced.task_pool[18]=replaced.task_pool[8];replaced.task_pool[18].task_id=18;
            replaced.curr_task_schedule={17,18,-1};replaced.task_pool.erase(7);replaced.task_pool.erase(8);
            engine.match(&replaced,again);require(again==std::vector<int>({17,18,-1}),"new tasks inherited stale robot admission credits");
        }
    }
    cfg=Config{};cfg.active_task_cap=22;cfg.active_cap_triage_credit=1;
    cfg.horizon=120;cfg.triage_scale=1;cfg.hungarian_limit=1000;cfg.fast_admission=true;
    cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.random_by_step=true;cfg.share_prefix=true;cfg.cost_cache=true;
    const auto serial=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(serial==simulate(cfg,12,5,5,true),"admission credit changed across workers or checkpoints");
    struct Setting {
        std::string key,old;bool present;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);setenv(k,v,1);
        }
        ~Setting(){if(present)setenv(key.c_str(),old.c_str(),1);else unsetenv(key.c_str());}
    };
    Setting cap("R05_ACTIVE_TASK_CAP","22"),credit("R05_ACTIVE_CAP_TRIAGE_CREDIT","1"),horizon("R05_HORIZON","120");
    SharedEnvironment gate;bool rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"triage admission credit escaped the explicit trick gate");
    gate.trick_instance="RANDOM-04";require(Config::environment(gate).active_cap_triage_credit==1,"valid admission credit was rejected");
    setenv("R05_ACTIVE_CAP_TRIAGE_CREDIT","1.1",1);rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"admission credit accepted a fraction above one");
}

void nonlinear_progress_objective() {
    // Equal movement competes for one cell; one chain finishes there and the
    // other has more work. Smooth utility must prefer the nearer completion.
    auto env=environment(1,3,2);env.curr_states[1].location=2;env.curr_states[1].orientation=2;
    Task a;a.task_id=0;a.locations={1};env.task_pool[0]=a;
    Task b;b.task_id=1;b.locations={1,0};env.task_pool[1]=b;env.curr_task_schedule={0,1};
    Config cfg;cfg.futures=64;cfg.depth=2;cfg.matching=false;cfg.progress_softcap=4;
    Engine engine(cfg);engine.initialize(&env);std::vector<Action> plan;std::vector<int> schedule;
    engine.compute(&env,plan,schedule);
    for(int i=0;i<2;++i) {
        require(plan[i]!=FW,"nonlinear score moved before its promise");
        if(plan[i]==CR)env.curr_states[i].orientation=(env.curr_states[i].orientation+1)%4;
        if(plan[i]==CCR)env.curr_states[i].orientation=(env.curr_states[i].orientation+3)%4;
    }
    env.curr_timestep=1;engine.compute(&env,plan,schedule);
    require(plan[0]==FW && plan[1]!=FW,"nonlinear progress failed to prefer completion");
    cfg=Config{};cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.progress_softcap=10;cfg.cost_cache=true;cfg.hungarian_limit=1000;
    cfg.active_task_cap=22;cfg.fast_admission=true;
    const auto serial=simulate(cfg,12,5,5,true);
    cfg.share_prefix=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;
    cfg.shared_rankings_mb=1;cfg.shared_orders=true;cfg.threads=3;
    require(serial==simulate(cfg,12,5,5,true),"nonlinear utility changed with prefix/cache sharing or worker scheduling");
    const char* old=std::getenv("R05_PROGRESS_SOFTCAP");const bool present=old;
    const std::string saved=old?old:"";setenv("R05_PROGRESS_SOFTCAP","10",1);
    SharedEnvironment gate;bool rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"nonlinear short-task utility escaped the explicit trick flag");
    gate.trick_instance="RANDOM-04";require(Config::environment(gate).progress_softcap==10,"nonlinear utility parser changed its scale");
    if(present)setenv("R05_PROGRESS_SOFTCAP",saved.c_str(),1);else unsetenv("R05_PROGRESS_SOFTCAP");
}

void priced_task_admission() {
    auto env=environment(3,3,4);env.curr_states[3].location=8;
    for(int j=0;j<4;++j){Task t;t.task_id=j;t.locations={8,8};env.task_pool[j]=t;}
    Config cfg;cfg.hungarian_limit=1000;cfg.keep_bonus=0;cfg.admission_price=.5f;
    std::vector<int> schedule;
    for(int cap:{0,1,3})for(int limit:{0,1000})for(bool fast:{false,true}) {
        cfg.active_task_cap=cap;cfg.hungarian_limit=limit;cfg.fast_admission=fast;
        Engine engine(cfg);engine.initialize(&env);engine.match(&env,schedule);
        require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==1 && schedule[3]>=0,
                "priced admission filled an expensive pair or skipped the inexpensive pair");
        auto opened=env;opened.curr_task_schedule=schedule;opened.task_pool.at(schedule[3]).idx_next_loc=1;
        opened.curr_states[3].location=4;engine.match(&opened,schedule);
        require(schedule==opened.curr_task_schedule,"priced admission changed an opened task");
    }
    cfg.active_task_cap=2;cfg.hungarian_limit=1000;cfg.admission_price=100;
    Engine cap(cfg);cap.initialize(&env);cap.match(&env,schedule);
    require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==2,
            "optional columns bypassed the mandatory admission cap");
    cfg.admission_price=.5f;cfg.admission_price_steps=10;
    for(int limit:{0,1000}) {
        cfg.hungarian_limit=limit;Engine startup(cfg);startup.initialize(&env);
        env.curr_timestep=9;startup.match(&env,schedule);
        require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==1,
                "startup price expired before the declared boundary");
        env.curr_timestep=10;startup.match(&env,schedule);
        require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==2,
                "startup price did not expire while preserving the task cap");
    }
    env.curr_timestep=0;cfg.admission_price_steps=0;cfg.hungarian_limit=1000;
    cfg.active_task_cap=0;cfg.admission_price=0;env.task_pool.erase(0);env.task_pool.erase(1);env.task_pool.erase(2);
    Engine scarce(cfg);scarce.initialize(&env);scarce.match(&env,schedule);
    require(std::count_if(schedule.begin(),schedule.end(),[](int t){return t>=0;})==1,
            "priced admission with fewer tasks than robots lost a free assignment");
    env.task_pool.clear();scarce.match(&env,schedule);
    require(std::all_of(schedule.begin(),schedule.end(),[](int t){return t<0;}),"empty priced pool assigned a task");
    cfg=Config{};cfg.active_task_cap=22;cfg.admission_price=4;cfg.hungarian_limit=1000;
    cfg.admission_price_steps=51;
    cfg.fast_admission=true;cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;
    cfg.depth=6;cfg.random_by_step=true;cfg.share_prefix=true;cfg.cost_cache=true;
    const auto signature=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(signature==simulate(cfg,12),"priced admission changed with worker scheduling");
    const char* old=std::getenv("R05_ADMISSION_PRICE");const bool present=old;
    const std::string saved=old?old:"";setenv("R05_ADMISSION_PRICE","4",1);
    SharedEnvironment gate;bool rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"priced admission escaped the explicit trick flag");
    gate.trick_instance="RANDOM-04";require(Config::environment(gate).admission_price==4,"priced admission parser changed the price");
    setenv("R05_ADMISSION_PRICE","-.5",1);rejected=false;
    try{Config::environment(gate);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"priced admission accepted an invalid negative price");
    if(present)setenv("R05_ADMISSION_PRICE",saved.c_str(),1);else unsetenv("R05_ADMISSION_PRICE");
}

void blocker_priority_mutations() {
    auto env=environment(5,5,4);Config cfg;Graph graph(env,cfg);
    Task task;task.locations={4};Chain chain(graph,task,true);
    Frame frame;frame.loc={0,1,2,24};frame.pending=frame.loc;frame.dir={0,0,0,0};frame.stage={0,0,0,0};
    std::vector<const Chain*> assigned{&chain,&chain,&chain,nullptr};
    auto edges=priority_dependencies(graph,frame,assigned,1);
    require(dependency_neighborhood(edges,0,10)==std::vector<int>({0,1,2}),
            "route dependency neighborhood crossed an empty cell or missed a blocking robot");
    require(dependency_neighborhood(edges,0,2)==std::vector<int>({0,1}),
            "route dependency neighborhood exceeded its declared size");
    require(dependency_neighborhood(edges,3,10)==std::vector<int>({3}),
            "unrelated idle robot was attached to a mutation group");
    Task chained;chained.locations={0,4};Chain arrival(graph,chained,true);assigned[0]=&arrival;
    frame.loc[0]=5;
    require(priority_dependencies(graph,frame,assigned,1)==edges,
            "dependency graph ignored promised occupancy or failed to advance a reached errand");
    cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=4;cfg.persist_elites=4;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    const auto ordinary=simulate(cfg,12);
    for(int degree:{1,2}) {
        cfg.blocker_mutation_size=8;cfg.blocker_mutation_period=2;cfg.blocker_mutation_edges=degree;
        cfg.threads=1;cfg.candidate_cache=false;cfg.shared_rankings_mb=0;
        const auto changed=simulate(cfg,12,5,5,true);
        require(changed!=ordinary,"dependency mutation fixture did not exercise different priorities");
        cfg.threads=3;cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.shared_rankings_mb=4;
        require(changed==simulate(cfg,12),"dependency mutation changed with caches or workers");
    }
}

void physical_guidance_edges() {
    auto env=environment(5,5,4);env.map[12]=1;
    Config cfg;cfg.guidance="lanes";cfg.turn_cost=.6f;cfg.goal_cache=true;
    Graph original(env,cfg);cfg.guidance_edge_mix=.25f;Graph mixed(env,cfg);
    for(int v=0;v<mixed.cells;++v)for(int d=0;d<5;++d)
        require(mixed.weight[v][d]==.75f*original.weight[v][d]+.5f,"physical edge mixture changed a price incorrectly");
    // The mixed edge model must still obey the exact one-step Bellman relation,
    // including required rotations, walls and prescribed arrival headings.
    for(int target=0;target<mixed.states;++target)for(int source=0;source<mixed.states;++source) {
        if(target==source){require(mixed.dist(target,source)==0,"nonzero diagonal distance");continue;}
        int v=source/4,d=source%4;float best=1e30f;
        int u=mixed.next[v][d];
        if(u>=0)best=std::min(best,mixed.weight[v][d]+mixed.dist(target,u*4+d));
        for(int q:{(d+1)%4,(d+3)%4})best=std::min(best,mixed.weight[v][4]+mixed.dist(target,v*4+q));
        require(std::abs(best-mixed.dist(target,source))<1e-4f,"mixed edge distance violates Bellman consistency");
    }
    cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.random_by_step=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;cfg.guided_matching=true;
    const auto base=simulate(cfg,12,5,5,true);cfg.cost_cache=true;cfg.candidate_cache=true;cfg.shared_rankings_mb=4;cfg.threads=3;
    require(base==simulate(cfg,12),"physical edge mixtures changed with caches or workers");
}

void mixed_guidance_potentials() {
    auto env=environment(5,5,4);env.map[12]=1;
    Config cfg;cfg.guidance="lanes";cfg.turn_cost=.6f;cfg.goal_cache=true;
    Graph original(env,cfg),mixed(env,cfg);auto physical_cfg=cfg;physical_cfg.guidance="none";physical_cfg.turn_cost=2;
    Graph physical(env,physical_cfg);mixed.blend_distances(physical,.25f,2);
    require(mixed.weight==original.weight,"mixing potentials changed lane edge prices");
    for(int target=0;target<mixed.cells;++target)for(int source=0;source<mixed.states;++source) {
        float best=1e30f;
        for(int d=0;d<4;++d) {
            const float expected=.75f*original.dist(target*4+d,source)+.25f*physical.dist(target*4+d,source);
            require(mixed.dist(target*4+d,source)==expected,"oriented potential mixture is incorrect");
            best=std::min(best,expected);
        }
        require(mixed.approach(target,source)==best,"goal cache retained the old potential or blended incompatible arrival headings");
    }
    cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;cfg.random_by_step=true;
    cfg.guidance_distance_mix=.25f;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    cfg.guided_matching=true;const auto uncached=simulate(cfg,12,5,5,true);
    cfg.cost_cache=true;cfg.candidate_cache=true;cfg.shared_rankings_mb=4;cfg.threads=3;
    require(uncached==simulate(cfg,12),"mixed potentials changed with caches or workers");
    struct Setting {
        std::string key,old;bool present;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);require(setenv(k,v,1)==0,"cannot configure mixed potential fixture");
        }
        ~Setting(){if(present)setenv(key.c_str(),old.c_str(),1);else unsetenv(key.c_str());}
    };
    Setting mixture("R05_GUIDANCE_DISTANCE_MIX","0.25"),guidance("R05_GUIDANCE","lanes");
    env.trick_instance="RANDOM-04";
    require(Config::environment(env).guidance_distance_mix==.25f,"mixed potential parser lost the fraction");
    env.trick_instance.clear();bool rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"mixed weighted guidance escaped the trick gate");
    env.trick_instance="RANDOM-04";Setting window("R05_WINDOW","8");rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"mixed potentials escaped the windowed-search guard");
}

void optional_immediate_moves() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    const auto ordinary=simulate(cfg,12);bool changed=false;
    for(int period:{1,2,4}) {
        cfg.early_root_period=period;cfg.threads=1;cfg.candidate_cache=false;cfg.kinematic_mask=false;
        const auto selected=simulate(cfg,12,5,5,true);changed|=selected!=ordinary;
        cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.threads=3;
        require(selected==simulate(cfg,12),"optional immediate moves changed with cache or workers");
        cfg.share_prefix=false;
        require(selected==simulate(cfg,12),"shared continuations changed optional first moves");
        cfg.share_prefix=true;
    }
    require(changed,"optional immediate moves never affected the dense fixture");
    cfg.futures=192;cfg.screen_branches=2;cfg.screen_keep=2;cfg.threads=1;
    const auto screened=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(screened==simulate(cfg,12),"screened immediate-move proposals changed with worker scheduling");
    cfg.rescore_roots=2;cfg.rescore_branches=4;cfg.threads=1;
    const auto rescored=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(rescored==simulate(cfg,12),"rescoring substituted an optional immediate-move decision");
}

void joint_assignment_semantics() {
    Config cfg;
    {auto env=environment(1,2,2);Graph g(env,cfg);Frame f;
     f.loc={0,1};f.pending=f.loc;f.dir={0,2};f.stage={0,0};
     Task x;x.task_id=0;x.locations={1};Task y;y.task_id=1;y.locations={0};
     Chain a(g,x,false),b(g,y,false);
     auto move=joint_move_assignment(g,cfg,f,{&a,&b},0);
     require(move.targets==f.loc && move.canceled_swaps==1,"joint assignment executed a head-on swap");
     cfg.joint_repair_rounds=4;move=joint_move_assignment(g,cfg,f,{&a,&b},0);
     require(move.targets==f.loc && move.canceled_swaps==0 && move.repaired_swaps==1,
             "joint repair failed to forbid an unavoidable swap while keeping both waits");
     cfg.joint_repair_rounds=0;}
    {auto env=environment(2,2,4);Graph g(env,cfg);Frame f;
     f.loc={0,1,3,2};f.pending=f.loc;f.dir={0,1,2,3};f.stage.assign(4,0);
     std::vector<std::unique_ptr<Chain>> storage;std::vector<const Chain*> tasks;
     const std::vector<int> targets={1,3,2,0};
     for(int a=0;a<4;++a){Task t;t.task_id=a;t.locations={targets[a]};
       storage.push_back(std::make_unique<Chain>(g,t,false));tasks.push_back(storage.back().get());}
     auto move=joint_move_assignment(g,cfg,f,tasks,0);
     require(move.targets==targets && move.canceled_swaps==0,"joint assignment lost a legal simultaneous cycle");}
    {auto env=environment(1,3,2);Graph g(env,cfg);Frame f;
     f.loc={0,1};f.pending=f.loc;f.dir={0,0};f.stage={0,0};
     Task t;t.task_id=0;t.locations={2};Chain chain(g,t,false);
     auto move=joint_move_assignment(g,cfg,f,{&chain,nullptr},0);
     require(move.targets==std::vector<int>({1,2}),"joint assignment lost a push chain into a hole");}
    {auto env=environment(3,3,1);Graph g(env,cfg);Frame f;
     f.loc={3};f.pending={4};f.dir={0};f.stage={0};
     Task t;t.task_id=0;t.locations={1};Chain chain(g,t,false);
     auto move=joint_move_assignment(g,cfg,f,{&chain},0);
     require(move.headings[0]==0 && (move.targets[0]==4 || move.targets[0]==5),
             "joint assignment rotated a robot during its promised forward action");}
    struct Setting {
        const char* key;bool present;std::string old;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);setenv(k,v,1);
        }
        ~Setting(){if(present)setenv(key,old.c_str(),1);else unsetenv(key);}
    };
    auto env=environment(3,3,1);Setting proposal("R05_JOINT_PROPOSALS","1");
    require(Config::environment(env).joint_proposals==1,"general joint proposal option rejected");
    {Setting repair("R05_JOINT_REPAIR_ROUNDS","9");bool rejected=false;
     try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
     require(rejected,"joint repair escaped its bounded matching work");}
    {Setting repair("R05_JOINT_REPAIR_ROUNDS","2");
     require(Config::environment(env).joint_repair_rounds==2,"valid joint repair rejected");}

    {Setting groups("R05_JOINT_GROUPS","129");bool rejected=false;
     try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
     require(rejected,"joint components escaped their complete-work bound");}
    {Setting groups("R05_JOINT_GROUPS","16");
     require(Config::environment(env).joint_groups==16,"valid joint component budget rejected");
     Setting off("R05_JOINT_PROPOSALS","0");bool rejected=false;
     try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
     require(rejected,"joint components accepted without a proposal source");}
    {Setting value("R05_JOINT_PROPOSALS","3");bool rejected=false;
     try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
     require(rejected,"joint proposal budget escaped its declared bound");}
    {Setting window("R05_WINDOW","8");bool rejected=false;
     try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
     require(rejected,"joint proposal accepted an unsupported temporal model");}
}

void joint_assignment_futures() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    for(int count:{1,2}) {
        cfg.joint_proposals=count;cfg.joint_repair_rounds=count==2?4:0;cfg.joint_groups=count==2?8:0;cfg.threads=1;cfg.candidate_cache=false;cfg.kinematic_mask=false;
        cfg.reverse_penalty=count==2?.2f:0.f;
        const auto selected=simulate(cfg,12,5,5,true);
        cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.threads=3;
        require(selected==simulate(cfg,12),"joint proposals changed with caches or workers");
        cfg.share_prefix=false;
        require(selected==simulate(cfg,12),"joint proposal changed with shared futures");
        cfg.share_prefix=true;
    }
    cfg.futures=192;cfg.screen_branches=2;cfg.screen_keep=2;cfg.threads=1;
    const auto screened=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(screened==simulate(cfg,12),"joint proposals changed with screened worker scheduling");
    cfg.joint_repair_rounds=0;cfg.joint_groups=32;cfg.threads=1;
    const auto components=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(components==simulate(cfg,12),"unrepaired joint components changed across workers");
}

void idle_alignment() {
    // No orders: prepare a legal heading while keeping the promised wait.
    Config cfg;cfg.futures=1;cfg.depth=1;cfg.wait_cost=.5f;cfg.idle_align=true;
    auto env=environment(1,3,1);env.curr_states[0].location=2;env.curr_states[0].orientation=0;
    Engine aligned(cfg);aligned.initialize(&env);
    std::vector<Action> plan;std::vector<int> schedule;
    for(int t=0;t<3;++t) {
        env.curr_timestep=t;aligned.compute(&env,plan,schedule);
        require(plan[0]==(t<2?CR:W),"idle alignment skipped a required quarter turn or kept rotating");
        require(aligned.checkpoint(env).at("pending").get<std::vector<int>>()==std::vector<int>({2}),"idle alignment changed the promised wait");
        require(schedule==std::vector<int>({-1}),"idle alignment invented an order");
        if(plan[0]==CR)env.curr_states[0].orientation=(env.curr_states[0].orientation+1)%4;
        env.curr_task_schedule=schedule;
    }
    cfg.idle_align=false;Engine old(cfg);env.curr_states[0].orientation=0;env.curr_timestep=0;
    old.initialize(&env);old.compute(&env,plan,schedule);
    require(plan[0]==W,"default idle policy changed");
    // Active orders retain their goal-based heading and immutable assignment.
    Task task;task.task_id=0;task.locations={2,0};task.idx_next_loc=1;
    env.task_pool[0]=task;env.curr_task_schedule[0]=0;
    Engine active_old(cfg);active_old.initialize(&env);active_old.compute(&env,plan,schedule);
    const auto active_plan=plan;cfg.idle_align=true;Engine active_new(cfg);
    active_new.initialize(&env);active_new.compute(&env,plan,schedule);
    require(plan==active_plan && schedule==std::vector<int>({0}),"idle alignment changed an active order");
    // Admitted and goal-less robots interact throughout dense task turnover.
    cfg=Config{};cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    cfg.idle_align=true;cfg.active_task_cap=8;cfg.wait_cost=.5f;cfg.guidance="lanes";
    const auto serial=simulate(cfg,12,5,5,true);
    cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.threads=3;
    require(serial==simulate(cfg,12),"idle alignment changed with caches or workers");
    cfg.share_prefix=true;
    require(serial==simulate(cfg,12),"idle alignment changed with shared futures");
}

void optional_arrival_proposals() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    const auto ordinary=simulate(cfg,12);bool changed=false;
    cfg.arrival_priority=200;
    for(int period:{1,2,4}) {
        cfg.arrival_root_period=period;cfg.threads=1;cfg.candidate_cache=false;cfg.kinematic_mask=false;
        const auto selected=simulate(cfg,12,5,5,true);changed|=selected!=ordinary;
        cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.threads=3;
        require(selected==simulate(cfg,12),"arrival proposal changed with cache or workers");
        cfg.share_prefix=false;
        require(selected==simulate(cfg,12),"shared prefix changed arrival proposal decisions");
        cfg.share_prefix=true;
    }
    require(changed,"arrival portfolio did not affect the dense fixture");
    cfg.futures=192;cfg.screen_branches=2;cfg.screen_keep=2;cfg.threads=1;
    const auto screened=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(screened==simulate(cfg,12),"screened arrival proposals changed with workers");
    cfg.rescore_roots=2;cfg.rescore_branches=4;cfg.threads=1;
    const auto rescored=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(rescored==simulate(cfg,12),"rescoring changed an arrival proposal decision");
}

void remaining_work_priorities() {
    Config cfg;cfg.futures=64;cfg.continuations=4;cfg.continuation_start=2;cfg.depth=6;
    cfg.generations=2;cfg.elites=2;cfg.persist_elites=2;cfg.random_by_step=true;
    cfg.cost_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;cfg.hungarian_limit=1000;
    const auto baseline=simulate(cfg,12);
    cfg.priority_remaining_weight=2;cfg.priority_remaining_steps=68;
    const auto biased=simulate(cfg,12,5,5,true);
    require(biased!=baseline,"remaining-work priorities did not exercise the startup preference");
    cfg.candidate_cache=true;cfg.kinematic_mask=true;cfg.threads=3;
    require(biased==simulate(cfg,12),"priority preference changed with cache or workers");
    struct Setting {
        std::string key,old;bool present;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);
            require(setenv(k,v,1)==0,"cannot configure priority fixture");
        }
        ~Setting(){if(present)setenv(key.c_str(),old.c_str(),1);else unsetenv(key.c_str());}
    };
    Setting weight("R05_PRIORITY_REMAINING","2"),steps("R05_PRIORITY_REMAINING_STEPS","200");
    auto env=environment(5,5,4);env.trick_instance="RANDOM-04";
    require(Config::environment(env).priority_remaining_steps==200,"startup priority parser lost its boundary");
    env.trick_instance.clear();bool rejected=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"remaining-work priority escaped the explicit trick gate");
}

void horizon_aware_matching() {
    auto env=environment(1,5,1);
    Task a;a.task_id=7;a.locations={0,4,0};env.task_pool[7]=a;
    Task b;b.task_id=8;b.locations={3,3};env.task_pool[8]=b;
    Config cfg;cfg.hungarian_limit=1000;cfg.keep_bonus=0;cfg.horizon=4;cfg.triage_scale=1;
    std::vector<int> schedule;Engine ordinary(cfg);ordinary.initialize(&env);ordinary.match(&env,schedule);
    require(schedule==std::vector<int>({7}),"horizon matching fixture did not prefer the nearer long chain");
    cfg.match_horizon_weight=1;Engine finishable(cfg);finishable.initialize(&env);finishable.match(&env,schedule);
    require(schedule==std::vector<int>({8}),"horizon-aware matching did not choose the shorter total trip");
    cfg=Config{};cfg.horizon=180;cfg.match_horizon_weight=1;cfg.futures=32;cfg.continuations=4;
    cfg.cost_cache=true;cfg.candidate_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;
    cfg.random_by_step=true;cfg.hungarian_limit=1000;
    const auto signature=simulate(cfg,12,5,5,true);cfg.threads=3;
    require(signature==simulate(cfg,12),"horizon matching changed with worker scheduling");
    struct Setting {
        std::string key,old;bool present;
        Setting(const char* k,const char* v):key(k),present(std::getenv(k)!=nullptr) {
            if(present)old=std::getenv(k);require(setenv(k,v,1)==0,"cannot configure horizon fixture");
        }
        ~Setting(){if(present)setenv(key.c_str(),old.c_str(),1);else unsetenv(key.c_str());}
    };
    Setting horizon("R05_HORIZON","180"),weight("R05_MATCH_HORIZON_WEIGHT","1");
    auto configured=environment(5,5,4);configured.trick_instance="RANDOM-04";
    require(Config::environment(configured).match_horizon_weight==1,"horizon matching parser rejected an explicit trick");
    configured.trick_instance.clear();bool rejected=false;
    try{Config::environment(configured);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"horizon matching escaped the explicit trick gate");
    configured.trick_instance="RANDOM-04";setenv("R05_HORIZON","0",1);rejected=false;
    try{Config::environment(configured);}catch(const std::invalid_argument&){rejected=true;}
    require(rejected,"horizon matching accepted an unknown end");
}

void destination_demand_matching() {
    auto env=environment(1,5,2);env.curr_states[1].location=2;
    Task locked;locked.task_id=7;locked.locations={0,1};locked.idx_next_loc=1;locked.agent_assigned=0;
    env.task_pool[7]=locked;env.curr_task_schedule={7,-1};
    Task a;a.task_id=8;a.locations={1};env.task_pool[8]=a;
    Task b;b.task_id=9;b.locations={3};env.task_pool[9]=b;
    Config cfg;cfg.keep_bonus=0;Engine ordinary(cfg);ordinary.initialize(&env);
    std::vector<int> schedule;ordinary.match(&env,schedule);
    require(schedule==std::vector<int>({7,8}),"destination demand fixture did not establish equal approach costs");
    cfg.destination_load=4;cfg.hungarian_limit=1000;Engine balanced(cfg);balanced.initialize(&env);
    balanced.match(&env,schedule);
    require(schedule==std::vector<int>({7,9}),"destination demand did not avoid the busy goal while preserving the opened task");
    cfg=Config{};cfg.futures=32;cfg.continuations=4;cfg.continuation_start=2;
    cfg.cost_cache=true;cfg.candidate_cache=true;cfg.share_prefix=true;cfg.scratch_reuse=true;
    cfg.random_by_step=true;cfg.hungarian_limit=1000;cfg.destination_load=4;
    const auto signature=simulate(cfg,12,5,5,true);cfg.threads=2;
    require(signature==simulate(cfg,12),"destination-aware matching changed with worker scheduling");
}

void guidance_reversal() {
    auto env=environment(5,5,4);env.map[7]=1;env.map[17]=1;
    Config cfg;cfg.guidance="flow";cfg.flow_iterations=3;cfg.flow_normalize=true;cfg.flow_flips=2;
    Graph forward(env,cfg);cfg.flow_reverse=true;Graph reverse(env,cfg);
    bool changed=false;
    for(int v=0;v<forward.cells;++v) {
        require(forward.weight[v][4]==reverse.weight[v][4],"reversing guidance changed turn costs");
        for(int d=0;d<4;++d)if(forward.next[v][d]>=0) {
            const int u=forward.next[v][d];
            require(reverse.next[v][d]==u && reverse.weight[v][d]==forward.weight[u][(d+2)%4],
                    "global guidance reversal changed topology or failed to swap edge prices");
            changed|=forward.weight[v][d]!=reverse.weight[v][d];
        }
    }
    require(changed,"guidance reversal fixture did not exercise directional prices");
}

void window_configuration() {
    struct Setting {
        std::string key,old;bool present;
        Setting(const char* name,const char* value):key(name),present(std::getenv(name)!=nullptr) {
            if(present)old=std::getenv(name);
            require(setenv(name,value,1)==0,"could not set configuration fixture");
        }
        ~Setting(){if(present)setenv(key.c_str(),old.c_str(),1);else unsetenv(key.c_str());}
    };
    Setting window("R05_WINDOW","8"),rank("R05_SCORE_RANK_POWER","0.5"),startup("R05_SCORE_RANK_STEPS","20");
    {
        Setting progress("R05_WINDOW_PROGRESS_TIE","1");
        auto general=environment(5,5,4);general.trick_instance="RANDOM-03";
        require(Config::environment(general).window_progress_tie,"window progress tie option was not accepted");
        Setting disabled("R05_WINDOW","0");bool rejected=false;
        try{Config::environment(general);}catch(const std::invalid_argument&){rejected=true;}
        require(rejected,"window progress ties accepted a missing window");
    }
    {
        Setting invalid("R05_WINDOW_PROGRESS_TIE","2");
        auto general=environment(5,5,4);general.trick_instance="RANDOM-03";bool rejected=false;
        try{Config::environment(general);}catch(const std::invalid_argument&){rejected=true;}
        require(rejected,"window progress ties accepted a non-boolean value");
    }
    {
        Setting orders("R05_WINDOW_REPAIR_ORDERS","2"),rank_off("R05_SCORE_RANK_POWER","0"),startup_off("R05_SCORE_RANK_STEPS","0");
        auto general=environment(5,5,4);
        require(Config::environment(general).window_repair_orders==2,"general two-order repair was rejected");
        Setting disabled("R05_WINDOW","0");bool rejected=false;
        try{Config::environment(general);}catch(const std::invalid_argument&){rejected=true;}
        require(rejected,"two-order repair accepted a missing window");
    }
    {
        Setting merge("R05_WINDOW_SEED_MERGE","1"),rank_off("R05_SCORE_RANK_POWER","0"),startup_off("R05_SCORE_RANK_STEPS","0");
        auto general=environment(5,5,4);
        require(Config::environment(general).window_seed_merge,"general seed merging was rejected");
        Setting disabled("R05_WINDOW","0");bool rejected=false;
        try{Config::environment(general);}catch(const std::invalid_argument&){rejected=true;}
        require(rejected,"seed merging accepted a missing window");
    }
    {
        Setting merge("R05_WINDOW_SEED_MERGE","2");
        auto general=environment(5,5,4);bool rejected=false;
        try{Config::environment(general);}catch(const std::invalid_argument&){rejected=true;}
        require(rejected,"seed merging accepted a non-boolean value");
    }
    for(const char* value:{"0","3"}) {
        Setting orders("R05_WINDOW_REPAIR_ORDERS",value);
        auto general=environment(5,5,4);bool rejected=false;
        try{Config::environment(general);}catch(const std::invalid_argument&){rejected=true;}
        require(rejected,"window repair accepted an unsupported order budget");
    }
    auto env=environment(5,5,4);env.trick_instance="RANDOM-03";
    const auto cfg=Config::environment(env);
    require(cfg.window==8 && cfg.score_rank_power==.5 && cfg.score_rank_steps==20,
            "explicit weighted window configuration was not accepted");
    env.trick_instance.clear();bool refused=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){refused=true;}
    require(refused,"task-priority window scoring bypassed its explicit trick gate");
    env.trick_instance="RANDOM-03";Setting rematch("R05_ROLLOUT_MATCH","1");refused=false;
    try{Config::environment(env);}catch(const std::invalid_argument&){refused=true;}
    require(refused,"weighted window allowed unsupported future reassignment");
}

void window_components() {
    auto e=environment(3,5,3);Config cfg;Graph g(e,cfg);
    using Paths=std::vector<std::vector<int>>;
    auto certify_mix=[&](const Paths& left,const Paths& right) {
        auto groups=window_conflict_components(g,left,right);
        require(groups.size()<8,"unexpected component count in small fixture");
        for(unsigned mask=0;mask<(1u<<groups.size());++mask) {
            Paths paths=left;
            for(size_t k=0;k<groups.size();++k)if(mask&(1u<<k))
                for(int a:groups[k])paths[a]=right[a];
            for(size_t t=1;t<paths[0].size();++t) {
                std::vector<int> from,to;
                for(const auto& path:paths){from.push_back(path[t-1]/4);to.push_back(path[t]/4);}
                Engine::certify(g,from,to);
            }
        }
        return groups;
    };
    // Two route choices compete for a cell only near the end of the window;
    // a third robot's improvement remains independently composable.
    Paths left={{0,4,8},{18,18,14},{40,40,40}};
    Paths right={{0,0,4},{18,14,10},{40,44,48}};
    const auto groups=certify_mix(left,right);
    require(groups==std::vector<std::vector<int>>({{0,1},{2}}),"late vertex conflict or independent route was grouped incorrectly");
    // No cross-parent vertex conflict, but mixing these two individually legal
    // plans would exchange cells6 and7 in opposite directions at timestep2.
    left={{24,24,28},{30,31,11}};right={{24,25,45},{30,30,26}};
    require(certify_mix(left,right)==std::vector<std::vector<int>>({{0,1}}),
            "cross-parent reverse edge was split across components");
    require(window_conflict_components(g,left,left).empty(),"identical windows produced changed components");
}

void window_single_agent() {
    // Repeated waypoints consume separate simulator ticks. LNS can also move
    // immediately, instead of inheriting the pipeline's initial idle action.
    auto e=environment(1,7,1);e.curr_states[0].orientation=0;
    Task task;task.task_id=1;task.locations={3,3,4};e.task_pool[1]=task;
    Config cfg;cfg.window=12;cfg.window_islands=2;cfg.window_iterations=4;
    cfg.window_neighborhood=1;cfg.threads=2;cfg.cost_cache=true;cfg.wait_cost=0.5;
    Engine engine(cfg);engine.initialize(&e);int completed=-1;
    for(int t=0;t<10;++t) {
        e.curr_timestep=t;std::vector<Action> plan;std::vector<int> schedule;
        engine.compute(&e,plan,schedule);
        if(t==0)require(plan[0]==FW,"window repair retained unnecessary initial idle");
        auto& state=e.curr_states[0];
        if(plan[0]==FW)state.location=engine.graph->next[state.location][state.orientation];
        else if(plan[0]==CR)state.orientation=(state.orientation+1)%4;
        else if(plan[0]==CCR)state.orientation=(state.orientation+3)%4;
        e.curr_task_schedule=schedule;
        auto& active=e.task_pool.at(1);
        if(active.locations[active.idx_next_loc]==state.location) {
            ++active.idx_next_loc;
            if(active.idx_next_loc==int(active.locations.size())){completed=t+1;break;}
        }
    }
    require(completed==5,"window task-chain planner mishandled repeated waypoints");
}
void window_local_goal_optimum() {
    // Exhaustive finite-horizon dynamic programming independently checks the
    // bounded A* repair and complete-plan evaluator on a changing goal chain.
    // Both repeated goals and direction-dependent local prices are exercised.
    auto e=environment(5,5,1);e.trick_instance="RANDOM-03";
    e.curr_states[0].orientation=0;
    Task task;task.task_id=1;task.locations={24,24,2};e.task_pool[1]=task;
    Config cfg;cfg.guidance="lanes";cfg.goal_local_radius=3;cfg.goal_local_mix=.75;
    cfg.window=12;cfg.window_keep=3;cfg.window_islands=1;cfg.window_iterations=1;
    cfg.window_neighborhood=1;cfg.window_expansions=50000;cfg.threads=1;
    cfg.wait_cost=2;cfg.turn_cost=2;cfg.cost_cache=true;
    Engine engine(cfg);engine.initialize(&e);std::vector<Action> actions;std::vector<int> schedule;
    engine.compute(&e,actions,schedule);
    require(schedule==std::vector<int>{1},"single-agent local guidance changed the assignment");
    const auto& g=*engine.graph;Chain chain(g,task);
    const int stages=int(chain.goals.size())+1,states=g.states;
    std::vector<double> costs(stages*states,1e30),next(costs.size());
    costs[e.curr_states[0].location*4+e.curr_states[0].orientation]=0;
    for(int tick=0;tick<cfg.window;++tick) {
        std::fill(next.begin(),next.end(),1e30);
        for(int stage=0;stage<stages;++stage)for(int state=0;state<states;++state) {
            const double previous=costs[stage*states+state];if(previous>=1e29)continue;
            const bool active=stage<stages-1;const int goal=active?chain.goals[stage]:-1;
            const int cell=state/4,dir=state%4,forward=g.next[cell][dir];
            const std::array<int,4> destinations={state,cell*4+(dir+1)%4,
                cell*4+(dir+3)%4,forward<0?-1:forward*4+dir};
            for(int dest:destinations)if(dest>=0) {
                double price=dest==state?(active?cfg.wait_cost:0):
                    dest/4==cell?g.weight[cell][4]:g.forward_weight(goal,cell,dir);
                int future_stage=stage+(active && dest/4==goal);
                auto& best=next[future_stage*states+dest];best=std::min(best,previous+price);
            }
        }
        costs.swap(next);
    }
    double optimum=1e30;
    for(int stage=0;stage<stages;++stage)for(int state=0;state<states;++state)
        optimum=std::min(optimum,costs[stage*states+state]+chain.cost(g,stage,state/4,state%4));
    const auto paths=engine.checkpoint(e).at("window_paths").get<std::vector<std::vector<int>>>();
    const auto& path=paths.at(0);require(path.size()==size_t(cfg.window+1),"local guidance lost its full window");
    double actual=0;int stage=0;
    for(size_t tick=1;tick<path.size();++tick) {
        const int from=path[tick-1],to=path[tick];const bool active=stage<stages-1;
        const int goal=active?chain.goals[stage]:-1;
        actual+=to==from?(active?cfg.wait_cost:0):to/4==from/4?g.weight[from/4][4]:
            g.forward_weight(goal,from/4,from%4);
        if(active && to/4==goal)++stage;
    }
    actual+=chain.cost(g,stage,path.back()/4,path.back()%4);
    require(std::abs(actual-optimum)<1e-4,"goal-local window repair differs from exhaustive optimum");
}

void window_reproducibility() {
    Config cfg;cfg.window=8;cfg.window_keep=3;cfg.window_islands=4;
    cfg.window_iterations=3;cfg.window_neighborhood=5;cfg.window_expansions=800;
    cfg.futures=4;cfg.threads=1;cfg.cost_cache=true;cfg.scratch_reuse=true;
    const auto reference=simulate(cfg,8,5,5,true);
    cfg.threads=2;require(reference==simulate(cfg,8),"parallel window repairs changed the full trajectory");
    cfg.cost_cache=false;require(reference==simulate(cfg,8),"window search depends on cost caching");
    cfg.cost_cache=true;cfg.window_reuse=true;
    require(reference==simulate(cfg,8,5,5,true),"reused window search storage changed paths or checkpoint replay");
    cfg.window_heap4=true;
    require(reference==simulate(cfg,8,5,5,true),"four-way search heap changed complete paths or checkpoint replay");
    cfg.window_heap4=false;
    cfg.window_rounds=3;cfg.window_iterations=12;
    const auto shared=simulate(cfg,8,5,5,true);
    cfg.threads=1;require(shared==simulate(cfg,8),"window sharing rounds depend on worker scheduling");
    cfg.window_reuse=false;require(shared==simulate(cfg,8),"window sharing depends on reused search storage");
    cfg.window_temperature=2;
    const auto annealed=simulate(cfg,8,5,5,true);
    cfg.threads=2;require(annealed==simulate(cfg,8),"annealed window repairs depend on worker scheduling");
    cfg.cost_cache=false;require(annealed==simulate(cfg,8),"annealed window repairs depend on cost caching");
    cfg.window_heap4=true;
    require(annealed==simulate(cfg,8,5,5,true),"four-way heap changed annealed repairs or checkpoint replay");
    cfg.window_heap4=false;
    cfg.window_temperature=0;cfg.window_first_iterations=3;cfg.window_initial_steps=4;
    const auto first_budget=simulate(cfg,8,5,5,true);
    cfg.threads=1;require(first_budget==simulate(cfg,8),"fixed startup repair budget depends on worker count");
    cfg.window_first_iterations=0;cfg.window_initial_steps=1;
    cfg.window_rounds=1;cfg.window_iterations=3;cfg.threads=2;
    cfg.window_blockers=true;cfg.window_equal=true;cfg.window_starts=4;cfg.cost_cache=true;
    const auto linked=simulate(cfg,8,5,5,true);
    cfg.threads=1;require(linked==simulate(cfg,8),"blocker-based window repairs depend on worker count");
    cfg.cost_cache=false;require(linked==simulate(cfg,8),"blocker guides depend on cost caching");
    cfg.window_fast_groups=true;
    require(linked==simulate(cfg,8,5,5,true),"skipping unused spatial sorts changed repairs or checkpoint replay");
    cfg.threads=2;cfg.cost_cache=true;
    require(linked==simulate(cfg,8),"fast blocker groups changed across workers or cost caching");
    cfg.window_heap4=true;
    require(linked==simulate(cfg,8,5,5,true),"four-way heap changed blocker repairs or checkpoint replay");
    cfg.window_blocker_rotation=1;
    const auto rotated=simulate(cfg,8,5,5,true);
    cfg.threads=1;cfg.window_fast_groups=false;
    require(rotated==simulate(cfg,8),"rotated blocker groups changed across workers or spatial-sort optimization");
    cfg.window_blocker_rotation=2;
    const auto mixed=simulate(cfg,8,5,5,true);
    cfg.threads=2;cfg.window_fast_groups=true;
    require(mixed==simulate(cfg,8),"mixed blocker neighborhoods depend on worker scheduling");
    cfg.window_merge=true;cfg.window_rounds=3;cfg.window_iterations=12;
    const auto merged=simulate(cfg,8,5,5,true);
    cfg.threads=1;cfg.window_heap4=false;
    require(merged==simulate(cfg,8),"window component merging changed with heap layout or worker scheduling");
    cfg.score_rank_power=.5;cfg.score_rank_steps=51;
    const auto weighted=simulate(cfg,8,5,5,true);
    cfg.threads=2;cfg.cost_cache=false;
    require(weighted==simulate(cfg,8),"weighted window scoring changed with workers or cost caching");
    cfg.score_rank_power=0;cfg.score_rank_steps=0;
    cfg.window_progress_tie=true;cfg.cost_cache=true;cfg.window_heap4=true;
    const auto early_progress=simulate(cfg,8,5,5,true);
    cfg.threads=1;cfg.cost_cache=false;cfg.window_heap4=false;
    require(early_progress==simulate(cfg,8),"window progress ties depend on workers, cost caching or heap layout");
    cfg.threads=2;cfg.cost_cache=true;
    require(early_progress==simulate(cfg,8,5,5,true),"window progress ties changed after checkpoint restoration");
    cfg.window_temperature=2;
    const auto annealed_progress=simulate(cfg,8,5,5,true);
    cfg.threads=1;
    require(annealed_progress==simulate(cfg,8),"annealed progress ties depend on worker scheduling");
    cfg.window_progress_tie=false;cfg.window_temperature=0;
    cfg.guidance="lanes";cfg.goal_local_radius=2;cfg.goal_local_mix=.5;
    const auto local_guidance=simulate(cfg,8,5,5,true);
    cfg.threads=2;cfg.cost_cache=false;cfg.window_heap4=true;
    require(local_guidance==simulate(cfg,8),"goal-local windows depend on workers, chain caching or heap layout");
    cfg.threads=1;cfg.cost_cache=true;
    require(local_guidance==simulate(cfg,8,5,5,true),"goal-local windows changed after checkpoint restoration");
    cfg.guidance="none";cfg.goal_local_radius=0;cfg.goal_local_mix=0;
    cfg.window_repair_orders=2;cfg.window_progress_tie=true;
    const auto both_orders=simulate(cfg,8,5,5,true);
    cfg.threads=2;cfg.cost_cache=false;cfg.window_heap4=false;cfg.window_reuse=false;
    require(both_orders==simulate(cfg,8),"two-order repairs depend on worker count, cache, heap or reused search storage");
    cfg.threads=1;cfg.cost_cache=true;cfg.window_heap4=true;cfg.window_reuse=true;
    require(both_orders==simulate(cfg,8,5,5,true),"two-order repairs changed after checkpoint restoration");
    cfg.window_temperature=2;
    const auto annealed_orders=simulate(cfg,8,5,5,true);
    cfg.threads=2;
    require(annealed_orders==simulate(cfg,8),"annealed two-order repairs depend on worker scheduling");
    cfg.window_temperature=0;cfg.window_progress_tie=false;
    cfg.window_seed_merge=true;
    for(int starts:{1,4}) {
        cfg.window_starts=starts;cfg.threads=1;cfg.window_repair_orders=1;
        cfg.cost_cache=true;cfg.window_heap4=true;cfg.window_reuse=true;
        const auto combined_seed=simulate(cfg,8,5,5,true);
        cfg.threads=3;cfg.cost_cache=false;cfg.window_heap4=false;cfg.window_reuse=false;
        require(combined_seed==simulate(cfg,8),"seed merging depends on workers, caches, heap or search storage");
        require(combined_seed==simulate(cfg,8,5,5,true),"seed merging changed after checkpoint restoration");
    }
    cfg.window_repair_orders=2;cfg.window_iterations=0;
    const auto seed_only=simulate(cfg,8,5,5,true);
    cfg.threads=1;
    require(seed_only==simulate(cfg,8),"unoptimized legal seed merging depends on workers");
    cfg.window_seed_merge=false;
    cfg.window_expansions=1;cfg.window_iterations=3;
    const auto failed_repairs=simulate(cfg,8);
    cfg.window_iterations=0;
    require(failed_repairs==simulate(cfg,8),"failed window repairs damaged the complete fallback plan");
}

int main() {
    arrival_priority_semantics();
    optional_arrival_proposals();
    joint_assignment_semantics();joint_assignment_futures();idle_alignment();
    for(float bonus:{50.f,200.f}) {
        Config cfg;cfg.futures=8;cfg.depth=6;cfg.random_by_step=true;cfg.guidance="lanes";
        cfg.arrival_priority=bonus;cfg.threads=1;cfg.active_task_cap=18;
        const auto reference=simulate(cfg,12,5,5,true);
        cfg.threads=2;cfg.cost_cache=true;cfg.goal_cache=true;cfg.candidate_cache=true;
        cfg.shared_rankings_mb=16;cfg.shared_orders=true;cfg.kinematic_mask=true;
        require(reference==simulate(cfg,12),"arrival priority changed with caches or workers");
    }

    idle_displacement_price();
    for(float price:{.25f,1.f,2.f}) {
        Config cfg;cfg.futures=8;cfg.depth=6;cfg.random_by_step=true;cfg.guidance="lanes";
        cfg.push_price=price;cfg.threads=1;cfg.scratch_reuse=true;cfg.kinematic_mask=true;
        const auto reference=simulate(cfg,12,5,5,true);
        cfg.fast_push=true;
        require(reference==simulate(cfg,12),"cached displacement costs changed legacy decisions");
        cfg.cost_cache=true;cfg.goal_cache=true;cfg.candidate_cache=true;
        cfg.shared_rankings_mb=16;cfg.shared_orders=true;cfg.threads=2;
        require(reference==simulate(cfg,12),"shared cached displacement costs or workers changed legacy decisions");
    }
    for(int flags=1;flags<=3;++flags) {
        Config cfg;cfg.futures=8;cfg.depth=6;cfg.random_by_step=true;cfg.guidance="lanes";
        cfg.push_price=.5;cfg.fast_push=true;cfg.push_idle_free=flags&1;cfg.push_exclude_swap=flags&2;
        cfg.active_task_cap=18;cfg.threads=1;
        const auto reference=simulate(cfg,12,5,5,true);
        cfg.cost_cache=true;cfg.goal_cache=true;cfg.candidate_cache=true;cfg.kinematic_mask=true;
        cfg.shared_rankings_mb=16;cfg.shared_orders=true;cfg.threads=2;
        require(reference==simulate(cfg,12),"corrected displacement costs changed with caches or workers");
    }

    local_goal_guidance();
    {
        Config cfg;cfg.futures=8;cfg.depth=6;cfg.random_by_step=true;cfg.guidance="lanes";
        cfg.goal_local_radius=2;cfg.goal_local_mix=.5;cfg.threads=1;
        const auto reference=simulate(cfg,12,5,5,true);
        cfg.cost_cache=true;cfg.goal_cache=true;cfg.shared_rankings_mb=16;
        require(reference==simulate(cfg,12),"goal-local shared rankings changed decisions");
        cfg.shared_orders=true;cfg.threads=2;
        require(reference==simulate(cfg,12),"goal-local compact rankings or workers changed decisions");
    }

    feasible_move_proposals();
    rollout_elite_diversity();
    bounded_order_rankings();
    compact_prepared_rankings();
    active_travel_calibration();
    observed_progress_triage();
    rectangular_matching_optimality();
    exact_dummy_prefix();
    active_task_admission();
    priced_task_admission();
    nonlinear_progress_objective();
    deferred_task_admission_credit();
    blocker_priority_mutations();
    physical_guidance_edges();
    mixed_guidance_potentials();
    optional_immediate_moves();
    remaining_work_priorities();
    horizon_aware_matching();
    destination_demand_matching();
    guidance_reversal();
    window_configuration();
    window_components();
    window_single_agent();
    window_local_goal_optimum();
    window_reproducibility();
    move_proposal_bias();
    ranked_task_progress();
    independent_candidate_rescoring();
    waypoint_priority_retention();
    replanning_forecast();
    normalized_directional_triage();
    motion_component_search();
    checkpoint_replay();
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
    shared_task_rankings();
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
    retained_guidance_flips();
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
    nonlinear_matching();
    std::cout<<"All Random05 checks passed\n";
}
