#include "cgar.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
#include <thread>
#include <exception>
using namespace cgar;
int nb(int u,int d,int R,int C) {
 int r=u/C,c=u%C;
 if(d==0)return c+1<C?u+1:-1;
 if(d==1)return r+1<R?u+C:-1;
 if(d==2)return c>0?u-1:-1;
 return r>0?u-C:-1;
}
std::vector<State> step(const SharedEnvironment& e,const std::vector<State>& s,const std::vector<Action>& a) {
 auto out=s;std::set<int> occupied;
 for(size_t i=0;i<s.size();++i){
  if(a[i]==Action::FW)out[i].location=nb(s[i].location,s[i].orientation,e.rows,e.cols);
  if(a[i]==Action::CR)out[i].orientation=(s[i].orientation+1)%4;
  if(a[i]==Action::CCR)out[i].orientation=(s[i].orientation+3)%4;
  int v=out[i].location;
  if(v<0||e.map[v]||!occupied.insert(v).second)return {};
  for(size_t j=0;j<i;++j)if(v==s[j].location&&out[j].location==s[i].location)return {};
 }
 return out;
}
int exact_two_robot_solution(const SharedEnvironment& e,int g0,int g1) {
 using Key=std::array<int,4>;
 auto key=[](const std::vector<State>& s){return Key{s[0].location,s[0].orientation,s[1].location,s[1].orientation};};
 std::queue<std::pair<std::vector<State>,int>> q;std::set<Key> seen;
 q.push({e.curr_states,0});seen.insert(key(e.curr_states));
 while(!q.empty()){
  auto [s,d]=q.front();q.pop();
  if(s[0].location==g0&&s[1].location==g1)return d;
  for(int a=0;a<4;++a)for(int b=0;b<4;++b){
   auto t=step(e,s,{static_cast<Action>(a),static_cast<Action>(b)});
   if(!t.empty()&&seen.insert(key(t)).second)q.push({t,d+1});
  }
 }
 return -1;
}
void pocket_case(int length = 2){
 SharedEnvironment e;e.num_of_agents=2;e.rows=2;e.cols=length+2;e.map.assign(e.rows*e.cols,1);
 for(int i=0;i<e.cols;++i)e.map[i]=0;e.map[e.cols]=e.map[e.cols+1]=0;
 e.curr_states={State(length,0,0),State(length+1,0,2)};e.goal_locations={{{length+1,0}},{{length,0}}};e.curr_task_schedule={-1,-1};
 int exact=exact_two_robot_solution(e,length+1,length);
 if(exact<0)throw std::runtime_error("pocket fixture is unsolvable");
 Cgar c;c.initialize(&e,1000);std::vector<Action> a;int done=0;
 for(int t=0;t<300;++t){
  e.curr_timestep=t;c.plan(&e,100,a);auto next=step(e,e.curr_states,a);
  if(next.empty())throw std::runtime_error("invalid pocket action");
  e.curr_states=next;
  for(int i=0;i<2;++i)if(!e.goal_locations[i].empty()&&e.curr_states[i].location==e.goal_locations[i][0].first){++done;e.goal_locations[i].clear();}
  if(done==2)break;
 }
 if(done!=2)throw std::runtime_error("pocket goals did not finish");
 std::cout<<"POCKET_RESULT exact_joint_solution_steps="<<exact<<" cgar_goals_reached="<<done<<"/2 after_steps="<<e.curr_timestep+1<<" final_cells="<<e.curr_states[0].location<<","<<e.curr_states[1].location<<"\n";
}
void scheduler_case(){
 SharedEnvironment e;e.num_of_agents=10000;e.rows=101;e.cols=101;e.map.assign(e.rows*e.cols,0);
 e.curr_task_schedule.assign(e.num_of_agents,-1);e.goal_locations.resize(e.num_of_agents);
 for(int i=0;i<e.num_of_agents;++i){e.curr_states.emplace_back(i,0,0);Task task;task.task_id=i;task.t_revealed=0;task.locations={i};e.task_pool.emplace(i,task);}
 Cgar c;c.initialize(&e,1000);std::vector<int> proposed;auto start=std::chrono::steady_clock::now();c.schedule(&e,5000,proposed);
 int assigned=0;for(int t:proposed)assigned+=t>=0;
 auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
 if(assigned!=10000)throw std::runtime_error("scheduler omitted idle robots");
 std::set<int> unique(proposed.begin(),proposed.end());if(unique.size()!=10000)throw std::runtime_error("duplicate task assignment");
 std::cout<<"SCHEDULER_RESULT idle_robots=10000 available_tasks=10000 assigned="<<assigned<<" budget_ms=5000 elapsed_ms="<<ms<<"\n";
}
int reach(const Certificate& c,int start,int removed,bool promotion){
 std::vector<char> seen(c.core.size(),0);std::queue<int> q;q.push(start);seen[start]=1;int count=0;
 while(!q.empty()){int u=q.front();q.pop();++count;for(int d=0;d<4;++d){int v=nb(u,d,c.rows,c.cols);if(v<0||v==removed||!c.core[v]||seen[v]||(promotion&&!(c.promotion[u]>>d&1)))continue;seen[v]=1;q.push(v);}}
 return count;
}
void certificates(){
 std::mt19937 rng(13);int tested=0,valid=0;
 for(int k=0;k<200;++k){std::vector<int> map(30);int nfree=0;for(int& b:map){b=(rng()%5==0);nfree+=!b;}
  for(int n=1;n<nfree;n+=3){++tested;auto c=build_certificate(map,5,6,n);if(!c.valid)continue;++valid;
   if(c.core_size<=static_cast<size_t>(n))throw std::runtime_error("missing spare capacity");
   for(int u=0;u<30;++u)if(c.core[u]){
    if(reach(c,u,-1,true)!=static_cast<int>(c.core_size))throw std::runtime_error("promotion is not strongly connected");
    for(int d=0;d<4;++d)if(c.promotion[u]>>d&1){int v=nb(u,d,5,6);if(reach(c,v,u,false)<n)throw std::runtime_error("promotion lacks reach capacity");}
   }
  }
 }
 std::cout<<"CERTIFICATE_RESULT cases="<<tested<<" certified="<<valid<<" topology_checks=passed\n";
}

void persistent_primary() {
 SharedEnvironment e;e.num_of_agents=2;e.rows=2;e.cols=2;e.map.assign(4,0);
 e.curr_states={State(0,0,0),State(1,0,0)};e.goal_locations={{{3,0}},{{2,0}}};e.curr_task_schedule={0,1};
 for(int i=0;i<3;++i){Task t;t.task_id=i;t.locations={i==1?2:3};e.task_pool.emplace(i,t);}
 Cgar c;c.initialize(&e,1000);std::vector<Action>a;
 // Observations can remain unchanged while a previously offered move is delayed.
 for(int t=0;t<100;++t){e.curr_timestep=t;c.plan(&e,100,a);if(c.primary()!=0)throw std::runtime_error("pending primary was demoted");}
 e.curr_task_schedule[0]=2;c.plan(&e,100,a);
 if(c.primary()!=1)throw std::runtime_error("new task at same goal reused an old epoch");
 std::cout<<"PERSISTENT_PRIMARY passed\n";
}
void capacity_bootstrap() {
 SharedEnvironment e;e.num_of_agents=5;e.rows=2;e.cols=4;e.map={0,0,0,0,0,0,1,1};
 for(int cell:{0,1,4,5,3})e.curr_states.emplace_back(cell,0,0);
 e.curr_task_schedule.assign(5,-1);e.goal_locations.resize(5);
 for(int cell:{0,1,2,3,4,5}){Task t;t.task_id=cell;t.t_revealed=0;t.locations={cell};e.task_pool.emplace(cell,t);}
 Cgar c;c.initialize(&e,1000);std::vector<Action>a;std::vector<int> proposed;
 for(int t=0;t<20&&!c.active_certified();++t){
  e.curr_timestep=t;c.schedule(&e,100,proposed);c.plan(&e,100,a);
  auto next=step(e,e.curr_states,a);if(next.empty())throw std::runtime_error("invalid parking move");e.curr_states=next;
 }
 c.schedule(&e,100,proposed);
 if(!c.active_certified()||c.parked_count()!=2)throw std::runtime_error("capacity bootstrap failed");
 int assigned=0;
 for(int i=0;i<5;++i){int t=proposed[i];if(t>=0){++assigned;if(t==2||t==3||e.curr_states[i].location==2||e.curr_states[i].location==3)throw std::runtime_error("assigned parked robot or excluded task");}}
 if(assigned!=3)throw std::runtime_error("wrong active fleet size");
 std::cout<<"CAPACITY_BOOTSTRAP passed active=3 parked=2\n";
}
void fair_sparse_schedule() {
 setenv("CGAR_MAX_PAIRS","1",1);
 SharedEnvironment e;e.num_of_agents=2;e.rows=2;e.cols=3;e.map.assign(6,0);e.curr_timestep=20;
 e.curr_states={State(0,0,0),State(1,0,0)};e.curr_task_schedule={-1,-1};e.goal_locations.resize(2);
 for(int i=0;i<4;++i){Task t;t.task_id=i;t.t_revealed=10;t.locations={i%2};e.task_pool.emplace(i,t);}
 Task old;old.task_id=100;old.t_revealed=0;for(int i=0;i<50;++i)old.locations.push_back(i%2?0:5);e.task_pool.emplace(100,old);
 Cgar c;c.initialize(&e,1000);std::vector<int> proposed;c.schedule(&e,100,proposed);
 if(proposed[0]<0||proposed[1]<0||proposed[0]==proposed[1])throw std::runtime_error("small candidate cap broke coverage");
 if(proposed[0]==100||proposed[1]==100)throw std::runtime_error("fairness fixture did not leave old task pending");
 for(int t:proposed)e.task_pool.erase(t);
 c.schedule(&e,100,proposed);
 if(proposed[0]!=100&&proposed[1]!=100)throw std::runtime_error("pruned oldest task missed its fair slot");
 unsetenv("CGAR_MAX_PAIRS");
 std::cout<<"FAIR_SPARSE_SCHEDULE passed\n";
}
void sparse_fallback_quality() {
 // Both pickups are beyond the local search radius. Task-ID order sends each
 // robot across the map; choosing among unused alternatives avoids that trip.
 SharedEnvironment e;e.num_of_agents=2;e.rows=201;e.cols=201;e.map.assign(201*201,0);
 e.curr_states={State(100*201+50,0,0),State(100*201+150,0,0)};
 e.curr_task_schedule={-1,-1};e.goal_locations.resize(2);
 Task far;far.task_id=0;far.t_revealed=0;far.locations={100*201+195};e.task_pool.emplace(0,far);
 Task near;near.task_id=1;near.t_revealed=0;near.locations={100*201+5};e.task_pool.emplace(1,near);
 setenv("CGAR_FALLBACK_REPAIR","0",1);
 Cgar legacy;legacy.initialize(&e,1000);std::vector<int> before;legacy.schedule(&e,5000,before);
 setenv("CGAR_FALLBACK_REPAIR","1",1);
 Cgar repaired;repaired.initialize(&e,1000);std::vector<int> after;repaired.schedule(&e,5000,after);
 unsetenv("CGAR_FALLBACK_REPAIR");
 if(before!=std::vector<int>({0,1})||after!=std::vector<int>({1,0}))
  throw std::runtime_error("sparse fallback did not reduce pickup travel while preserving unique coverage");
 if(repaired.stats().skipped_empty_searches!=2||repaired.stats().candidate_searches!=2)
  throw std::runtime_error("empty local searches were repeated");
 std::cout<<"SPARSE_FALLBACK_QUALITY passed pickup_distance=290->90 searches=4->2\n";
}
void replenish_taken_candidate() {
 // The first candidate is claimed by another robot. A second local search must
 // still find the remaining nearby task even when global sampling is disabled.
 setenv("CGAR_MAX_PAIRS","2",1);setenv("CGAR_FALLBACK_SAMPLES","0",1);
 SharedEnvironment e;e.num_of_agents=2;e.rows=101;e.cols=101;e.map.assign(101*101,0);
 e.curr_states={State(50*101+50,0,0),State(50*101+51,0,0)};
 e.curr_task_schedule={-1,-1};e.goal_locations.resize(2);
 for(auto item:std::vector<std::pair<int,int>>{{0,0},{1,50*101+50},{2,50*101+55}}){
  Task task;task.task_id=item.first;task.t_revealed=0;task.locations={item.second};e.task_pool.emplace(item.first,task);
 }
 Cgar c;c.initialize(&e,1000);std::vector<int> proposed;c.schedule(&e,5000,proposed);
 unsetenv("CGAR_MAX_PAIRS");unsetenv("CGAR_FALLBACK_SAMPLES");
 if(proposed!=std::vector<int>({1,2}))throw std::runtime_error("taken candidates were not replenished locally");
 std::cout<<"REPLENISH_TAKEN_CANDIDATE passed\n";
}
void bounded_scheduler_work() {
 setenv("CGAR_FALLBACK_SAMPLES","4096",1);
 SharedEnvironment e;e.num_of_agents=1000;e.rows=200;e.cols=200;e.map.assign(40000,0);
 e.curr_task_schedule.assign(1000,-1);e.goal_locations.resize(1000);
 for(int i=0;i<1000;++i){
  e.curr_states.emplace_back(i,0,0);Task task;task.task_id=i;task.t_revealed=0;
  task.locations={39999-i};e.task_pool.emplace(i,task);
 }
 Cgar c;c.initialize(&e,1000);std::vector<int> proposed;
 bool timed_out=false;auto start=std::chrono::steady_clock::now();
 try { c.schedule(&e,1,proposed); } catch(const cgar::Timeout&) { timed_out=true; }
 auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
 unsetenv("CGAR_FALLBACK_SAMPLES");
 if(!timed_out)throw std::runtime_error("unfinished scheduler work returned instead of timing out");
 if(ms>500)throw std::runtime_error("scheduler did not stop promptly at its deadline");
 std::cout<<"BOUNDED_SCHEDULER_WORK passed explicit_timeout=1 budget_ms=1 elapsed_ms="<<ms<<"\n";
}
void compact_distances() {
 auto cert=build_certificate({0,0,0,0,0,0,1,1},2,4,2);
 DistanceOracle oracle;oracle.init(&cert,1024);
 const auto& pocket=oracle.table(3);
 if(pocket.size()!=6||oracle.value(pocket,0)!=3||oracle.value(pocket,4)!=4||oracle.value(pocket,6)!=kInf)
  throw std::runtime_error("compact distance mapping or obstacle handling failed");
 const auto& core=oracle.table(0);
 if(oracle.value(core,3)!=kInf||oracle.dist(3,0)!=3)
  throw std::runtime_error("compact distance table crossed an excluded pocket");
 std::cout<<"COMPACT_DISTANCES passed\n";
}
void bounded_distance_work() {
 SharedEnvironment e;e.num_of_agents=1000;e.rows=200;e.cols=200;e.map.assign(40000,0);
 e.curr_task_schedule.assign(1000,-1);e.goal_locations.resize(1000);
 for(int i=0;i<1000;++i){e.curr_states.emplace_back(i,0,0);e.goal_locations[i]={{{39999-i,0}}};}
 auto cert=build_certificate(e.map,e.rows,e.cols,e.num_of_agents);
 DistanceOracle oracle;oracle.init(&cert,1<<20);
 bool table_timeout=false;
 try { oracle.try_table(39999,std::chrono::steady_clock::now()); } catch(const cgar::Timeout&) { table_timeout=true; }
 if(!table_timeout||oracle.has(39999))throw std::runtime_error("expired BFS did not raise a timeout without publishing a partial table");
 const auto& table=oracle.table(39999);
 if(oracle.value(table,0)!=398)throw std::runtime_error("wrong compact distance");
 setenv("CGAR_PLAN_TABLES","100000",1);Cgar c;c.initialize(&e,1000);std::vector<Action>a;
 bool plan_timeout=false;auto start=std::chrono::steady_clock::now();
 try { c.plan(&e,5,a); } catch(const cgar::Timeout&) { plan_timeout=true; }
 auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
 unsetenv("CGAR_PLAN_TABLES");
 if(!plan_timeout)throw std::runtime_error("unfinished planning work returned instead of timing out");
 if(ms>500)throw std::runtime_error("distance work ignored the planning deadline");
 std::cout<<"BOUNDED_DISTANCE_WORK passed explicit_timeout=1 budget_ms=5 elapsed_ms="<<ms<<"\n";
}
void cache_and_chain_consistency() {
 // The center obstacle makes the route from 3 to 5 cost four moves, not two.
 auto cert=build_certificate({0,0,0,0,1,0,0,0,0},3,3,1);
 DistanceOracle oracle;oracle.init(&cert,8*sizeof(int));
 ChainCostCache cache;Task task;task.task_id=7;task.locations={3,5};
 int budget=0;auto deadline=std::chrono::steady_clock::time_point::max();
 if(cache.estimate(task,oracle,budget,deadline,true)!=2)throw std::runtime_error("missing approximate chain fixture");
 oracle.table(5);
 if(cache.estimate(task,oracle,budget,deadline,true)!=4||cache.refined_legs!=1||cache.changed_costs!=1)
  throw std::runtime_error("cached approximate chain did not refine from a complete table");
 oracle.table(0);oracle.peek(5);oracle.trim();
 if(oracle.has(5)||!oracle.has(0))throw std::runtime_error("scheduler peek promoted LRU entry");
 if(cache.estimate(task,oracle,budget,deadline,true)!=4)throw std::runtime_error("table eviction lost refined scalar cost");
 oracle.table(5);oracle.find(0);oracle.trim();
 if(oracle.has(5)||!oracle.has(0))throw std::runtime_error("routing lookup did not promote LRU entry");
 task.idx_next_loc=1;
 if(cache.estimate(task,oracle,budget,deadline,true)!=0||cache.invalidations!=1)
  throw std::runtime_error("task stop change did not invalidate chain cost");
 task.idx_next_loc=0;task.locations={3,0};
 if(cache.estimate(task,oracle,budget,deadline,true)!=1||cache.invalidations!=2)
  throw std::runtime_error("task locations change did not invalidate chain cost");
 cache.retain({});task.locations={3,5};
 if(cache.estimate(task,oracle,budget,deadline,true)!=2)throw std::runtime_error("finished task cost was retained");
 std::cout<<"CACHE_AND_CHAIN_CONSISTENCY passed approximate=2 refined=4 retained_after_eviction=4\n";
}
void consistent_progress_basis() {
 Agent agent;agent.ticket=42;
 agent.observe_progress(2,ProgressBasis::Manhattan,true);
 for(int i=0;i<5;++i)agent.observe_progress(2,ProgressBasis::Manhattan,true);
 if(agent.stall!=5)throw std::runtime_error("stall observation did not accumulate");
 if(!agent.observe_progress(6,ProgressBasis::RouteTable,true)||agent.best!=6||agent.stall!=0||agent.ticket!=42)
  throw std::runtime_error("potential change inherited a false stall or changed ticket");
 for(int i=0;i<4;++i)agent.observe_progress(6,ProgressBasis::RouteTable,true);
 if(agent.stall!=4)throw std::runtime_error("consistent potential kept resetting recovery timer");
 agent.observe_progress(5,ProgressBasis::RouteTable,true);
 if(agent.best!=5||agent.stall!=0)throw std::runtime_error("real progress did not reset stall");
 Agent legacy;legacy.observe_progress(2,ProgressBasis::Manhattan,false);
 legacy.observe_progress(6,ProgressBasis::RouteTable,false);
 if(legacy.best!=2||legacy.stall!=1)throw std::runtime_error("disabled switch changed baseline progress policy");
 std::cout<<"CONSISTENT_PROGRESS_BASIS passed\n";
}
void global_task_candidates() {
 // A nearby batch fills the local shortlist, but a slightly farther pickup
 // has a much shorter complete task. It must compete before fallback is needed.
 SharedEnvironment e;e.num_of_agents=1;e.rows=2;e.cols=50;e.map.assign(100,0);
 e.curr_states={State(0,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);
 for(int id=0;id<16;++id){Task t;t.task_id=id;t.t_revealed=0;t.locations={1,49,1,49};e.task_pool.emplace(id,t);}
 Task shorter;shorter.task_id=100;shorter.t_revealed=0;shorter.locations={20,21};e.task_pool.emplace(100,shorter);
 setenv("CGAR_HRRN","0",1);
 Cgar control;control.initialize(&e,1000);std::vector<int> before;control.schedule(&e,100,before);
 setenv("CGAR_GLOBAL_SAMPLES","64",1);
 Cgar broad;broad.initialize(&e,1000);std::vector<int> after;broad.schedule(&e,100,after);
 if(before!=std::vector<int>{0}||after!=std::vector<int>{100}||broad.stats().global_assignments!=1||broad.stats().global_evaluations!=1)
  throw std::runtime_error("global candidates did not complement the nonempty local shortlist");
 // The independent oldest-task admission still runs with the broader shortlist.
 e.task_pool.erase(100);broad.schedule(&e,100,after);
 if(after!=std::vector<int>{0}||broad.stats().fair_assignments!=1)
  throw std::runtime_error("global candidates bypassed fair admission");
 // Tight pair caps must preserve coverage and the existing fairness fixture.
 fair_sparse_schedule();
 unsetenv("CGAR_GLOBAL_SAMPLES");unsetenv("CGAR_HRRN");
 std::cout<<"GLOBAL_TASK_CANDIDATES passed local_nonempty=1 shorter_chain=1 unique_samples=1 fair_admission=1 bounded_pairs=1\n";
}

void weighted_pickup_assignment() {
 SharedEnvironment e;e.num_of_agents=1;e.rows=2;e.cols=11;e.map.assign(22,0);
 e.curr_states={State(0,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);
 Task near;near.task_id=0;near.t_revealed=0;near.locations={1,9};e.task_pool.emplace(0,near);
 Task far;far.task_id=1;far.t_revealed=0;far.locations={4};e.task_pool.emplace(1,far);
 setenv("CGAR_PICKUP_WEIGHT","1",1);Cgar control;control.initialize(&e,1000);std::vector<int>a;
 control.schedule(&e,100,a);
 setenv("CGAR_PICKUP_WEIGHT","5",1);Cgar weighted;weighted.initialize(&e,1000);std::vector<int>b;
 weighted.schedule(&e,100,b);unsetenv("CGAR_PICKUP_WEIGHT");
 if(a!=std::vector<int>{1}||b!=std::vector<int>{0})throw std::runtime_error("pickup weight did not trade empty travel against remaining task travel");
 // Waiting-time priority remains effective under the weighted objective.
 e.task_pool.at(1).t_revealed=-1000;
 setenv("CGAR_PICKUP_WEIGHT","5",1);Cgar aged;aged.initialize(&e,1000);aged.schedule(&e,100,b);unsetenv("CGAR_PICKUP_WEIGHT");
 if(b!=std::vector<int>{1})throw std::runtime_error("pickup weighting bypassed waiting-time priority");
 std::cout<<"WEIGHTED_PICKUP_ASSIGNMENT passed empty_travel=4->1 aging_preserved=1\n";
}
SharedEnvironment swap_fixture() {
 SharedEnvironment e;e.num_of_agents=2;e.rows=3;e.cols=12;e.map.assign(36,0);e.curr_timestep=0;
 e.curr_states={State(0,0,0),State(11,0,2)};e.curr_task_schedule={0,1};e.goal_locations={{{10,0}},{{1,0}}};
 for(int i=0;i<2;++i){Task t;t.task_id=i;t.t_revealed=-5;t.agent_assigned=i;t.locations={i?1:10,i?13:22};e.task_pool.emplace(i,t);}
 return e;
}
void unopened_reassignment() {
 setenv("CGAR_REASSIGN","1",1);
 auto e=swap_fixture();Cgar c;c.initialize(&e,1000);std::vector<int> proposed;c.schedule(&e,100,proposed);
 if(proposed!=std::vector<int>({1,0})||c.stats().reassign_swaps!=1||c.stats().reassign_saving!=18)
  throw std::runtime_error("unopened task swap did not reduce pickup travel from 20 to 2");
 if(e.curr_task_schedule!=std::vector<int>({0,1})||e.task_pool.at(0).agent_assigned!=0||e.task_pool.at(0).t_revealed!=-5)
  throw std::runtime_error("speculative reassignment mutated simulator task state");
 e.curr_task_schedule=proposed;e.curr_states[0].location=11;e.curr_states[1].location=0;e.curr_timestep=20;
 c.schedule(&e,100,proposed);
 if(proposed!=e.curr_task_schedule||c.stats().reassign_swaps!=1)
  throw std::runtime_error("task reassignment limit permitted repeated redirection");
 auto started=swap_fixture();started.task_pool.at(0).idx_next_loc=1;
 Cgar immutable;immutable.initialize(&started,1000);immutable.schedule(&started,100,proposed);
 if(proposed!=started.curr_task_schedule||immutable.stats().reassign_swaps)
  throw std::runtime_error("started task was reassigned");
 auto small=swap_fixture();small.curr_states={State(4,0,0),State(5,0,2)};
 small.task_pool.at(0).locations={6};small.task_pool.at(1).locations={3};
 Cgar margin;margin.initialize(&small,1000);margin.schedule(&small,100,proposed);
 if(proposed!=small.curr_task_schedule)throw std::runtime_error("insignificant two-step saving caused reassignment");
 unsetenv("CGAR_REASSIGN");
 std::cout<<"UNOPENED_REASSIGNMENT passed pickup_distance=20->2 task_cap=1 started_tasks_preserved=1\n";
}
void reassignment_primary_and_commitments() {
 setenv("CGAR_REASSIGN","1",1);
 for(bool turning:{false,true}){
  auto e=swap_fixture();e.num_of_agents=3;e.curr_task_schedule={0,1,2};
  e.curr_states={State(24,0,0),State(0,0,turning?3:0),State(11,0,turning?3:2)};
  e.goal_locations={{{35,0}},{{10,0}},{{1,0}}};e.task_pool.clear();
  for(int i=0;i<3;++i){Task t;t.task_id=i;t.agent_assigned=i;t.locations={i==0?35:(i==1?10:1)};e.task_pool.emplace(i,t);}
  Cgar c;c.initialize(&e,1000);std::vector<Action>a;c.plan(&e,100,a);
  auto states=step(e,e.curr_states,a);if(states.empty())throw std::runtime_error("invalid swap setup action");e.curr_states=states;
  e.curr_timestep=10;std::vector<int> proposed;c.schedule(&e,100,proposed);
  if(c.primary()!=0||proposed[0]!=0||c.stats().reassign_primary_protected==0)
   throw std::runtime_error("fair primary changed during reassignment");
  if(turning&&proposed!=e.curr_task_schedule)throw std::runtime_error("unfinished turn commitment was redirected");
  if(!turning&&proposed!=std::vector<int>({0,2,1}))throw std::runtime_error("primary protection unnecessarily blocked other swaps");
 }
 unsetenv("CGAR_REASSIGN");
 std::cout<<"REASSIGNMENT_PRIMARY_AND_COMMITMENTS passed\n";
}
void reassignment_recovery_protection() {
 setenv("CGAR_REASSIGN","1",1);
 SharedEnvironment e;e.num_of_agents=3;e.rows=2;e.cols=2;e.map.assign(4,0);e.curr_states={State(0,0,0),State(1,0,1),State(2,0,0)};
 e.curr_task_schedule={0,1,2};e.goal_locations={{{3,0}},{{2,0}},{{1,0}}};
 for(int i=0;i<3;++i){Task t;t.task_id=i;t.agent_assigned=i;t.locations={i==0?3:(i==1?2:1)};e.task_pool.emplace(i,t);}
 Cgar c;c.initialize(&e,1000);std::vector<Action>a;
 for(int t=0;t<10&&!c.stats().txns;++t){e.curr_timestep=t;c.plan(&e,100,a);}
 if(!c.stats().txns)throw std::runtime_error("recovery fixture did not install a witness");
 e.curr_timestep=10;std::vector<int> proposed;c.schedule(&e,100,proposed);
 if(proposed!=e.curr_task_schedule||!c.stats().reassign_recovery_protected)
  throw std::runtime_error("reassignment failed to protect recovery participants");
 unsetenv("CGAR_REASSIGN");std::cout<<"REASSIGNMENT_RECOVERY_PROTECTION passed\n";
}
void reassignment_fair_admission() {
 setenv("CGAR_REASSIGN","1",1);
 SharedEnvironment e;e.num_of_agents=2;e.rows=2;e.cols=11;e.map.assign(22,0);e.curr_timestep=20;
 e.curr_states={State(0,0,0),State(10,0,2)};e.curr_task_schedule={-1,-1};e.goal_locations.resize(2);
 for(int i=0;i<2;++i){Task t;t.task_id=i;t.t_revealed=0;t.locations={i*10};e.task_pool.emplace(i,t);}
 Task old;old.task_id=100;old.t_revealed=-1;old.locations={8};for(int i=0;i<100;++i)old.locations.push_back(i%2?8:0);e.task_pool.emplace(100,old);
 Cgar c;c.initialize(&e,1000);std::vector<int> proposed;c.schedule(&e,100,proposed);
 if(proposed!=std::vector<int>({0,1}))throw std::runtime_error("fair reassignment fixture did not defer old task");
 e.task_pool.erase(0);e.task_pool.erase(1);Task fresh;fresh.task_id=2;fresh.t_revealed=20;fresh.locations={0};e.task_pool.emplace(2,fresh);
 c.schedule(&e,100,proposed);
 if(proposed!=std::vector<int>({2,100})||c.stats().fair_assignments!=1)throw std::runtime_error("old task did not receive fair admission");
 e.curr_task_schedule=proposed;e.curr_states[0].location=8;e.curr_states[1].location=0;e.curr_timestep=40;
 for(int i=0;i<2;++i)e.task_pool.at(proposed[i]).agent_assigned=i;
 c.schedule(&e,100,proposed);
 if(proposed!=e.curr_task_schedule||!c.stats().reassign_fair_protected)throw std::runtime_error("fair admission was redirected by a cheaper swap");
 unsetenv("CGAR_REASSIGN");std::cout<<"REASSIGNMENT_FAIR_ADMISSION passed\n";
}
void oriented_distances() {
 const int rows=4,cols=5;std::vector<int> map={0,0,0,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,1,1,1};
 const auto cert=build_certificate(map,rows,cols,2);TurnDistanceOracle oracle;oracle.init(&cert,1);
 int checked=0;
 for(int goal=0;goal<rows*cols;++goal)if(!map[goal]){
  const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(1));
  for(int from=0;from<rows*cols;++from)if(!map[from])for(int ori=0;ori<4;++ori){
   int expected=kInf;
   if(cert.core[from]||cert.pocket[from]==cert.pocket[goal]){
    std::vector<int> distance(rows*cols*4,-1);std::queue<int> q;q.push(from*4+ori);distance[from*4+ori]=0;
    while(!q.empty()){
     int u=q.front();q.pop();int cell=u/4,dir=u%4;
     if(cell==goal){expected=distance[u];break;}
     for(Action action:{Action::FW,Action::CR,Action::CCR}){
      int v=cell,d=dir;
      if(action==Action::FW)v=nb(cell,dir,rows,cols);
      else d=(dir+(action==Action::CR?1:3))%4;
      if(v<0||map[v]||(!cert.core[v]&&cert.pocket[v]!=cert.pocket[goal]))continue;
      int node=v*4+d;if(distance[node]>=0)continue;distance[node]=distance[u]+1;q.push(node);
     }
    }
   }
   if(oracle.value(*table,from,ori)!=expected)throw std::runtime_error("oriented oracle disagrees with independent action search");
   ++checked;
  }
  oracle.trim();if(oracle.find(goal))throw std::runtime_error("oriented cache exceeded its retained limit");
 }
 // Demand admission can explicitly retain one resident and evict another.
 TurnDistanceOracle retained;retained.init(&cert,1<<20);
 retained.table(0,std::chrono::steady_clock::now()+std::chrono::seconds(1));
 retained.table(1,std::chrono::steady_clock::now()+std::chrono::seconds(1));
 retained.retain({1});
 if(retained.has(0)||!retained.has(1)||retained.capacity()<2)throw std::runtime_error("orientation retention failed");
 bool timed_out=false;try{oracle.table(0,std::chrono::steady_clock::now());}catch(const Timeout&){timed_out=true;}
 if(!timed_out||oracle.find(0))throw std::runtime_error("expired oriented traversal entered the cache");
 std::cout<<"ORIENTED_DISTANCES passed state_goal_pairs="<<checked<<" explicit_timeout=1 bounded_cache=1\n";
}

void movement_diagnostics() {
 SharedEnvironment e;e.num_of_agents=2;e.rows=2;e.cols=3;e.map.assign(6,0);
 e.curr_states={State(0,0,0),State(1,0,0)};e.curr_task_schedule={0,1};e.goal_locations={{{2,0}},{{4,0}}};
 for(int i=0;i<2;++i){Task t;t.task_id=i;t.locations={i?4:2};e.task_pool.emplace(i,t);}
 unsetenv("CGAR_DIAGNOSTICS");Cgar plain;plain.initialize(&e,1000);std::vector<Action>a;plain.plan(&e,100,a);
 setenv("CGAR_DIAGNOSTICS","1",1);Cgar measured;measured.initialize(&e,1000);std::vector<Action>b;measured.plan(&e,100,b);
 unsetenv("CGAR_DIAGNOSTICS");
 if(a!=b||a!=std::vector<Action>({Action::W,Action::CR}))throw std::runtime_error("turn dependency fixture or diagnostic equivalence failed");
 const auto& m=measured.stats().movement[1];
 if(m.actions[0]!=0||m.actions[1]!=1||m.actions[2]!=0||m.actions[3]!=1||m.blocked_forward!=1||m.turn_dependency!=1||m.planned_wait||m.safety_cancel)
  throw std::runtime_error("diagnostic wait attribution failed");
 std::cout<<"MOVEMENT_DIAGNOSTICS passed turn_dependency=1 actions_unchanged=1\n";
}

void temporal_kernel_regression() {
 auto validate=[](const TemporalPibt& search,const std::vector<int>& starts,int cells){
  std::vector<int> previous=starts,owner(cells,-1);
  for(int t=0;t<5;++t){
   for(size_t i=0;i<starts.size();++i){int v=search.choice(i).path->cells[t];if(v<0||v>=cells||owner[v]>=0)throw std::runtime_error("temporal test vertex conflict");owner[v]=i;}
   for(size_t i=0;i<starts.size();++i){int j=owner[previous[i]];if(j>=0&&j!=static_cast<int>(i)&&previous[j]==search.choice(i).path->cells[t])throw std::runtime_error("temporal test swap");}
   for(size_t i=0;i<starts.size();++i){previous[i]=search.choice(i).path->cells[t];owner[previous[i]]=-1;}
  }
 };
 // A displacement chain longer than all earlier prototype depth limits.
 const int n=1500;TemporalGeometry geometry;geometry.initialize(std::vector<char>(n+1,1),1,n+1,[]{});
 std::vector<std::vector<TemporalChoice>> choices(n);std::vector<int> starts(n),order(n);std::vector<char> fixed(n,0);std::vector<double> power(n,1);
 int forward=-1;const auto& ops=TemporalGeometry::operations();
 for(int k=1;k<129;++k)if(ops[k]==std::array<uint8_t,5>{0,3,3,3,3})forward=k;
 if(forward<0)throw std::runtime_error("missing forward temporal operation");
 for(int i=0;i<n;++i){starts[i]=order[i]=i;const auto& paths=geometry.paths(i,0);choices[i]={{&paths[0],0,0},{&paths[forward],-1,forward}};}
 TemporalPibt full(n+1,choices,fixed,power,8192,0);full.construct(order,[]{});validate(full,starts,n+1);
 if(full.stats.max_depth!=n-1)throw std::runtime_error("temporal chain was truncated");
 for(int i=0;i<n;++i)if(full.choice(i).path->cells[0]!=i+1)throw std::runtime_error("temporal chain did not advance");
 TemporalPibt bounded(n+1,choices,fixed,power,8,0);bounded.construct(order,[]{});validate(bounded,starts,n+1);
 if(!bounded.stats.budget_exhausted||bounded.selected(0)!=0)throw std::runtime_error("temporal bounded rollback failed");
 bool timeout=false;try{TemporalPibt x(n+1,choices,fixed,power,8192,0);x.construct(order,[]{throw Timeout("temporal_test");});}catch(const Timeout&){timeout=true;}
 if(!timeout)throw std::runtime_error("temporal deadline did not propagate");
 // Random legal geometries, movable waits, immutable protected robots, and
 // independent collision checks after both construction and fixed-count search.
 TemporalGeometry small;small.initialize(std::vector<char>(36,1),6,6,[]{});
 std::mt19937 random(202);
 for(int seed=0;seed<48;++seed){
  const int count=12;std::vector<int> locations(36);std::iota(locations.begin(),locations.end(),0);std::shuffle(locations.begin(),locations.end(),random);locations.resize(count);
  std::vector<std::vector<TemporalChoice>> options(count);std::vector<char> protected_robot(count,0);protected_robot[seed%count]=1;
  std::vector<double> weights(count,1);std::vector<int> ranks(count);std::iota(ranks.begin(),ranks.end(),0);
  for(int i=0;i<count;++i){const auto& paths=small.paths(locations[i],random()%4);int goal=random()%36;
   auto cost=[&](const TemporalPath& path,int op){int cell=path.cells[4];return int64_t(50)*(std::abs(cell/6-goal/6)+std::abs(cell%6-goal%6))-op;};
   options[i].push_back({&paths[0],cost(paths[0],0),0});
   for(int k=1;k<129;++k)if(paths[k].valid)options[i].push_back({&paths[k],cost(paths[k],k),k});
   std::sort(options[i].begin()+1,options[i].end(),[](const auto& a,const auto& b){return std::tie(a.cost,a.operation)<std::tie(b.cost,b.operation);});
  }
  TemporalPibt search(36,options,protected_robot,weights,8192,seed);search.construct(ranks,[]{});validate(search,locations,36);search.repair(256,[]{});validate(search,locations,36);
  if(search.selected(seed%count)!=0||search.stats.repairs!=256)throw std::runtime_error("temporal fixed-work or protection failed");
  TemporalPibt work_a(36,options,protected_robot,weights,8192,seed),work_b(36,options,protected_robot,weights,8192,seed);
  work_a.construct(ranks,[]{});work_b.construct(ranks,[]{});
  const long long limit=work_a.stats.candidates+1000;
  work_a.repair(10000,[]{},limit);
  work_b.repair(10000,[]{check_deadline(std::chrono::steady_clock::time_point::max(),"work_counter_test");},limit);
  validate(work_a,locations,36);validate(work_b,locations,36);
  if(work_a.stats.candidates<limit||work_a.stats.repairs>=10000||work_a.stats.repairs!=work_b.stats.repairs)
   throw std::runtime_error("candidate-work stop condition was not deterministic");
  for(int i=0;i<count;++i)if(work_a.selected(i)!=work_b.selected(i))throw std::runtime_error("deadline observation changed candidate-work decisions");
 }
 std::cout<<"TEMPORAL_KERNEL passed chain_robots=1500 random_cases=48 bounded_rollback=1 explicit_timeout=1 deterministic_candidate_work=1\n";
}

void temporal_primary_regression() {
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);
 SharedEnvironment e;e.num_of_agents=2;e.rows=3;e.cols=4;e.map.assign(12,0);
 e.curr_states={State(6,0,3),State(5,0,2)};e.goal_locations={{},{{6,0}}};e.curr_task_schedule={-1,-1};
 Cgar c;c.initialize(&e,1000);std::vector<Action> actions;
 bool reached=false;
 for(int t=0;t<30;++t){
  e.curr_timestep=t;if(t==1)e.goal_locations[0]={{4,0}};
  c.plan(&e,1000,actions);auto next=step(e,e.curr_states,actions);
  if(next.empty())throw std::runtime_error("temporal primary fixture invalid");
  e.curr_states=next;
  if(e.curr_states[1].location==6){reached=true;break;}
 }
 if(!reached)throw std::runtime_error("temporal primary intent was not completed");
 // The original pocket/recovery fixtures must also finish with the layer active.
 pocket_case();persistent_primary();capacity_bootstrap();
 unsetenv("CGAR_TEMPORAL");unsetenv("CGAR_ORIENTATION_GUIDANCE");unsetenv("CGAR_TEMPORAL_STEPS");
 std::cout<<"TEMPORAL_INTEGRATION passed primary_intent=1 pockets=1 capacity=1\n";
}

void initialization_failure_recovery() {
 SharedEnvironment e;e.num_of_agents=1;e.rows=2;e.cols=2;e.map.assign(4,0);
 e.curr_states={State(0,0,0)};e.curr_task_schedule={-1};e.goal_locations={{{3,0}}};
 Cgar c;setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","0",1);
 bool failed=false;try{c.initialize(&e,1000);}catch(const std::invalid_argument&){failed=true;}
 unsetenv("CGAR_TEMPORAL");unsetenv("CGAR_ORIENTATION_GUIDANCE");
 if(!failed)throw std::runtime_error("invalid initialization did not fail");
 c.initialize(&e,1000);std::vector<Action> actions;c.plan(&e,1000,actions);
 if(step(e,e.curr_states,actions).empty())throw std::runtime_error("failed initialization left a false ready state");
 std::cout<<"INITIALIZATION_FAILURE_RECOVERY passed failed_state_not_ready=1 retry_valid=1\n";
}

void temporal_kernel_on_thread() {
 std::exception_ptr error;
 std::thread worker([&]{try{temporal_kernel_regression();}catch(...){error=std::current_exception();}});
 worker.join();if(error)std::rethrow_exception(error);
 std::cout<<"TEMPORAL_THREAD_STACK passed displacement_depth=1499\n";
}

void temporal_idle_blocker() {
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);
 SharedEnvironment e;e.num_of_agents=2;e.rows=3;e.cols=4;e.map.assign(12,0);
 e.curr_states={State(6,0,3),State(5,0,2)};e.goal_locations={{},{{6,0}}};e.curr_task_schedule={-1,-1};
 Cgar c;c.initialize(&e,1000);std::vector<Action> actions;bool reached=false;
 for(int t=0;t<100;++t){
  e.curr_timestep=t;c.plan(&e,1000,actions);auto next=step(e,e.curr_states,actions);
  if(next.empty())throw std::runtime_error("idle temporal blocker caused a collision");e.curr_states=next;
  if(e.curr_states[1].location==6){reached=true;break;}
 }
 if(!reached)throw std::runtime_error("primary did not clear an idle temporal blocker");
 for(const char* name:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_STEPS"})unsetenv(name);
 std::cout<<"TEMPORAL_IDLE_BLOCKER passed goal_reached=1 transactions="<<c.stats().txns<<" steps="<<e.curr_timestep+1<<"\n";
}

void temporal_parallel_regression() {
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_TEMPORAL_WORKERS","4",1);setenv("CGAR_TEMPORAL_STEPS","256",1);
 SharedEnvironment e;e.rows=8;e.cols=8;e.num_of_agents=24;e.map.assign(64,0);e.curr_task_schedule.assign(24,-1);e.goal_locations.resize(24);
 for(int i=0;i<24;++i){e.curr_states.emplace_back(i,0,i%4);e.goal_locations[i]={{63-i,0}};}
 setenv("CGAR_TEMPORAL_THREADS","1",1);Cgar serial;serial.initialize(&e,1000);
 setenv("CGAR_TEMPORAL_THREADS","4",1);Cgar parallel;parallel.initialize(&e,1000);
 int comparisons=0;
 for(int t=0;t<80;++t){
  e.curr_timestep=t;std::vector<Action>a,b;serial.plan(&e,1000,a);parallel.plan(&e,1000,b);
  if(a!=b)throw std::runtime_error("temporal parallel scheduling changed decisions");
  auto next=step(e,e.curr_states,a);if(next.empty())throw std::runtime_error("temporal parallel collision");e.curr_states=next;
  for(int i=0;i<24;++i)if(e.curr_states[i].location==e.goal_locations[i][0].first)e.goal_locations[i][0].first=(e.goal_locations[i][0].first+37)%64;
  comparisons+=a.size();
 }
 for(const char* name:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_WORKERS","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_THREADS"})unsetenv(name);
 std::cout<<"TEMPORAL_PARALLEL passed workers=4 serial_vs_parallel_robot_decisions="<<comparisons<<"\n";
}

int main(){try{initialization_failure_recovery();temporal_idle_blocker();global_task_candidates();temporal_parallel_regression();temporal_kernel_on_thread();temporal_primary_regression();oriented_distances();movement_diagnostics();unopened_reassignment();reassignment_primary_and_commitments();reassignment_recovery_protection();reassignment_fair_admission();weighted_pickup_assignment();cache_and_chain_consistency();consistent_progress_basis();certificates();pocket_case();pocket_case(20);persistent_primary();capacity_bootstrap();scheduler_case();fair_sparse_schedule();sparse_fallback_quality();replenish_taken_candidate();bounded_scheduler_work();compact_distances();bounded_distance_work();std::cout<<"All CGAR regression checks passed\n";}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
