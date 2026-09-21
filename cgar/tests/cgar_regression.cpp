#include <map>
#include "cgar.hpp"
#include <algorithm>
#include <atomic>
#include <array>
#include <chrono>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
#include <thread>
#include <exception>
#include <sched.h>
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

void oriented_pickup_search_regression() {
 OrientedPickupSearch search;CompletePickupSearch complete;FullPickupField full;int compared=0;
 constexpr int R=7,C=8,N=R*C;
 for(int field=0;field<12;++field)for(int scale:{1,2,4,8})for(int extra:{0,1,2}){
  std::vector<char> free(N,true);for(int u=0;u<N;++u)if((u*17+field*13)%11<2)free[u]=false;
  if(field==0)for(int c=0;c<C;++c)free[3*C+c]=false;
  std::vector<int> costs(N*4);for(int u=0;u<N;++u)for(int d=0;d<4;++d)costs[u*4+d]=scale*(1+(u*19+d*7+field)%2);
  const int turn=scale*(1+field%2)+extra;if(turn>16)continue;
  for(int start:{0,N/2,N-1}){free[start]=true;for(int heading=0;heading<4;++heading){
   // Independent repeated relaxation on the explicit state graph; no heap,
   // generation stamps, early endpoint callbacks or shared search routine.
   std::vector<int> expected(N*4,kInf);expected[start*4+heading]=0;
   for(int pass=0;pass<N*4;++pass){bool changed=false;
    auto relax=[&](int u,int v,int w){if(expected[u]<kInf&&expected[v]>expected[u]+w){expected[v]=expected[u]+w;changed=true;}};
    for(int cell=0;cell<N;++cell)if(free[cell])for(int d=0;d<4;++d){const int state=cell*4+d;
     relax(state,cell*4+(d+1)%4,turn);relax(state,cell*4+(d+3)%4,turn);
     const int v=nb(cell,d,R,C);if(v>=0&&free[v])relax(state,v*4+d,costs[state]);
    }
    if(!changed)break;
   }
   std::vector<int> got(N,-1);auto stats=search.run(N,start,heading,8192,turn,
    [&](int u,int d){return nb(u,d,R,C);},[&](int u){return free[u];},[&](int u,int d){return costs[u*4+d];},
    [&](int u,int cost){if(got[u]>=0)throw std::runtime_error("pickup cell published twice");got[u]=cost;return false;},[]{});
   if(stats.limited||stats.stopped||stats.pops>8192)throw std::runtime_error("complete small pickup graph hit a work bound");
   complete.run(N,start,heading,turn,kInf,[&](int u,int d){return nb(u,d,R,C);},
    [&](int u){return free[u];},[&](int u,int d){return costs[u*4+d];},[]{},full);
   long long reachable=0;for(int value:expected)reachable+=value<kInf;
   if(full.states!=reachable||full.pops<full.states)throw std::runtime_error("complete pickup field did not settle every reachable state");
   Certificate cert;cert.rows=R;cert.cols=C;cert.free=cert.core=free;cert.pocket.assign(N,-1);
   TurnDistanceOracle reverse;reverse.init(&cert,1<<20,turn,true,scale);
   reverse.set_forward_costs(std::vector<uint8_t>(costs.begin(),costs.end()));
   for(int goal:{0,N/2,N-1})if(free[goal]){
    const auto* table=reverse.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(1));
    if(reverse.value(*table,start,heading)!=full.distance[goal])throw std::runtime_error("reverse, bounded and complete pickup metrics disagree");
   }
   for(int goal=0;goal<N;++goal){int want=kInf;for(int d=0;d<4;++d)want=std::min(want,expected[goal*4+d]);
    if((want==kInf? -1:want)!=got[goal]||full.distance[goal]!=want)throw std::runtime_error("pickup cost differs from independent graph relaxation");++compared;
   }
  }}
 }
 std::vector<int> reached;auto bounded=search.run(4,0,0,2,16,
  [](int u,int d){return nb(u,d,1,4);},[](int){return true;},[](int,int){return 1;},
  [&](int u,int cost){if(cost!=u)throw std::runtime_error("unsettled pickup cost published");reached.push_back(u);return false;},[]{});
 if(reached!=std::vector<int>({0,1})||bounded.pops!=2||!bounded.limited)throw std::runtime_error("pickup queue-pop budget did not bound work");
 auto stopped=search.run(4,0,0,8192,16,[](int u,int d){return nb(u,d,1,4);},[](int){return true;},[](int,int){return 1;},[](int u,int){return u==1;},[]{});
 if(!stopped.stopped||stopped.limited||stopped.pops!=2)throw std::runtime_error("pickup endpoint stop built unnecessary candidates");
 int checks=0;bool failed=false;
 try{search.run(400,0,0,8192,1,[](int u,int d){return nb(u,d,20,20);},[](int){return true;},[](int,int){return 1;},[](int,int){return false;},[&]{if(++checks==3)throw Timeout("injected_pickup_search");});}catch(const Timeout&){failed=true;}
 if(!failed||checks!=3)throw std::runtime_error("in-search pickup deadline was swallowed");
 int recovered=-1;search.run(2,0,2,128,2,[](int u,int d){return nb(u,d,1,2);},[](int){return true;},[](int,int){return 3;},[&](int u,int cost){if(u==1)recovered=cost;return false;},[]{});
 if(recovered!=7)throw std::runtime_error("pickup scratch reuse after failure or turn cost is incorrect");
 for(int bound:{0,65537}){bool invalid=false;try{search.run(2,0,0,bound,1,[](int u,int d){return nb(u,d,1,2);},[](int){return true;},[](int,int){return 1;},[](int,int){return false;},[]{});}catch(const std::invalid_argument&){invalid=true;}if(!invalid)throw std::runtime_error("invalid pickup work bound accepted");}
 std::cout<<"ORIENTED_PICKUP_SEARCH passed independent_goal_costs="<<compared<<" complete_dial_field=1 reverse_oracle_basis=1 fractional_turn_surcharges=0,1,2 scales=1,2,4,8 all_headings=1 obstacles_disconnection=1 settled_only=1 fixed_queue_pops=1 early_endpoint_stop=1 injected_mid_search_timeout=1 scratch_reuse=1\n";
}

void pickup_flow_scheduler_regression() {
 auto blank=[](){SharedEnvironment e;e.rows=e.cols=9;e.num_of_agents=1;e.map.assign(81,0);e.curr_states={State(40,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);return e;};
 auto add_tasks=[](SharedEnvironment& e){for(int id=0;id<2;++id){
  const int dir=(e.curr_states[0].orientation+(id?0:2))%4;
  const int first=nb(e.curr_states[0].location,dir,e.rows,e.cols),second=first<0?-1:nb(first,dir,e.rows,e.cols);
  if(first<0||second<0)throw std::runtime_error("pickup heading fixture reached a boundary");
  Task t;t.task_id=id;t.t_revealed=e.curr_timestep;t.locations={first,second};e.task_pool.emplace(id,t);
 }};
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","1",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","0",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","0",1);
 for(const char* bad:{"-1","2"}){setenv("CGAR_PICKUP_FLOW",bad,1);auto e=blank();bool rejected=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}if(!rejected)throw std::runtime_error("invalid pickup flow flag accepted");}
 setenv("CGAR_PICKUP_FLOW","1",1);
 for(const char* bad:{"0","65537"}){setenv("CGAR_PICKUP_FLOW_NODES",bad,1);auto e=blank();bool rejected=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}if(!rejected)throw std::runtime_error("invalid pickup search work bound accepted");}
 unsetenv("CGAR_PICKUP_FLOW_NODES");setenv("CGAR_FLOW_STRENGTH","0",1);{auto e=blank();bool rejected=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}if(!rejected)throw std::runtime_error("pickup flow accepted without a published-flow provider");}setenv("CGAR_FLOW_STRENGTH","1",1);
 // Warmup preserves the original shortlist and action-independent task choice.
 {auto e=blank();add_tasks(e);setenv("CGAR_PICKUP_FLOW","0",1);Cgar plain;plain.initialize(&e,1000);setenv("CGAR_PICKUP_FLOW","1",1);Cgar cold;cold.initialize(&e,1000);std::vector<int>a,b;plain.schedule(&e,1000,a);cold.schedule(&e,1000,b);
  if(a!=std::vector<int>{0}||a!=b||cold.stats().pickup_flow_searches||cold.stats().pickup_flow_warmup_calls!=1)throw std::runtime_error("pickup warmup changed the baseline scheduler");}
 for(const char* scale:{"1","4"}){
  setenv("CGAR_FLOW_COST_SCALE",scale,1);auto e=blank();Cgar c;c.initialize(&e,1000);std::vector<Action>actions;
  for(int t=0;t<2;++t){e.curr_timestep=t;c.plan(&e,1000,actions);auto states=step(e,e.curr_states,actions);if(states.empty())throw std::runtime_error("pickup warmup planner collided");e.curr_states=states;}
  e.curr_timestep=2;add_tasks(e);std::vector<int>proposed;c.schedule(&e,1000,proposed);
  if(proposed!=std::vector<int>{1}||!c.stats().pickup_flow_searches||c.stats().pickup_flow_snapshot_publication!=1||c.stats().estimated_pickup_cost!=1)
   throw std::runtime_error(std::string("heading-aware pickup choice or scaled cost units are incorrect: scale=")+scale+" choice="+(proposed.empty()?"empty":std::to_string(proposed[0]))+" searches="+std::to_string(c.stats().pickup_flow_searches)+" publication="+std::to_string(c.stats().pickup_flow_snapshot_publication)+" estimate="+std::to_string(c.stats().estimated_pickup_cost)+" cell="+std::to_string(e.curr_states[0].location)+" heading="+std::to_string(e.curr_states[0].orientation));
  c.schedule(&e,1000,proposed);
  if(proposed!=std::vector<int>{0}||c.stats().fair_assignments!=1||e.task_pool.at(0).t_revealed!=2||e.curr_task_schedule!=std::vector<int>{-1})
   throw std::runtime_error("pickup metric bypassed oldest-task admission or mutated task metadata");
  e.curr_task_schedule={1};e.curr_states[0].location=e.task_pool.at(1).locations[0];e.task_pool.at(1).agent_assigned=0;e.task_pool.at(1).idx_next_loc=1;e.goal_locations={{{e.task_pool.at(1).locations[1],0}}};
  c.schedule(&e,1000,proposed);if(proposed!=std::vector<int>{1}||e.task_pool.at(1).idx_next_loc!=1)throw std::runtime_error("pickup metric changed a started task");
 }
 unsetenv("CGAR_FLOW_COST_SCALE");setenv("CGAR_FLOW_STRENGTH","3",1);
 // Ingest a prescribed physically valid observation trace (FW, CR, CR).
 // This isolates the learned metric from the planner's preferred warmup route;
 // full simulator runs separately validate execution of the offered actions.
 for(const char* enabled:{"0","1"}){
  setenv("CGAR_PICKUP_FLOW",enabled,1);auto e=blank();e.rows=5;e.cols=7;e.map.assign(35,0);e.curr_states={State(17,0,0)};Cgar c;c.initialize(&e,1000);std::vector<Action>offered;
  const std::array<Action,3> observed{Action::FW,Action::CR,Action::CR};
  for(int t=0;t<3;++t){e.curr_timestep=t;c.plan(&e,1000,offered);auto states=step(e,e.curr_states,{observed[t]});if(states.empty())throw std::runtime_error("pickup toll observation trace is physically invalid");e.curr_states=states;}
  if(e.curr_states[0].location!=18||e.curr_states[0].orientation!=2||c.stats().flow_publications!=1)throw std::runtime_error("pickup toll observations did not produce the requested metric snapshot");
  e.curr_timestep=3;
  Task west;west.task_id=0;west.t_revealed=3;west.locations={17};e.task_pool.emplace(0,west);Task north;north.task_id=1;north.t_revealed=3;north.locations={11};e.task_pool.emplace(1,north);
  std::vector<int>proposed;c.schedule(&e,1000,proposed);if(proposed!=std::vector<int>{enabled[0]=='1'?1:0})throw std::runtime_error("published traffic toll did not affect pickup ranking");
 }
 for(const char* key:{"CGAR_PICKUP_FLOW","CGAR_PICKUP_FLOW_NODES","CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_FLOW_COST_SCALE","CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_ORIENTATION_GUIDANCE"})unsetenv(key);
 std::cout<<"PICKUP_FLOW_SCHEDULER passed cold_default_equivalence=1 heading_choice=1 published_toll_choice=1 scaled_units=1 fair_admission=1 started_immutable=1 metadata_unchanged=1 invalid_configuration=1\n";
}

void complete_pickup_scheduler_regression() {
 // Each worker owns reusable scratch; task order changes with worker count.
 auto fields=[](int threads){std::vector<CompletePickupSearch>workers(threads);std::vector<FullPickupField>out(12);
  run_temporal_preparation(threads,[&](int worker){for(int i=worker;i<12;i+=threads)
   workers[worker].run(400,i*13,i%4,4,kInf,[](int u,int d){return nb(u,d,20,20);},
    [](int u){return u%23!=7;},[](int u,int d){return 4+(u*7+d*3)%5;},[]{},out[i]);});return out;};
 auto serial=fields(1),parallel=fields(4);
 for(size_t i=0;i<serial.size();++i)if(serial[i].distance!=parallel[i].distance||serial[i].pops!=parallel[i].pops||serial[i].states!=parallel[i].states)
  throw std::runtime_error("complete pickup fields changed with parallel allocation");
 std::atomic<int>finished{0};bool failed=false;
 try{run_temporal_preparation(4,[&](int worker){CompletePickupSearch search;FullPickupField out;int checks=0;
  search.run(400,0,worker,1,kInf,[](int u,int d){return nb(u,d,20,20);},[](int){return true;},[](int,int){return 1;},
   [&]{if(worker==2&&++checks==3)throw Timeout("injected_complete_pickup");},out);++finished;});}catch(const Timeout&){failed=true;}
 if(!failed||finished!=3)throw std::runtime_error("complete pickup failure did not join other field builders");
 CompletePickupSearch reusable;FullPickupField recovered;int checks=0;failed=false;
 try{reusable.run(400,0,0,1,kInf,[](int u,int d){return nb(u,d,20,20);},[](int){return true;},[](int,int){return 1;},
  [&]{if(++checks==3)throw Timeout("injected_complete_pickup_reuse");},recovered);}catch(const Timeout&){failed=true;}
 if(!failed)throw std::runtime_error("complete pickup in-search timeout not propagated");
 reusable.run(2,0,2,2,kInf,[](int u,int d){return nb(u,d,1,2);},[](int){return true;},[](int,int){return 3;},[]{},recovered);
 if(recovered.distance!=std::vector<int>({0,7}))throw std::runtime_error("complete pickup scratch retained a failed field");
 for(int edge:{0,17}){bool invalid=false;try{reusable.run(2,0,0,1,kInf,[](int u,int d){return nb(u,d,1,2);},[](int){return true;},[&](int,int){return edge;},[]{},recovered);}catch(const std::invalid_argument&){invalid=true;}if(!invalid)throw std::runtime_error("complete pickup accepted invalid edge cost");}
 auto blank=[](){SharedEnvironment e;e.rows=9;e.cols=41;e.num_of_agents=1;e.map.assign(e.rows*e.cols,0);e.curr_states={State(4*41+20,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);return e;};
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","1",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","0",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","0",1);
 setenv("CGAR_PICKUP_FLOW","1",1);setenv("CGAR_HRRN","0",1);setenv("CGAR_PICKUP_WEIGHT","1",1);
 for(const auto& invalid:std::vector<std::pair<const char*,const char*>>{{"CGAR_PICKUP_FULL_ROBOTS","-1"},{"CGAR_PICKUP_FULL_ROBOTS","257"},{"CGAR_PICKUP_FULL_THREADS","0"},{"CGAR_PICKUP_FULL_THREADS","33"},{"CGAR_PICKUP_FULL_COST_KEY","-1"},{"CGAR_PICKUP_FULL_COST_KEY","2"},{"CGAR_PICKUP_FULL_COST_KEY","1"}}){
  setenv(invalid.first,invalid.second,1);auto e=blank();Cgar c;bool rejected=false;try{c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}unsetenv(invalid.first);if(!rejected)throw std::runtime_error("invalid complete pickup configuration accepted");
 }
 setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);setenv("CGAR_PICKUP_FLOW","0",1);
 {auto e=blank();Cgar c;bool rejected=false;try{c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}if(!rejected)throw std::runtime_error("complete pickup accepted missing published metric provider");}
 setenv("CGAR_PICKUP_FLOW","1",1);
 // Test the real operating-system affinity guard, including when temporal
 // planning is disabled. Restore the calling thread's allocation on every path.
 {cpu_set_t original,single;CPU_ZERO(&original);CPU_ZERO(&single);
  if(sched_getaffinity(0,sizeof(original),&original))throw std::runtime_error("cannot inspect regression CPU affinity");
  for(int cpu=0;cpu<CPU_SETSIZE;++cpu)if(CPU_ISSET(cpu,&original)){CPU_SET(cpu,&single);break;}
  if(!CPU_COUNT(&single)||sched_setaffinity(0,sizeof(single),&single))throw std::runtime_error("cannot restrict regression CPU affinity");
  try{
   for(const char* temporal:{"0","1"}){
    setenv("CGAR_TEMPORAL",temporal,1);setenv("CGAR_PICKUP_FULL_ROBOTS","2",1);setenv("CGAR_PICKUP_FULL_THREADS","2",1);
    auto e=blank();e.num_of_agents=2;e.curr_states.push_back(State(1,0,0));e.curr_task_schedule.push_back(-1);e.goal_locations.resize(2);
    bool rejected=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument& error){rejected=std::string(error.what()).find("CPU affinity")!=std::string::npos;}
    if(!rejected)throw std::runtime_error("complete pickup silently oversubscribed a one-CPU allocation");
    // A one-field quota can use only one worker even if the configured ceiling
    // is larger. A disabled feature needs no workers at all.
    for(const char* quota:{"1","0"}){setenv("CGAR_PICKUP_FULL_ROBOTS",quota,1);setenv("CGAR_PICKUP_FULL_THREADS","32",1);Cgar c;c.initialize(&e,1000);}
   }
  }catch(...){sched_setaffinity(0,sizeof(original),&original);throw;}
  if(sched_setaffinity(0,sizeof(original),&original))throw std::runtime_error("cannot restore regression CPU affinity");
  setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);unsetenv("CGAR_PICKUP_FULL_THREADS");
 }
 // Before publication, enabling complete fields leaves the candidate policy alone.
 {auto e=blank();Task t;t.task_id=0;t.locations={e.curr_states[0].location+1};e.task_pool.emplace(0,t);Cgar c;c.initialize(&e,1000);std::vector<int>proposed;c.schedule(&e,1000,proposed);
  if(proposed!=std::vector<int>{0}||c.stats().pickup_full_fields)throw std::runtime_error("complete pickup bypassed publication warmup");}
 for(const char* count:{"0","1"}){
  setenv("CGAR_PICKUP_FULL_ROBOTS",count,1);auto e=blank();Cgar c;c.initialize(&e,1000);std::vector<Action>offered;
  const std::array<Action,3> observed{Action::FW,Action::CR,Action::CR};
  for(int t=0;t<3;++t){e.curr_timestep=t;c.plan(&e,1000,offered);auto states=step(e,e.curr_states,{observed[t]});if(states.empty())throw std::runtime_error("complete pickup fixture trace invalid");e.curr_states=states;}
  e.curr_timestep=3;const int at=e.curr_states[0].location;
  if(e.curr_states[0].orientation!=2)throw std::runtime_error("complete pickup fixture heading incorrect");
  // The nearest16 endpoints all lead to longer chains; task16 is farther away
  // but has the smallest complete pickup+chain cost, outside that shortlist.
  for(int id=0;id<17;++id){Task task;task.task_id=id;task.t_revealed=0;task.locations=id<16?std::vector<int>{at-1,at+16}:std::vector<int>{at-10,at-10};e.task_pool.emplace(id,task);}
  std::vector<int>proposed;c.schedule(&e,1000,proposed);
  if(proposed!=std::vector<int>{count[0]=='1'?16:0})throw std::runtime_error("complete pickup failed to rank a better task beyond the local shortlist");
  if(count[0]=='1'){
   if(c.stats().pickup_full_fields!=1||c.stats().pickup_full_scans<17||c.stats().pickup_full_candidates!=16)throw std::runtime_error("complete pickup budget or candidate count incorrect");
   c.schedule(&e,1000,proposed);if(proposed!=std::vector<int>{0}||c.stats().fair_assignments!=1)throw std::runtime_error("complete pickup bypassed oldest-task admission");
   e.curr_task_schedule={16};e.task_pool.at(16).agent_assigned=0;e.task_pool.at(16).idx_next_loc=1;e.curr_states[0].location=at-10;e.goal_locations={{{at-10,0}}};
   c.schedule(&e,1000,proposed);if(proposed!=std::vector<int>{16})throw std::runtime_error("complete pickup changed a started assignment");
  }
 }
 // With more free robots than the quota, one gets a complete field and the
 // other still receives a distinct task through the existing candidate search.
 {setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);auto e=blank();e.num_of_agents=2;
  e.curr_states={State(2*41+20,0,0),State(6*41+20,0,0)};e.curr_task_schedule={-1,-1};e.goal_locations.resize(2);
  Cgar c;c.initialize(&e,1000);std::vector<Action>offered;const std::array<Action,3>observed{Action::FW,Action::CR,Action::CR};
  for(int t=0;t<3;++t){e.curr_timestep=t;c.plan(&e,1000,offered);auto states=step(e,e.curr_states,{observed[t],observed[t]});if(states.empty())throw std::runtime_error("complete pickup quota trace invalid");e.curr_states=states;}
  e.curr_timestep=3;for(int id=0;id<2;++id){Task task;task.task_id=id;task.t_revealed=0;task.locations={e.curr_states[id].location-1};e.task_pool.emplace(id,task);}
  std::vector<int>proposed;c.schedule(&e,1000,proposed);
  if(proposed!=std::vector<int>({0,1})||c.stats().pickup_full_fields!=1||c.stats().pickup_flow_searches<1||e.curr_task_schedule!=std::vector<int>({-1,-1})||e.task_pool.at(0).agent_assigned!=-1||e.task_pool.at(1).agent_assigned!=-1)
   throw std::runtime_error("complete pickup quota lost an assignment, uniqueness or metadata isolation");
 }
 // Separate the shortlist key from final assignment ranking. Sixteen cheap
 // recent tasks exclude a more costly old task only in cost-key discovery.
 // Direct-cost dispatch stays identical; age-weighted dispatch differs, and
 // oldest-task admission must still override either shortlist on its next call.
 for(const char* hrrn:{"0","1"})for(const char* key:{"0","1"}){
  setenv("CGAR_HRRN",hrrn,1);setenv("CGAR_PICKUP_FULL_COST_KEY",key,1);setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);
  auto e=blank();Cgar c;c.initialize(&e,1000);std::vector<Action>offered;const std::array<Action,3>observed{Action::FW,Action::CR,Action::CR};
  for(int t=0;t<3;++t){e.curr_timestep=t;c.plan(&e,1000,offered);auto states=step(e,e.curr_states,{observed[t]});if(states.empty())throw std::runtime_error("cost-key observation fixture invalid");e.curr_states=states;}
  e.curr_timestep=100;const int at=e.curr_states[0].location;
  for(int id=0;id<17;++id){Task task;task.task_id=id;task.t_revealed=id<16?99:0;task.locations=id<16?std::vector<int>{at-1,at-2}:std::vector<int>{at-10,at+10};e.task_pool.emplace(id,task);}
  std::vector<int>proposed;c.schedule(&e,1000,proposed);const int expected=hrrn[0]=='1'&&key[0]=='0'?16:0;
  if(proposed!=std::vector<int>{expected}||c.stats().pickup_full_candidates!=16)throw std::runtime_error("complete pickup shortlist key did not separate cost from age priority");
  c.schedule(&e,1000,proposed);
  if(proposed!=std::vector<int>{16}||c.stats().fair_assignments!=1||e.task_pool.at(16).t_revealed!=0||e.curr_task_schedule!=std::vector<int>{-1})throw std::runtime_error("cost-key shortlist bypassed fair admission or changed simulator metadata");
 }
 // Exercise real field storage and assignment above the old64-field ceiling.
 // There are more eligible robots than every quota; all builders must finish,
 // and the unselected robots must still receive unique tasks without metadata edits.
 auto large_quota=[](int quota,int threads){
  setenv("CGAR_PICKUP_FULL_ROBOTS",std::to_string(quota).c_str(),1);
  setenv("CGAR_PICKUP_FULL_THREADS",std::to_string(threads).c_str(),1);
  setenv("CGAR_PICKUP_FULL_COST_KEY","0",1);setenv("CGAR_HRRN","0",1);
  SharedEnvironment e;e.rows=19;e.cols=79;e.num_of_agents=257;e.map.assign(e.rows*e.cols,0);
  e.curr_task_schedule.assign(e.num_of_agents,-1);e.goal_locations.resize(e.num_of_agents);
  for(int r=0;r<e.num_of_agents;++r)e.curr_states.emplace_back((2+2*(r/37))*e.cols+2+2*(r%37),0,0);
  Cgar c;c.initialize(&e,5000);std::vector<Action>offered;
  const std::array<Action,3>observed{Action::FW,Action::CR,Action::CR};
  for(int t=0;t<3;++t){e.curr_timestep=t;c.plan(&e,5000,offered);
   auto states=step(e,e.curr_states,std::vector<Action>(e.num_of_agents,observed[t]));
   if(states.empty())throw std::runtime_error("large complete pickup fixture trace invalid");e.curr_states=states;}
  e.curr_timestep=3;
  for(int r=0;r<e.num_of_agents;++r){Task task;task.task_id=r;task.t_revealed=0;
   task.locations={e.curr_states[r].location-1};e.task_pool.emplace(r,task);}
  std::vector<int>proposed;c.schedule(&e,5000,proposed);
  if(c.stats().pickup_full_fields!=quota||c.stats().pickup_flow_searches<1)
   throw std::runtime_error("large complete pickup quota did not build prescribed fields and preserve fallback");
  for(int r=0;r<e.num_of_agents;++r)
   if(proposed[r]!=r||e.curr_task_schedule[r]!=-1||e.task_pool.at(r).agent_assigned!=-1)
    throw std::runtime_error("large complete pickup quota lost nearest unique assignment or metadata isolation");
  return proposed;
 };
 for(int quota:{64,128,256})if(large_quota(quota,1)!=large_quota(quota,4))
  throw std::runtime_error("large complete pickup quotas changed assignments with parallel allocation");
 for(const char* key:{"CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_ORIENTATION_GUIDANCE","CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_PICKUP_FLOW","CGAR_HRRN","CGAR_PICKUP_WEIGHT","CGAR_PICKUP_FULL_ROBOTS","CGAR_PICKUP_FULL_THREADS","CGAR_PICKUP_FULL_COST_KEY"})unsetenv(key);
 std::cout<<"COMPLETE_PICKUP_FIELDS passed serial_parallel_exact=1 in_search_timeout=1 joined_after_failure=1 scratch_reuse=1 invalid_costs=1 cold_default=1 beyond_local_shortlist=1 fair_admission=1 started_immutable=1 fixed_robot_quota=1 actual_cpu_affinity_guard=1 cost_key_and_hrrn=1 large_quotas=64,128,256 large_serial_parallel_assignments=1542\n";
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
void assignment_permutation_regression() {
 auto check=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
 int callbacks=0;
 const int inf=kInf;
 std::vector<int> cycle_costs={10,1,4, 4,10,1, 1,4,10};
 auto cycle=minimum_pickup_permutation(cycle_costs,3,inf,[&]{++callbacks;});
 check(cycle.column==std::vector<int>({1,2,0})&&cycle.before==30&&cycle.after==3&&cycle.changed==3,
       "Hungarian cycle objective differs from hand optimum");
 auto cycles=pickup_permutation_cycles(cycle_costs,cycle,4,[&]{++callbacks;});
 check(cycles.size()==1&&cycles[0].rows==std::vector<int>({0,1,2})&&cycles[0].accepted&&
       cycles[0].before==30&&cycles[0].after==3,"three-cycle threshold or decomposition failed");
 check(callbacks>0,"Hungarian deadline callback was not exercised");

 std::vector<int> tied(16,4);
 auto identity=minimum_pickup_permutation(tied,4,inf,[]{});
 check(identity.column==std::vector<int>({0,1,2,3})&&identity.changed==0,
       "Hungarian tie objective did not minimize moved tasks");
 std::vector<int> blocked={0,inf,inf,inf,  inf,0,inf,inf, inf,inf,0,inf, inf,inf,inf,0};
 auto fixed=minimum_pickup_permutation(blocked,4,inf,[]{});
 check(fixed.column==std::vector<int>({0,1,2,3})&&pickup_permutation_cycles(blocked,fixed,1,[]{}).empty(),
       "forbidden edges allowed a non-identity permutation");

 // Exhaustively compare every feasible permutation on many small matrices.
 // Positive diagonals, tied costs and forbidden off-diagonals exercise real
 // rematching; all-zero diagonals would only test the trivial identity optimum.
 std::mt19937 matrix_rng(481507);int nonidentity=0,forbidden_cases=0;
 for(int n=1;n<=6;++n)for(int trial=0;trial<32;++trial){
  std::vector<int> costs(size_t(n)*n);
  for(int i=0;i<n;++i)for(int j=0;j<n;++j){
   costs[i*n+j]=int(matrix_rng()%17);
   if(i!=j&&matrix_rng()%5==0){costs[i*n+j]=inf;++forbidden_cases;}
  }
  auto answer=minimum_pickup_permutation(costs,n,inf,[]{});
  std::vector<int> order(n);std::iota(order.begin(),order.end(),0);
  long long best_cost=std::numeric_limits<long long>::max();int best_changed=n+1;
  do{
   long long total=0;int changed=0;bool feasible=true;
   for(int i=0;i<n;++i){feasible=feasible&&costs[i*n+order[i]]<inf;
    total+=costs[i*n+order[i]];changed+=order[i]!=i;}
   if(feasible&&(total<best_cost||(total==best_cost&&changed<best_changed))){best_cost=total;best_changed=changed;}
  }while(std::next_permutation(order.begin(),order.end()));
  check(answer.after==best_cost&&answer.changed==best_changed&&
        std::set<int>(answer.column.begin(),answer.column.end()).size()==size_t(n),
        "Hungarian result disagrees with exhaustive feasible lexicographic optimum");
  nonidentity+=answer.changed>0;
 }
 check(nonidentity>80&&forbidden_cases>100,"brute-force matrices do not exercise nontrivial/forbidden matching");
 // Integer arithmetic near INT_MAX must remain lossless for a maximum group.
 const int huge_inf=std::numeric_limits<int>::max();std::vector<int> huge(32*32,huge_inf);
 for(int i=0;i<32;++i){huge[i*32+i]=huge_inf-1;huge[i*32+(i+1)%32]=huge_inf-100;}
 auto huge_answer=minimum_pickup_permutation(huge,32,huge_inf,[]{});
 check(huge_answer.changed==32&&huge_answer.before==32LL*(huge_inf-1)&&
       huge_answer.after==32LL*(huge_inf-100),"large finite pickup cost arithmetic overflowed");
 // A strong cycle can be retained while a disjoint weak cycle stays identity.
 std::vector<int> split={100,1,inf,inf, 1,100,inf,inf, inf,inf,100,99, inf,inf,99,100};
 auto split_answer=minimum_pickup_permutation(split,4,inf,[]{});
 auto split_cycles=pickup_permutation_cycles(split,split_answer,4,[]{});
 check(split_cycles.size()==2&&split_cycles[0].accepted&&!split_cycles[1].accepted,
       "independent cycle thresholds did not separate strong and weak savings");
 bool rejected=false;
 try { minimum_pickup_permutation(std::vector<int>{inf,1,1,inf},2,inf,[]{ }); }
 catch(const std::invalid_argument&) { rejected=true; }
 check(rejected,"non-finite identity was accepted");
 bool timeout=false;
 try {
  minimum_pickup_permutation(cycle_costs,3,inf,[]{throw Timeout("assignment_regression");});
 } catch(const Timeout&) { timeout=true; }
 check(timeout,"Hungarian callback exception was swallowed");
 std::cout<<"ASSIGNMENT_PERMUTATION passed brute_force_n<=6 matrices=192 nontrivial_optima=1 int_max_group32=1 partial_cycles=1 three_cycle=1 forbidden_edges=1 tie_break=1 explicit_callback=1\n";
}
void unopened_matching_production() {
 auto e=swap_fixture();e.num_of_agents=3;e.curr_states={State(0,0,0),State(11,0,2),State(1,0,0)};
 e.curr_task_schedule={0,1,2};e.goal_locations={{{10,0}},{{1,0}},{{11,0}}};e.task_pool.clear();
 for(int i=0;i<3;++i){Task t;t.task_id=i;t.t_revealed=-5;t.agent_assigned=i;
  t.locations={i==0?10:(i==1?1:11)};e.task_pool.emplace(i,t);}
 const auto matching_initial=e;
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);
 setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_FLOW_STRENGTH","1",1);
 setenv("CGAR_FLOW_WARMUP","1",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","0",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","0",1);
 setenv("CGAR_PICKUP_FLOW","1",1);
 setenv("CGAR_GUIDE_ROUTES","0",1);setenv("CGAR_REASSIGN","0",1);setenv("CGAR_REASSIGN_POOL","0",1);
 setenv("CGAR_CHAIN_FLOW_PRICING","0",1);setenv("CGAR_TEMPORAL_REMAINING_FLOW","0",1);
 setenv("CGAR_REASSIGN_MATCH","1",1);
 Cgar c;c.initialize(&e,5000);std::vector<Action>actions;
 for(int t=0;t<2;++t){e.curr_timestep=t;c.plan(&e,5000,actions);auto next=step(e,e.curr_states,actions);
  if(next.empty())throw std::runtime_error("unopened matching fixture planner collided");e.curr_states=next;}
 if(c.stats().flow_publications!=1)throw std::runtime_error("unopened matching fixture did not publish learned flow");
 e.curr_timestep=10;std::vector<int>proposed;c.schedule(&e,5000,proposed);
 if(proposed!=std::vector<int>({0,2,1})||c.stats().match_passes!=1||c.stats().match_groups!=1||
    c.stats().match_accepted_cycles!=1||c.stats().match_moved!=2||c.stats().match_saving<8)
  throw std::runtime_error("resident unopened matching did not commit the bounded beneficial cycle");
 if(e.curr_task_schedule!=std::vector<int>({0,1,2}))
  throw std::runtime_error("unopened matching changed the simulator schedule");
 for(int i=0;i<3;++i)if(e.task_pool.at(i).agent_assigned!=i||e.task_pool.at(i).t_revealed!=-5||e.task_pool.at(i).idx_next_loc!=0)
  throw std::runtime_error("unopened matching changed task metadata");
 // A prescribed shorter cadence reaches the same useful resident-only cycle
 // earlier. Neither task identity, simulator metadata nor the retarget cap is relaxed.
 auto cadence_trial=[&](int interval){
  if(interval)setenv("CGAR_REASSIGN_MATCH_INTERVAL",std::to_string(interval).c_str(),1);
  else unsetenv("CGAR_REASSIGN_MATCH_INTERVAL");
  auto trial=matching_initial;Cgar policy;policy.initialize(&trial,5000);std::vector<Action> act;
  for(int t=0;t<2;++t){trial.curr_timestep=t;policy.plan(&trial,5000,act);auto next=step(trial,trial.curr_states,act);
   if(next.empty())throw std::runtime_error("matching cadence setup collided");trial.curr_states=next;}
  std::vector<long long> trace;
  for(int tick:{4,5,6,10,30}){
   trial.curr_timestep=tick;std::vector<int> next;policy.schedule(&trial,5000,next);
   const bool shorter=interval==5;
   if(tick==5&&(next!=(shorter?std::vector<int>{0,2,1}:std::vector<int>{0,1,2})||
      policy.stats().match_passes!=(shorter?1:0)))throw std::runtime_error("matching cadence did not control the real first cycle");
   if(next[0]!=0)throw std::runtime_error("matching cadence redirected protected primary");
   for(int r=0;r<3;++r){trace.push_back(next[r]);trial.curr_task_schedule[r]=next[r];
    auto& task=trial.task_pool.at(next[r]);task.agent_assigned=r;trial.goal_locations[r]={{{task.locations[0],0}}};}
   trace.push_back(policy.stats().match_passes);trace.push_back(policy.stats().match_moved);trace.push_back(policy.stats().match_saving);
  }
  if(policy.stats().match_moved!=2||policy.stats().match_accepted_cycles!=1||policy.stats().match_budget_protected<2||
     policy.stats().match_passes!=(interval==5?3:2))throw std::runtime_error("matching cadence changed the one-retarget limit or pass count");
  return trace;
 };
 const auto cadence_default=cadence_trial(0),cadence_explicit=cadence_trial(10),cadence_five=cadence_trial(5);
 if(cadence_default!=cadence_explicit||cadence_five==cadence_default)throw std::runtime_error("matching cadence default equivalence/active fixture failed");
 for(const char* bad:{"-1","0","101"}){setenv("CGAR_REASSIGN_MATCH_INTERVAL",bad,1);bool invalid=false;
  try{auto trial=matching_initial;Cgar policy;policy.initialize(&trial,5000);}catch(const std::invalid_argument&){invalid=true;}
  if(!invalid)throw std::runtime_error("invalid matching interval accepted");}
 setenv("CGAR_REASSIGN_MATCH_INTERVAL","5",1);setenv("CGAR_REASSIGN_MATCH","0",1);
 bool cadence_rejected=false;try{auto trial=matching_initial;Cgar policy;policy.initialize(&trial,5000);}catch(const std::invalid_argument&){cadence_rejected=true;}
 if(!cadence_rejected)throw std::runtime_error("nondefault cadence accepted without matching");
 const bool had_cadence_diagnostics=std::getenv("CGAR_DIAGNOSTICS");
 const std::string old_cadence_diagnostics=had_cadence_diagnostics?std::getenv("CGAR_DIAGNOSTICS"):"";
 setenv("CGAR_DIAGNOSTICS","1",1);setenv("CGAR_REASSIGN_MATCH","1",1);setenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE","10",1);
 {auto trial=matching_initial;Cgar policy;policy.initialize(&trial,5000);} // interval5, compatible shadow10
 setenv("CGAR_REASSIGN_MATCH_INTERVAL","3",1);
 cadence_rejected=false;try{auto trial=matching_initial;Cgar policy;policy.initialize(&trial,5000);}catch(const std::invalid_argument&){cadence_rejected=true;}
 if(!cadence_rejected)throw std::runtime_error("shadow stride incompatible with matching cadence accepted");
 unsetenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE");unsetenv("CGAR_REASSIGN_MATCH_INTERVAL");
 if(had_cadence_diagnostics)setenv("CGAR_DIAGNOSTICS",old_cadence_diagnostics.c_str(),1);else unsetenv("CGAR_DIAGNOSTICS");
 std::cout<<"MATCH_CADENCE passed default_exact=1 earlier_cycle=1 protected_primary=1 retarget_limit=1 prescribed_passes=1 config_guards=5\n";
 // Holder129 is outside the first128 eligible anchor IDs but close to holder1.
 // Sampling the spatial pool before grouping loses this useful two-cycle.
 SharedEnvironment wide;wide.rows=128;wide.cols=128;wide.map.assign(wide.rows*wide.cols,0);
 wide.num_of_agents=130;wide.goal_locations.resize(130);wide.curr_task_schedule.resize(130);
 for(int i=0;i<130;++i){
  int cell=(100+(i/16))*wide.cols+90+2*(i%16), goal=cell-30*wide.cols;
  if(i==0){cell=90*wide.cols+50;goal=50*wide.cols+50;}
  if(i==1){cell=3*wide.cols+3;goal=3*wide.cols+16;}
  if(i==129){cell=3*wide.cols+15;goal=3*wide.cols+2;}
  wide.curr_states.emplace_back(cell,0,i==129?2:0);wide.curr_task_schedule[i]=i;
  wide.goal_locations[i]={{goal,0}};Task task;task.task_id=i;task.t_revealed=-5;
  task.agent_assigned=i;task.locations={goal};wide.task_pool.emplace(i,task);
 }
 setenv("CGAR_TURN_BUILD_LIMIT","256",1);
 const auto wide_initial=wide;
 Cgar local;local.initialize(&wide,10000);
 for(int t=0;t<2;++t){wide.curr_timestep=t;local.plan(&wide,10000,actions);
  auto next=step(wide,wide.curr_states,actions);
  if(next.empty())throw std::runtime_error("all-resident matching fixture planner collided");wide.curr_states=next;}
 wide.curr_timestep=10;local.schedule(&wide,10000,proposed);
 if(proposed[1]!=129||proposed[129]!=1||local.stats().match_resident<=128||
    local.stats().match_selected>128||local.stats().match_groups>4||local.stats().match_nodes>8192||
    local.stats().match_full_groups<1||local.stats().match_anchors>4)
  throw std::runtime_error("local matching missed the non-anchor holder or exceeded fixed work");
 if(std::set<int>(proposed.begin(),proposed.end()).size()!=130)
  throw std::runtime_error("all-resident matching lost the task bijection");
 for(int i=0;i<130;++i)if(wide.curr_task_schedule[i]!=i||wide.task_pool.at(i).agent_assigned!=i||
    wide.task_pool.at(i).t_revealed!=-5||wide.task_pool.at(i).idx_next_loc!=0)
  throw std::runtime_error("all-resident matching changed simulator metadata");
 // A larger prescribed quota must genuinely reach beyond128participants,
 // while respecting its own groups/nodes/participants limits and protections.
 setenv("CGAR_REASSIGN_MATCH_GROUPS","64",1);
 auto broad=wide_initial;broad.num_of_agents=160;
 for(int i=130;i<160;++i){
  const int cell=(100+i/16)*broad.cols+90+2*(i%16),goal=cell-30*broad.cols;
  broad.curr_states.emplace_back(cell,0,0);broad.curr_task_schedule.push_back(i);
  broad.goal_locations.push_back({{goal,0}});Task task;task.task_id=i;task.t_revealed=-5;
  task.agent_assigned=i;task.locations={goal};broad.task_pool.emplace(i,task);
 }
 Cgar broad_match;broad_match.initialize(&broad,10000);
 for(int t=0;t<2;++t){broad.curr_timestep=t;broad_match.plan(&broad,10000,actions);
  auto next=step(broad,broad.curr_states,actions);
  if(next.empty())throw std::runtime_error("broad matching fixture collided");broad.curr_states=next;}
 broad.curr_timestep=10;broad_match.schedule(&broad,10000,proposed);
 if(proposed[0]!=0||proposed[1]!=129||proposed[129]!=1||
    broad_match.stats().match_selected<=128||broad_match.stats().match_selected>2048||
    broad_match.stats().match_groups>64||broad_match.stats().match_nodes>131072||
    std::set<int>(proposed.begin(),proposed.end()).size()!=160)
  throw std::runtime_error("broad matching quota: selected="+std::to_string(broad_match.stats().match_selected)+
    " resident="+std::to_string(broad_match.stats().match_resident)+" groups="+std::to_string(broad_match.stats().match_groups)+
    " primary="+std::to_string(broad_match.primary())+" proposed="+std::to_string(proposed[0])+","+
    std::to_string(proposed[1])+","+std::to_string(proposed[129]));
 unsetenv("CGAR_REASSIGN_MATCH_GROUPS");
 for(const char* quota:{"0","65"}){
  setenv("CGAR_REASSIGN_MATCH_GROUPS",quota,1);bool rejected=false;
  try{Cgar invalid;invalid.initialize(&broad,10000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid matching group quota accepted");
 }
 setenv("CGAR_REASSIGN_MATCH_GROUPS","64",1);setenv("CGAR_REASSIGN_MATCH","0",1);bool rejected=false;
 try{Cgar invalid;invalid.initialize(&broad,10000);}catch(const std::invalid_argument&){rejected=true;}
 if(!rejected)throw std::runtime_error("active group quota accepted without matching");
 unsetenv("CGAR_REASSIGN_MATCH_GROUPS");setenv("CGAR_REASSIGN_MATCH","1",1);
 // A far holder with a nearby pickup is invisible to robot-location groups.
 // Mixed groups must find that large crossed assignment under the SAME limits.
 SharedEnvironment crossed;crossed.rows=64;crossed.cols=160;crossed.map.assign(64*160,0);
 crossed.num_of_agents=3;crossed.curr_task_schedule={0,1,2};crossed.goal_locations.resize(3);
 const int cells[]={60*160+60,30*160+10,30*160+144};
 const int goals[]={40*160+60,30*160+145,30*160+11};
 for(int i=0;i<3;++i){crossed.curr_states.emplace_back(cells[i],0,i==2?2:0);
  crossed.goal_locations[i]={{{goals[i],0}}};Task task;task.task_id=i;task.t_revealed=-5;
  task.agent_assigned=i;task.locations={goals[i]};crossed.task_pool.emplace(i,task);}
 for(int mode=0;mode<=1;++mode){
  setenv("CGAR_REASSIGN_MATCH_PICKUP_GROUPS",mode?"1":"0",1);
  auto trial=crossed;Cgar match;match.initialize(&trial,10000);
  for(int t=0;t<2;++t){trial.curr_timestep=t;match.plan(&trial,10000,actions);
   auto next=step(trial,trial.curr_states,actions);
   if(next.empty())throw std::runtime_error("pickup grouping setup collided");trial.curr_states=next;}
  trial.curr_timestep=10;match.schedule(&trial,10000,proposed);
  const auto expected=mode?std::vector<int>({0,2,1}):std::vector<int>({0,1,2});
  if(proposed!=expected||match.stats().match_moved!=(mode?2:0)||
     match.stats().match_pickup_selected!=(mode?1:0)||match.stats().match_groups>4||
     match.stats().match_selected>128||match.stats().match_nodes>8192||
     (mode&&match.stats().match_saving<200))
   throw std::runtime_error("pickup grouping missed far holder or violated bounds/protection: mode="+std::to_string(mode)+
    " moved="+std::to_string(match.stats().match_moved)+" pickup="+std::to_string(match.stats().match_pickup_selected));
  for(int i=0;i<3;++i)if(trial.curr_task_schedule[i]!=i||trial.task_pool.at(i).agent_assigned!=i||
      trial.task_pool.at(i).t_revealed!=-5||trial.task_pool.at(i).idx_next_loc!=0)
   throw std::runtime_error("pickup grouping changed simulator metadata");
 }
 for(const char* value:{"-1","2"}){
  setenv("CGAR_REASSIGN_MATCH_PICKUP_GROUPS",value,1);bool invalid=false;
  try{Cgar test;test.initialize(&crossed,5000);}catch(const std::invalid_argument&){invalid=true;}
  if(!invalid)throw std::runtime_error("invalid pickup grouping mode accepted");
 }
 setenv("CGAR_REASSIGN_MATCH_PICKUP_GROUPS","1",1);setenv("CGAR_REASSIGN_MATCH","0",1);rejected=false;
 try{Cgar invalid;invalid.initialize(&crossed,5000);}catch(const std::invalid_argument&){rejected=true;}
 if(!rejected)throw std::runtime_error("pickup grouping accepted without matching");
 unsetenv("CGAR_REASSIGN_MATCH_PICKUP_GROUPS");setenv("CGAR_REASSIGN_MATCH","1",1);
 // Same-call new assignments must not borrow stale tickets in next-primary
 // selection, and mandatory fair admissions must retain their protection.
 auto warm=[](Cgar& planner,SharedEnvironment& env){
  planner.initialize(&env,5000);std::vector<Action> offered;
  for(int t=0;t<2;++t){env.curr_timestep=t;planner.plan(&env,5000,offered);
   auto next=step(env,env.curr_states,offered);
   if(next.empty())throw std::runtime_error("fresh matching setup collided");env.curr_states=next;}
  if(planner.stats().flow_publications!=1)throw std::runtime_error("fresh matching setup lacks published flow");
 };
 std::string protection_failures;
 {
  auto fresh=matching_initial;Cgar guarded;warm(guarded,fresh);
  fresh.curr_timestep=10;fresh.curr_states[0].location=10;
  fresh.curr_task_schedule[0]=-1;fresh.goal_locations[0].clear();fresh.task_pool.erase(0);
  Task task;task.task_id=3;task.t_revealed=10;task.locations={1};fresh.task_pool.emplace(3,task);
  guarded.schedule(&fresh,5000,proposed);
  if(proposed!=std::vector<int>({3,1,2})||guarded.stats().match_primary_protected!=2)
   protection_failures+=" fresh stale ticket masked next primary;";
  if(fresh.curr_task_schedule!=std::vector<int>({-1,1,2})||fresh.task_pool.at(3).agent_assigned!=-1)
   throw std::runtime_error("fresh matching next-primary fixture changed simulator metadata");
 }
 {
  auto fresh=matching_initial;fresh.curr_states[0]=State(24,0,0);
  fresh.task_pool.at(2).locations={1};fresh.goal_locations[2]={{{1,0}}};
  Cgar eligible;warm(eligible,fresh);fresh.curr_timestep=10;fresh.curr_states[2].location=1;
  fresh.curr_task_schedule[2]=-1;fresh.goal_locations[2].clear();fresh.task_pool.erase(2);
  Task task;task.task_id=3;task.t_revealed=10;task.locations={10};fresh.task_pool.emplace(3,task);
  eligible.schedule(&fresh,5000,proposed);
  if(proposed!=std::vector<int>({0,3,1})||eligible.stats().match_moved!=2)
   protection_failures+=" fresh unprotected task cannot join matching;";
  if(fresh.curr_task_schedule!=std::vector<int>({0,1,-1})||fresh.task_pool.at(3).agent_assigned!=-1)
   throw std::runtime_error("fresh matching participation fixture changed simulator metadata");
 }
 {
  SharedEnvironment fair;fair.rows=2;fair.cols=12;fair.map.assign(24,0);fair.num_of_agents=2;
  fair.curr_states={State(0,0,0),State(11,0,2)};fair.curr_task_schedule={-1,-1};fair.goal_locations.resize(2);
  for(int i=0;i<2;++i){Task task;task.task_id=i;task.t_revealed=0;task.locations={i==0?2:9};fair.task_pool.emplace(i,task);}
  Cgar guarded;guarded.initialize(&fair,5000);guarded.schedule(&fair,5000,proposed);
  if(proposed!=std::vector<int>({0,1})||guarded.stats().fair_assignments)
   throw std::runtime_error("fresh fair matching setup failed initial regular assignments");
  fair.curr_task_schedule=proposed;
  for(int i=0;i<2;++i){fair.task_pool.at(i).agent_assigned=i;fair.goal_locations[i]={{{fair.task_pool.at(i).locations[0],0}}};}
  for(int t=0;t<2;++t){fair.curr_timestep=t;guarded.plan(&fair,5000,actions);auto next=step(fair,fair.curr_states,actions);
   if(next.empty())throw std::runtime_error("fresh fair matching setup collided");fair.curr_states=next;}
  fair.curr_timestep=10;fair.curr_task_schedule[1]=-1;fair.goal_locations[1].clear();fair.task_pool.erase(1);
  Task task;task.task_id=3;task.t_revealed=-100;task.locations={2};fair.task_pool.emplace(3,task);
  guarded.schedule(&fair,5000,proposed);
  if(proposed!=std::vector<int>({0,3})||guarded.stats().fair_assignments!=1||guarded.stats().match_fair_protected!=1)
   protection_failures+=" fair-admitted fresh task lacks rematching protection;";
  if(fair.curr_task_schedule!=std::vector<int>({0,-1})||fair.task_pool.at(3).agent_assigned!=-1)
   throw std::runtime_error("fresh fair matching fixture changed simulator metadata");
 }
 if(!protection_failures.empty())throw std::runtime_error("UNOPENED_MATCHING_PROTECTION:"+protection_failures);
 unsetenv("CGAR_TURN_BUILD_LIMIT");
 for(const char* key:{"CGAR_REASSIGN_MATCH","CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_ORIENTATION_GUIDANCE",
                      "CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES",
                      "CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_PICKUP_FLOW","CGAR_GUIDE_ROUTES",
                      "CGAR_REASSIGN","CGAR_REASSIGN_POOL","CGAR_CHAIN_FLOW_PRICING","CGAR_TEMPORAL_REMAINING_FLOW"})unsetenv(key);
 std::cout<<"UNOPENED_MATCHING passed resident_tables=1 primary_protected=1 fixed_groups=1 cycle_commit=1 task_metadata_untouched=1 non_anchor_holder=1 fresh_holder=1 next_primary=1 fresh_fair_protected=1 bounded_broad_quota=1 pickup_near_far_holder=1\n";
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
SharedEnvironment pool_exchange_fixture() {
 SharedEnvironment e;e.num_of_agents=4;e.rows=3;e.cols=20;e.map.assign(60,0);e.curr_timestep=0;
 e.curr_states={State(0,0,0),State(20,0,0),State(39,0,2),State(59,0,2)};
 e.curr_task_schedule={10,11,12,13};e.goal_locations={{{15,0}},{{35,0}},{{23,0}},{{36,0}}};
 const std::vector<std::vector<int>> stops={{15},{35,36},{40,23},{41,36}};
 for(int r=0;r<4;++r){Task t;t.task_id=10+r;t.t_revealed=-7;t.agent_assigned=r;t.locations=stops[r];t.idx_next_loc=r>=2?1:0;e.task_pool.emplace(t.task_id,t);}
 Task available;available.task_id=100;available.t_revealed=-3;available.locations={22,23};e.task_pool.emplace(100,available);
 return e;
}
void pool_exchange_regression() {
 auto prepare=[](Cgar& c,SharedEnvironment& e){
  c.initialize(&e,1000);std::vector<Action>a;c.plan(&e,1000,a);
  auto states=step(e,e.curr_states,a);if(states.empty()||states[1].location!=21)throw std::runtime_error("pool exchange fixture did not advance to expected source");
  e.curr_states=states;e.curr_timestep=10;
 };
 for(bool enabled:{false,true}){
  setenv("CGAR_REASSIGN_POOL",enabled?"1":"0",1);auto e=pool_exchange_fixture();Cgar c;prepare(c,e);std::vector<int> proposed;c.schedule(&e,1000,proposed);
  const auto expected=enabled?std::vector<int>{10,100,12,13}:e.curr_task_schedule;
  if(proposed!=expected||c.stats().pool_exchanges!=int(enabled)||c.stats().pool_pickup_saving!=13*int(enabled)||c.stats().pool_total_saving!=13*int(enabled))
   throw std::runtime_error("pool exchange did not preserve disabled behavior or save independently counted pickup distance");
  if(e.curr_task_schedule!=std::vector<int>({10,11,12,13})||e.task_pool.at(11).agent_assigned!=1||e.task_pool.at(11).t_revealed!=-7||e.task_pool.at(100).agent_assigned!=-1||e.task_pool.at(100).t_revealed!=-3||e.task_pool.size()!=5)
   throw std::runtime_error("pool exchange mutated simulator state or discarded the released task");
  if(enabled&&(!c.stats().pool_primary_protected||proposed[2]!=12||proposed[3]!=13))throw std::runtime_error("pool exchange redirected a protected or started task");
 }
 setenv("CGAR_REASSIGN_POOL","1",1);
 for(int mode=0;mode<4;++mode){
  auto e=pool_exchange_fixture();
  if(mode==0){e.task_pool.at(11).locations={35,23};e.task_pool.at(100).locations={37};} // cheaper total, worse pickup
  if(mode==1)e.task_pool.at(100).locations={22,36,15,36}; // better pickup, worse total
  if(mode==2)e.task_pool.at(100).locations={22,57}; // missing complete chain table
  if(mode==3)setenv("CGAR_PLAN_TABLES","0",1); // missing incumbent pickup table
  Cgar c;prepare(c,e);std::vector<int> proposed;c.schedule(&e,1000,proposed);unsetenv("CGAR_PLAN_TABLES");
  if(proposed!=e.curr_task_schedule||c.stats().pool_exchanges)throw std::runtime_error("pool exchange accepted missing or non-improving distance evidence");
  if(mode==2&&!c.stats().pool_missing_chain)throw std::runtime_error("missing pool chain table was not observed");
  if(mode==3&&!c.stats().pool_missing_pickup)throw std::runtime_error("missing old pickup table was not observed");
 }
 // Once-retargeted tasks remain protected after the robot cooldown expires.
 auto e=pool_exchange_fixture();e.task_pool.at(100).locations={29};Cgar c;prepare(c,e);std::vector<int> proposed;c.schedule(&e,1000,proposed);
 if(proposed!=std::vector<int>({10,100,12,13})||c.stats().pool_exchanges!=1)throw std::runtime_error("pool retarget-limit fixture did not make its first exchange");
 e.curr_task_schedule=proposed;e.task_pool.at(11).agent_assigned=-1;e.task_pool.at(100).agent_assigned=1;e.goal_locations[1]={{{29,0}}};e.curr_timestep=11;
 std::vector<Action>a;c.plan(&e,1000,a);auto states=step(e,e.curr_states,a);
 if(states.empty()||states[1].location!=22)throw std::runtime_error("pool retarget-limit fixture did not finish its movement commitment");
 e.curr_states=states;e.curr_timestep=40;Task closer;closer.task_id=101;closer.t_revealed=40;closer.locations={23};e.task_pool.emplace(101,closer);
 c.schedule(&e,1000,proposed);
 if(proposed!=e.curr_task_schedule||c.stats().pool_exchanges!=1)throw std::runtime_error("pool exchange repeatedly retargeted an unopened task after cooldown");
 unsetenv("CGAR_REASSIGN_POOL");
 for(const char* value:{"-1","2"}){setenv("CGAR_REASSIGN_POOL",value,1);auto invalid=pool_exchange_fixture();bool rejected=false;try{Cgar x;x.initialize(&invalid,1000);}catch(const std::invalid_argument&){rejected=true;}if(!rejected)throw std::runtime_error("pool exchange accepted invalid boolean configuration");}
 unsetenv("CGAR_REASSIGN_POOL");
 std::cout<<"POOL_EXCHANGE passed pickup_distance=14->1 chain_distance=1->1 disabled_equivalence=1 independent_pickup_guard=1 total_guard=1 missing_tables=1 started_primary_protected=1 task_age_and_release_preserved=1 finite_retarget=1\n";
}
void pool_exchange_fair_admission() {
 setenv("CGAR_REASSIGN_POOL","1",1);
 SharedEnvironment e;e.num_of_agents=2;e.rows=2;e.cols=31;e.map.assign(62,0);e.curr_timestep=20;
 e.curr_states={State(0,0,0),State(30,0,2)};e.curr_task_schedule={-1,-1};e.goal_locations.resize(2);
 for(int i=0;i<2;++i){Task t;t.task_id=i;t.t_revealed=0;t.locations={i*30};e.task_pool.emplace(i,t);}
 Task old;old.task_id=100;old.t_revealed=-1;old.locations={20};for(int i=0;i<100;++i)old.locations.push_back(i%2?20:7);e.task_pool.emplace(100,old);
 Cgar c;c.initialize(&e,1000);std::vector<int> proposed;c.schedule(&e,1000,proposed);
 if(proposed!=std::vector<int>({0,1}))throw std::runtime_error("pool fair fixture did not defer old task");
 e.task_pool.erase(0);e.task_pool.erase(1);Task fresh;fresh.task_id=2;fresh.t_revealed=20;fresh.locations={7};e.task_pool.emplace(2,fresh);
 c.schedule(&e,1000,proposed);
 if(proposed!=std::vector<int>({2,100})||c.stats().fair_assignments!=1)throw std::runtime_error("pool fair fixture did not admit old task");
 e.curr_task_schedule=proposed;e.goal_locations={{{7,0}},{{20,0}}};for(int r=0;r<2;++r)e.task_pool.at(proposed[r]).agent_assigned=r;
 std::vector<Action>a;c.plan(&e,1000,a);auto states=step(e,e.curr_states,a);
 if(states.empty()||states[0].location!=1||states[1].location!=29||c.primary()!=0)throw std::runtime_error("pool fair fixture did not establish a non-primary fair episode");
 e.curr_states=states;e.curr_timestep=40;Task tempting;tempting.task_id=200;tempting.t_revealed=40;tempting.locations={28};e.task_pool.emplace(200,tempting);
 c.schedule(&e,1000,proposed);
 if(proposed!=e.curr_task_schedule||!c.stats().pool_fair_protected||c.stats().pool_exchanges)throw std::runtime_error("pool exchange redirected a fair admission toward a cheaper pickup");
 unsetenv("CGAR_REASSIGN_POOL");std::cout<<"POOL_EXCHANGE_FAIR_ADMISSION passed attractive_alternative=1 non_primary_fair_episode=1\n";
}

void oriented_distances() {
 const int rows=4,cols=5;std::vector<int> map={0,0,0,0,0, 0,1,1,0,0, 0,0,0,0,0, 0,0,1,1,1};
 const auto cert=build_certificate(map,rows,cols,2);TurnDistanceOracle oracle;
 TemporalGeometry geometry;geometry.initialize(cert.free,rows,cols,[]{});
 int checked=0,score_checks=0;
 for(int flow:{0,1}){
  std::vector<uint8_t> edge_costs(rows*cols*4,1);
  if(flow)for(int u=0;u<rows*cols;++u)for(int dir=0;dir<4;++dir)edge_costs[u*4+dir]=1+(u*11+dir*7)%3;
  for(int turn_cost:{1,2,4,8}){
  oracle.init(&cert,1,turn_cost,true);
  if(flow)oracle.set_forward_costs(edge_costs);
  for(int goal=0;goal<rows*cols;++goal)if(!map[goal]){
   const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(1));
   auto distance=[&](int cell,int dir){return oracle.value(*table,cell,dir);};
   for(int from=0;from<rows*cols;++from)if(!map[from])for(int ori=0;ori<4;++ori){
    int expected=kInf;
    if(cert.core[from]||cert.pocket[from]==cert.pocket[goal]){
     // Independent forward heap Dijkstra; production uses reverse BFS/buckets.
     using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
     std::vector<int> distances(rows*cols*4,kInf);q.push({0,from*4+ori});distances[from*4+ori]=0;
     while(!q.empty()){
      auto [cost,u]=q.top();q.pop();if(cost!=distances[u])continue;int cell=u/4,dir=u%4;
      if(cell==goal){expected=cost;break;}
      for(Action action:{Action::FW,Action::CR,Action::CCR}){
       int v=cell,d=dir;
       if(action==Action::FW)v=nb(cell,dir,rows,cols);
       else d=(dir+(action==Action::CR?1:3))%4;
       if(v<0||map[v]||(!cert.core[v]&&cert.pocket[v]!=cert.pocket[goal]))continue;
       int node=v*4+d,new_cost=cost+(action==Action::FW?edge_costs[cell*4+dir]:turn_cost);
       if(new_cost>=distances[node])continue;distances[node]=new_cost;q.push({new_cost,node});
      }
     }
    }
    if(distance(from,ori)!=expected)throw std::runtime_error("oriented oracle disagrees with independent weighted action search");
    ++checked;
    if(expected>=kInf)continue;
    const auto& paths=geometry.paths(from,ori);
    for(int op=0;op<129;++op){
     const auto& path=paths[op];if(!path.valid)continue;
     bool domain=true,completed=false;
     for(int cell:path.cells){domain&=cert.core[cell]||cert.pocket[cell]==cert.pocket[goal];completed|=cell==goal;}
     if(!domain||completed)continue;
     // Over five slots, physically attainable progress cannot exceed five
     // after charging extra turns, including virtual terminal-wait rotations.
     const int extra=TemporalGeometry::forward_surcharge(path,from,goal,[&](int u,int v){
      for(int dir=0;dir<4;++dir)if(nb(u,dir,rows,cols)==v)return int(edge_costs[u*4+dir]);
      throw std::runtime_error("invalid directed cost test edge");
     });
     for(int scale:{50,256,1024}){
      const int64_t score=TemporalGeometry::cost(path,op,goal,turn_cost,distance,scale)+int64_t(extra)*scale;
      if(score+op<int64_t(expected-5)*scale)throw std::runtime_error("temporal score rewards unpaid weighted movement");
      ++score_checks;
     }
    }
   }
   oracle.trim();if(oracle.find(goal))throw std::runtime_error("oriented cache exceeded its retained limit");
  }
  bool timed_out=false;try{oracle.table(0,std::chrono::steady_clock::now());}catch(const Timeout&){timed_out=true;}
  if(!timed_out||oracle.find(0))throw std::runtime_error("expired oriented traversal entered the cache");
 }
 }
 // Demand admission can explicitly retain one resident and evict another.
 TurnDistanceOracle retained;retained.init(&cert,1<<20);
 retained.table(0,std::chrono::steady_clock::now()+std::chrono::seconds(1));
 retained.table(1,std::chrono::steady_clock::now()+std::chrono::seconds(1));
 retained.retain({1});
 if(retained.has(0)||!retained.has(1)||retained.capacity()<2)throw std::runtime_error("orientation retention failed");
 std::cout<<"ORIENTED_DISTANCES passed state_goal_pairs="<<checked<<" weighted_score_checks="<<score_checks<<" turn_costs=1,2,4,8 directed_weights=1 explicit_timeout=1 bounded_cache=1\n";
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
 const bool warm=getenv("CGAR_TEMPORAL_WARM_START")&&std::string(getenv("CGAR_TEMPORAL_WARM_START"))!="0";
 const bool mixed=getenv("CGAR_TEMPORAL_MIXED_START")&&std::string(getenv("CGAR_TEMPORAL_MIXED_START"))!="0";
 const long long expected_warm=warm?79*(mixed?1:4):0;
 for(const auto* planner:{&serial,&parallel}){
  if(planner->stats().temporal_warm_worker_runs!=expected_warm||planner->stats().temporal_cold_worker_runs!=320-expected_warm||
     (warm&&!planner->stats().temporal_warm_retained))throw std::runtime_error("complete worker warm/cold participation was not exercised");
 }
 if(getenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN")&&std::string(getenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN"))=="1"){
  if(serial.stats().temporal_promise_calls!=80||!serial.stats().temporal_promise_retained||
     serial.stats().temporal_promise_calls!=parallel.stats().temporal_promise_calls||
     serial.stats().temporal_promise_retained!=parallel.stats().temporal_promise_retained||
     serial.stats().temporal_promise_collision_resets!=parallel.stats().temporal_promise_collision_resets)
   throw std::runtime_error("parallel promise fixture was vacuous or depended on thread scheduling");
 }
 for(const char* name:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_WORKERS","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_THREADS"})unsetenv(name);
 std::cout<<"TEMPORAL_PARALLEL passed workers=4 serial_vs_parallel_robot_decisions="<<comparisons<<"\n";
}

void temporal_priority_portfolio_regression() {
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 auto rejects=[&](auto run,const char* message){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,message);};
 std::vector<int> priority(64),order(64),goals(64,1);
 std::iota(order.begin(),order.end(),0);for(int i=0;i<64;++i)priority[i]=4*i;
 goals.back()=-1;priority.back()=kInf;
 TemporalPriorityPortfolio persistent,cold,disabled;
 persistent.configure(50,true,30,73);cold.configure(50,false,30,73);disabled.configure(0,false,30,73);
 const auto a=persistent.prepare(priority,order,goals,8,4,0,[]{});
 const auto b=cold.prepare(priority,order,goals,8,4,0,[]{});
 const auto small=persistent.prepare(priority,order,goals,4,4,0,[]{});
 require(a.orders==b.orders&&a.offsets==b.offsets&&!a.reused,"first-step persistence changed proposals");
 require(a.orders[0]==order&&a.changed_orders>0,"priority proposals failed to preserve anchor or change order");
 for(int worker=0;worker<8;++worker) {
  auto sorted=a.orders[worker];std::sort(sorted.begin(),sorted.end());require(sorted==order,"priority proposal lost a robot");
  require(a.orders[worker].back()==63&&a.offsets[worker].back()==0,"priority portfolio promoted an idle robot");
  for(int value:a.offsets[worker])require(value>=-50&&value<=50,"priority offset outside declared range");
  if(worker<4)require(a.orders[worker]==small.orders[worker]&&a.offsets[worker]==small.offsets[worker],"worker count changed portfolio prefix");
 }
 persistent.remember(a,4,0);cold.remember(b,4,0);
 const auto next=persistent.prepare(priority,order,goals,8,4,1,[]{});
 const auto fresh=cold.prepare(priority,order,goals,8,4,1,[]{});
 require(next.reused&&next.offsets[0]==a.offsets[4]&&!fresh.reused&&fresh.offsets[0]==std::vector<int>(64,0),"priority winner did not persist explicitly");
 require(persistent.prepare(priority,order,goals,8,4,1,[]{}).offsets==next.offsets,"proposal generation mutated persistent state");
 require(!persistent.prepare(priority,order,goals,8,4,2,[]{}).reused,"priority history reused across a missing step");
 const auto off=disabled.prepare(priority,order,goals,8,4,0,[]{});
 for(int worker=0;worker<8;++worker)require(off.orders[worker]==order&&off.offsets[worker]==std::vector<int>(64,0),"disabled priority proposals changed ordering");
 int checks=0;bool timeout=false;
 try{persistent.prepare(priority,order,goals,8,4,1,[&]{if(++checks==3)throw Timeout("priority_test");});}
 catch(const Timeout&){timeout=true;}
 require(timeout&&persistent.prepare(priority,order,goals,8,4,1,[]{}).offsets==next.offsets,"priority deadline returned partial work or changed history");
 auto duplicate=order;duplicate[1]=0;
 rejects([&]{persistent.prepare(priority,duplicate,goals,8,4,1,[]{});},"invalid priority permutation accepted");
 rejects([&]{persistent.remember(a,8,0);},"invalid priority winner accepted");
 // Real CGAR execution preserves all protected seeds and checks collisions.
 // Thread count changes only scheduling, including after repeated goal turnover.
 for(const char* history:{"0","1"}) {
  setenv("CGAR_TEMPORAL_PRIORITY_NOISE","50",1);setenv("CGAR_TEMPORAL_PRIORITY_PERSIST",history,1);
  temporal_parallel_regression();
 }
 for(const char* key:{"CGAR_TEMPORAL_PRIORITY_NOISE","CGAR_TEMPORAL_PRIORITY_PERSIST"})unsetenv(key);
 SharedEnvironment e;e.rows=e.cols=8;e.num_of_agents=24;e.map.assign(64,0);e.curr_task_schedule.assign(24,-1);e.goal_locations.resize(24);
 for(int i=0;i<24;++i){e.curr_states.emplace_back(i,0,i%4);e.goal_locations[i]={{63-i,0}};}
 for(const auto& setting:std::vector<std::pair<const char*,const char*>>{{"CGAR_TEMPORAL","1"},{"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_TEMPORAL_WORKERS","4"},{"CGAR_TEMPORAL_THREADS","1"},{"CGAR_TEMPORAL_STEPS","256"}})setenv(setting.first,setting.second,1);
 Cgar original;original.initialize(&e,1000);
 setenv("CGAR_TEMPORAL_PRIORITY_NOISE","0",1);setenv("CGAR_TEMPORAL_PRIORITY_PERSIST","0",1);setenv("CGAR_TEMPORAL_PRIORITY_MUTATION","30",1);
 Cgar zero;zero.initialize(&e,1000);
 for(int tick=0;tick<30;++tick) {
  e.curr_timestep=tick;std::vector<Action>a,b;original.plan(&e,1000,a);zero.plan(&e,1000,b);
  require(a==b&&original.primary()==zero.primary(),"explicit zero priority settings changed generic actions or primary");
  const auto next=step(e,e.curr_states,a);require(!next.empty(),"priority zero control collision");e.curr_states=next;
  for(int i=0;i<24;++i)if(e.curr_states[i].location==e.goal_locations[i][0].first)e.goal_locations[i][0].first=(e.goal_locations[i][0].first+37)%64;
 }
 for(const char* key:{"CGAR_TEMPORAL_PRIORITY_NOISE","CGAR_TEMPORAL_PRIORITY_PERSIST","CGAR_TEMPORAL_PRIORITY_MUTATION"}) {
  for(const char* bad:{"","-1","true","1.5","1000001"}) {
   unsetenv("CGAR_TEMPORAL_PRIORITY_NOISE");unsetenv("CGAR_TEMPORAL_PRIORITY_PERSIST");unsetenv("CGAR_TEMPORAL_PRIORITY_MUTATION");
   setenv(key,bad,1);rejects([&]{Cgar invalid;invalid.initialize(&e,1000);},"malformed priority configuration accepted");
  }
 }
 for(const auto& incompatible:std::vector<std::pair<const char*,const char*>>{{"CGAR_TEMPORAL","0"},{"CGAR_TEMPORAL_WORKERS","1"},{"CGAR_TEMPORAL_ORDER","0"},{"CGAR_TEMPORAL_PRIORITY_NOISE","0"}}) {
  setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_WORKERS","4",1);setenv("CGAR_TEMPORAL_ORDER","1",1);
  setenv("CGAR_TEMPORAL_PRIORITY_NOISE","50",1);setenv("CGAR_TEMPORAL_PRIORITY_PERSIST","1",1);unsetenv("CGAR_TEMPORAL_PRIORITY_MUTATION");
  setenv(incompatible.first,incompatible.second,1);rejects([&]{Cgar invalid;invalid.initialize(&e,1000);},"incompatible priority configuration accepted");
 }
 for(const char* key:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_WORKERS","CGAR_TEMPORAL_THREADS","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_ORDER","CGAR_TEMPORAL_PRIORITY_NOISE","CGAR_TEMPORAL_PRIORITY_PERSIST","CGAR_TEMPORAL_PRIORITY_MUTATION"})unsetenv(key);
 std::cout<<"TEMPORAL_PRIORITY_PORTFOLIO passed explicit_zero_actions=720 serial_parallel_actions=3840 retained_winner=1 worker_prefix=1 fixed_scores=1 deadline_atomic_history=1 bounded_offsets=1 invalid_configuration=1\n";
}

void temporal_mixed_start_regression() {
 setenv("CGAR_TEMPORAL_WARM_START","1",1);setenv("CGAR_TEMPORAL_MIXED_START","1",1);
 temporal_parallel_regression();
 setenv("CGAR_FLOW_STRENGTH","4",1);setenv("CGAR_FLOW_WARMUP","4",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 setenv("CGAR_FLOW_COST_SCALE","4",1);setenv("CGAR_TURN_SURCHARGE","1",1);
 temporal_parallel_regression();
 for(const char* key:{"CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_COST_SCALE","CGAR_TURN_SURCHARGE"})unsetenv(key);
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TEMPORAL_WORKERS","1",1);
 auto reject=[](){SharedEnvironment e;e.rows=e.cols=2;e.num_of_agents=1;e.map.assign(4,0);e.curr_states={State(0,0,0)};
  e.curr_task_schedule={-1};e.goal_locations.resize(1);bool rejected=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid mixed-start configuration accepted");};
 reject();setenv("CGAR_TEMPORAL_WORKERS","4",1);setenv("CGAR_TEMPORAL_WARM_START","0",1);reject();
 setenv("CGAR_TEMPORAL_WARM_START","1",1);setenv("CGAR_TEMPORAL_MIXED_START","2",1);reject();
 for(const char* key:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_WORKERS","CGAR_TEMPORAL_WARM_START","CGAR_TEMPORAL_MIXED_START"})unsetenv(key);
 std::cout<<"TEMPORAL_MIXED_START passed complete_worker0_warm=1 other3cold=1 first_entry_allcold=1 serial_parallel_actions=3840 fractional_metric_interaction=1 invalid_configuration=1\n";
}

void temporal_turn_progress_regression() {
 int checked=0,reproduced=0;
 const std::vector<std::vector<std::string>> fixtures={
  {"..#","#.#","#.#","#.#","#.#"}, {"..####","#....."}};
 for(size_t fixture=0;fixture<fixtures.size();++fixture){
  int rows=fixtures[fixture].size(),cols=fixtures[fixture][0].size();
  int start=0,goal=fixture?rows*cols-1:(rows-1)*cols+1;
  std::vector<char> free(rows*cols);
  for(int r=0;r<rows;++r)for(int c=0;c<cols;++c)free[r*cols+c]=fixtures[fixture][r][c]=='.';
  for(int rotation=0;rotation<4;++rotation){
   Certificate cert;cert.rows=rows;cert.cols=cols;cert.free=free;cert.core=free;cert.pocket.assign(free.size(),-1);
   TemporalGeometry geometry;geometry.initialize(free,rows,cols,[]{});
   auto neighbor=[&](int u,int dir){int v=nb(u,dir,rows,cols);return v>=0&&free[v]?v:-1;};
   std::vector<int> spatial(free.size(),kInf),queue{goal};spatial[goal]=0;
   for(size_t h=0;h<queue.size();++h)for(int dir=0;dir<4;++dir){
    int v=neighbor(queue[h],dir);if(v>=0&&spatial[v]==kInf){spatial[v]=spatial[queue[h]]+1;queue.push_back(v);}}
   auto fallback=[&](int u,int dir){return TemporalGeometry::fallback_distance(u,dir,kInf,[&](int v){return spatial[v];},neighbor);};
   const auto& paths=geometry.paths(start,rotation);
   for(int configured:{1,2,4,8}){
    if(rotation==0&&configured==(fixture?4:8)){
     // Reproduce the reviewed defect with the old fallback, then require the
     // production fallback to give strictly positive credit for an open route.
     auto legacy=[&](int u,int dir){return spatial[u]+configured*(fallback(u,dir)-spatial[u]);};
     int64_t wait=TemporalGeometry::cost(paths[0],0,goal,configured,legacy),best=wait;
     for(int op=1;op<129;++op)if(paths[op].valid)
      best=std::min(best,TemporalGeometry::cost(paths[op],op,goal,configured,legacy));
     if(best<wait)throw std::runtime_error("reviewed fallback counterexample was not reproduced");
     ++reproduced;
    }
    TurnDistanceOracle oracle;oracle.init(&cert,1<<20,configured,true);
    const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(1));
    for(bool exact:{false,true}){
     const int metric_turn_cost=exact?configured:1;
     auto distance=[&](int u,int dir){return exact?oracle.value(*table,u,dir):fallback(u,dir);};
     std::vector<std::vector<TemporalChoice>> choices(1);
     for(int op=0;op<129;++op)if(paths[op].valid)
      choices[0].push_back({&paths[op],TemporalGeometry::cost(paths[op],op,goal,metric_turn_cost,distance),op});
     std::sort(choices[0].begin()+1,choices[0].end(),[](const auto& a,const auto& b){return std::tie(a.cost,a.operation)<std::tie(b.cost,b.operation);});
     if(choices[0].size()<2||choices[0][1].cost>=choices[0][0].cost)
      throw std::runtime_error("an open corner route received no progress credit");
     std::vector<char> fixed(1,false);std::vector<double> power(1,1);
     TemporalPibt search(rows*cols,choices,fixed,power,8192,0);search.construct({0},[]{});search.repair(512,[]{});
     if(search.selected(0)==0||search.score()<=0)
      throw std::runtime_error("an unblocked temporal robot stayed motionless at a corner");
     ++checked;
    }
   }
   // Rotate both topology and orientation; no absolute direction is privileged.
   auto rotate=[&](int u){return (u%cols)*rows+(rows-1-u/cols);};
   std::vector<char> rotated(free.size());for(int u=0;u<int(free.size());++u)rotated[rotate(u)]=free[u];
   start=rotate(start);goal=rotate(goal);std::swap(rows,cols);free=std::move(rotated);
  }
 }
 std::cout<<"TEMPORAL_TURN_PROGRESS passed reproduced_old_defects="<<reproduced<<" exact_and_fallback_cases="<<checked<<" rotated_corner_routes=1\n";
}

void temporal_region_adapter_regression() {
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TEMPORAL_EQUAL_WEIGHT","1",1);
 setenv("CGAR_TEMPORAL_STEPS","256",1);setenv("CGAR_TEMPORAL_REGIONS","4",1);
 setenv("CGAR_TEMPORAL_REGION_STEPS","256",1);setenv("CGAR_TEMPORAL_REGION_ROUNDS","2",1);
 SharedEnvironment e;e.rows=9;e.cols=11;e.num_of_agents=48;e.map.assign(99,0);e.curr_task_schedule.assign(48,-1);e.goal_locations.resize(48);
 for(int r=0;r<48;++r){e.curr_states.emplace_back(r,0,r%4);e.goal_locations[r]={{98-r,0}};}
 const bool had_preparation=std::getenv("CGAR_TEMPORAL_PREP_THREADS")!=nullptr;
 const std::string saved_preparation=had_preparation?std::getenv("CGAR_TEMPORAL_PREP_THREADS"):"";
 setenv("CGAR_TEMPORAL_PREP_THREADS","1",1);setenv("CGAR_TEMPORAL_REGION_THREADS","1",1);Cgar serial;serial.initialize(&e,1000);
 setenv("CGAR_TEMPORAL_PREP_THREADS","4",1);setenv("CGAR_TEMPORAL_REGION_THREADS","4",1);Cgar parallel;parallel.initialize(&e,1000);
 if(had_preparation)setenv("CGAR_TEMPORAL_PREP_THREADS",saved_preparation.c_str(),1);else unsetenv("CGAR_TEMPORAL_PREP_THREADS");
 int checked=0;long long observed_turns=0;
 for(int t=0;t<100;++t){
  e.curr_timestep=t;std::vector<Action>a,b;serial.plan(&e,1000,a);parallel.plan(&e,1000,b);
  for(Action action:a)observed_turns+=action==Action::CR||action==Action::CCR;
  if(a!=b)throw std::runtime_error("production regional threads changed decisions");
  auto next=step(e,e.curr_states,a);if(next.empty())throw std::runtime_error("production regional collision");e.curr_states=next;
  for(int r=0;r<48;++r)if(e.curr_states[r].location==e.goal_locations[r][0].first)e.goal_locations[r][0].first=(e.goal_locations[r][0].first+37)%99;
  checked+=48;
 }
 for(const Cgar* planner:{&serial,&parallel}){
  const auto& stats=planner->stats();
  if(stats.temporal_seed_rotations+stats.temporal_planned_rotations+stats.temporal_protected_rotations!=observed_turns||
     stats.temporal_tied_seed_rotations>stats.temporal_seed_rotations||stats.temporal_seed_rotations>stats.temporal_wait_seeds)
   throw std::runtime_error("temporal turn attribution does not match independently executed actions");
  if(std::getenv("CGAR_TEMPORAL_STRICT_WAIT_TURNS")&&std::atoi(std::getenv("CGAR_TEMPORAL_STRICT_WAIT_TURNS"))&&stats.temporal_tied_seed_rotations)
   throw std::runtime_error("strict wait mode issued an equal-distance seed rotation");
 }
 const int audit_stride=std::getenv("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE")?std::atoi(std::getenv("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE")):0;
 if(serial.stats().temporal_conflict_audits!=(audit_stride?100/audit_stride:0)||
    parallel.stats().temporal_conflict_audits!=serial.stats().temporal_conflict_audits)
  throw std::runtime_error("production forward audit used the wrong fixed sample schedule");
 if(serial.stats().temporal_prepared_robots!=checked||parallel.stats().temporal_prepared_robots!=checked||
    serial.stats().temporal_parallel_preparations||parallel.stats().temporal_parallel_preparations!=100)
  throw std::runtime_error("production preparation threads did not process the complete fleet");
 if(std::getenv("CGAR_FLOW_STRENGTH")){
  const int refresh=std::getenv("CGAR_FLOW_REFRESH_INTERVAL")?std::atoi(std::getenv("CGAR_FLOW_REFRESH_INTERVAL")):0;
  const int warmup=std::getenv("CGAR_FLOW_WARMUP")?std::atoi(std::getenv("CGAR_FLOW_WARMUP")):128;
  const int expected=99<warmup?0:1+(refresh?(99-warmup)/refresh:0);
  const bool cache_only=std::getenv("CGAR_FLOW_CACHE_ONLY_REFRESH")&&std::atoi(std::getenv("CGAR_FLOW_CACHE_ONLY_REFRESH"));
  const int expected_cache_only=cache_only?std::max(0,expected-1):0;
  if(serial.stats().flow_cache_only_resets!=expected_cache_only||parallel.stats().flow_cache_only_resets!=expected_cache_only)
   throw std::runtime_error("production cache-only reset count differs from fixed observation schedule");
  if(serial.stats().flow_freezes!=(refresh?0:expected)||parallel.stats().flow_freezes!=(refresh?0:expected)||
     serial.stats().flow_publications!=expected||parallel.stats().flow_publications!=expected||
     !serial.stats().flow_penalized_edges||serial.stats().flow_penalized_edges!=parallel.stats().flow_penalized_edges||
     serial.stats().flow_cache_resets!=parallel.stats().flow_cache_resets||(refresh&&serial.stats().flow_cache_resets<2))
   throw std::runtime_error("production learned-flow publications or cache resets differ across threads");
 }
 if(std::getenv("CGAR_TEMPORAL_WARM_START")&&
    (!serial.stats().temporal_warm_retained||serial.stats().temporal_warm_retained!=parallel.stats().temporal_warm_retained||
     serial.stats().temporal_warm_collision_resets!=parallel.stats().temporal_warm_collision_resets))
  throw std::runtime_error("production warm start failed to reuse identical valid suffixes");
 if(getenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN")&&std::string(getenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN"))=="1"){
  if(serial.stats().temporal_promise_calls!=100||!serial.stats().temporal_promise_retained||
     serial.stats().temporal_promise_calls!=parallel.stats().temporal_promise_calls||
     serial.stats().temporal_promise_retained!=parallel.stats().temporal_promise_retained||
     serial.stats().temporal_promise_collision_resets!=parallel.stats().temporal_promise_collision_resets)
   throw std::runtime_error("regional promise fixture was vacuous or depended on thread scheduling");
 }
 if(std::getenv("CGAR_GUIDE_ROUTES")&&
    (!serial.stats().guide_robot_steps||!serial.stats().guide_solved||serial.stats().guide_robot_steps!=parallel.stats().guide_robot_steps||
     serial.stats().guide_solved!=parallel.stats().guide_solved||serial.stats().guide_expanded!=parallel.stats().guide_expanded))
  throw std::runtime_error("production guide routes did not produce identical nontrivial guidance");
 if(std::getenv("CGAR_GUIDE_RECONNECT_STEPS")&&std::atoi(std::getenv("CGAR_GUIDE_RECONNECT_STEPS"))>0&&
    (!serial.stats().guide_reconnections||serial.stats().guide_reconnections!=parallel.stats().guide_reconnections))
  throw std::runtime_error("production guide reconnection was vacuous or changed with thread count");
 if(std::getenv("CGAR_GUIDE_REFINE_BATCH")&&std::atoi(std::getenv("CGAR_GUIDE_REFINE_BATCH"))>0&&
    (!serial.stats().guide_refinements||serial.stats().guide_refinements!=parallel.stats().guide_refinements))
  throw std::runtime_error("production guide refinement was vacuous or changed with thread count");
 for(const char* name:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_EQUAL_WEIGHT","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_REGIONS","CGAR_TEMPORAL_REGION_STEPS","CGAR_TEMPORAL_REGION_ROUNDS","CGAR_TEMPORAL_REGION_THREADS"})unsetenv(name);
 std::cout<<"TEMPORAL_REGION_ADAPTER passed identical_robot_decisions="<<checked<<" threads=1,4 preparation_threads=1,4 valid_episodes=1\n";
}

void temporal_regions_regression(long long candidate_limit = 0) {
 const int rows=9,cols=11,cells=rows*cols,count=48;
 std::vector<char> free(cells,true);TemporalGeometry geometry;geometry.initialize(free,rows,cols,[]{});
 int checked=0,boundary_cases=0,improved_cases=0,temperature_changes=0;long long searched_candidates=0,limited_batches=0;bool work_reduced=false;
 std::vector<std::vector<int>> default_results(24);
 for(int temperature:{1000,100,0})for(int trial=0;trial<24;++trial){
  std::mt19937_64 random(trial+77);std::vector<int> locations(cells);std::iota(locations.begin(),locations.end(),0);
  std::shuffle(locations.begin(),locations.end(),random);locations.resize(count);
  std::vector<std::vector<TemporalChoice>> choices(count);std::vector<char> fixed(count,false);std::vector<double> power(count,1);
  std::vector<int> order(count);std::iota(order.begin(),order.end(),0);std::shuffle(order.begin(),order.end(),random);
  for(int r=0;r<count;++r){
   int goal=random()%cells;const auto& paths=geometry.paths(locations[r],random()%4);
   for(int k=0;k<129;++k)if(paths[k].valid){
    int end=paths[k].cells[4],distance=std::abs(end/cols-goal/cols)+std::abs(end%cols-goal%cols);
    choices[r].push_back({&paths[k],int64_t(distance)*50-k,k});
   }
   std::sort(choices[r].begin()+1,choices[r].end(),[](const auto& a,const auto& b){return std::tie(a.cost,a.operation)<std::tie(b.cost,b.operation);});
  }
  TemporalPibt initial(cells,choices,fixed,power,8192,trial);initial.construct(order,[]{});initial.repair(512,[]{});
  fixed[trial%count]=true; // Protect a complete path that may already be moving.
  TemporalRegionOptions options;options.parts=4;options.rounds=trial%2?4:1;options.steps=256;options.threads=1;options.temperature_ppm=temperature;
  options.candidate_limit=candidate_limit;
  std::mt19937_64 serial_rng(trial+100),parallel_rng(trial+100);TemporalRegionStats a_stats,b_stats;
  auto serial=repair_temporal_regions(rows,cols,locations,choices,fixed,power,8192,initial,options,serial_rng,a_stats,[]{});
  options.threads=4;
  auto parallel=repair_temporal_regions(rows,cols,locations,choices,fixed,power,8192,initial,options,parallel_rng,b_stats,[]{});
  if(serial->selections()!=parallel->selections()||a_stats.candidates!=b_stats.candidates||a_stats.repairs!=b_stats.repairs||serial_rng!=parallel_rng)
   throw std::runtime_error("regional parallel scheduling changed the complete result");
  TemporalRegionStats audit_stats;std::mt19937_64 audit_rng(trial+100);options.audit_peaks=true;
  auto audited=repair_temporal_regions(rows,cols,locations,choices,fixed,power,8192,initial,options,audit_rng,audit_stats,[]{});
  if(audited->selections()!=serial->selections()||audited->score()!=serial->score()||audit_rng!=serial_rng||
     audit_stats.candidates!=a_stats.candidates||audit_stats.repairs!=a_stats.repairs||audit_stats.accepted!=a_stats.accepted||
     audit_stats.kept_regions!=a_stats.kept_regions||audit_stats.reverted_regions!=a_stats.reverted_regions||
     audit_stats.round_scores!=a_stats.round_scores||!audit_stats.peaks.batches||
     audit_stats.peaks.attempts!=audit_stats.repairs||
     (!candidate_limit&&audit_stats.peaks.attempts!=audit_stats.peaks.batches*options.steps)||audit_stats.peaks.max_peak_attempt>options.steps)
   throw std::runtime_error("regional peak audit changed search/RNG or missed complete attempts");
  if(serial->score()+1e-6<initial.score())throw std::runtime_error("regional complete-plan score decreased");
  if(serial->score()>initial.score()+1e-6)++improved_cases;
  if(temperature==1000)default_results[trial]=serial->selections();
  else temperature_changes+=default_results[trial]!=serial->selections();
  searched_candidates+=a_stats.candidates;
  limited_batches+=a_stats.candidate_limited_batches;
  if(a_stats.candidate_limited_batches!=b_stats.candidate_limited_batches||a_stats.max_batch_candidates!=b_stats.max_batch_candidates)
   throw std::runtime_error("regional candidate budgets differ across thread schedules");
  if(candidate_limit&&temperature==1000&&trial==0){
   auto uncapped_options=options;uncapped_options.candidate_limit=0;
   TemporalRegionStats uncapped_stats;std::mt19937_64 uncapped_rng(trial+100);
   repair_temporal_regions(rows,cols,locations,choices,fixed,power,8192,initial,uncapped_options,uncapped_rng,uncapped_stats,[]{});
   work_reduced=a_stats.candidates<uncapped_stats.candidates&&a_stats.repairs<uncapped_stats.repairs;
  }
  if(!candidate_limit&&trial==0){
   auto unused_cap_options=options;unused_cap_options.candidate_limit=100000000;
   TemporalRegionStats unused_cap_stats;std::mt19937_64 unused_cap_rng(trial+100);
   auto unused_cap=repair_temporal_regions(rows,cols,locations,choices,fixed,power,8192,initial,unused_cap_options,unused_cap_rng,unused_cap_stats,[]{});
   if(unused_cap->selections()!=serial->selections()||unused_cap_stats.candidates!=a_stats.candidates||unused_cap_rng!=serial_rng||unused_cap_stats.candidate_limited_batches)
    throw std::runtime_error("nonbinding regional candidate limit changed decisions or work");
  }
  if(a_stats.kept_regions!=b_stats.kept_regions||a_stats.reverted_regions!=b_stats.reverted_regions||
     a_stats.frozen_crossers!=b_stats.frozen_crossers||a_stats.round_scores!=b_stats.round_scores||
     a_stats.score_after!=serial->score()||a_stats.round_scores.size()!=size_t(options.rounds))
   throw std::runtime_error("regional contribution counters are inconsistent");
  // Count boundary-crossing input paths directly; protected robots alone do
  // not establish that the geometric boundary rule was exercised.
  auto quadrant=[&](int cell){return (cell/cols)*2/rows*2+(cell%cols)*2/cols;};
  bool crosses=false;
  for(int r=0;r<count;++r)if(!fixed[r])for(int cell:initial.choice(r).path->cells)
   crosses|=quadrant(cell)!=quadrant(locations[r]);
  if(crosses)++boundary_cases;
  if(options.rounds==1){
   for(int r=0;r<count;++r){
    int region=quadrant(locations[r]);bool crossing=false;
    for(int cell:initial.choice(r).path->cells)crossing|=quadrant(cell)!=region;
    if(crossing&&serial->selected(r)!=initial.selected(r))throw std::runtime_error("one-round repair changed a boundary crosser");
    if(serial->selected(r)!=initial.selected(r))for(int cell:serial->choice(r).path->cells)
     if(quadrant(cell)!=region)throw std::runtime_error("changed regional path escaped its start region");
   }
  }
  for(int r=0;r<count;++r)if(fixed[r]&&serial->selected(r)!=initial.selected(r))throw std::runtime_error("regional repair changed protected path");
  for(int t=0;t<5;++t){
   std::vector<int> owner(cells,-1),previous(count);
   for(int r=0;r<count;++r){const auto& path=*serial->choice(r).path;int to=path.cells[t];previous[r]=t?path.cells[t-1]:locations[r];
    if(owner[to]>=0)throw std::runtime_error("regional merge vertex collision");owner[to]=r;}
   for(int r=0;r<count;++r){int other=owner[previous[r]];
    if(other>=0&&other!=r&&previous[other]==serial->choice(r).path->cells[t])throw std::runtime_error("regional merge edge swap");}
  }
  if(trial==0){
   std::atomic<int> calls{0};std::mt19937_64 failure_rng(100);TemporalRegionStats ignored;bool failed=false;
   try{repair_temporal_regions(rows,cols,locations,choices,fixed,power,8192,initial,options,failure_rng,ignored,[&]{if(++calls>30)throw Timeout("regional_fixture");});}
   catch(const Timeout&){failed=true;}
   if(!failed)throw std::runtime_error("regional worker timeout was swallowed");
  }
  checked+=count;
 }
 if(!boundary_cases||!improved_cases||!searched_candidates||!temperature_changes)throw std::runtime_error("regional fixture was vacuous");
 if(candidate_limit&&(!limited_batches||!work_reduced))throw std::runtime_error("regional work limit did not reduce observed candidate work and attempts");
 std::cout<<"TEMPORAL_REGIONS passed serial_vs_parallel_robots="<<checked<<" boundary_cases="<<boundary_cases<<" improved_cases="<<improved_cases<<" searched_candidates="<<searched_candidates<<" temperature_changes="<<temperature_changes<<" candidate_limit="<<candidate_limit<<" limited_batches="<<limited_batches<<" temperatures=1000,100,0 complete_merge_valid=1 protected_paths=1 explicit_timeout=1\n";
 // Exercise the production adapter, including primary/pocket/capacity protection.
 setenv("CGAR_TEMPORAL_REGIONS","4",1);setenv("CGAR_TEMPORAL_REGION_THREADS","4",1);
 setenv("CGAR_TEMPORAL_REGION_STEPS","128",1);setenv("CGAR_TEMPORAL_REGION_ROUNDS","2",1);
 temporal_primary_regression();
 for(const char* name:{"CGAR_TEMPORAL_REGIONS","CGAR_TEMPORAL_REGION_THREADS","CGAR_TEMPORAL_REGION_STEPS","CGAR_TEMPORAL_REGION_ROUNDS"})unsetenv(name);
}

void temporal_region_budget_regression() {
 SharedEnvironment e;e.rows=3;e.cols=3;e.num_of_agents=1;e.map.assign(9,0);
 e.curr_states.emplace_back(0,0,0);e.curr_task_schedule.assign(1,-1);e.goal_locations={{{8,0}}};
 for(const char* value:{"-1","100000001","1"}){
  setenv("CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT",value,1);
  setenv("CGAR_TEMPORAL_REGIONS",std::string(value)=="1"?"0":"4",1);
  bool rejected=false;try{Cgar planner;planner.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid regional candidate budget was accepted");
 }
 unsetenv("CGAR_TEMPORAL_REGIONS");unsetenv("CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT");
 temporal_regions_regression(2000);
 setenv("CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT","2000",1);
 temporal_region_adapter_regression();
 setenv("CGAR_TEMPORAL_REGIONS","4",1);setenv("CGAR_TEMPORAL_REGION_STEPS","128",1);
 temporal_primary_regression();
 for(const char* name:{"CGAR_TEMPORAL_REGIONS","CGAR_TEMPORAL_REGION_STEPS","CGAR_TEMPORAL_REGION_CANDIDATE_LIMIT"})unsetenv(name);
 std::cout<<"TEMPORAL_REGION_BUDGET passed invalid_settings=3 serial_parallel_equal=1 candidate_work_reduced=1 ownership_protection=1\n";
}

void turn_prewarm_regression() {
 auto require=[](bool ok,const char* message){if(!ok)throw std::runtime_error(message);};
 const int rows=8,cols=9;std::vector<int> map(rows*cols,0);
 for(int cell:{12,13,21,39,48})map[cell]=1;
 const auto cert=build_certificate_feasible(map,rows,cols,8);
 std::vector<int> goals;for(int cell=0;cell<int(map.size());++cell)if(!map[cell])goals.push_back(cell);
 const size_t logical_bytes=goals.size()*goals.size()*4*sizeof(int);
 auto deadline=[](){return std::chrono::steady_clock::now()+std::chrono::seconds(10);};
 std::vector<uint8_t> weights(map.size()*4,20);
 for(size_t i=0;i<weights.size();++i)weights[i]=(i%7==0?200:20)+(i%11==0);
 long long compared=0;
 for(int turn:{1,4})for(bool compact:{false,true})for(int threads:{1,4}){
  TurnDistanceOracle oracle;oracle.init(&cert,logical_bytes,turn,compact,20,201);oracle.set_forward_costs(weights);
  oracle.table(goals.front(),deadline()); // Preserve an already complete table.
  const auto bytes=oracle.prewarm_all(threads,deadline());
  require(bytes==goals.size()*goals.size()*4*(compact?sizeof(uint16_t):sizeof(int)),"prewarm resident byte accounting");
  require(oracle.prefetched_builds==int(goals.size())-1&&oracle.prefetched_hits==oracle.prefetched_builds&&
          !oracle.prefetched_discarded,"prewarm skipped or discarded prescribed work");
  auto neighbor=[&](int cell,int h){
   if((h==0&&cell%cols+1==cols)||(h==1&&cell/cols+1==rows)||(h==2&&cell%cols==0)||(h==3&&cell/cols==0))return -1;
   return cell+(h==0?1:h==1?cols:h==2?-1:-cols);
  };
  for(int goal:goals){
   // Independent heap traversal, with original cells rather than compact indices.
   std::vector<int> expected(map.size()*4,kInf);
   using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> heap;
   for(int h=0;h<4;++h){expected[goal*4+h]=0;heap.push({0,goal*4+h});}
   while(!heap.empty()){
    auto x=heap.top();heap.pop();const int d=x.first,cell=x.second/4,h=x.second%4;
    if(d!=expected[x.second])continue;
    auto relax=[&](int to,int cost){if(d+cost<expected[to]){expected[to]=d+cost;heap.push({d+cost,to});}};
    relax(cell*4+(h+1)%4,turn);relax(cell*4+(h+3)%4,turn);
    const int from=neighbor(cell,(h+2)%4);
    if(from>=0&&!map[from]&&(cert.core[from]||cert.pocket[from]==cert.pocket[goal]))relax(from*4+h,weights[from*4+h]);
   }
   const auto* table=oracle.peek(goal);require(table,"prewarm omitted a free goal");
   for(int cell:goals)for(int h=0;h<4;++h){
    require(oracle.value(*table,cell,h)==expected[cell*4+h],"prewarm differs from independent heap");++compared;
   }
  }
  const auto builds=oracle.prefetched_builds;
  require(oracle.prewarm_all(threads,deadline())==bytes&&oracle.prefetched_builds==builds,"repeat prewarm rebuilt complete tables");
 }
 TurnDistanceOracle small;small.init(&cert,logical_bytes-1,1,true,20,201);
 bool rejected=false;try{small.prewarm_all(4,deadline());}catch(const std::invalid_argument&){rejected=true;}
 require(rejected&&!small.prefetched_builds&&!small.peek(goals.front()),"insufficient prewarm capacity accepted or mutated cache");
 TurnDistanceOracle expired;expired.init(&cert,logical_bytes,1,true,20,201);
 rejected=false;try{expired.prewarm_all(4,std::chrono::steady_clock::now());}catch(const Timeout&){rejected=true;}
 require(rejected&&!expired.prefetched_builds&&!expired.peek(goals.front()),"expired prewarm returned a partial success");
 require(expired.prewarm_all(4,deadline())>0,"prewarm cannot retry after timeout");
 for(const char* value:{"","-1","33","999999999999999999","1x"}){
  setenv("CGAR_TRICK_NATIVE_PREWARM_THREADS",value,1);
  rejected=false;try{tricks::options("WAREHOUSE");}catch(const std::invalid_argument&){rejected=true;}
  require(rejected,"invalid prewarm selector accepted");
 }
 setenv("CGAR_TRICK_NATIVE_PREWARM_THREADS","0",1);
 rejected=false;try{tricks::options("");}catch(const std::invalid_argument&){rejected=true;}
 require(rejected,"prewarm selector accepted without CLI trick");
 rejected=false;try{tricks::options("WAREHOUSE");}catch(const std::invalid_argument&){rejected=true;}
 require(rejected,"prewarm selector accepted without native metric");
 setenv("CGAR_TRICK_NATIVE_METRIC","1",1);
 require(tricks::options("WAREHOUSE").native_prewarm_threads==0,"explicit prewarm zero changed default");
 for(const char* value:{"1","8","32"}){
  setenv("CGAR_TRICK_NATIVE_PREWARM_THREADS",value,1);
  require(tricks::options("WAREHOUSE").native_prewarm_threads==std::stoi(value),"prewarm selector parsed incorrectly");
 }
 unsetenv("CGAR_TRICK_NATIVE_PREWARM_THREADS");unsetenv("CGAR_TRICK_NATIVE_METRIC");
 require(!tricks::options("WAREHOUSE").native_prewarm_threads,"missing prewarm selector enabled work");
 std::cout<<"TURN_PREWARM passed independent_heap_states="<<compared
  <<" multi_batch=1 serial_parallel=1 compact_wide=1 existing_tables=1 complete_residency=1 capacity_guard=1 explicit_timeout=1 cli_gate=1\n";
}

void turn_prefetch_regression() {
 const int rows=4,cols=5;std::vector<int> map(rows*cols,0);const auto cert=build_certificate(map,rows,cols,2);
 const size_t bytes=rows*cols*4*sizeof(int)*2;
 for(int weight:{1,4}){
  TurnDistanceOracle plain,parallel;plain.init(&cert,bytes,weight);parallel.init(&cert,bytes,weight,true);
  auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(10);
  parallel.prefetch({0,1,1,2},4,deadline);
  for(int goal:{0,1,2})if(parallel.has(goal))throw std::runtime_error("speculative table changed cache admission");
  for(int goal:{1,3,2,1,4}){
   if(*plain.table(goal,deadline)!=*parallel.table(goal,deadline))throw std::runtime_error("prefetched distance values changed");
   plain.trim();parallel.trim();
   for(int check=0;check<5;++check)if(plain.has(check)!=parallel.has(check))throw std::runtime_error("prefetch changed LRU eviction");
  }
  parallel.discard_prefetch();
  if(parallel.prefetched_builds!=3||parallel.prefetched_hits!=2||parallel.prefetched_discarded!=1)
   throw std::runtime_error("prefetch accounting did not include unused complete work");
  bool failed=false;try{parallel.prefetch({7,8},4,std::chrono::steady_clock::now());}catch(const Timeout&){failed=true;}
  if(!failed||parallel.has(7)||parallel.has(8))throw std::runtime_error("expired speculative batch entered the cache");
 }
 int compared=0;
 for(const char* weight:{"1","4"}){
  setenv("CGAR_TURN_COST",weight,1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
  setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","256",1);
  SharedEnvironment e;e.rows=10;e.cols=10;e.num_of_agents=64;e.map.assign(100,0);e.curr_task_schedule.assign(64,-1);e.goal_locations.resize(64);
  for(int r=0;r<64;++r){e.curr_states.emplace_back(r,0,r%4);e.goal_locations[r]={{99-r,0}};}
  setenv("CGAR_TURN_COMPACT","0",1);setenv("CGAR_TURN_PREFETCH_THREADS","0",1);Cgar plain;plain.initialize(&e,1000);
  setenv("CGAR_TURN_PREFETCH_THREADS","1",1);Cgar serial;serial.initialize(&e,1000);
  setenv("CGAR_TURN_COMPACT","1",1);setenv("CGAR_TURN_PREFETCH_THREADS","4",1);Cgar parallel;parallel.initialize(&e,1000);
  setenv("CGAR_TURN_PREFETCH_THREADS","0",1);Cgar packed;packed.initialize(&e,1000);
  for(int t=0;t<100;++t){
   e.curr_timestep=t;std::vector<Action>a,b,c,d;plain.plan(&e,1000,a);serial.plan(&e,1000,b);parallel.plan(&e,1000,c);packed.plan(&e,1000,d);
   if(a!=b||a!=c||a!=d)throw std::runtime_error("speculative or compact orientation work changed planner decisions");
   if(plain.stats().oriented_builds!=serial.stats().oriented_builds||plain.stats().oriented_builds!=parallel.stats().oriented_builds||plain.stats().oriented_builds!=packed.stats().oriented_builds||
      plain.stats().oriented_fallback!=serial.stats().oriented_fallback||plain.stats().oriented_fallback!=parallel.stats().oriented_fallback||plain.stats().oriented_fallback!=packed.stats().oriented_fallback)
    throw std::runtime_error("speculative orientation work changed cache admission counts");
   auto next=step(e,e.curr_states,a);if(next.empty())throw std::runtime_error("prefetched planner collision");e.curr_states=next;
   for(int r=0;r<64;++r)if(e.curr_states[r].location==e.goal_locations[r][0].first)
    e.goal_locations[r][0].first=(e.goal_locations[r][0].first+37)%100;
   compared+=64;
  }
 }
 for(const char* name:{"CGAR_TURN_COST","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_TURN_PREFETCH_THREADS","CGAR_TURN_COMPACT"})unsetenv(name);
 std::cout<<"TURN_PREFETCH passed identical_robot_decisions="<<compared<<" cache_values=1 demand_admission=1 lru_order=1 unused_discarded=1 compact_equivalence=1 explicit_timeout=1\n";
}

void compact_turn_tables() {
 const std::vector<int> values{0,1,65534,kInf};TurnTable compact(values,true),wide(values,false);
 if(compact!=wide||!compact.is_compact()||compact.storage_bytes()*2!=wide.storage_bytes())
  throw std::runtime_error("compact turn table did not preserve the sentinel and finite boundary");
 for(int boundary:{65535,65536}){
  TurnTable fallback({0,boundary,kInf},true);
  if(fallback.is_compact()||fallback[1]!=boundary||fallback[2]!=kInf)
   throw std::runtime_error("large distance overflowed compact turn storage");
 }
 // A real reverse traversal whose finite distances exceed the 16-bit sentinel.
 Certificate corridor;corridor.rows=1;corridor.cols=70000;corridor.free.assign(70000,true);
 corridor.core=corridor.free;corridor.pocket.assign(70000,-1);
 TurnDistanceOracle oracle;oracle.init(&corridor,4<<20,1,true);
 const auto* table=oracle.table(69999,std::chrono::steady_clock::now()+std::chrono::seconds(10));
 if(table->is_compact()||oracle.value(*table,0,0)!=69999||oracle.value(*table,0,2)!=70001)
  throw std::runtime_error("large graph did not use exact wide orientation storage");
 std::cout<<"COMPACT_TURN_TABLES passed sentinel=1 finite_boundaries=1 wide_fallback_cells=70000 storage_halved_when_safe=1\n";
}

void flow_guidance_regression() {
 const std::vector<int> cycle{0,1,3,2,0};std::vector<char> free(4,true);
 FlowGuidance flow;flow.initialize(free,2,2,4,4,1);
 for(int t=0;t<=4;++t){
  if(flow.observe(t,{cycle[t]})!=(t==4))throw std::runtime_error("flow freeze was not tied to fixed observed work");
  if(flow.observe(t,{cycle[t]}))throw std::runtime_error("duplicate observation changed flow work");
 }
 if(!flow.frozen()||flow.samples()!=4||flow.moves()!=4||flow.penalized_edges()!=4)
  throw std::runtime_error("flow failed to learn the observed cycle");
 const auto original=flow.costs();
 for(auto [u,dir]:std::vector<std::pair<int,int>>{{0,0},{1,1},{3,2},{2,3}}){
  int v=nb(u,dir,2,2);
  if(original[u*4+dir]!=1||original[v*4+(dir+2)%4]!=5)throw std::runtime_error("flow penalized the observed dominant direction");
 }
 for(int t=5;t<20;++t)if(flow.observe(t,{cycle[(20-t)%4]})||flow.costs()!=original)
  throw std::runtime_error("frozen flow changed with later movement");
 // Rotation and reflection equivariance exclude absolute-direction templates.
 for(bool reflection:{false,true})for(int rotation=0;rotation<4;++rotation){
  auto transform_cell=[&](int u){int r=u/2,c=u%2;if(reflection)c=1-c;for(int k=0;k<rotation;++k){int old_r=r;r=c;c=1-old_r;}return r*2+c;};
  auto transform_dir=[&](int dir){if(reflection)dir=(2-dir+4)%4;return (dir+rotation)%4;};
  FlowGuidance transformed;transformed.initialize(free,2,2,4,4,1);
  for(int t=0;t<=4;++t)transformed.observe(t,{transform_cell(cycle[t])});
  for(int u=0;u<4;++u)for(int dir=0;dir<4;++dir)
   if(transformed.costs()[transform_cell(u)*4+transform_dir(dir)]!=original[u*4+dir])
    throw std::runtime_error("learned flow used an absolute-direction template");
 }
 FlowGuidance neutral;neutral.initialize(free,2,2,4,4,1);
 for(int t=0;t<=4;++t)neutral.observe(t,{t%2});
 if(neutral.penalized_edges()||*std::max_element(neutral.costs().begin(),neutral.costs().end())!=1)
  throw std::runtime_error("equal opposing flows received a directional preference");
 FlowGuidance sparse;sparse.initialize(free,2,2,2,4,2);sparse.observe(0,{0});sparse.observe(1,{1});sparse.observe(2,{3});
 if(sparse.penalized_edges())throw std::runtime_error("insufficient evidence received a directional preference");
 FlowGuidance skipped;skipped.initialize(free,2,2,2,4,1);skipped.observe(0,{0});skipped.observe(2,{3});
 if(skipped.samples()||skipped.moves())throw std::runtime_error("missing observations invented intermediate moves");
 skipped.observe(3,{2});skipped.observe(4,{0});if(skipped.moves()!=2||!skipped.frozen())throw std::runtime_error("consecutive flow observations were lost");
 Certificate cert;cert.rows=2;cert.cols=2;cert.free=free;cert.core=free;cert.pocket.assign(4,-1);
 TurnDistanceOracle oracle;oracle.init(&cert,1<<20,1,true);auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(1);
 oracle.table(0,deadline);if(oracle.set_forward_costs(neutral.costs())||!oracle.has(0))throw std::runtime_error("neutral flow unnecessarily invalidated the cache");
 oracle.prefetch({1},1,deadline);
 if(!oracle.set_forward_costs(original)||oracle.has(0)||oracle.prefetched_discarded!=1)
  throw std::runtime_error("a metric change retained old-metric table data");
 const auto* weighted=oracle.table(0,deadline);
 // From 1 facing west: costly reverse edge takes 5, clockwise three-edge
 // detour requires three turns and costs 6. Independent hand-computed optimum.
 if(oracle.value(*weighted,1,2)!=5)throw std::runtime_error("directed learned costs produced a wrong distance");
 if(oracle.set_forward_costs(original)||!oracle.has(0))throw std::runtime_error("unchanged flow invalidated the cache");
 auto invalid=original;invalid[0]=0;bool rejected=false;
 try{oracle.set_forward_costs(invalid);}catch(const std::invalid_argument&){rejected=true;}
 if(!rejected||!oracle.has(0))throw std::runtime_error("invalid flow altered the existing metric");
 setenv("CGAR_FLOW_STRENGTH","2",1);setenv("CGAR_FLOW_WARMUP","8",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 temporal_region_adapter_regression();temporal_primary_regression();
 for(const char* name:{"CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES"})unsetenv(name);
 std::cout<<"FLOW_GUIDANCE passed observed_moves_only=1 fixed_freeze=1 rotation_reflection=1 neutral_evidence=1 cache_invalidation=1 protected_progress=1 threaded_replay=1\n";
}

void temporal_distance_scale_regression() {
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(144,true),12,12,[]{});
 const auto& paths=geometry.paths(65,0);int checked=0;
 for(int scale:{256,1024}){
  // Over the whole native operation index range, one extra distance unit must
  // lose to a lower-distance choice. At legacy scale 50 it need not do so.
  for(int better_op=0;better_op<129;++better_op)for(int worse_op=0;worse_op<129;++worse_op){
   const auto better=TemporalGeometry::cost(paths[better_op],better_op,143,1,[](int,int){return 10;},scale);
   const auto worse=TemporalGeometry::cost(paths[worse_op],worse_op,143,1,[](int,int){return 11;},scale);
   if(better>=worse)
    throw std::runtime_error("operation preference overrode scaled distance progress");
   ++checked;
  }
  auto distance=[](int cell,int){return 100+cell;};
  for(int op=0;op<129;++op)if(paths[op].valid){
   const int64_t legacy=TemporalGeometry::cost(paths[op],op,143,1,distance);
   const int64_t weighted=TemporalGeometry::cost(paths[op],op,143,1,distance,scale);
   if((legacy+op)*scale!=(weighted+op)*50)throw std::runtime_error("scale changed the native operation tie term");
  }
 }
 if(TemporalGeometry::cost(paths[0],0,143,1,[](int,int){return 10;})<=
    TemporalGeometry::cost(paths[128],128,143,1,[](int,int){return 11;}))throw std::runtime_error("legacy operation-weight example was not reproduced");
 setenv("CGAR_TEMPORAL_DISTANCE_SCALE","256",1);temporal_primary_regression();temporal_region_adapter_regression();
 unsetenv("CGAR_TEMPORAL_DISTANCE_SCALE");
 std::cout<<"TEMPORAL_DISTANCE_SCALE passed dominance_pairs="<<checked<<" native_tie_term=1 protected_progress=1 threaded_replay=1\n";
}

void temporal_wait_turn_regression() {
 int reproduced=0,retained=0,strict_improvements=0;
 for(int rotation=0;rotation<4;++rotation)for(int scale:{1,2,4,8}){
  Certificate cert;cert.rows=cert.cols=5;cert.free.assign(25,1);cert.core=cert.free;cert.pocket.assign(25,-1);
  int goal=24;for(int k=0;k<rotation;++k)goal=(goal%5)*5+(4-goal/5);
  const int start=12;
  TurnDistanceOracle oracle;oracle.init(&cert,1<<20,scale,true,scale);
  const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(1));
  auto distance=[&](int u,int dir){return oracle.value(*table,u,dir);};
  if(distance(start,rotation)!=5*scale||distance(start,(rotation+1)%4)!=5*scale)
   throw std::runtime_error("independent diagonal-goal distance counterexample changed");
  TemporalGeometry geometry;geometry.initialize(cert.free,5,5,[]{});
  std::vector<TemporalPath> seeds(5);std::vector<std::vector<TemporalChoice>> choices(5);
  const auto& paths=geometry.paths(start,rotation);
  for(int op=0;op<129;++op)if(paths[op].valid)
   choices[0].push_back({&paths[op],TemporalGeometry::cost(paths[op],op,goal,scale,distance,50,scale),op});
  std::sort(choices[0].begin()+1,choices[0].end(),[](const auto& a,const auto& b){return std::tie(a.cost,a.operation)<std::tie(b.cost,b.operation);});
  for(int d=0;d<4;++d){
   seeds[d+1]=geometry.seed(nb(start,d,5,5),d,3);choices[d+1].push_back({&seeds[d+1],0,0});
  }
  std::vector<char> fixed={false,true,true,true,true};std::vector<double> power(5,1);
  TemporalPibt search(25,choices,fixed,power,8192,0);search.construct({0,1,2,3,4},[]{});search.repair(128,[]{});
  if(search.selected(0)!=0)throw std::runtime_error("blocked diagonal fixture did not select the wait seed");
  int heading=rotation;
  for(int t=0;t<8;++t){
   const int wait=distance(start,heading),right=distance(start,(heading+1)%4),left=distance(start,(heading+3)%4);
   const int legacy=TemporalGeometry::wait_action(wait,right,left,false);
   if(legacy==3||(legacy==1?right:left)!=wait)throw std::runtime_error("equal-distance rotation cycle was not reproduced");
   heading=(heading+(legacy==1?1:3))%4;++reproduced;
   if(TemporalGeometry::wait_action(distance(start,rotation),distance(start,(rotation+1)%4),distance(start,(rotation+3)%4),true)!=3)
    throw std::runtime_error("strict mode failed to retain a tied heading");
   ++retained;
  }
  if(heading!=rotation)throw std::runtime_error("legacy rotations did not return to their starting heading");
  const int back=(rotation+2)%4;
  const int action=TemporalGeometry::wait_action(distance(start,back),distance(start,(back+1)%4),distance(start,(back+3)%4),true);
  if(action==3||distance(start,(back+(action==1?1:3))%4)>=distance(start,back))
   throw std::runtime_error("strict mode prevented a useful orientation improvement");
  ++strict_improvements;
 }
 if(TemporalGeometry::wait_action(kInf,kInf,kInf,true)!=3||TemporalGeometry::wait_action(0,0,0,true)!=3||
    TemporalGeometry::wait_action(9,7,7,true)!=1)
  throw std::runtime_error("strict wait turn boundary semantics failed");
 setenv("CGAR_TEMPORAL_STRICT_WAIT_TURNS","1",1);
 temporal_region_adapter_regression();temporal_primary_regression();
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","8",1);setenv("CGAR_FLOW_MIN_SAMPLES","2",1);
 setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","50",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);setenv("CGAR_TEMPORAL_WARM_START","1",1);
 temporal_region_adapter_regression();temporal_primary_regression();
 for(const char* key:{"CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_TEMPORAL_WARM_START","CGAR_TEMPORAL_STRICT_WAIT_TURNS"})unsetenv(key);
 std::cout<<"TEMPORAL_WAIT_TURNS passed reproduced_tied_rotations="<<reproduced<<" retained_heading_checks="<<retained
          <<" useful_strict_turns="<<strict_improvements<<" blocked_kernel_fixture=1 rotated_and_scaled=1 independent_turn_counters=1 protected_parallel_warm_actions=9600\n";
}

void flow_cost_scale_regression() {
 const int rows=5,cols=7,cells=rows*cols;std::vector<char> free(cells,true);
 for(int row=1;row<4;++row)free[row*cols+3]=false;
 Certificate cert;cert.rows=rows;cert.cols=cols;cert.free=cert.core=free;cert.pocket.assign(cells,-1);
 TemporalGeometry geometry;geometry.initialize(free,rows,cols,[]{});
 long long distances_checked=0,scores_checked=0,uniform_scores=0;
 for(int unit:{1,2,4,8})for(bool biased:{false,true})for(int extra_turn:{0,1,2}){
  const int turn=unit+extra_turn;
  std::vector<uint8_t> edge(cells*4,unit);
  if(biased)for(int u=0;u<cells;++u)for(int d=0;d<4;++d)edge[u*4+d]+=(u*13+d*3)%5==0;
  TurnDistanceOracle oracle;oracle.init(&cert,1<<20,turn,true,unit);
  if(oracle.set_forward_costs(edge)!=biased)throw std::runtime_error("scaled neutral field changed the metric");
  for(int goal:{0,16,18,34}){
   const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(1));
   std::vector<int> exact(cells*4,kInf);
   // Independent forward heap search for each source state; production uses
   // reverse BFS or integer buckets and compact goal tables.
   for(int cell=0;cell<cells;++cell)if(free[cell])for(int dir=0;dir<4;++dir){
    using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;
    std::vector<int> d(cells*4,kInf);const int start=cell*4+dir;d[start]=0;q.push({0,start});
    while(!q.empty()){
     const auto [cost,state]=q.top();q.pop();if(cost!=d[state])continue;
     if(state/4==goal){exact[start]=cost;break;}
     auto relax=[&](int next,int step_cost){if(cost+step_cost<d[next]){d[next]=cost+step_cost;q.push({d[next],next});}};
     relax(state/4*4+(state%4+1)%4,turn);relax(state/4*4+(state%4+3)%4,turn);
     const int next=nb(state/4,state%4,rows,cols);
     if(next>=0&&free[next])relax(next*4+state%4,edge[state]);
    }
    if(oracle.value(*table,cell,dir)!=exact[start])throw std::runtime_error("scaled distance disagreed with independent Dijkstra");
    ++distances_checked;
   }
   auto distance=[&](int cell,int dir){return oracle.value(*table,cell,dir);};
   for(int cell=0;cell<cells;++cell)if(free[cell])for(int dir=0;dir<4;++dir){
    const auto& paths=geometry.paths(cell,dir);
    for(int op=0;op<129;++op)if(paths[op].valid){
     const auto& path=paths[op];const int extra=TemporalGeometry::forward_surcharge(path,cell,goal,[&](int from,int to){
      for(int d=0;d<4;++d)if(nb(from,d,rows,cols)==to)return int(edge[from*4+d]);
      throw std::runtime_error("invalid scaled-score edge");},unit);
     const int64_t cost=TemporalGeometry::cost(path,op,goal,turn,distance,50,unit)+int64_t(extra)*50;
     const bool reached=std::find(path.cells.begin(),path.cells.end(),goal)!=path.cells.end();
     if(!reached&&cost+int64_t(op)*unit<int64_t(exact[cell*4+dir]-5*unit)*50)
      throw std::runtime_error("scaled temporal score rewarded unpaid progress");
     ++scores_checked;
     if(!biased&&!extra_turn){
      auto original=[&](int u,int d){return exact[u*4+d]/unit;};
      const int64_t expected=TemporalGeometry::cost(path,op,goal,1,original)*unit;
      if(extra||cost!=expected||TemporalGeometry::cost(path,op,-1,unit,distance,50,unit)!=int64_t(op)*unit)
       throw std::runtime_error("pure unit scaling changed terminal, wait, completion or operation preferences");
      ++uniform_scores;
     }
    }
   }
  }
  auto invalid=edge;invalid[0]=unit-1;bool rejected=false;
  try{oracle.set_forward_costs(invalid);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected||!oracle.has(34))throw std::runtime_error("below-base costs changed a valid scaled metric");
 }
 for(int unit:{2,4,8}){
  FlowGuidance flow;flow.initialize(std::vector<char>(4,true),2,2,4,1,1,50,0,unit);
  const int cycle[5]={0,1,3,2,0};for(int t=0;t<=4;++t)flow.observe(t,{cycle[t]});
  for(auto [u,d]:std::vector<std::pair<int,int>>{{0,0},{1,1},{3,2},{2,3}}){
   const int v=nb(u,d,2,2);
   if(flow.costs()[u*4+d]!=unit||flow.costs()[v*4+(d+2)%4]!=unit+1)
    throw std::runtime_error("scaled flow did not preserve a one-unit directional penalty");
  }
 }
 for(int scale:{0,3,16}){bool rejected=false;try{FlowGuidance flow;flow.initialize(std::vector<char>(4,true),2,2,4,1,1,0,0,scale);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid flow unit scale was accepted");}

 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","256",1);setenv("CGAR_TEMPORAL_EQUAL_WEIGHT","1",1);
 setenv("CGAR_TEMPORAL_ORDER","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TURN_FIRST","1",1);
 setenv("CGAR_TURN_TABLE_MB","1",1);setenv("CGAR_TURN_COMPACT","1",1);setenv("CGAR_TEMPORAL_PREP_THREADS","4",1);
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","8",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","100",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);setenv("CGAR_TEMPORAL_WARM_START","1",1);
 SharedEnvironment e;e.rows=50;e.cols=50;e.num_of_agents=48;e.map.assign(2500,0);e.curr_task_schedule.assign(48,-1);e.goal_locations.resize(48);
 for(int r=0;r<48;++r){e.curr_states.emplace_back(r*17,0,r%4);e.goal_locations[r]={{2499-r*13,0}};}
 Cgar planners[4];const int units[4]={1,2,4,8};
 for(int k=0;k<4;++k){setenv("CGAR_FLOW_COST_SCALE",std::to_string(units[k]).c_str(),1);planners[k].initialize(&e,1000);}
 int identical=0;
 for(int t=0;t<80;++t){
  e.curr_timestep=t;std::vector<Action> reference;
  planners[0].plan(&e,1000,reference);
  for(int k=1;k<4;++k){std::vector<Action> actions;planners[k].plan(&e,1000,actions);
   if(actions!=reference)throw std::runtime_error("neutral unit scaling changed production decisions under cache eviction and warm reuse");identical+=48;}
  auto next=step(e,e.curr_states,reference);if(next.empty())throw std::runtime_error("scaled-metric neutral episode collided");e.curr_states=next;
  for(int r=0;r<48;++r)if(e.curr_states[r].location==e.goal_locations[r][0].first)e.goal_locations[r][0].first=(e.goal_locations[r][0].first+997)%2500;
 }
 const auto& reference=planners[0].stats();
 for(const auto& planner:planners){const auto& stats=planner.stats();
  if(stats.oriented_builds<=48||stats.oriented_builds!=reference.oriented_builds||stats.oriented_guided!=reference.oriented_guided||
     stats.oriented_fallback!=reference.oriented_fallback||stats.flow_publications!=5||stats.flow_cache_resets||stats.flow_penalized_edges)
   throw std::runtime_error("scaled neutral field changed cache behavior or failed to exercise refresh");
 }
 for(const char*name:{"CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_EQUAL_WEIGHT","CGAR_TEMPORAL_ORDER","CGAR_ORIENTATION_GUIDANCE",
      "CGAR_TURN_FIRST","CGAR_TURN_TABLE_MB","CGAR_TEMPORAL_PREP_THREADS"})unsetenv(name);
 setenv("CGAR_FLOW_COST_SCALE","4",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","50",1);
 temporal_region_adapter_regression();temporal_primary_regression();
 for(const char*name:{"CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL",
      "CGAR_FLOW_COST_SCALE","CGAR_TEMPORAL_WARM_START","CGAR_TURN_COMPACT"})unsetenv(name);
 std::cout<<"FLOW_COST_SCALE passed independent_weighted_distances="<<distances_checked<<" paid_progress_scores="<<scores_checked
          <<" exact_uniform_scores="<<uniform_scores<<" neutral_robot_decisions="<<identical
          <<" fractional_turn_surcharges=0,1,2 scales=1,2,4,8 cache_pressure=1 repeated_neutral_fields=5 protected_parallel_warm_actions=4800\n";
}

void fractional_turn_scheduler_regression() {
 auto blank=[](){SharedEnvironment e;e.rows=e.cols=9;e.num_of_agents=1;e.map.assign(81,0);
  e.curr_states={State(40,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);return e;};
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","1",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","100",1);setenv("CGAR_FLOW_COST_SCALE","4",1);setenv("CGAR_PICKUP_FLOW","1",1);
 int checked=0;
 for(int extra:{0,1,2,12})for(int mode=0;mode<3;++mode){
  setenv("CGAR_TURN_SURCHARGE",std::to_string(extra).c_str(),1);
  setenv("CGAR_PICKUP_FULL_ROBOTS",mode==1?"1":"0",1);
  auto e=blank();if(mode==2)e.goal_locations[0]={{39,0}};Cgar c;c.initialize(&e,1000);
  // Physically valid all-wait observations isolate metric publication and
  // cache preparation from the action the planner would choose for this goal.
  for(int t=0;t<2;++t){e.curr_timestep=t;std::vector<Action>a;c.plan(&e,1000,a);
   if(step(e,e.curr_states,{Action::W}).empty())throw std::runtime_error("fractional pickup observation invalid");}
  e.curr_timestep=2;e.goal_locations[0].clear();Task task;task.task_id=7;task.t_revealed=2;task.locations={39};e.task_pool.emplace(7,task);
  std::vector<int> proposed;c.schedule(&e,1000,proposed);
  const int expected=(2*(4+extra)+4+3)/4;
  if(proposed!=std::vector<int>{7}||c.stats().estimated_pickup_cost!=expected||c.stats().pickup_flow_snapshot_publication!=1)
   throw std::runtime_error("production pickup did not use the fractional turn metric");
  if(mode==1?c.stats().pickup_full_fields!=1:!c.stats().pickup_flow_searches)
   throw std::runtime_error("fractional pickup test missed its intended search path");
  if(mode==2){c.schedule(&e,1000,proposed);
   if(proposed!=std::vector<int>{7}||c.stats().estimated_pickup_cost!=2*expected||!c.stats().pickup_flow_cached_estimates)
    throw std::runtime_error("cached fair estimate disagreed with fractional pickup search");}
  if(e.curr_task_schedule!=std::vector<int>{-1}||e.task_pool.at(7).idx_next_loc||e.task_pool.at(7).agent_assigned!=-1)
   throw std::runtime_error("fractional pickup changed simulator task metadata");
  ++checked;
 }
 unsetenv("CGAR_PICKUP_FULL_ROBOTS");
 for(const char* bad:{"-1","13","16"}){setenv("CGAR_TURN_SURCHARGE",bad,1);auto e=blank();bool rejected=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid effective fractional turn cost accepted");}
 setenv("CGAR_TURN_SURCHARGE","1",1);setenv("CGAR_TEMPORAL","0",1);{auto e=blank();bool rejected=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("fractional turn accepted without temporal flow");}
 for(const char* key:{"CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_ORIENTATION_GUIDANCE","CGAR_FLOW_STRENGTH",
     "CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_COST_SCALE","CGAR_PICKUP_FLOW","CGAR_TURN_SURCHARGE"})unsetenv(key);
 std::cout<<"FRACTIONAL_TURN_SCHEDULER passed production_cases="<<checked<<" bounded_complete_cached=1 neutral_default=1 edge16_boundary=1 metadata_unchanged=1\n";
}

void temporal_transaction_regression() {
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(12,true),3,4,[]{});
 auto operation=[&](std::array<uint8_t,5> actions){const auto& ops=TemporalGeometry::operations();
  for(int k=0;k<int(ops.size());++k)if(ops[k]==actions)return k;
  throw std::runtime_error("missing transaction fixture operation");};
 const int root=operation({0,0,3,3,3}),down=operation({0,3,3,3,3}),trap=operation({0,1,0,3,3});
 std::vector<std::vector<TemporalChoice>> choices(3);
 choices[0]={{&geometry.paths(0,0)[0],1000,0},{&geometry.paths(0,0)[root],0,root}};
 choices[1]={{&geometry.paths(1,1)[0],0,0},{&geometry.paths(1,1)[down],100,down}};
 choices[2]={{&geometry.paths(2,1)[0],0,0},{&geometry.paths(2,1)[trap],0,trap},{&geometry.paths(2,1)[down],100,down}};
 std::vector<char> fixed(3,false);std::vector<double> power(3,1);std::vector<int> order{0};
 TemporalPibt baseline(12,choices,fixed,power,8192,123);const auto original=baseline.selections();
 TemporalTransactionOptions options;options.work=1000;options.roots=1;options.per_root=1000;
 TemporalTransactionStats stats;options.max_owners=1;
 auto single=repair_temporal_transactions(12,choices,fixed,power,8192,baseline,options,order,0,stats,[]{});
 if(single||stats.accepted||baseline.selections()!=original)throw std::runtime_error("one-owner repair displaced two owners or changed its input");
 options.max_owners=2;
 auto result=repair_temporal_transactions(12,choices,fixed,power,8192,baseline,options,order,0,stats,[]{});
 if(!result||result->selections()!=std::vector<int>({1,1,2})||result->score()!=800||stats.accepted!=1||baseline.selections()!=original)
  throw std::runtime_error("atomic two-owner repair failed sibling backtracking or complete score accounting");
 // Independently replay the three paths, checking positions and reverse edges.
 for(int t=0;t<5;++t)for(int a=0;a<3;++a)for(int b=a+1;b<3;++b){
  const auto& pa=*result->choice(a).path;const auto& pb=*result->choice(b).path;
  const int froma=t?pa.cells[t-1]:a,fromb=t?pb.cells[t-1]:b;
  if(pa.cells[t]==pb.cells[t]||(pa.cells[t]==fromb&&pb.cells[t]==froma))
   throw std::runtime_error("transaction fixture has an independent vertex or swap conflict");
 }
 // Rejected aggregate scores must restore every moved sibling and reservation.
 choices[1][1].cost=2000;TemporalPibt bad_score(12,choices,fixed,power,8192,123);
 auto rejected=repair_temporal_transactions(12,choices,fixed,power,8192,bad_score,options,order,0,stats,[]{});
 if(rejected||stats.accepted||!stats.terminals||bad_score.selections()!=original)
  throw std::runtime_error("transaction committed a root-only gain with aggregate score loss");
 choices[1][1].cost=100;
 for(int work:{1,2,3}){
  options.work=work;TemporalPibt budgeted(12,choices,fixed,power,8192,123);
  auto limited=repair_temporal_transactions(12,choices,fixed,power,8192,budgeted,options,order,0,stats,[]{});
  if(limited||stats.candidates>work||!stats.work_exhausted||budgeted.selections()!=original)
   throw std::runtime_error("transaction work exhaustion leaked a partial sibling plan");
 }
 options.work=1000;
 fixed[1]=true;TemporalPibt protected_owner(12,choices,fixed,power,8192,123);
 if(repair_temporal_transactions(12,choices,fixed,power,8192,protected_owner,options,order,0,stats,[]{}))
  throw std::runtime_error("transaction displaced a protected owner");
 fixed[1]=false;choices[1].resize(1);TemporalPibt missing_exit(12,choices,fixed,power,8192,123);
 if(repair_temporal_transactions(12,choices,fixed,power,8192,missing_exit,options,order,0,stats,[]{}))
  throw std::runtime_error("transaction abandoned an owner without an escape");
 choices[1].push_back({&geometry.paths(1,1)[down],100,down});
 int checks=0;bool threw=false;
 try {repair_temporal_transactions(12,choices,fixed,power,8192,baseline,options,order,0,stats,[&]{
  if(++checks==5)throw std::runtime_error("injected transaction deadline");});}
 catch(const std::runtime_error& e){threw=std::string(e.what())=="injected transaction deadline";}
 if(!threw||baseline.selections()!=original||baseline.score()!=0)
  throw std::runtime_error("transaction deadline did not propagate without mutating the complete input");
 for(int value:{-1,2000001}){
  setenv("CGAR_TEMPORAL_BRANCH_WORK",std::to_string(value).c_str(),1);SharedEnvironment e;e.rows=e.cols=1;e.num_of_agents=0;e.map={0};bool invalid=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){invalid=true;}
  if(!invalid)throw std::runtime_error("invalid branching work accepted");
 }
 setenv("CGAR_TEMPORAL_BRANCH_WORK","10000",1);
 for(const char* owners:{"1","2"}){setenv("CGAR_TEMPORAL_BRANCH_OWNERS",owners,1);temporal_region_adapter_regression();temporal_primary_regression();}
 unsetenv("CGAR_TEMPORAL_BRANCH_WORK");unsetenv("CGAR_TEMPORAL_BRANCH_OWNERS");
 std::cout<<"TEMPORAL_TRANSACTION passed two_owner_backtracking=1 aggregate_score_rejection=1 exact_owner_rollback=1 fixed_owner=1 missing_escape=1 work_cutoffs=3 exception_propagation=1 independent_paths=1 protected_serial_parallel_actions=9600\n";
}

void temporal_table_batch_regression() {
 auto fixture=[](){SharedEnvironment e;e.rows=e.cols=50;e.num_of_agents=160;e.map.assign(2500,0);e.curr_task_schedule.assign(160,-1);e.goal_locations.resize(160);
  for(int r=0;r<160;++r){e.curr_states.emplace_back(r*7,0,r%4);e.goal_locations[r]={{2499-r*3,0}};}return e;};
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TURN_FIRST","1",1);
 setenv("CGAR_TEMPORAL_STEPS","512",1);setenv("CGAR_TEMPORAL_EQUAL_WEIGHT","1",1);setenv("CGAR_TEMPORAL_ORDER","1",1);
 setenv("CGAR_TEMPORAL_PREP_THREADS","4",1);setenv("CGAR_TURN_COMPACT","1",1);setenv("CGAR_FLOW_STRENGTH","1",1);
 setenv("CGAR_FLOW_WARMUP","8",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","50",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);
 for(const char* value:{"-1","1025"}){setenv("CGAR_TEMPORAL_TABLE_BATCH",value,1);auto e=fixture();bool invalid=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){invalid=true;}if(!invalid)throw std::runtime_error("invalid temporal table batch accepted");}
 setenv("CGAR_TEMPORAL_TABLE_BATCH","64",1);
 for(const char* value:{"0","33"}){setenv("CGAR_TEMPORAL_TABLE_THREADS",value,1);auto e=fixture();bool invalid=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){invalid=true;}if(!invalid)throw std::runtime_error("invalid temporal table batch thread count accepted");}
 auto e=fixture();setenv("CGAR_TEMPORAL_TABLE_THREADS","1",1);Cgar serial;serial.initialize(&e,1000);
 setenv("CGAR_TEMPORAL_TABLE_THREADS","4",1);Cgar parallel;parallel.initialize(&e,1000);int checked=0;
 for(int t=0;t<80;++t){
  e.curr_timestep=t;const auto before=serial.stats().temporal_batch_built;std::vector<Action>a,b;
  serial.plan(&e,1000,a);parallel.plan(&e,1000,b);
  const auto& x=serial.stats();const auto& y=parallel.stats();
  if(a!=b||x.oriented_builds!=y.oriented_builds||x.temporal_batch_built!=y.temporal_batch_built||x.temporal_batch_covered!=y.temporal_batch_covered||
     x.temporal_batch_built-before>64||(t==0&&x.temporal_batch_built!=64)||serial.primary()!=parallel.primary())
   throw std::runtime_error("parallel complete-table admission changed actions, protection, order or fixed work");
  auto states=step(e,e.curr_states,a);if(states.empty())throw std::runtime_error("temporal batch fixture has an independent collision");e.curr_states=states;checked+=160;
  for(int r=0;r<160;++r)if(e.curr_states[r].location==e.goal_locations[r][0].first)e.goal_locations[r][0].first=(e.goal_locations[r][0].first+997)%2500;
 }
 if(serial.stats().temporal_batch_passes!=80||serial.stats().flow_publications!=5||serial.stats().temporal_batch_built<=64||!serial.stats().flow_cache_resets)
  throw std::runtime_error("temporal table batch did not exercise refresh invalidation and multiple complete chunks");
 for(const char* key:{"CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_TEMPORAL_TABLE_BATCH","CGAR_TEMPORAL_TABLE_THREADS","CGAR_TEMPORAL_PREP_THREADS","CGAR_TURN_COMPACT","CGAR_TURN_FIRST","CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_EQUAL_WEIGHT","CGAR_TEMPORAL_ORDER"})unsetenv(key);
 // Exercise primary, recovery and capacity paths under the enabled policy too.
 setenv("CGAR_TEMPORAL_TABLE_BATCH","64",1);setenv("CGAR_TEMPORAL_TABLE_THREADS","4",1);
 temporal_primary_regression();temporal_region_adapter_regression();
 unsetenv("CGAR_TEMPORAL_TABLE_BATCH");unsetenv("CGAR_TEMPORAL_TABLE_THREADS");
 std::cout<<"TEMPORAL_TABLE_BATCH passed identical_robot_decisions="<<checked<<" threads=1,4 fixed_work=1 multiple_chunks=1 refreshed_fields=5 protected_episodes=1 invalid_configuration=1\n";
}

void turn_build_limit_regression() {
 auto fixture=[](){SharedEnvironment e;e.rows=e.cols=20;e.num_of_agents=80;e.map.assign(400,0);e.curr_task_schedule.assign(80,-1);e.goal_locations.resize(80);
  for(int r=0;r<80;++r){e.curr_states.emplace_back(r,0,r%4);e.goal_locations[r]={{399-r,0}};}return e;};
 setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TURN_FIRST","1",1);setenv("CGAR_TURN_COMPACT","1",1);
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","512",1);setenv("CGAR_TEMPORAL_ORDER","1",1);setenv("CGAR_TEMPORAL_EQUAL_WEIGHT","1",1);
 int checked=0;
 for(int limit:{0,1,16,32,64}){
  setenv("CGAR_TURN_BUILD_LIMIT",std::to_string(limit).c_str(),1);auto e=fixture();Cgar c;c.initialize(&e,1000);
  for(int t=0;t<3;++t){
   e.curr_timestep=t;const auto before=c.stats().oriented_builds;std::vector<Action>a;c.plan(&e,1000,a);
   const auto built=c.stats().oriented_builds-before;
   if(built>limit||(t==0&&built!=limit))throw std::runtime_error("oriented table construction ignored the fixed per-step allowance");
   auto states=step(e,e.curr_states,a);if(states.empty())throw std::runtime_error("fixed table build allowance produced an invalid complete action set");e.curr_states=states;++checked;
  }
 }
 unsetenv("CGAR_TURN_BUILD_LIMIT");auto e=fixture();Cgar implicit;implicit.initialize(&e,1000);
 setenv("CGAR_TURN_BUILD_LIMIT","32",1);Cgar explicit_default;explicit_default.initialize(&e,1000);
 for(int t=0;t<3;++t){
  e.curr_timestep=t;std::vector<Action>a,b;implicit.plan(&e,1000,a);explicit_default.plan(&e,1000,b);
  if(a!=b||implicit.stats().oriented_builds!=explicit_default.stats().oriented_builds)throw std::runtime_error("explicit default turn build allowance changed decisions or cache admission");
  auto states=step(e,e.curr_states,a);if(states.empty())throw std::runtime_error("default turn build allowance fixture collided");e.curr_states=states;
 }
 for(const char* value:{"-1","257"}){setenv("CGAR_TURN_BUILD_LIMIT",value,1);auto invalid=fixture();bool rejected=false;try{Cgar c;c.initialize(&invalid,1000);}catch(const std::invalid_argument&){rejected=true;}if(!rejected)throw std::runtime_error("invalid turn table build allowance accepted");}
 for(const char* key:{"CGAR_TURN_BUILD_LIMIT","CGAR_ORIENTATION_GUIDANCE","CGAR_TURN_FIRST","CGAR_TURN_COMPACT","CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_ORDER","CGAR_TEMPORAL_EQUAL_WEIGHT"})unsetenv(key);
 std::cout<<"TURN_BUILD_LIMIT passed complete_steps="<<checked<<" limits=0,1,16,32,64 exact_default_actions=240 invalid_configuration=1\n";
}

void temporal_transaction_safety_regression() {
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(15,true),3,5,[]{});
 auto operation=[](std::array<uint8_t,5> actions){const auto& ops=TemporalGeometry::operations();
  for(int k=0;k<int(ops.size());++k)if(ops[k]==actions)return k;
  throw std::runtime_error("missing transaction safety fixture operation");};
 const int one=operation({0,3,3,3,3}),two=operation({0,0,3,3,3}),bend=operation({0,1,0,3,3});
 auto valid=[](const std::vector<std::vector<TemporalChoice>>& choices,const std::vector<int>& selected,const std::vector<int>& starts){
  for(int t=0;t<5;++t)for(int a=0;a<int(selected.size());++a)for(int b=a+1;b<int(selected.size());++b){
   const auto& pa=*choices[a][selected[a]].path;const auto& pb=*choices[b][selected[b]].path;
   const int froma=t?pa.cells[t-1]:starts[a],fromb=t?pb.cells[t-1]:starts[b];
   if(pa.cells[t]==pb.cells[t]||(pa.cells[t]==fromb&&pb.cells[t]==froma))return false;
  }
  return true;
 };
 auto score=[](const std::vector<std::vector<TemporalChoice>>& choices,const std::vector<int>& selected,const std::vector<double>& power){
  long double total=0;for(int r=0;r<int(selected.size());++r)total+=(static_cast<long double>(choices[r][0].cost)-choices[r][selected[r]].cost)*power[r];return total;
 };
 TemporalTransactionOptions options;options.work=4096;options.roots=3;options.per_root=1024;options.max_owners=1;options.max_agents=2;
 // A moves X, B tries to move X again but loses aggregate score, then C
 // succeeds by taking that same escape. C requires X's prior lock to be gone.
 std::vector<std::vector<TemporalChoice>> choices(4);
 choices[0]={{&geometry.paths(0,0)[0],1000,0},{&geometry.paths(0,0)[one],0,one}};
 choices[1]={{&geometry.paths(1,0)[0],0,0},{&geometry.paths(1,0)[two],100,two},{&geometry.paths(1,0)[bend],600,bend}};
 choices[2]={{&geometry.paths(8,3)[0],300,0},{&geometry.paths(8,3)[one],0,one}};
 choices[3]={{&geometry.paths(4,2)[0],1000,0},{&geometry.paths(4,2)[one],0,one}};
 const std::vector<int> starts{0,1,8,4},order{0,2,3};std::vector<char> fixed(4,false);
 for(const std::vector<double>& power:std::vector<std::vector<double>>{{1,1,1,1},{1.0/3,1.0/7,1.0/11,1.0/5}}){
  TemporalPibt baseline(15,choices,fixed,power,8192,0);const auto original=baseline.selections();TemporalTransactionStats stats;
  auto result=repair_temporal_transactions(15,choices,fixed,power,8192,baseline,options,order,0,stats,[]{});
  if(!result||result->selections()!=std::vector<int>({1,2,0,1})||stats.roots!=3||stats.accepted!=2||stats.rollbacks!=1||stats.terminals<3||
     !valid(choices,result->selections(),starts)||baseline.selections()!=original||std::abs(score(choices,result->selections(),power)-result->score())>1e-9L)
   throw std::runtime_error("multi-root transaction failed commit, rollback or reuse of an unlocked owner");
  for(int work:{3,5}){
   options.work=work;auto partial_pass=repair_temporal_transactions(15,choices,fixed,power,8192,baseline,options,order,0,stats,[]{});
   if(!partial_pass||partial_pass->selections()!=std::vector<int>({1,1,0,0})||stats.accepted!=1||!stats.work_exhausted||stats.candidates>work||!valid(choices,partial_pass->selections(),starts))
    throw std::runtime_error("exhausted later transaction erased an earlier complete commit or leaked an unfinished branch");
  }
  options.work=4096;
 }
 // A reverse-edge collision without any simultaneous vertex collision. The
 // owner's moving seed is valid before the root's change, and can wait safely.
 choices.assign(2,{});choices[0]={{&geometry.paths(0,0)[0],1000,0},{&geometry.paths(0,0)[two],0,two}};
 choices[1]={{&geometry.paths(3,2)[0],600,0},{&geometry.paths(3,2)[two],0,two}};
 for(int t=0;t<5;++t)if(choices[0][1].path->cells[t]==choices[1][1].path->cells[t])throw std::runtime_error("edge-only fixture also has a vertex conflict");
 if(valid(choices,{1,1},{0,3}))throw std::runtime_error("independent reverse-edge collision was not reproduced");
 fixed.assign(2,false);std::vector<double> power(2,1);std::vector<int> initial{0,1};options.roots=1;
 for(bool protect:{false,true}){
  fixed[1]=protect;TemporalPibt baseline(15,choices,fixed,power,8192,0,&initial);TemporalTransactionStats stats;
  auto result=repair_temporal_transactions(15,choices,fixed,power,8192,baseline,options,{0},0,stats,[]{});
  if(protect?bool(result):(!result||result->selections()!=std::vector<int>({1,0})||result->score()!=1000||!valid(choices,result->selections(),{0,3})))
   throw std::runtime_error("transaction failed edge-only owner detection or protection");
  if(baseline.selections()!=initial||baseline.score()!=600)throw std::runtime_error("edge-only transaction mutated its input");
 }
 // A four-robot displacement chain succeeds at its exact size bound and
 // restores the complete baseline when the bound is one robot too small.
 choices.assign(4,{});fixed.assign(4,false);power.assign(4,1);
 for(int r=0;r<4;++r)choices[r]={{&geometry.paths(r,0)[0],r?0:1000,0},{&geometry.paths(r,0)[one],r?100:0,one}};
 for(int limit:{3,4}){
  options.max_agents=limit;TemporalPibt baseline(15,choices,fixed,power,8192,0);TemporalTransactionStats stats;
  auto result=repair_temporal_transactions(15,choices,fixed,power,8192,baseline,options,{0},0,stats,[]{});
  if(limit==3?bool(result):(!result||result->selections()!=std::vector<int>({1,1,1,1})||result->score()!=700||!valid(choices,result->selections(),{0,1,2,3})))
   throw std::runtime_error("transaction displacement-size cutoff leaked or rejected a complete legal chain");
  if(baseline.selections()!=std::vector<int>(4,0))throw std::runtime_error("size-limited transaction mutated its input");
 }
 // Enumerate every candidate combination independently on tiny randomized
 // problems. We require sound committed results, not search completeness.
 std::mt19937 rng(719);int accepted=0;long long enumerated=0;
 for(int fixture=0;fixture<512;++fixture){
  const int n=2+fixture%3;std::vector<int> cells(15);for(int i=0;i<15;++i)cells[i]=i;std::shuffle(cells.begin(),cells.end(),rng);cells.resize(n);
  choices.assign(n,{});fixed.assign(n,false);power.resize(n);
  for(int r=0;r<n;++r){
   const int direction=rng()%4;const auto& paths=geometry.paths(cells[r],direction);choices[r].push_back({&paths[0],1000,0});
   std::vector<int> ops;for(int k=1;k<129;++k)if(paths[k].valid)ops.push_back(k);std::shuffle(ops.begin(),ops.end(),rng);
   for(int j=0;j<std::min(3,int(ops.size()));++j){const int k=ops[j];choices[r].push_back({&paths[k],int(rng()%1600),k});}
   std::sort(choices[r].begin()+1,choices[r].end(),[](const TemporalChoice& a,const TemporalChoice& b){return a.cost<b.cost||(a.cost==b.cost&&a.operation<b.operation);});
   fixed[r]=rng()%7==0;power[r]=fixture%2?1.0/(1+r+fixture%17):1;
  }
  TemporalPibt baseline(15,choices,fixed,power,8192,fixture);
  std::vector<int> roots(n);for(int r=0;r<n;++r)roots[r]=r;
  if(fixture%3==0){baseline.construct(roots,[]{});baseline.repair(128,[]{},0,nullptr,0);}
  const auto original=baseline.selections();const auto before=score(choices,original,power);std::set<std::vector<int>> feasible;
  std::vector<int> candidate(n,0);
  for(;;){
   ++enumerated;bool protected_ok=true;for(int r=0;r<n;++r)if(fixed[r]&&candidate[r]!=original[r])protected_ok=false;
   if(protected_ok&&valid(choices,candidate,cells))feasible.insert(candidate);
   int digit=0;for(;digit<n;++digit){if(++candidate[digit]<int(choices[digit].size()))break;candidate[digit]=0;}if(digit==n)break;
  }
  options.work=32+(fixture%11)*64;options.per_root=16+(fixture%7)*32;options.roots=n;options.max_agents=n;options.max_owners=1+fixture%2;
  TemporalTransactionStats stats;auto result=repair_temporal_transactions(15,choices,fixed,power,8192,baseline,options,roots,fixture,stats,[]{});
  if(baseline.selections()!=original||stats.candidates>options.work||!feasible.count(original))throw std::runtime_error("tiny transaction mutated input or exceeded prescribed work");
  if(result){
   ++accepted;const auto after=score(choices,result->selections(),power);
   if(!feasible.count(result->selections())||after<=before+1e-6L||std::abs(after-result->score())>1e-7L)
    throw std::runtime_error("transaction failed exhaustive tiny-instance soundness or fractional-score replay");
  }
 }
 if(!accepted)throw std::runtime_error("tiny transaction oracle exercised no accepted change");
 std::cout<<"TEMPORAL_TRANSACTION_SAFETY passed multi_root_commit_rollback_reuse=2 later_budget_exhaustion=4 edge_only_conflict=1 size_boundary=1 fractional_scores=1 tiny_fixtures=512 enumerated="<<enumerated<<" accepted="<<accepted<<"\n";
}

void temporal_service_audit_regression() {
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(7,true),1,7,[]{});
 auto op=[&](std::array<uint8_t,5> actions){const auto& all=TemporalGeometry::operations();
  for(int k=0;k<int(all.size());++k)if(all[k]==actions)return k;
  throw std::runtime_error("missing service-audit fixture operation");};
 const int fast=op({0,0,0,3,3}),one=op({0,3,3,3,3}),delayed=op({3,0,3,3,3}),two=op({0,0,3,3,3});
 int fixtures=0;
 // 0 free,1 known,2 finishing,3 mixed,4 no arrival,5 arrival-slot,
 // 6 protected,7 easier free option,8 edge-only,9 repeated owner,
 // 10 selected turn,11 selected forward,12 non-forward alternative.
 const int expected[]={0,1,2,3,4,4,4,0,1,1,1,1,1};
 for(int scenario=0;scenario<13;++scenario){
  const int count=scenario==0?1:scenario==3?3:2;
  std::vector<std::vector<TemporalChoice>> choices(count);std::vector<char> fixed(count,false),known(count,true);
  std::vector<double> power(count,1);std::vector<int> goals(count,5),initial(count,0);
  TemporalPath rotated=geometry.seed(0,0,1);
  choices[0]={{&geometry.paths(0,0)[0],2000,0},{&geometry.paths(0,0)[fast],-fast,fast}};
  if(count>1){choices[1]={{&geometry.paths(2,0)[0],0,0}};goals[1]=2;}
  if(scenario==2)known[1]=false;
  if(scenario==3){choices[2]={{&geometry.paths(3,0)[0],0,0}};goals[2]=3;known[2]=false;}
  if(scenario==4)goals[1]=4;
  if(scenario==5)choices[1][0].path=&geometry.paths(3,2)[delayed];
  if(scenario==6)fixed[1]=true;
  if(scenario==7)choices[0].push_back({&geometry.paths(0,0)[one],500-one,one});
  if(scenario==8)choices[1][0].path=&geometry.paths(2,2)[delayed];
  if(scenario==9)choices[0][1]={&geometry.paths(0,0)[two],-two,two};
  if(scenario==10)choices[0][0].path=&rotated;
  if(scenario==11){choices[0].push_back({&geometry.paths(0,0)[one],1000-one,one});initial[0]=2;}
  if(scenario==12){choices[0][1]={&geometry.paths(0,0)[delayed],-delayed,delayed};choices[1][0].path=&geometry.paths(1,0)[0];goals[1]=1;}
  TemporalPibt a(7,choices,fixed,power,8192,123,&initial),b(7,choices,fixed,power,8192,123,&initial);
  const auto selected=a.selections();const auto score=a.score();const auto stats=a.stats;
  const auto audit=a.audit_post_service(goals,known,50,1,[]{});
  int post=0;for(const auto& bucket:audit.post_service){post+=bucket.robots;
   if(bucket.selected_wait+bucket.selected_turn+bucket.selected_forward!=bucket.robots||bucket.candidate_forward>bucket.robots)
    throw std::runtime_error("service audit action partition does not conserve robots");}
  if(audit.eligible!=count-int(scenario==6)||audit.no_improving!=audit.eligible-1||
     audit.unblocked!=int(expected[scenario]==0)||audit.other_blocker!=int(expected[scenario]==4)||
     post!=int(expected[scenario]>=1&&expected[scenario]<=3)||
     audit.eligible!=audit.no_improving+audit.unblocked+audit.other_blocker+post)
   throw std::runtime_error("service audit misclassified hand-counted fixture "+std::to_string(scenario));
  if(post){const auto& bucket=audit.post_service[expected[scenario]-1];
   if(bucket.robots!=1||bucket.selected_turn!=int(scenario==10)||bucket.selected_forward!=int(scenario==11)||
      bucket.candidate_forward!=int(scenario!=12)||bucket.physical_gain<50)
    throw std::runtime_error("service audit lost owner/service/action attribution");}
  if(a.selections()!=selected||a.score()!=score||a.stats.candidates!=stats.candidates)
   throw std::runtime_error("service audit mutated search state");
  std::vector<int> order(count);for(int r=0;r<count;++r)order[r]=r;
  a.construct(order,[]{});b.construct(order,[]{});a.repair(256,[]{});b.repair(256,[]{});
  if(a.selections()!=b.selections()||a.score()!=b.score()||a.stats.candidates!=b.stats.candidates||a.stats.repairs_accepted!=b.stats.repairs_accepted)
   throw std::runtime_error("service audit changed subsequent random search");
  ++fixtures;
 }
 // Own future reservations are not other owners, even when currently moving.
 std::vector<std::vector<TemporalChoice>> choices(1);
 choices[0]={{&geometry.paths(0,0)[0],2000,0},{&geometry.paths(0,0)[one],1000-one,one},{&geometry.paths(0,0)[fast],-fast,fast}};
 std::vector<char> fixed(1,false),known(1,true);std::vector<double> power(1,1);std::vector<int> goals(1,6),initial(1,1);
 TemporalPibt self(7,choices,fixed,power,8192,7,&initial);
 if(self.audit_post_service(goals,known,50,1,[]{}).unblocked!=1)throw std::runtime_error("service audit counted own reservations");
 for(int unit:{1,4,8})for(int gain:{0,49,50}){
  choices[0]={{&geometry.paths(0,0)[0],int64_t(1000)*unit,0},{&geometry.paths(0,0)[one],int64_t(1000-gain-one)*unit,one}};
  TemporalPibt boundary(7,choices,fixed,power,8192,7);auto audit=boundary.audit_post_service(goals,known,50,unit,[]{});
  if(audit.unblocked!=int(gain==50)||audit.no_improving!=int(gain<50))throw std::runtime_error("service audit confused op ties with a full physical unit");
 }
 for(auto units:std::vector<std::pair<int,int>>{{0,1},{1,0},{-1,1},{1,-1}}){bool rejected=false;
  try{self.audit_post_service(goals,known,units.first,units.second,[]{});}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("service audit accepted invalid cost units");}
 bool rejected=false;try{self.audit_post_service({},known,50,1,[]{});}catch(const std::invalid_argument&){rejected=true;}
 if(!rejected)throw std::runtime_error("service audit accepted mismatched goal count");
 const auto selections=self.selections();const auto score=self.score();bool failed=false;
 try{self.audit_post_service(goals,known,50,1,[]{throw Timeout("service_fixture");});}catch(const Timeout&){failed=true;}
 if(!failed||self.selections()!=selections||self.score()!=score)throw std::runtime_error("service audit timeout altered a completed plan");
 for(const char* value:{"-1","4097","1"}){setenv("CGAR_TEMPORAL_SERVICE_AUDIT_STRIDE",value,1);
  SharedEnvironment e;e.rows=e.cols=1;e.num_of_agents=0;e.map={0};bool invalid=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){invalid=true;}
  if(!invalid)throw std::runtime_error("invalid/non-temporal service audit accepted");}
 setenv("CGAR_TEMPORAL_SERVICE_AUDIT_STRIDE","7",1);temporal_region_adapter_regression();temporal_primary_regression();unsetenv("CGAR_TEMPORAL_SERVICE_AUDIT_STRIDE");
 std::cout<<"TEMPORAL_SERVICE_AUDIT passed hand_counted_fixtures="<<fixtures<<" known_finishing_mixed=1 arrival_slot_and_fixed_excluded=1 edge_only_and_repeated_owner=1 easiest_alternative=1 selected_wait_turn_forward=1 own_reservations_excluded=1 physical_unit_boundaries=9 immutable_rng_reservations=1 explicit_timeout=1 protected_serial_parallel_actions=4800\n";
}

void temporal_forward_audit_regression() {
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(7,true),1,7,[]{});
 auto operation=[&](std::array<uint8_t,5> actions){const auto& ops=TemporalGeometry::operations();
  for(int k=0;k<int(ops.size());++k)if(ops[k]==actions)return k;
  throw std::runtime_error("missing forward-audit fixture operation");};
 const int forward=operation({0,0,0,3,3}),one_step=operation({0,3,3,3,3}),delayed=operation({3,0,3,3,3});
 int fixtures=0;
 for(int count=0;count<=3;++count)for(bool protect:{false,true}){
  if(protect&&!count)continue;
  const int n=count+1;std::vector<std::vector<TemporalChoice>> choices(n);std::vector<char> fixed(n,false);std::vector<double> power(n,1);
  for(int r=0;r<n;++r)choices[r].push_back({&geometry.paths(r,0)[0],r?0:100,0});
  choices[0].push_back({&geometry.paths(0,0)[forward],0,forward});
  if(protect)fixed[count]=true;
  TemporalPibt a(7,choices,fixed,power,8192,123),b(7,choices,fixed,power,8192,123);
  const auto selected=a.selections();const auto score=a.score();const auto stats=a.stats;
  const auto audit=a.audit_forward_blockers();
  if(audit.stationary!=n-int(protect)||audit.no_lower_forward!=count-int(protect)||
     audit.unblocked!=int(count==0)||audit.protected_blocker!=int(protect)||
     audit.one_movable!=int(!protect&&count==1)||audit.two_movable!=int(!protect&&count==2)||audit.many_movable!=int(!protect&&count==3)||
     a.selections()!=selected||a.score()!=score||a.stats.candidates!=stats.candidates)
   throw std::runtime_error("forward audit misclassified independently constructed blocker footprints or mutated search state");
  std::vector<int> order(n);for(int i=0;i<n;++i)order[i]=i;
  a.construct(order,[]{});b.construct(order,[]{});a.repair(256,[]{});b.repair(256,[]{});
  if(a.selections()!=b.selections()||a.score()!=b.score()||a.stats.candidates!=b.stats.candidates||a.stats.repairs_accepted!=b.stats.repairs_accepted)
   throw std::runtime_error("read-only forward audit changed subsequent random search");
  ++fixtures;
 }
 std::vector<std::vector<TemporalChoice>> choices(2);
 choices[0]={{&geometry.paths(0,0)[0],100,0},{&geometry.paths(0,0)[one_step],0,one_step}};
 choices[1]={{&geometry.paths(1,0)[0],0,0}};
 std::vector<char> fixed(2,false);std::vector<double> power(2,1);
 TemporalPibt repeated(7,choices,fixed,power,8192,1);
 if(repeated.audit_forward_blockers().one_movable!=1)throw std::runtime_error("forward audit counted repeated owner reservations more than once");
 choices.resize(1);fixed.resize(1);power.resize(1);
 choices[0].push_back({&geometry.paths(0,0)[delayed],50,delayed});std::vector<int> initial{2};
 TemporalPibt self(7,choices,fixed,power,8192,1,&initial);
 if(self.audit_forward_blockers().unblocked!=1)throw std::runtime_error("forward audit counted the robot's own future reservations");
 initial[0]=1;TemporalPibt moving(7,choices,fixed,power,8192,1,&initial);
 if(moving.audit_forward_blockers().stationary)throw std::runtime_error("forward audit included a first-forward selection");
 choices[0][1].cost=100;choices[0].resize(2);TemporalPibt equal(7,choices,fixed,power,8192,1);
 if(equal.audit_forward_blockers().no_lower_forward!=1)throw std::runtime_error("forward audit treated an equal-cost alternative as a strict improvement");
 power[0]=0;TemporalPibt idle(7,choices,fixed,power,8192,1);
 if(idle.audit_forward_blockers().stationary)throw std::runtime_error("forward audit included an idle zero-weight robot");
 // The cheapest forward path crosses three owners; a smaller improvement
 // crosses only one. Count the easiest alternative, not just the cheapest one.
 choices.assign(4,{});fixed.assign(4,false);power.assign(4,1);
 for(int r=0;r<4;++r)choices[r].push_back({&geometry.paths(r,0)[0],r?0:1000,0});
 choices[0].push_back({&geometry.paths(0,0)[forward],0,forward});
 choices[0].push_back({&geometry.paths(0,0)[one_step],500,one_step});
 TemporalPibt easier(7,choices,fixed,power,8192,1);auto split=easier.audit_forward_blockers();
 if(split.many_movable!=1||split.easiest.one_movable!=1||split.physical.one_movable!=1)
  throw std::runtime_error("forward audit missed the easier improving alternative");
 fixed[2]=true;TemporalPibt protected_best(7,choices,fixed,power,8192,1);split=protected_best.audit_forward_blockers();
 if(split.protected_blocker!=1||split.easiest.one_movable!=1||split.physical.one_movable!=1)
  throw std::runtime_error("forward audit let a protected best alternative hide a movable one");
 choices[1][0].path=&geometry.paths(4,0)[0];fixed[2]=false;
 TemporalPibt free_second(7,choices,fixed,power,8192,1);split=free_second.audit_forward_blockers();
 if(split.two_movable!=1||split.easiest.unblocked!=1||split.physical.unblocked!=1)
  throw std::runtime_error("forward audit missed a free non-best alternative");
 // Operation tie terms can exceed one scalar distance step. Remove them before
 // applying the physical threshold, and check its boundary under cost scaling.
 choices.resize(1);fixed.assign(1,false);power.assign(1,1);
 for(int scale:{1,4,8})for(int gain:{0,49,50}){
  choices[0]={{&geometry.paths(0,0)[0],int64_t(1000)*scale,0},
              {&geometry.paths(0,0)[one_step],int64_t(1000-gain-one_step)*scale,one_step}};
  TemporalPibt boundary(7,choices,fixed,power,8192,1);split=boundary.audit_forward_blockers(50,scale);
  if(split.easiest.unblocked!=1||split.physical.unblocked!=int(gain==50)||
     split.physical.no_lower_forward!=int(gain<50))
   throw std::runtime_error("forward audit confused operation ties with scaled physical gain");
 }
 for(auto units:std::vector<std::pair<int,int>>{{0,1},{1,0},{-1,1},{1,-1}}){
  TemporalPibt invalid(7,choices,fixed,power,8192,1);bool rejected=false;
  try{invalid.audit_forward_blockers(units.first,units.second);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("forward audit accepted invalid cost units");
 }
 for(const char* value:{"-1","4097","1"}){
  setenv("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE",value,1);SharedEnvironment e;e.rows=e.cols=1;e.num_of_agents=0;e.map={0};bool rejected=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid or non-temporal conflict audit was accepted");
 }
 setenv("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE","7",1);temporal_region_adapter_regression();temporal_primary_regression();unsetenv("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE");
 std::cout<<"TEMPORAL_FORWARD_AUDIT passed hand_counted_blocker_fixtures="<<fixtures<<" owner_deduplication=1 own_future_reservations_excluded=1 strict_cost_boundary=1 fixed_and_idle_excluded=1 search_rng_and_selections_unchanged=1 fixed_sample_schedule=1 easiest_alternative=1 tie_terms_removed=1 physical_gain_boundaries=9 protected_serial_parallel_actions=4800\n";
}

void flow_cache_only_regression() {
 const std::vector<char> free(4,true);std::vector<int> observed{0,1,3,2,0};
 for(int loop=0;loop<4;++loop)for(int u:{2,3,1,0})observed.push_back(u);
 int publications=0,rebuilt_values=0,prefetch_discards=0;
 for(int scale:{1,4,8}){
  FlowGuidance control;control.initialize(free,2,2,4,4,1,0,4,scale,true);
  std::vector<uint8_t> expected(16,scale);
  for(auto [u,d]:std::vector<std::pair<int,int>>{{0,0},{1,1},{3,2},{2,3}}){
   const int v=nb(u,d,2,2);expected[v*4+(d+2)%4]=scale+4;
  }
  Certificate cert;cert.rows=cert.cols=2;cert.free=free;cert.core=free;cert.pocket.assign(4,-1);
  TurnDistanceOracle oracle;oracle.init(&cert,1<<20,scale,true,scale);
  const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(5);
  std::vector<int> reference;
  for(int t=0;t<=20;++t){
   const bool published=control.observe(t,{observed[t]});
   if(published!=(t>0&&t%4==0)||control.observe(t,{observed[t]}))
    throw std::runtime_error("cache-only publication advanced on a duplicate or wrong observation count");
   if(!published)continue;
   ++publications;
   if(control.costs()!=expected||control.penalized_edges()!=4||!control.cache_only_refresh()||control.moves()!=uint64_t(t))
    throw std::runtime_error("cache-only control incorporated later reversed evidence into its initial metric");
   if(oracle.set_forward_costs(control.costs())!=(t==4))
    throw std::runtime_error("cache-only control changed the metric after its first publication");
   const auto* before=oracle.table(1,deadline);
   if(t==4)for(int u=0;u<4;++u)for(int d=0;d<4;++d)reference.push_back(oracle.value(*before,u,d));
   oracle.prefetch({0,3},2,deadline);
   const auto old_discarded=oracle.prefetched_discarded,old_hits=oracle.prefetched_hits;
   oracle.clear_tables();
   if(oracle.has(1)||oracle.find(1)||oracle.prefetched_discarded!=old_discarded+2)
    throw std::runtime_error("explicit cache clear retained an ordinary or speculative table");
   prefetch_discards+=2;
   for(int u=0;u<4;++u)for(int d=0;d<4;++d)
    if(oracle.forward_cost(u,d)!=expected[u*4+d])throw std::runtime_error("cache clear changed a forward cost");
   const auto* after=oracle.table(1,deadline);
   for(int u=0;u<4;++u)for(int d=0;d<4;++d){
    if(oracle.value(*after,u,d)!=reference[u*4+d])throw std::runtime_error("cache rebuild changed an exact oriented distance");
    ++rebuilt_values;
   }
   if(oracle.value(*after,0,0)!=scale)throw std::runtime_error("cache-only control lost the hand-counted forward distance");
   const auto* zero=oracle.table(0,deadline);
   if(oracle.value(*zero,1,2)!=scale+4||oracle.prefetched_hits!=old_hits)
    throw std::runtime_error("cache-only rebuild used a discarded speculative result or changed reverse distance");
   // Clear admitted goals so the next pair of speculative results is nonempty.
   oracle.clear_tables();
  }
 }
 FlowGuidance skipped;skipped.initialize(free,2,2,2,4,1,0,2,1,true);
 skipped.observe(0,{0});skipped.observe(2,{3});skipped.observe(3,{2});
 if(!skipped.observe(4,{0}))throw std::runtime_error("cache-only first publication counted a gap");
 const auto initial=skipped.costs();skipped.observe(6,{3});skipped.observe(7,{1});
 if(skipped.publications()!=1||!skipped.observe(8,{0})||skipped.costs()!=initial||skipped.samples()!=4)
  throw std::runtime_error("cache-only gap handling changed its schedule or retained field");
 bool reversed=false;try{skipped.observe(7,{0});}catch(const std::logic_error&){reversed=true;}
 if(!reversed||skipped.costs()!=initial)throw std::runtime_error("cache-only reversed time mutated its field");
 bool invalid=false;try{FlowGuidance f;f.initialize(free,2,2,4,1,1,0,0,1,true);}catch(const std::invalid_argument&){invalid=true;}
 if(!invalid)throw std::runtime_error("cache-only without a refresh interval was accepted");
 for(const char* setting:{"-1","2","1"}){
  setenv("CGAR_FLOW_CACHE_ONLY_REFRESH",setting,1);SharedEnvironment e;e.rows=e.cols=1;e.num_of_agents=0;e.map={0};
  invalid=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){invalid=true;}
  if(!invalid)throw std::runtime_error("invalid or disabled-flow cache-only configuration was accepted");
 }
 setenv("CGAR_FLOW_CACHE_ONLY_REFRESH","1",1);setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","8",1);
 setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","50",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);
 setenv("CGAR_TEMPORAL_WARM_START","1",1);setenv("CGAR_TURN_COMPACT","1",1);
 temporal_region_adapter_regression();temporal_primary_regression();
 for(const char*name:{"CGAR_FLOW_CACHE_ONLY_REFRESH","CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES",
     "CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_TEMPORAL_WARM_START","CGAR_TURN_COMPACT"})unsetenv(name);
 std::cout<<"FLOW_CACHE_ONLY passed fixed_publications="<<publications<<" rebuilt_values="<<rebuilt_values
          <<" speculative_discards="<<prefetch_discards<<" initial_costs_preserved=1 reversed_later_traffic=1 skipped_observations=1 invalid_configuration=1 protected_parallel_warm_actions=4800\n";
}

void flow_refresh_regression() {
 std::vector<char> free(4,true);std::vector<int> observed{0};
 for(int u:{1,3,2,0})observed.push_back(u);
 for(int loop=0;loop<4;++loop)for(int u:{2,3,1,0})observed.push_back(u);
 FlowGuidance adaptive;adaptive.initialize(free,2,2,4,4,1,0,4);
 Certificate cert;cert.rows=2;cert.cols=2;cert.free=free;cert.core=free;cert.pocket.assign(4,-1);
 TurnDistanceOracle oracle;oracle.init(&cert,1<<20,1,true);
 const auto deadline=std::chrono::steady_clock::now()+std::chrono::seconds(1);
 oracle.table(1,deadline);int resets=0;
 std::vector<std::vector<uint8_t>> fields;
 // Hand-counted evidence: one clockwise circuit, then four reverse circuits.
 // Publication 2 is neutral; publication 4 repeats publication 3 exactly.
 const int clockwise_cost[5]={1,1,3,3,4},counterclockwise_cost[5]={5,1,1,1,1};
 for(int t=0;t<=20;++t){
  const auto before=adaptive.costs();const bool changed=adaptive.observe(t,{observed[t]});
  if(changed!=(t>0&&t%4==0)||adaptive.observe(t,{observed[t]}))
   throw std::runtime_error("flow refresh was not tied to complete fixed observations");
  if(!changed){if(adaptive.costs()!=before)throw std::runtime_error("flow field changed between publications");continue;}
  const int publication=t/4-1;std::vector<uint8_t> expected(16,1);
  for(auto [u,d]:std::vector<std::pair<int,int>>{{0,0},{1,1},{3,2},{2,3}}){
   const int v=nb(u,d,2,2);expected[u*4+d]=clockwise_cost[publication];expected[v*4+(d+2)%4]=counterclockwise_cost[publication];
  }
  if(adaptive.costs()!=expected||adaptive.frozen()||adaptive.publications()!=publication+1||adaptive.samples()!=t||adaptive.moves()!=t)
   throw std::runtime_error("cumulative flow publication disagreed with hand-counted traffic");
  const auto* old=oracle.find(1);const bool reset=oracle.set_forward_costs(adaptive.costs());resets+=reset;
  if(reset!=(publication!=3)||(reset&&oracle.has(1))||(!reset&&oracle.find(1)!=old))
   throw std::runtime_error("refreshed flow cache invalidation was missing or unnecessary");
  const auto* one=oracle.table(1,deadline);const auto* zero=oracle.table(0,deadline);
  if(oracle.value(*one,0,0)!=clockwise_cost[publication]||oracle.value(*zero,1,2)!=counterclockwise_cost[publication])
   throw std::runtime_error("refreshed exact goal metric retained a stale edge cost");
  fields.push_back(expected);
 }
 if(resets!=4||fields.size()!=5||adaptive.refresh_interval()!=4)throw std::runtime_error("flow publication accounting failed");
 int transformed_edges=0;
 for(bool reflection:{false,true})for(int rotation=0;rotation<4;++rotation){
  auto cell=[&](int u){int r=u/2,c=u%2;if(reflection)c=1-c;for(int k=0;k<rotation;++k){int old=r;r=c;c=1-old;}return r*2+c;};
  auto direction=[&](int d){if(reflection)d=(2-d+4)%4;return(d+rotation)%4;};
  FlowGuidance transformed;transformed.initialize(free,2,2,4,4,1,0,4);
  for(int t=0;t<=20;++t)if(transformed.observe(t,{cell(observed[t])}))
   for(int u=0;u<4;++u)for(int d=0;d<4;++d){
    if(transformed.costs()[cell(u)*4+direction(d)]!=fields[t/4-1][u*4+d])throw std::runtime_error("flow refresh used an absolute-direction rule");
    ++transformed_edges;
   }
 }
 FlowGuidance skipped;skipped.initialize(free,2,2,2,4,1,0,2);
 skipped.observe(0,{0});skipped.observe(2,{3});skipped.observe(3,{2});
 if(!skipped.observe(4,{0})||skipped.publications()!=1)throw std::runtime_error("skipped first interval was miscounted");
 const auto first=skipped.costs();skipped.observe(6,{3});skipped.observe(7,{2});
 if(skipped.samples()!=3||skipped.publications()!=1||skipped.costs()!=first||!skipped.observe(8,{0})||
    skipped.samples()!=4||skipped.moves()!=4||skipped.publications()!=2||skipped.costs()!=first)
  throw std::runtime_error("skipped refresh invented movement or published early");
 bool reversed=false;try{skipped.observe(7,{0});}catch(const std::logic_error&){reversed=true;}
 if(!reversed||skipped.publications()!=2||skipped.costs()!=first)throw std::runtime_error("reversed observation changed the published field");
 for(int interval:{-1,4097}){bool invalid=false;try{FlowGuidance f;f.initialize(free,2,2,4,1,1,0,interval);}catch(const std::invalid_argument&){invalid=true;}
  if(!invalid)throw std::runtime_error("invalid flow refresh interval was accepted");}
 adaptive.initialize(free,2,2,4,4,1);
 for(int t=0;t<=20;++t)adaptive.observe(t,{observed[t]});
 if(!adaptive.frozen()||adaptive.publications()!=1||adaptive.samples()!=4||adaptive.costs()!=fields[0])
  throw std::runtime_error("default frozen behavior or reinitialization changed");
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","8",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","50",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);
 setenv("CGAR_TEMPORAL_WARM_START","1",1);setenv("CGAR_TURN_COMPACT","1",1);
 temporal_region_adapter_regression();temporal_primary_regression();
 for(const char*name:{"CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT",
      "CGAR_FLOW_REFRESH_INTERVAL","CGAR_TEMPORAL_WARM_START","CGAR_TURN_COMPACT"})unsetenv(name);
 std::cout<<"FLOW_REFRESH passed fixed_publications=5 cache_resets=4 neutral_and_reversed_costs=1 transformed_edges="<<transformed_edges
          <<" skipped_observations=1 strict_configuration=1 default_frozen_preserved=1 protected_threaded_warm_actions=4800\n";
}

void flow_margin_regression() {
 std::vector<char> free(4,true);std::vector<int> observed{0};
 const std::vector<int> clockwise{1,3,2,0},counterclockwise{2,3,1,0};
 for(int loop=0;loop<8;++loop){const auto& cycle=loop<5?clockwise:counterclockwise;observed.insert(observed.end(),cycle.begin(),cycle.end());}
 int checked=0;
 for(int margin:{0,24,25,50,100}){
  FlowGuidance original;original.initialize(free,2,2,32,4,8,margin);
  for(int t=0;t<=32;++t)original.observe(t,{observed[t]});
  // Every edge has five clockwise and three reverse uses: exactly 25% margin.
  if(original.penalized_edges()!=(margin<25?4:0)||original.moves()!=32||!original.frozen())
   throw std::runtime_error("flow margin boundary failed on independently counted cycles");
  for(bool reflection:{false,true})for(int rotation=0;rotation<4;++rotation){
   auto cell=[&](int u){int r=u/2,c=u%2;if(reflection)c=1-c;for(int k=0;k<rotation;++k){int previous=r;r=c;c=1-previous;}return r*2+c;};
   auto direction=[&](int d){if(reflection)d=(2-d+4)%4;return(d+rotation)%4;};
   FlowGuidance transformed;transformed.initialize(free,2,2,32,4,8,margin);
   for(int t=0;t<=32;++t)transformed.observe(t,{cell(observed[t])});
   for(int u=0;u<4;++u)for(int d=0;d<4;++d){
    if(transformed.costs()[cell(u)*4+direction(d)]!=original.costs()[u*4+d])throw std::runtime_error("flow margin used a direction template");
    ++checked;
   }
  }
 }
 setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","50",1);flow_guidance_regression();unsetenv("CGAR_FLOW_MIN_MARGIN_PERCENT");
 std::cout<<"FLOW_MARGIN passed exact_25_percent_boundary=1 transformed_edge_checks="<<checked<<" protected_threaded_episode=1\n";
}

void temporal_warm_start_regression() {
 const int cells=8,count=3;std::vector<char> free(cells,true);TemporalGeometry geometry;geometry.initialize(free,1,cells,[]{});
 const auto& operations=TemporalGeometry::operations();int forward=-1;
 for(int k=0;k<int(operations.size());++k){
  if(operations[k]==std::array<uint8_t,5>{0,0,0,0,0})forward=k;
  const int shifted=TemporalWarmStart::shifted_operation(k);
  for(int t=0;t<4;++t)if(operations[shifted][t]!=operations[k][t+1])throw std::runtime_error("invalid shifted action suffix");
  if(operations[shifted][4]!=3)throw std::runtime_error("shifted suffix did not append a wait");
 }
 if(forward<0)throw std::runtime_error("missing forward operation");
 const int suffix=TemporalWarmStart::shifted_operation(forward);
 std::vector<std::vector<TemporalChoice>> old_choices(count),choices(count);
 std::vector<int> locations={1,2,3},orientations(count,0),goals(count,7),initial(count,1);
 std::vector<char> fixed(count,false);std::vector<double> power(count,1);
 for(int r=0;r<count;++r){
  const auto& old_paths=geometry.paths(r,0);old_choices[r]={{&old_paths[0],250,0},{&old_paths[forward],0,forward}};
  const auto& paths=geometry.paths(r+1,0);choices[r]={{&paths[0],200,0},{&paths[suffix],0,suffix}};
 }
 TemporalPibt prior(cells,old_choices,fixed,power,8192,0,&initial);TemporalWarmStart history;
 history.remember(10,prior,goals,orientations);
 auto seed=[&](const std::vector<int>& target,const std::vector<std::vector<TemporalChoice>>& pool,const std::vector<char>& protected_robots,TemporalWarmStats& stats){
  return history.selections(11,cells,locations,orientations,target,pool,protected_robots,stats,[]{});
 };
 TemporalWarmStats kept_stats;auto kept=seed(goals,choices,fixed,kept_stats);
 if(kept!=initial||!kept_stats.history_valid||kept_stats.retained!=3)throw std::runtime_error("warm start discarded valid convoy suffixes");
 TemporalPibt reused(cells,choices,fixed,power,8192,0,&kept);
 auto protected_robots=fixed;protected_robots[2]=true;TemporalWarmStats protected_stats;
 auto stopped=seed(goals,choices,protected_robots,protected_stats);
 if(stopped!=std::vector<int>(3,0)||protected_stats.initial_resets!=1||protected_stats.collision_resets!=2||protected_stats.retained)
  throw std::runtime_error("protected seed did not reset the full conflicting convoy");
 TemporalPibt protected_plan(cells,choices,protected_robots,power,8192,0,&stopped);
 auto changed=goals;changed[2]=6;TemporalWarmStats goal_stats;
 if(seed(changed,choices,fixed,goal_stats)!=stopped||goal_stats.collision_resets!=2)throw std::runtime_error("changed goal retained a stale conflicting suffix");
 auto filtered=choices;filtered[1].resize(1);TemporalWarmStats filtered_stats;
 auto subset=seed(goals,filtered,fixed,filtered_stats);
 if(subset!=std::vector<int>({0,0,1})||filtered_stats.retained!=1||filtered_stats.collision_resets!=1)
  throw std::runtime_error("filtered warm candidate did not reset only its conflict closure");
 TemporalPibt filtered_plan(cells,filtered,fixed,power,8192,0,&subset);
 for(int mismatch=0;mismatch<3;++mismatch){
  auto positions=locations,dirs=orientations;int timestep=11;if(mismatch==0)++positions[0];if(mismatch==1)++dirs[0];if(mismatch==2)++timestep;
  TemporalWarmStats stale;
  if(!history.selections(timestep,cells,positions,dirs,goals,choices,fixed,stale,[]{}).empty()||stale.history_valid)
   throw std::runtime_error("mismatched temporal warm history was reused");
 }
 bool failed=false;TemporalWarmStats timeout_stats;
 try{history.selections(11,cells,locations,orientations,goals,choices,fixed,timeout_stats,[]{throw Timeout("warm_fixture");});}
 catch(const Timeout&){failed=true;}
 if(!failed)throw std::runtime_error("warm-start deadline was swallowed");
 TemporalWarmStats unchanged;if(seed(goals,choices,fixed,unchanged)!=initial)throw std::runtime_error("failed warm lookup mutated remembered history");
 history.clear();TemporalWarmStats empty;if(!seed(goals,choices,fixed,empty).empty())throw std::runtime_error("cleared warm history survived");
 setenv("CGAR_TEMPORAL_WARM_START","1",1);
 temporal_region_adapter_regression();temporal_primary_regression();temporal_parallel_regression();
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","4",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 temporal_region_adapter_regression();
 for(const char* name:{"CGAR_TEMPORAL_WARM_START","CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES"})unsetenv(name);
 std::cout<<"TEMPORAL_WARM_START passed shifted_operations=129 convoy_reuse=3 protected_cascade=2 filtered_closure=1 stale_history_rejected=1 explicit_timeout=1 protected_and_threaded_episodes=1\n";
}

void temporal_after_turn_promise_regression() {
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 const int cells=4,count=4;
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(cells,true),2,2,[]{});
 const auto& operations=TemporalGeometry::operations();
 auto operation=[&](std::array<uint8_t,5> actions){
  for(int k=0;k<int(operations.size());++k)if(operations[k]==actions)return k;
  throw std::runtime_error("missing promise fixture operation");
 };
 const std::vector<int> locations{0,1,3,2},orientations{0,1,2,3},goals{1,3,2,0};
 const std::vector<char> fixed(count,false);const std::vector<double> power(count,1);
 for(int fixture=0;fixture<3;++fixture){
  // Two full occupied cycles after a real turn: immediate forward or wait,
  // followed by a forward later. The third history did not turn and is ineligible.
  const int op=fixture==0?operation({1,0,3,3,3}):fixture==1?operation({1,3,3,3,0}):operation({3,0,3,3,3});
  std::vector<std::vector<TemporalChoice>> before(count),choices(count);
  std::vector<int> previous_selected(count,1);
  for(int r=0;r<count;++r){
   const auto& old=geometry.paths(locations[r],fixture==2?orientations[r]:(orientations[r]+3)%4);
   before[r]={{&old[0],1000,0},{&old[op],1000-op,op}};
   const auto& current=geometry.paths(locations[r],orientations[r]);
   choices[r].push_back({&current[0],1000,0});
   for(int k=1;k<int(current.size());++k)if(current[k].valid)choices[r].push_back({&current[k],1000-k,k});
   std::sort(choices[r].begin()+1,choices[r].end(),[](const auto& a,const auto& b){return a.cost<b.cost;});
  }
  TemporalPibt prior(cells,before,fixed,power,8192,0,&previous_selected);
  TemporalWarmStart history;history.remember(10,prior,goals,orientations);
  TemporalWarmStats stats;
  auto selected=history.selections(11,cells,locations,orientations,goals,choices,fixed,stats,[]{},true);
  require(stats.history_valid,"promise lost exact history");
  if(fixture==2){
   require(stats.retained==0&&selected==std::vector<int>(count,0),"non-turn history created a promise");
   TemporalWarmStats ordinary;
   history.selections(11,cells,locations,orientations,goals,choices,fixed,ordinary,[]{});
   require(ordinary.retained==count,"non-turn exclusion fixture was vacuous");continue;
  }
  require(stats.retained==count,"compatible occupied promise cycle was discarded");
  auto constrained=choices;
  const auto promised=TemporalWarmStart::constrain_first_actions(constrained,selected,fixed,[]{});
  for(int r=0;r<count;++r){
   require(promised[r]==(fixture?3:0)&&selected[r]>0,"promise changed first action or returned to ordinary seed");
   require(constrained[r][0].path==choices[r][0].path&&constrained[r][0].cost==choices[r][0].cost&&constrained[r][0].operation==0,
           "promise changed the score reference or ordinary seed");
   require(constrained[r][selected[r]].operation==TemporalWarmStart::shifted_operation(op),"promise lost its operation ID");
   for(size_t k=1;k<constrained[r].size();++k){
    require(constrained[r][k].path->first_action==promised[r],"promise left an incompatible searchable first action");
    if(k>1)require(constrained[r][k-1].cost<=constrained[r][k].cost,"promise changed candidate ordering");
   }
  }
  TemporalPibt kept(cells,constrained,fixed,power,8192,7,&selected);
  kept.construct({0,1,2,3},[]{});kept.repair(256,[]{});
  SharedEnvironment e;e.rows=2;e.cols=2;e.map.assign(cells,0);e.num_of_agents=count;
  std::vector<Action> actions;
  for(int r=0;r<count;++r){
   require(kept.selected(r)>0&&kept.choice(r).path->first_action==promised[r],"search broke a promised action");
   e.curr_states.emplace_back(locations[r],0,orientations[r]);actions.push_back(static_cast<Action>(promised[r]));
  }
  require(!step(e,e.curr_states,actions).empty(),"promised occupied cycle failed independent action validation");
  for(int change=0;change<2;++change){
   auto protected_robots=fixed;auto changed_goals=goals;
   if(change==0)protected_robots[0]=true;else changed_goals[0]=2;
   TemporalWarmStats reset;
   auto rejected=history.selections(11,cells,locations,orientations,changed_goals,choices,protected_robots,reset,[]{},true);
   require(rejected==std::vector<int>(count,0)&&reset.initial_resets==1&&reset.collision_resets==3,
           "protected path or changed goal did not cancel its complete dependent promise cycle");
   auto unchanged=choices;const auto no_promises=TemporalWarmStart::constrain_first_actions(unchanged,rejected,protected_robots,[]{});
   require(no_promises==std::vector<int>(count,-1),"cancelled promise still restricted actions");
   for(int r=0;r<count;++r)require(unchanged[r].size()==choices[r].size(),"cancelled promise removed ordinary choices");
   TemporalPibt reset_plan(cells,unchanged,protected_robots,power,8192,0,&rejected);
  }
  for(int mismatch=0;mismatch<3;++mismatch){
   auto loc=locations,dirs=orientations;int tick=11;
   if(mismatch==0)loc[0]=2;if(mismatch==1)dirs[0]=2;if(mismatch==2)tick=12;
   TemporalWarmStats stale;
   require(history.selections(tick,cells,loc,dirs,goals,choices,fixed,stale,[]{},true).empty()&&!stale.history_valid,
           "promise reused stale position, heading or timestep");
  }
  bool failed=false;TemporalWarmStats timeout;
  try{history.selections(11,cells,locations,orientations,goals,choices,fixed,timeout,[]{throw Timeout("promise_fixture");},true);}
  catch(const Timeout&){failed=true;}
  require(failed,"promise history swallowed deadline failure");
  failed=false;auto retry_choices=choices;auto retry_selected=previous_selected;
  try{TemporalWarmStart::constrain_first_actions(retry_choices,retry_selected,fixed,[]{throw Timeout("promise_filter_fixture");});}
  catch(const Timeout&){failed=true;}
  require(failed,"promise filtering swallowed deadline failure");
  TemporalWarmStats unchanged;history.selections(11,cells,locations,orientations,goals,choices,fixed,unchanged,[]{},true);
  require(unchanged.retained==count,"failed promise lookup mutated its history");
  history.clear();TemporalWarmStats empty;
  auto absent=history.selections(11,cells,locations,orientations,goals,choices,fixed,empty,[]{},true);
  auto original=choices;
  require(absent.empty()&&TemporalWarmStart::constrain_first_actions(original,absent,fixed,[]{})==std::vector<int>(count,-1),
          "missing history manufactured promises");
 }
 // Explicit OFF and absent settings must leave all ordinary decisions intact.
 {
  setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TEMPORAL_STEPS","256",1);
  SharedEnvironment e;e.rows=6;e.cols=6;e.num_of_agents=12;e.map.assign(36,0);e.curr_task_schedule.assign(12,-1);e.goal_locations.resize(12);
  for(int r=0;r<12;++r){e.curr_states.emplace_back(r,0,r%4);e.goal_locations[r]={{35-r,0}};}
  unsetenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN");Cgar absent;absent.initialize(&e,1000);
  setenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN","0",1);Cgar disabled;disabled.initialize(&e,1000);
  for(int t=0;t<60;++t){
   e.curr_timestep=t;std::vector<Action>a,b;absent.plan(&e,1000,a);disabled.plan(&e,1000,b);
   require(a==b,"explicit promise OFF changed ordinary actions");
   auto next=step(e,e.curr_states,a);require(!next.empty(),"disabled promise fixture collision");e.curr_states=next;
   for(int r=0;r<12;++r)if(e.curr_states[r].location==e.goal_locations[r][0].first)e.goal_locations[r][0].first=(e.goal_locations[r][0].first+17)%36;
  }
  require(!absent.stats().temporal_promise_calls&&!disabled.stats().temporal_promise_calls,"disabled promises recorded work");
  for(const char* name:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_PROMISE_AFTER_TURN"})unsetenv(name);
 }
 // Validate strict configuration before exercising serial/parallel production
 // planning, protected primaries, and changing online guidance.
 SharedEnvironment config;config.rows=5;config.cols=5;config.num_of_agents=1;config.map.assign(25,0);
 config.curr_states={State(0,0,0)};config.curr_task_schedule={-1};config.goal_locations={{{24,0}}};
 setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 for(const char* value:{"","2","-1","true","1x"," 1"}){
  setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN",value,1);bool rejected=false;
  try{Cgar planner;planner.initialize(&config,1000);}catch(const std::invalid_argument&){rejected=true;}
  require(rejected,"invalid promise boolean accepted");
 }
 setenv("CGAR_TEMPORAL_PROMISE_AFTER_TURN","1",1);
 for(int mode=0;mode<3;++mode){
  setenv("CGAR_TEMPORAL",mode?"1":"0",1);
  if(mode==1)setenv("CGAR_TEMPORAL_WARM_START","1",1);
  if(mode==2)setenv("CGAR_TEMPORAL_MIXED_START","1",1);
  bool rejected=false;try{Cgar planner;planner.initialize(&config,1000);}catch(const std::invalid_argument&){rejected=true;}
  require(rejected,"promise accepted missing temporal mode or an incompatible warm/mixed start");
  unsetenv("CGAR_TEMPORAL_WARM_START");unsetenv("CGAR_TEMPORAL_MIXED_START");
 }
 temporal_parallel_regression();temporal_region_adapter_regression();temporal_primary_regression();
 setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_FLOW_WARMUP","4",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);
 temporal_region_adapter_regression();
 for(const char* name:{"CGAR_TEMPORAL_PROMISE_AFTER_TURN","CGAR_FLOW_STRENGTH","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES"})unsetenv(name);
 std::cout<<"TEMPORAL_AFTER_TURN_PROMISE passed occupied_cycle=4 promised_wait=4 unchanged_score_reference=1 protected_cascade=3 changed_goal=1 stale_history=1 explicit_timeout=1 invalid_configuration=9 disabled_equivalence_actions=720 protected_threaded_episodes=1\n";
}

void guide_routes_regression() {
 const int rows=5,cols=7,cells=rows*cols;
 std::vector<char> core(cells,true),eligible(2,true);
 std::vector<int> loc{14,20},ori{0,2},goals{20,14};
 GuideRouteOptions options;options.batch=1;options.base_cost=1;options.opposite_cost=8;options.load_cost=1;
 GuideRoutes guides;guides.initialize(core,rows,cols,2,options);
 auto neighbor=[&](int cell,int dir){int x=cell%cols,y=cell/cols;
  if(dir==0)return x+1<cols?cell+1:-1;if(dir==1)return y+1<rows?cell+cols:-1;
  if(dir==2)return x>0?cell-1:-1;return y>0?cell-cols:-1;};
 auto heuristic=[&](int r,int cell,int){return std::abs(cell/cols-goals[r]/cols)+std::abs(cell%cols-goals[r]%cols);};
 auto audit=[&]{
  std::vector<int> expected(cells*4,0);long long uses=0;
  for(int r=0;r<2;++r){const auto& path=guides.route_states(r);
   for(size_t k=guides.route_begin(r)+1;k<path.size();++k){int a=path[k-1],b=path[k];
    if(a/4==b/4){if((a%4+1)%4!=b%4&&(a%4+3)%4!=b%4)throw std::runtime_error("guide emitted invalid rotation");}
    else{if(neighbor(a/4,a%4)!=b/4||a%4!=b%4)throw std::runtime_error("guide emitted invalid forward");++expected[a];++uses;}
   }
  }
  if(expected!=guides.flow()||uses!=guides.directed_uses())throw std::runtime_error("guide flow conservation failed");
 };
 auto optimum=[&](int robot,const std::vector<int>& flow){
  // Independent forward Dijkstra, including current orientation and exact
  // marginal opposing/load costs from before this robot was admitted.
  std::vector<int64_t>d(cells*4,INT64_MAX/4);using Item=std::pair<int64_t,int>;
  std::priority_queue<Item,std::vector<Item>,std::greater<Item>> q;int start=loc[robot]*4+ori[robot];d[start]=0;q.push({0,start});
  while(!q.empty()){auto item=q.top();q.pop();int u=item.second;if(item.first!=d[u])continue;
   if(u/4==goals[robot])return item.first;
   auto relax=[&](int v,int64_t w){if(item.first+w<d[v]){d[v]=item.first+w;q.push({d[v],v});}};
   relax(u/4*4+(u%4+1)%4,options.base_cost);relax(u/4*4+(u%4+3)%4,options.base_cost);
   int v=neighbor(u/4,u%4);if(v>=0&&core[v]){int reverse=flow[v*4+(u%4+2)%4];
    relax(v*4+u%4,options.base_cost+int64_t(options.opposite_cost)*reverse+int64_t(options.load_cost)*(flow[u]+reverse));}
  }
  return INT64_MAX/4;
 };
 int covered=0;
 TemporalGeometry geometry;geometry.initialize(core,rows,cols,[]{});
 for(int r=0;r<2;++r){const auto previous=guides.flow();const auto best=optimum(r,previous);
  auto stats=guides.update(loc,ori,goals,eligible,heuristic,[]{});
  if(stats.attempted!=1||stats.solved!=1||stats.limited||stats.expanded>options.expansions)throw std::runtime_error("guide admission/expansion accounting failed");
  const auto& path=guides.route_states(r);int64_t cost=0;
  for(size_t k=1;k<path.size();++k){int a=path[k-1],b=path[k];cost+=options.base_cost;
   if(a/4!=b/4){int rev=previous[b/4*4+(a%4+2)%4];cost+=int64_t(options.opposite_cost)*rev+int64_t(options.load_cost)*(previous[a]+rev);}}
  if(path.front()!=loc[r]*4+ori[r]||path.back()/4!=goals[r]||cost!=best)throw std::runtime_error("guide A* disagrees with independent Dijkstra");
  for(const auto& candidate:geometry.paths(loc[r],ori[r]))if(candidate.valid)
   for(int d=0;d<4;++d){if(guides.distance(r,candidate.cells[4],d)<0)throw std::runtime_error("guide window omitted a valid candidate orientation");++covered;}
  audit();
 }
 if(guides.route_states(1).size()<=7)throw std::runtime_error("opposing-flow fixture did not change the intended route");
 for(int weight:{2,4}){
  auto weighted_options=options;weighted_options.heuristic_weight=weight;GuideRoutes weighted;weighted.initialize(core,rows,cols,2,weighted_options);
  for(int r=0;r<2;++r){const auto previous=weighted.flow();const auto best=optimum(r,previous);
   auto stats=weighted.update(loc,ori,goals,eligible,heuristic,[]{});const auto& path=weighted.route_states(r);int64_t cost=0;
   if(stats.solved!=1||path.empty()||path.back()/4!=goals[r])throw std::runtime_error("weighted guide did not finish a complete route");
   for(size_t k=1;k<path.size();++k){int a=path[k-1],b=path[k];cost+=options.base_cost;
    if(a/4!=b/4){int rev=previous[b/4*4+(a%4+2)%4];cost+=int64_t(options.opposite_cost)*rev+int64_t(options.load_cost)*(previous[a]+rev);}}
   if(cost<best||cost>weight*best)throw std::runtime_error("weighted guide exceeded the independent route-cost bound");
  }
 }
 // Execute one edge, change a goal, revoke admission, and deviate off a route.
 const auto first=guides.route_states(0);loc[0]=first[1]/4;ori[0]=first[1]%4;
 guides.update(loc,ori,goals,eligible,heuristic,[]{});audit();
 goals[0]=0;auto changed=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit();
 if(!changed.invalidated||!changed.goal_resets)throw std::runtime_error("changed guide goal retained old flow");
 eligible[0]=false;auto protected_reset=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit();
 if(!protected_reset.protected_resets)throw std::runtime_error("protected guide reset was not recorded");
 if(guides.guided(0)||!guides.route_states(0).empty())throw std::runtime_error("protected robot retained guide influence");
 loc[1]=34;auto deviated=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit();
 if(!deviated.invalidated||!deviated.deviation_resets)throw std::runtime_error("off-route location retained stale flow");
 eligible[1]=false;guides.update(loc,ori,goals,eligible,heuristic,[]{});audit();
 if(guides.directed_uses())throw std::runtime_error("retired guides leaked directed flow");

 GuideRoutes bounded;auto small=options;small.expansions=1;bounded.initialize(core,rows,cols,2,small);
 loc={0,34};ori={0,2};goals={6,33};eligible={true,true};
 auto one=bounded.update(loc,ori,goals,eligible,heuristic,[]{});
 auto two=bounded.update(loc,ori,goals,eligible,heuristic,[]{});
 if(one.attempted!=1||one.expanded!=1||one.limited!=1||one.solved||two.solved!=1||bounded.guided(0)||!bounded.guided(1))
  throw std::runtime_error("fixed guide work failed to give the next robot an admission turn");
 bool failed=false;try{bounded.update(loc,ori,goals,eligible,heuristic,[]{throw Timeout("guide_fixture");});}catch(const Timeout&){failed=true;}
 if(!failed)throw std::runtime_error("guide deadline was swallowed");
 // Disconnected core components must leave a complete baseline-only result.
 std::vector<char> split(5,true);split[2]=false;GuideRoutes disconnected;disconnected.initialize(split,1,5,1,options);
 auto no_path=disconnected.update({0},{0},{4},{true},[](int,int cell,int){return 4-cell;},[]{});
 if(no_path.solved||disconnected.guided(0)||disconnected.directed_uses())throw std::runtime_error("unreachable guide was published");

 setenv("CGAR_GUIDE_ROUTES","1",1);setenv("CGAR_GUIDE_BATCH","32",1);
 temporal_region_adapter_regression();
 setenv("CGAR_GUIDE_HEURISTIC_WEIGHT","2",1);
 setenv("CGAR_TEMPORAL_WARM_START","1",1);temporal_region_adapter_regression();unsetenv("CGAR_TEMPORAL_WARM_START");
 unsetenv("CGAR_GUIDE_HEURISTIC_WEIGHT");temporal_primary_regression();
 unsetenv("CGAR_GUIDE_ROUTES");unsetenv("CGAR_GUIDE_BATCH");
 std::cout<<"GUIDE_ROUTES passed independent_dijkstra=2 weighted_bounds=4 counter_conservation=1 candidate_orientations="<<covered<<" changed_goal=1 protected_reset=1 deviation_reset=1 fixed_work_fairness=1 explicit_timeout=1 production_actions=9600\n";
}

void guide_reconnect_regression() {
 const int rows=5,cols=19,cells=rows*cols;
 std::vector<char> core(cells,true),eligible(1,true);
 std::vector<int> loc{39},ori{0},goals{55};
 GuideRouteOptions options;options.batch=1;options.reconnect_steps=16;
 GuideRoutes guides;guides.initialize(core,rows,cols,1,options);
 auto heuristic=[&](int,int cell,int){return std::abs(cell/cols-goals[0]/cols)+std::abs(cell%cols-goals[0]%cols);};
 auto neighbor=[&](int cell,int dir){if(dir==0)return cell%cols+1<cols?cell+1:-1;if(dir==1)return cell/cols+1<rows?cell+cols:-1;
  if(dir==2)return cell%cols>0?cell-1:-1;return cell/cols>0?cell-cols:-1;};
 auto audit=[&](const GuideRoutes& value){
  std::vector<int> flow(cells*4);long long uses=0;const auto& path=value.route_states(0);
  if(!path.empty()&&path.back()/4!=goals[0])throw std::runtime_error("reconnected route lost its current goal");
  for(size_t k=value.route_begin(0)+1;k<path.size();++k){int a=path[k-1],b=path[k];
   if(a/4==b/4){if((a%4+1)%4!=b%4&&(a%4+3)%4!=b%4)throw std::runtime_error("reconnected route has an invalid turn");}
   else{if(neighbor(a/4,a%4)!=b/4||a%4!=b%4)throw std::runtime_error("reconnected route has an invalid edge");++flow[a];++uses;}
  }
  if(flow!=value.flow()||uses!=value.directed_uses())throw std::runtime_error("reconnection violated exact flow conservation");
 };
 guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides);
 const auto original=guides.route_states(0);
 ori[0]=1;guides.update(loc,ori,goals,eligible,heuristic,[]{});loc[0]+=cols;
 auto joined=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides);
 if(joined.reconnect_attempts!=1||joined.reconnected!=1||joined.attempted||joined.invalidated||
    joined.reconnect_actions<1||joined.reconnect_actions>16||guides.route_states(0).front()!=loc[0]*4+ori[0])
  throw std::runtime_error("nearby guide deviation did not retain a complete route without A*");
 TemporalGeometry geometry;geometry.initialize(core,rows,cols,[]{});int covered=0;
 for(const auto& candidate:geometry.paths(loc[0],ori[0]))if(candidate.valid)for(int d=0;d<4;++d){
  if(guides.distance(0,candidate.cells[4],d)<0)throw std::runtime_error("reconnected window omitted a candidate");++covered;}
 const auto connected=guides.route_states(0);
 for(size_t k=1;k<connected.size();++k){loc[0]=connected[k]/4;ori[0]=connected[k]%4;guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides);}
 if(guides.directed_uses())throw std::runtime_error("executed reconnected route retained consumed edges");
 goals[0]=0;auto changed=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides);
 if(changed.goal_resets!=1||changed.reconnected)throw std::runtime_error("reconnection reused a stale goal");
 eligible[0]=false;auto protected_reset=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides);
 if(protected_reset.protected_resets!=1||protected_reset.reconnected||guides.guided(0))throw std::runtime_error("reconnection altered a protected guide");

 GuideRoutes bounded;options.reconnect_steps=1;bounded.initialize(core,rows,cols,1,options);
 loc={39};ori={0};goals={55};eligible={true};bounded.update(loc,ori,goals,eligible,heuristic,[]{});
 loc[0]+=cols;ori[0]=1;auto limited=bounded.update(loc,ori,goals,eligible,heuristic,[]{});audit(bounded);
 if(limited.reconnected||limited.reconnect_attempts!=1||limited.deviation_resets!=1)
  throw std::runtime_error("reconnection exceeded its fixed action limit");
 GuideRoutes interrupted;options.reconnect_steps=16;interrupted.initialize(core,rows,cols,1,options);
 loc={39};ori={0};interrupted.update(loc,ori,goals,eligible,heuristic,[]{});const auto before=interrupted.flow();
 loc[0]+=cols;ori[0]=1;int checks=0;bool failed=false;
 try{interrupted.update(loc,ori,goals,eligible,heuristic,[&]{if(++checks==3)throw Timeout("reconnect_fixture");});}catch(const Timeout&){failed=true;}
 if(!failed||interrupted.flow()!=before||interrupted.route_states(0)!=original)throw std::runtime_error("interrupted connector published partial flow/path changes");
 audit(interrupted);

 setenv("CGAR_GUIDE_ROUTES","1",1);setenv("CGAR_GUIDE_BATCH","32",1);setenv("CGAR_GUIDE_HEURISTIC_WEIGHT","2",1);
 setenv("CGAR_GUIDE_RECONNECT_STEPS","16",1);setenv("CGAR_TEMPORAL_WARM_START","1",1);
 temporal_region_adapter_regression();
 for(const char* name:{"CGAR_GUIDE_ROUTES","CGAR_GUIDE_BATCH","CGAR_GUIDE_HEURISTIC_WEIGHT","CGAR_GUIDE_RECONNECT_STEPS","CGAR_TEMPORAL_WARM_START"})unsetenv(name);
 std::cout<<"GUIDE_RECONNECT passed complete_suffix=1 exact_flow_conservation=1 no_astar_on_reconnect=1 candidate_orientations="<<covered<<" fixed_limit=1 stale_goal=1 protected=1 explicit_timeout_no_partial_changes=1 production_actions=4800\n";
}

void guide_refine_regression() {
 const int rows=5,cols=7,cells=rows*cols;
 std::vector<char> core(cells,true),eligible(2,true);
 std::vector<int> loc{14,20},ori{0,2},goals{20,14};
 GuideRouteOptions options;options.batch=1;options.base_cost=1;options.opposite_cost=8;
 options.load_cost=1;options.refine_batch=1;
 GuideRoutes guides;guides.initialize(core,rows,cols,2,options);
 auto neighbor=[&](int cell,int dir){if(dir==0)return cell%cols+1<cols?cell+1:-1;if(dir==1)return cell/cols+1<rows?cell+cols:-1;
  if(dir==2)return cell%cols>0?cell-1:-1;return cell/cols>0?cell-cols:-1;};
 auto heuristic=[&](int r,int cell,int){return std::abs(cell/cols-goals[r]/cols)+std::abs(cell%cols-goals[r]%cols);};
 auto audit=[&](const GuideRoutes& value,int robots){
  std::vector<int> expected(cells*4);long long uses=0;
  for(int r=0;r<robots;++r){const auto& path=value.route_states(r);
   for(size_t k=value.route_begin(r)+1;k<path.size();++k){int a=path[k-1],b=path[k];
    if(a/4==b/4){if((a%4+1)%4!=b%4&&(a%4+3)%4!=b%4)throw std::runtime_error("refined guide has an invalid turn");}
    else{if(neighbor(a/4,a%4)!=b/4||a%4!=b%4)throw std::runtime_error("refined guide has an invalid edge");++expected[a];++uses;}
   }
  }
  if(expected!=value.flow()||uses!=value.directed_uses())throw std::runtime_error("refined guide violated exact flow conservation");
 };
 auto first=guides.update(loc,ori,goals,eligible,heuristic,[]{});
 if(first.solved!=1||first.refine_attempted)throw std::runtime_error("new guide was immediately refined");
 guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides,2);
 if(guides.route_states(1).size()<=7)throw std::runtime_error("refinement fixture failed to create a congestion detour");
 const int expected_saved=int(guides.route_states(1).size())-1-6;
 eligible[0]=false;
 auto changed=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides,2);
 const auto improved=guides.route_states(1);
 if(changed.protected_resets!=1||changed.refine_attempted!=1||changed.refined!=1||changed.refine_cost_saved!=expected_saved||
    improved.size()!=7||improved.front()!=loc[1]*4+ori[1]||improved.back()/4!=goals[1])
  throw std::runtime_error("refinement did not remove a newly unnecessary detour at strictly lower actual cost");
 // Independent unit-action BFS: after the other guide retires, excluding self
 // flow must give exactly the physical shortest path, despite positive load.
 std::vector<int> distance(cells*4,-1),queue{loc[1]*4+ori[1]};distance[queue[0]]=0;int optimum=-1;
 for(size_t head=0;head<queue.size();++head){int u=queue[head];if(u/4==goals[1]){optimum=distance[u];break;}
  auto push=[&](int v){if(distance[v]<0){distance[v]=distance[u]+1;queue.push_back(v);}};
  push(u/4*4+(u%4+1)%4);push(u/4*4+(u%4+3)%4);int v=neighbor(u/4,u%4);if(v>=0)push(v*4+u%4);
 }
 if(optimum!=int(improved.size())-1)throw std::runtime_error("refined route disagrees with independent shortest-path BFS");
 for(int t=0;t<4;++t){auto same=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides,2);
  if(same.refine_attempted!=1||same.refined||same.refine_cost_saved||guides.route_states(1)!=improved)
   throw std::runtime_error("refinement penalized its own flow or replaced an equal-cost route");
 }
 TemporalGeometry geometry;geometry.initialize(core,rows,cols,[]{});int covered=0;
 for(const auto& candidate:geometry.paths(loc[1],ori[1]))if(candidate.valid)for(int d=0;d<4;++d){
  if(guides.distance(1,candidate.cells[4],d)<0)throw std::runtime_error("refined window omitted a candidate");++covered;
 }
 const auto old_flow=guides.flow();bool failed=false;int heuristic_calls=0;
 try{guides.update(loc,ori,goals,eligible,[&](int,int,int)->int{++heuristic_calls;throw Timeout("refine_search_fixture");},[]{});}
 catch(const Timeout&){failed=true;}
 if(!failed||!heuristic_calls||guides.flow()!=old_flow||guides.route_states(1)!=improved)
  throw std::runtime_error("interrupted refinement changed a complete route or its flow");
 audit(guides,2);
 goals[1]=0;auto new_goal=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides,2);
 if(new_goal.goal_resets!=1||new_goal.refine_attempted||guides.route_states(1).back()/4!=0)
  throw std::runtime_error("refinement retained a stale goal or refined a new route");
 eligible[1]=false;auto retired=guides.update(loc,ori,goals,eligible,heuristic,[]{});audit(guides,2);
 if(retired.refine_attempted||guides.directed_uses())throw std::runtime_error("refinement acted on an ineligible guide");

 // Two complete one-edge routes are admitted within a one-expansion limit.
 // Turning both robots away then makes refinement hit that exact limit. A
 // separate cursor must give each eligible robot its turn, retaining both paths.
 GuideRoutes bounded;auto limited_options=options;limited_options.batch=2;limited_options.expansions=1;
 bounded.initialize(core,rows,cols,2,limited_options);
 loc={14,28};ori={0,0};goals={15,29};eligible={true,true};
 auto initial=bounded.update(loc,ori,goals,eligible,heuristic,[]{});audit(bounded,2);
 if(initial.solved!=2||initial.refine_attempted)throw std::runtime_error("refinement limit fixture failed to admit two complete routes");
 auto path0=bounded.route_states(0),path1=bounded.route_states(1),counts=bounded.flow();
 ori={2,2};std::vector<int> visited;
 for(int t=0;t<2;++t){auto limited=bounded.update(loc,ori,goals,eligible,[&](int r,int cell,int dir){visited.push_back(r);return heuristic(r,cell,dir);},[]{});
  if(limited.refine_attempted!=1||limited.refine_limited!=1||limited.refine_expanded!=1||limited.refined||
     bounded.route_states(0)!=path0||bounded.route_states(1)!=path1||bounded.flow()!=counts)
   throw std::runtime_error("bounded refinement replaced a route after an incomplete search");
  audit(bounded,2);
 }
 if(std::find(visited.begin(),visited.end(),0)==visited.end()||std::find(visited.begin(),visited.end(),1)==visited.end())
  throw std::runtime_error("bounded refinement starved an eligible route");

 setenv("CGAR_GUIDE_ROUTES","1",1);setenv("CGAR_GUIDE_BATCH","32",1);setenv("CGAR_GUIDE_HEURISTIC_WEIGHT","2",1);
 setenv("CGAR_GUIDE_RECONNECT_STEPS","16",1);setenv("CGAR_GUIDE_REFINE_BATCH","16",1);
 setenv("CGAR_GUIDE_BASE_COST","1",1);setenv("CGAR_GUIDE_OPPOSITE_COST","8",1);setenv("CGAR_GUIDE_LOAD_COST","1",1);
 setenv("CGAR_TEMPORAL_WARM_START","1",1);temporal_region_adapter_regression();temporal_primary_regression();
 for(const char* name:{"CGAR_GUIDE_ROUTES","CGAR_GUIDE_BATCH","CGAR_GUIDE_HEURISTIC_WEIGHT","CGAR_GUIDE_RECONNECT_STEPS",
     "CGAR_GUIDE_REFINE_BATCH","CGAR_GUIDE_BASE_COST","CGAR_GUIDE_OPPOSITE_COST","CGAR_GUIDE_LOAD_COST","CGAR_TEMPORAL_WARM_START"})unsetenv(name);
 std::cout<<"GUIDE_REFINE passed strict_cost_improvement="<<expected_saved<<" independent_unit_bfs=1 self_flow_excluded=1 equal_cost_retained=1 exact_flow_conservation=1 candidate_orientations="<<covered<<" stale_goal=1 protected=1 bounded_fairness=2 explicit_timeout_no_partial_changes=1 production_actions=4800\n";
}

void guide_window_regression() {
 int covered=0,rankings=0;
 // Rotate the straight corridor through all headings. Cached windows remain
 // active as the robot approaches a waypoint; every horizon state beyond that
 // waypoint must still receive its true remaining action distance.
 for(int direction=0;direction<4;++direction){
  const int rows=direction%2?49:1,cols=direction%2?1:49,cells=49;
  const int start=direction<2?0:48,delta=direction<2?1:-1,goal=start+40*delta;
  std::vector<char> core(cells,true);GuideRouteOptions options;options.lookahead=8;options.batch=1;
  GuideRoutes guides;guides.initialize(core,rows,cols,1,options);
  TemporalGeometry geometry;geometry.initialize(core,rows,cols,[]{});
  auto heuristic=[&](int,int cell,int){return std::abs(cell-goal);};
  for(int k=0;k<=40;++k){const int current=start+k*delta;
   guides.update({current},{direction},{goal},{true},heuristic,[]{});
   for(int m=0;m<=5&&k+m<=40;++m)for(int d=0;d<4;++d){
    const int remaining=40-k-m,turn=(direction-d+4)%4;
    const int expected=remaining?remaining+std::min(turn,4-turn):0;
    if(guides.distance(0,current+m*delta,d)!=expected)
     throw std::runtime_error("cached guide waypoint penalized a valid forward continuation");
    ++covered;
   }
   if(k==5){
    const auto& paths=geometry.paths(current,direction);const auto& operations=TemporalGeometry::operations();
    int forward=-1,park=-1;
    for(int op=0;op<int(operations.size());++op){
     if(operations[op]==std::array<uint8_t,5>{0,0,0,0,0})forward=op;
     if(operations[op]==std::array<uint8_t,5>{0,0,0,3,3})park=op;
    }
    auto distance=[&](int cell,int d){return guides.distance(0,cell,d);};
    if(forward<0||park<0||TemporalGeometry::cost(paths[forward],forward,goal,1,distance)>=
                            TemporalGeometry::cost(paths[park],park,goal,1,distance))
     throw std::runtime_error("guide candidate preferred phantom parking over forward progress");
    ++rankings;
   }
  }
 }
 // The whole small obstacle graph fits in the local box. Independently run
 // unit-action reverse BFS from all four terminal headings; merged seed/FIFO
 // propagation must agree for every reachable orientation, including corners.
 const int rows=7,cols=9,cells=rows*cols,goal=62;
 std::vector<char> core(cells,true);for(int u:{12,13,14,21,30,31,32,41,50})core[u]=false;
 auto neighbor=[&](int u,int d){if(d==0)return u%cols+1<cols?u+1:-1;if(d==1)return u/cols+1<rows?u+cols:-1;
  if(d==2)return u%cols>0?u-1:-1;return u/cols>0?u-cols:-1;};
 GuideRoutes guides;guides.initialize(core,rows,cols,1);
 auto stats=guides.update({0},{0},{goal},{true},[&](int,int u,int){return std::abs(u/cols-goal/cols)+std::abs(u%cols-goal%cols);},[]{});
 if(stats.solved!=1)throw std::runtime_error("obstacle guide-window fixture failed to find a complete route");
 std::vector<int> expected(cells*4,-1),queue;
 for(int d=0;d<4;++d){expected[goal*4+d]=0;queue.push_back(goal*4+d);}
 for(size_t head=0;head<queue.size();++head){int state=queue[head],u=state/4,d=state%4;
  auto push=[&](int cell,int dir){if(cell<0||!core[cell])return;int v=cell*4+dir;
   if(expected[v]<0){expected[v]=expected[state]+1;queue.push_back(v);}};
  push(u,(d+1)%4);push(u,(d+3)%4);push(neighbor(u,(d+2)%4),d);
 }
 for(int u=0;u<cells;++u)if(core[u])for(int d=0;d<4;++d){
  if(guides.distance(0,u,d)!=expected[u*4+d])throw std::runtime_error("multi-source guide window disagrees with independent BFS");
  ++covered;
 }
 std::cout<<"GUIDE_WINDOW passed rotated_forward_states="<<covered<<" forward_over_parking_rankings="<<rankings<<" independent_obstacle_bfs=1 all_goal_headings=1\n";
}

void temporal_preparation_regression() {
 // Every worker must finish or report its failure before the caller returns,
 // including when the main worker throws and when a child worker throws.
 for(int fault=-1;fault<4;++fault){
  std::array<std::atomic<int>,4> entered,finished;for(auto&v:entered)v.store(0);for(auto&v:finished)v.store(0);
  bool failed=false;
  try{run_temporal_preparation(4,[&](int worker){entered[worker].store(1);if(worker==fault)throw Timeout("prepare_fixture");finished[worker].store(1);});}
  catch(const Timeout&){failed=true;}
  if(failed!=(fault>=0))throw std::runtime_error("preparation worker failure was swallowed");
  for(int worker=0;worker<4;++worker)if(entered[worker].load()!=1||finished[worker].load()!=(worker!=fault))
   throw std::runtime_error("preparation returned before every other worker completed");
 }
 bool invalid=false;try{run_temporal_preparation(0,[](int){});}catch(const std::invalid_argument&){invalid=true;}
 if(!invalid)throw std::runtime_error("invalid preparation thread count was accepted");

 // A one-MiB logical turn cache cannot hold all 48 distinct goals on this
 // graph. Repeated planning therefore exposes changes to LRU lookup order,
 // not just parallel scoring of a warm, never-evicted set of small tables.
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","256",1);setenv("CGAR_TEMPORAL_EQUAL_WEIGHT","1",1);
 setenv("CGAR_TEMPORAL_ORDER","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TURN_FIRST","1",1);
 setenv("CGAR_TURN_TABLE_MB","1",1);setenv("CGAR_TURN_COMPACT","1",1);
 SharedEnvironment e;e.rows=50;e.cols=50;e.num_of_agents=48;e.map.assign(2500,0);e.curr_task_schedule.assign(48,-1);e.goal_locations.resize(48);
 for(int r=0;r<48;++r){e.curr_states.emplace_back(r*17,0,r%4);e.goal_locations[r]={{2499-r*13,0}};}
 setenv("CGAR_TEMPORAL_PREP_THREADS","1",1);Cgar serial;serial.initialize(&e,1000);
 setenv("CGAR_TEMPORAL_PREP_THREADS","4",1);Cgar parallel;parallel.initialize(&e,1000);
 int checked=0;
 for(int t=0;t<80;++t){
  e.curr_timestep=t;std::vector<Action>a,b;serial.plan(&e,1000,a);parallel.plan(&e,1000,b);
  if(a!=b)throw std::runtime_error("parallel preparation changed decisions under turn-cache eviction");
  auto next=step(e,e.curr_states,a);if(next.empty())throw std::runtime_error("parallel preparation produced a collision");e.curr_states=next;
  for(int r=0;r<48;++r)if(e.curr_states[r].location==e.goal_locations[r][0].first)e.goal_locations[r][0].first=(e.goal_locations[r][0].first+997)%2500;
  checked+=48;
 }
 const auto&a=serial.stats();const auto&b=parallel.stats();
 if(a.oriented_builds<=48||a.oriented_builds!=b.oriented_builds||a.oriented_guided!=b.oriented_guided||a.oriented_fallback!=b.oriented_fallback||
    a.temporal_prepared_robots!=checked||b.temporal_prepared_robots!=checked||a.temporal_parallel_preparations||b.temporal_parallel_preparations!=80)
  throw std::runtime_error("parallel preparation changed cache behavior or failed to exercise all workers");
 for(const char*name:{"CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_EQUAL_WEIGHT","CGAR_TEMPORAL_ORDER",
     "CGAR_ORIENTATION_GUIDANCE","CGAR_TURN_FIRST","CGAR_TURN_TABLE_MB","CGAR_TURN_COMPACT","CGAR_TEMPORAL_PREP_THREADS"})unsetenv(name);
 std::cout<<"TEMPORAL_PREPARATION passed joined_failure_cases=4 joined_success=1 invalid_threads=1 identical_robot_decisions="<<checked
          <<" turn_builds="<<a.oriented_builds<<" exact_lru_effects=1 threads=1,4\n";
}

struct NextErrandOracle {
 int rows,cols,U,T,tolls,n;TemporalGeometry geometry;std::vector<std::vector<int>> dist;
 NextErrandOracle(int r,int c,int unit,int extra,int toll):rows(r),cols(c),U(unit),T(unit+extra),tolls(toll),n(r*c){
  geometry.initialize(std::vector<char>(n,true),r,c,[]{});
  dist.assign(n,std::vector<int>(4*n,1000000));
  // Independent Bellman relaxation of the explicit directed orientation graph.
  for(int goal=0;goal<n;++goal){auto& d=dist[goal];for(int h=0;h<4;++h)d[4*goal+h]=0;
   bool changed=true;int passes=0;
   while(changed){changed=false;if(++passes>4*n+1)throw std::runtime_error("distance relaxation did not converge");
    for(int cell=0;cell<n;++cell)for(int h=0;h<4;++h){int z=4*cell+h,best=d[z];
     best=std::min({best,T+d[4*cell+(h+1)%4],T+d[4*cell+(h+3)%4]});
     int to=neighbor(cell,h);if(to>=0)best=std::min(best,forward(cell,h)+d[4*to+h]);
     if(best<d[z]){d[z]=best;changed=true;}
    }
   }
  }
 }
 int neighbor(int cell,int h)const{int r=cell/cols,c=cell%cols;if(h==0)++c;else if(h==1)++r;else if(h==2)--c;else --r;return r<0||r>=rows||c<0||c>=cols?-1:r*cols+c;}
 int forward(int cell,int h)const{return U+(tolls?(cell*7+h*3)%3:0);}
 int direction(int a,int b)const{for(int h=0;h<4;++h)if(neighbor(a,h)==b)return h;throw std::runtime_error("nonadjacent move");}
 int value(int goal,int cell,int h)const{return dist[goal][4*cell+h];}
 int baseline(int goal,int next)const{int b=1000000;for(int h=0;h<4;++h)b=std::min(b,value(next,goal,h));return b;}
 int operation(const std::string& word)const{for(int op=0;op<129;++op){std::string s;for(int a:TemporalGeometry::operations()[op])s+="FRCW"[a];if(s==word)return op;}throw std::runtime_error("missing operation");}
 int64_t native(const TemporalPath&p,int op,int start,int goal)const{
  const int extra=TemporalGeometry::forward_surcharge(p,start,goal,[&](int from,int to){return forward(from,direction(from,to));},U);
  return TemporalGeometry::cost(p,op,goal,T,[&](int c,int h){return value(goal,c,h);},50,U)+int64_t(extra)*50;
 }
 int64_t proposed(const TemporalPath&p,int op,int start,int goal,int next,int mode)const{
  if(!mode||next<0||next==goal||TemporalGeometry::first_goal_hit(p,goal)<0)return native(p,op,start,goal);
  return TemporalGeometry::next_errand_cost(p,op,start,T,baseline(goal,next),
   [&](int cell,int h){return value(next,cell,h);},
   [&](int from,int to){return forward(from,direction(from,to));},50,U);
 }
 // Independently replay complete weighted actions, explicitly replacing up to
 // two trailing waits by all wait/right/left combinations. The next-goal terminal
 // potential deliberately persists after service: this is the proposed surrogate,
 // not an exact multi-errand completion-time objective.
 int oracle_tail(const TemporalPath&p,int op,int start,int heading,int next)const{
  auto original=TemporalGeometry::operations()[op];if(op==0)original[0]=p.first_action;
  int suffix=0;if(original[4]==3){suffix=1;if(original[3]==3)suffix=2;}
  int variants=suffix==2?9:suffix==1?3:1,best=1000000;
  for(int v=0;v<variants;++v){auto actions=original;int code=v;
   for(int k=0;k<suffix;++k){actions[4-k]=std::array<int,3>{3,1,2}[code%3];code/=3;}
   int cell=start,h=heading,cost=0;
   for(int a:actions){if(a==0){cost+=forward(cell,h);cell=neighbor(cell,h);if(cell<0)throw std::runtime_error("oracle left grid");}
    else if(a==1){cost+=T;h=(h+1)%4;}else if(a==2){cost+=T;h=(h+3)%4;}else cost+=U;}
   best=std::min(best,cost+value(next,cell,h)-5*U);
  }
  return best;
 }
};

void temporal_remaining_flow_regression() {
 long long checked=0,neutral=0,arrivals=0,changed_rankings=0;
 for(int unit:{1,4,8})for(int turn_extra:{0,2})for(int toll:{0,1}) {
  NextErrandOracle independent(4,5,unit,turn_extra,toll);
  for(int start:{0,7,19})for(int heading=0;heading<4;++heading)for(int goal:{2,7,16}) {
   const auto& paths=independent.geometry.paths(start,heading);
   int old_best=-1,new_best=-1;int64_t old_min=INT64_MAX,new_min=INT64_MAX;
   for(int op=0;op<129;++op)if(paths[op].valid) {
    const auto& path=paths[op];auto actions=TemporalGeometry::operations()[op];
    int cell=start,h=heading,first=-1,last=-1,turns=0,turns_first=0,paid=0;
    // Replay legal actions independently of cost()/forward_surcharge().
    for(int tick=0;tick<5;++tick) {
     int action=actions[tick];
     if(action==0){if(first<0)paid+=independent.forward(cell,h)-unit;cell=independent.neighbor(cell,h);}
     else if(action==1){++turns;h=(h+1)%4;}else if(action==2){++turns;h=(h+3)%4;}
     if(cell==goal){if(first<0){first=tick;turns_first=turns;}last=tick;}
    }
    int tail=1000000;
    // Enumerate the same allowed virtual rotations in trailing wait slots.
    int suffix=actions[4]==3?(actions[3]==3?2:1):0;
    for(int code=0;code<(suffix==2?9:suffix==1?3:1);++code){
     int hh=h,extra=0,v=code;
     for(int k=0;k<suffix;++k){int a=v%3;v/=3;if(a){hh=(hh+(a==1?1:3))%4;extra+=turn_extra;}}
     tail=std::min(tail,independent.value(goal,cell,hh)+extra);
    }
    const int64_t expected=(int64_t(last<0?tail:-last*unit)+turn_extra*(first<0?turns:turns_first))*50-int64_t(op)*unit;
    auto distance=[&](int c,int d){return independent.value(goal,c,d);};
    auto edge=[&](int from,int to){return independent.forward(from,independent.direction(from,to));};
    const auto on=TemporalGeometry::flow_cost(path,op,start,goal,unit+turn_extra,distance,edge,50,unit,true);
    const auto off=TemporalGeometry::flow_cost(path,op,start,goal,unit+turn_extra,distance,edge,50,unit,false);
    if(on!=expected||off!=expected+int64_t(paid)*50||off!=independent.native(path,op,start,goal))
     throw std::runtime_error("remaining-flow score disagreed with independent legal-action replay");
    if(!toll){if(on!=off)throw std::runtime_error("neutral flow changed scores");++neutral;}
    if(first>=0)++arrivals;
    if(off<old_min){old_min=off;old_best=op;}if(on<new_min){new_min=on;new_best=op;}++checked;
   }
   changed_rankings+=old_best!=new_best;
  }
 }
 if(!changed_rankings||!arrivals||!neutral)throw std::runtime_error("remaining-flow score fixtures were vacuous");
 // FFFFF crosses first-service goal at t=2. Pay only extras4+0+2=6,
 // even though later edges carry extra9. Native terminal score is-912.
 TemporalGeometry line;line.initialize(std::vector<char>(7,true),1,7,[]{});
 const auto& path=line.paths(0,0)[128];
 auto distance=[](int cell,int){return std::abs(cell-3)*4;};
 auto edge=[](int from,int){return from==0?8:from==2?6:from>=3?13:4;};
 if(TemporalGeometry::flow_cost(path,128,0,3,4,distance,edge,50,4,false)!=-612||
    TemporalGeometry::flow_cost(path,128,0,3,4,distance,edge,50,4,true)!=-912)
  throw std::runtime_error("remaining-flow first-service hand-counted fixture failed");

 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_FLOW_STRENGTH","4",1);setenv("CGAR_FLOW_COST_SCALE","4",1);
 setenv("CGAR_TEMPORAL_STEPS","256",1);setenv("CGAR_TEMPORAL_EQUAL_WEIGHT","1",1);
 setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);
 SharedEnvironment e;e.rows=9;e.cols=11;e.num_of_agents=24;e.map.assign(99,0);
 e.curr_task_schedule.assign(24,-1);e.goal_locations.resize(24);
 for(int r=0;r<24;++r){e.curr_states.emplace_back(r*3,0,r%4);e.goal_locations[r]={{98-r*3,0}};}
 int identical=0;
 for(bool prepublication:{true,false}) {
  auto state=e;setenv("CGAR_FLOW_WARMUP",prepublication?"128":"8",1);
  setenv("CGAR_FLOW_MIN_MARGIN_PERCENT",prepublication?"25":"100",1);
  setenv("CGAR_TEMPORAL_REMAINING_FLOW","0",1);setenv("CGAR_TEMPORAL_PREP_THREADS","1",1);Cgar off;off.initialize(&state,1000);
  setenv("CGAR_TEMPORAL_REMAINING_FLOW","1",1);setenv("CGAR_TEMPORAL_PREP_THREADS","4",1);Cgar on;on.initialize(&state,1000);
  for(int t=0;t<80;++t){state.curr_timestep=t;std::vector<Action>a,b;off.plan(&state,1000,a);on.plan(&state,1000,b);
   if(a!=b)throw std::runtime_error("remaining-flow changed neutral or prepublication production actions");
   auto next=step(state,state.curr_states,a);if(next.empty())throw std::runtime_error("remaining-flow neutral collision");state.curr_states=next;
   for(int r=0;r<24;++r)if(state.curr_states[r].location==state.goal_locations[r][0].first)
    state.goal_locations[r][0].first=(state.goal_locations[r][0].first+37)%99;
   identical+=24;
  }
  if(on.stats().flow_publications!=(prepublication?0:5)||on.stats().flow_penalized_edges||
     on.stats().oriented_builds!=off.stats().oriented_builds||on.stats().flow_cache_resets!=off.stats().flow_cache_resets)
   throw std::runtime_error("remaining-flow neutral lifecycle was not exercised exactly");
 }
 auto reject=[&](bool specific){bool rejected=false;try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument& error){
   rejected=true;if(specific&&std::string(error.what()).find("remaining-flow scoring")==std::string::npos)
    throw std::runtime_error("remaining-flow fixture hit an unrelated configuration guard");}
  if(!rejected)throw std::runtime_error("invalid remaining-flow combination accepted");};
 for(const char* value:{"-1","2"}){setenv("CGAR_TEMPORAL_REMAINING_FLOW",value,1);reject(true);}
 setenv("CGAR_TEMPORAL_REMAINING_FLOW","1",1);setenv("CGAR_FLOW_COST_SCALE","1",1);
 for(const char* key:{"CGAR_TEMPORAL","CGAR_FLOW_STRENGTH","CGAR_ORIENTATION_GUIDANCE"}){
  const char* saved_value=getenv(key);if(!saved_value)throw std::runtime_error("missing remaining-flow fixture setting");
  const std::string saved=saved_value;setenv(key,"0",1);reject(std::string(key)!="CGAR_ORIENTATION_GUIDANCE");setenv(key,saved.c_str(),1);}
 setenv("CGAR_FLOW_COST_SCALE","4",1);
 for(const char* key:{"CGAR_TEMPORAL_NEXT_ERRAND","CGAR_GUIDE_ROUTES","CGAR_TEMPORAL_BRANCH_WORK",
                     "CGAR_TEMPORAL_SERVICE_AUDIT_STRIDE","CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE"}){
  setenv(key,"1",1);reject(std::string(key)!="CGAR_GUIDE_ROUTES");unsetenv(key);}
 // Each arm executes its own complete decisions. The first divergence must
 // therefore occur with equal prior observations, rather than replaying an
 // action that only one arm actually offered. Both primaries must serve.
 setenv("CGAR_FLOW_WARMUP","4",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","25",1);
 SharedEnvironment dense;dense.rows=9;dense.cols=11;dense.num_of_agents=48;dense.map.assign(99,0);
 dense.curr_task_schedule.assign(48,-1);dense.goal_locations.resize(48);
 for(int r=0;r<48;++r){dense.curr_states.emplace_back(r,0,r%4);dense.goal_locations[r]={{98-r,0}};}
 SharedEnvironment active[2]={dense,dense};Cgar planners[2];bool primary_served[2]={false,false};int active_differences=0;
 for(int mode=0;mode<2;++mode){setenv("CGAR_TEMPORAL_REMAINING_FLOW",mode?"1":"0",1);planners[mode].initialize(&active[mode],1000);}
 for(int t=0;t<120;++t){std::vector<Action> actions[2];
  for(int mode=0;mode<2;++mode){auto& state=active[mode];state.curr_timestep=t;planners[mode].plan(&state,1000,actions[mode]);
   auto next=step(state,state.curr_states,actions[mode]);if(next.empty())throw std::runtime_error("active remaining-flow fixture collided");state.curr_states=next;
   if(state.curr_states[0].location==98)primary_served[mode]=true;
   for(int r=0;r<48;++r)if(state.curr_states[r].location==state.goal_locations[r][0].first)
    state.goal_locations[r][0].first=(state.goal_locations[r][0].first+37)%99;
  }
  if(t<4&&actions[0]!=actions[1])throw std::runtime_error("active remaining-flow arms diverged before publication");
  active_differences+=actions[0]!=actions[1];
 }
 if(!active_differences||!primary_served[0]||!primary_served[1]||
    !planners[0].stats().flow_penalized_edges||!planners[1].stats().flow_penalized_edges)
  throw std::runtime_error("active remaining-flow adapter effect or primary service was not exercised");


 temporal_region_adapter_regression();temporal_primary_regression();temporal_parallel_regression();
 for(const char* key:{"CGAR_TEMPORAL_REMAINING_FLOW","CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_FLOW_STRENGTH",
     "CGAR_FLOW_COST_SCALE","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT",
     "CGAR_FLOW_REFRESH_INTERVAL","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_EQUAL_WEIGHT","CGAR_TEMPORAL_PREP_THREADS"})unsetenv(key);
 std::cout<<"TEMPORAL_REMAINING_FLOW passed independent_macro_scores="<<checked<<" neutral_scores="<<neutral
          <<" arriving_scores="<<arrivals<<" changed_best_macros="<<changed_rankings<<" first_service_hand_count=1 neutral_production_actions="<<identical
          <<" active_different_steps="<<active_differences<<" active_collision_checks=11520 both_primaries_served=1 invalid_combinations=10 protected_primary_recovery_capacity=1 regional_actions=4800 worker_actions=1920\n";
}

void temporal_next_errand_regression() {
 long long arriving=0;
 for(int U:{1,4,8})for(int extra:{0,1,2})for(int toll:{0,1}){
  NextErrandOracle m(3,7,U,extra,toll);
  for(int goal:{8,10,11}){const int next=(goal+7)%m.n;
   for(int start=0;start<m.n;++start)for(int h=0;h<4;++h){const auto& paths=m.geometry.paths(start,h);
    for(int op=0;op<129;++op){const auto&p=paths[op];if(!p.valid)continue;
     const auto first=std::find(p.cells.begin(),p.cells.end(),goal);
     const int expected=first==p.cells.end()?-1:int(first-p.cells.begin());
     if(TemporalGeometry::first_goal_hit(p,goal)!=expected)throw std::runtime_error("next-errand first service slot incorrect");
     if(expected<0)continue;
     const auto score=m.proposed(p,op,start,goal,next,1);
     const int64_t physical=(score+int64_t(op)*U)/50+4*U+m.baseline(goal,next);
     if(physical!=m.oracle_tail(p,op,start,h,next))throw std::runtime_error("next-errand score disagrees with independent paid-action replay");
     ++arriving;
    }
   }
  }
 }
 NextErrandOracle m(3,7,4,0,0);
 const int pa=m.operation("FRFFF"),pb=m.operation("RFCFW");const auto& paths=m.geometry.paths(14,3);
 const auto physical=[&](int op){return (m.proposed(paths[op],op,14,8,13,1)+op*4)/50;};
 if(physical(pa)!=-24||physical(pb)!=-16)throw std::runtime_error("next-errand common baseline refunded bad arrival heading");
 auto compatible=[](const TemporalPath&a,const TemporalPath&b){for(int t=0;t<5;++t)
  if(a.cells[t]==b.cells[t]||(a.edges[t]>=0&&a.edges[t]==b.edges[t]))return false;return true;};
 for(int fixture=0;fixture<2;++fixture){int sa=fixture?12:11,ga=fixture?11:10,next=fixture?4:3,gb=fixture?7:8;
  const auto&A=m.geometry.paths(sa,2);const auto&B=m.geometry.paths(5,1);
  for(int mode:{0,1}){int ai=-1,bi=-1;int64_t best=INT64_MAX;
   for(int i=0;i<129;++i)if(A[i].valid)for(int j=0;j<129;++j)if(B[j].valid&&compatible(A[i],B[j])){
    const auto score=m.proposed(A[i],i,sa,ga,next,mode)+m.native(B[j],j,5,gb);
    if(score<best){best=score;ai=i;bi=j;}}
   const auto expected=mode?(fixture?-1364:-1564):(fixture?-840:-1040);
   if(best!=expected||ai<0||bi<0||(mode&&(A[ai].first_action!=0||B[bi].first_action!=0)))
    throw std::runtime_error("next-errand joint collision-free fixture did not reproduce");
  }
 }
 for(auto values:std::vector<std::array<int,4>>{{0,4,4,50},{4,3,4,50},{4,4,-1,50},{4,4,4,0}}){bool failed=false;
  try{TemporalGeometry::next_errand_cost(paths[pa],pa,14,values[1],values[2],[](int,int){return 4;},[](int,int){return 4;},values[3],values[0]);}
  catch(const std::invalid_argument&){failed=true;}if(!failed)throw std::runtime_error("invalid next-errand units accepted");}
 bool interrupted=false;try{TemporalGeometry::next_errand_cost(paths[pa],pa,14,4,4,[](int,int)->int{throw Timeout("next_distance_fixture");},[](int,int){return 4;},50,4);}
 catch(const Timeout&){interrupted=true;}if(!interrupted)throw std::runtime_error("next-errand lookup failure was swallowed");
 for(const char* invalid:{"-1","2","1"}){
  setenv("CGAR_TEMPORAL_NEXT_ERRAND",invalid,1);setenv("CGAR_TEMPORAL",invalid[0]=='1'?"0":"1",1);
  SharedEnvironment e;e.rows=e.cols=1;e.num_of_agents=0;e.map={0};bool rejected=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid/non-temporal next-errand option accepted");
 }
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);setenv("CGAR_TURN_FIRST","1",1);
 setenv("CGAR_TEMPORAL_STEPS","256",1);setenv("CGAR_TEMPORAL_EQUAL_WEIGHT","1",1);setenv("CGAR_TURN_TABLE_MB","1",1);
 setenv("CGAR_TEMPORAL_TABLE_BATCH","32",1);setenv("CGAR_TEMPORAL_PREP_THREADS","4",1);
 auto environment=[](int kind){
  SharedEnvironment e;e.rows=e.cols=7;e.num_of_agents=4;e.map.assign(49,0);
  e.curr_states={State(0,0,1),State(24,0,2),State(6,0,1),State(42,0,0)};
  std::vector<int> goals{48,23,16,44};
  if(kind==5){e.rows=3;e.cols=9;e.map.assign(27,0);for(int r=0;r<3;++r)e.map[r*9+4]=1;
   e.curr_states={State(0,0,1),State(11,0,2),State(6,0,1),State(8,0,1)};goals={18,10,16,26};}
  if(kind==7){e.rows=7;e.cols=9;e.map.assign(63,1);
   for(int r=0;r<7;++r)for(int c=0;c<7;++c)e.map[r*9+c]=0;e.map[34]=e.map[35]=0;
   e.curr_states={State(0,0,1),State(32,0,0),State(6,0,1),State(54,0,0)};goals={60,33,11,56};}
  if(kind==8)goals[1]=24;
  e.curr_task_schedule={0,1,2,3};e.goal_locations.resize(4);
  for(int r=0;r<4;++r){Task task;task.task_id=r;task.t_revealed=0;task.agent_assigned=r;task.locations={goals[r]};
   if(r==1&&kind!=4)task.locations.push_back(kind==1?17:kind==2?goals[r]:kind==7?11:16);
   if(r==0&&kind==4)task.locations.push_back(16);
   e.task_pool.emplace(r,task);e.goal_locations[r]={{goals[r],0}};}
  return e;
 };
 // 0 resident next table,1 absent,2 repeated location,3 current fallback,
 // 4 only a pinned primary has a next errand,5 disconnected next,6 guided metric,
 // 7 finite current goal but an excluded pocket endpoint,8 assigned at current cell.
 for(int kind=0;kind<9;++kind){
  setenv("CGAR_TEMPORAL_TABLE_BATCH",kind==3?"0":"32",1);setenv("CGAR_TURN_BUILD_LIMIT",kind==3?"0":"32",1);
  setenv("CGAR_GUIDE_ROUTES",kind==6?"1":"0",1);auto e=environment(kind);
  setenv("CGAR_TEMPORAL_NEXT_ERRAND","0",1);Cgar native;native.initialize(&e,1000);
  setenv("CGAR_TEMPORAL_NEXT_ERRAND","1",1);Cgar changed;changed.initialize(&e,1000);
  std::vector<Action>a,b;native.plan(&e,1000,a);changed.plan(&e,1000,b);
  if(step(e,e.curr_states,b).empty())throw std::runtime_error("next-errand adapter caused a collision");
  if(native.primary()!=changed.primary()||native.primary()<0||a[native.primary()]!=b[changed.primary()])
   throw std::runtime_error("next-errand adapter changed protected primary action");
  const auto&s=changed.stats();
  if(s.temporal_next_known!=s.temporal_next_eligible+s.temporal_next_unavailable)
   throw std::runtime_error("next-errand metric partition lost robots");
  if(kind==0||kind==8){if(!s.temporal_next_eligible||!s.temporal_next_arriving_choices||!s.temporal_next_changed_choices)
   throw std::runtime_error("next-errand production fixture was vacuous");}
  else if(a!=b||s.temporal_next_eligible||s.temporal_next_changed_choices||native.stats().oriented_builds!=s.oriented_builds)
   throw std::runtime_error("next-errand fall-through changed native planning, kind="+std::to_string(kind));
  if(native.stats().temporal_next_known||native.stats().temporal_next_changed_choices)
   throw std::runtime_error("disabled next-errand option performed optional scoring");
  for(int r=0;r<4;++r)if(e.curr_task_schedule[r]!=r||e.task_pool.at(r).idx_next_loc!=0||e.task_pool.at(r).agent_assigned!=r)
   throw std::runtime_error("next-errand preparation changed visible task metadata");
  if(kind==7){
   const auto cert=build_certificate(e.map,e.rows,e.cols,4);TurnDistanceOracle oracle;oracle.init(&cert,1024*1024);
   const auto* table=oracle.table(11,std::chrono::steady_clock::now()+std::chrono::seconds(1));
   TemporalGeometry geometry;geometry.initialize(cert.free,e.rows,e.cols,[]{});
   if(!cert.valid||!cert.core[33]||cert.core[35]||cert.pocket[35]<0||s.temporal_next_known!=1||s.temporal_next_unavailable!=1)
    throw std::runtime_error("next-errand pocket fallback fixture did not exercise the intended robot/domain");
   for(int h=0;h<4;++h)if(oracle.value(*table,33,h)>=kInf||oracle.value(*table,35,h)!=kInf)
    throw std::runtime_error("next-errand pocket fixture lacks finite goal and infinite endpoint");
   bool witness=false;for(const auto& path:geometry.paths(32,0))
    witness|=path.valid&&TemporalGeometry::first_goal_hit(path,33)>=0&&path.cells[4]==35;
   if(!witness||a!=b||s.temporal_next_changed_choices||s.temporal_next_eligible)
    throw std::runtime_error("finite-goal/infinite-endpoint fallback changed the complete native plan");
  }
  if(kind==8){
   const auto cert=build_certificate(e.map,e.rows,e.cols,4);TemporalGeometry geometry;geometry.initialize(cert.free,e.rows,e.cols,[]{});
   const auto& own=geometry.paths(24,2);
   if(TemporalGeometry::first_goal_hit(own[0],24)!=0)throw std::runtime_error("own-cell wait seed did not serve at first action");
   bool departing=false;for(const auto& path:own)if(path.valid&&path.first_action==0&&
    std::find(path.cells.begin(),path.cells.end(),24)==path.cells.end()){
     departing=true;if(TemporalGeometry::first_goal_hit(path,24)!=-1)throw std::runtime_error("departure was credited with own-cell service");}
   // The next errand is northwest, and the robot already faces west. WFRFW
   // serves the current cell before departing, reaches next at t=3, and scores
   // (0-3-4)*50-59=-409. A first clockwise turn requires two turns overall.
   NextErrandOracle independent(7,7,1,0,0);int best=-1;int64_t best_score=INT64_MAX;
   for(int op=0;op<129;++op)if(own[op].valid){const auto score=independent.proposed(own[op],op,24,24,16,1);
    if(score<best_score){best_score=score;best=op;}}
   if(!departing||best!=59||best_score!=-409||own[best].first_action!=3||b[1]!=Action::W||
      a[1]!=Action::CR||native.stats().temporal_wait_seeds!=1||native.stats().temporal_tied_seed_rotations!=1)
    throw std::runtime_error("own-cell service disagreed with independent continuation score or native seed projection");
   auto after=step(e,e.curr_states,b);if(after.empty()||after[1].location!=24||after[1].orientation!=2)
    throw std::runtime_error("own-cell wait did not preserve the service location and useful west heading");
   // TaskManager advances once AFTER the action. The planner did not alter it.
   e.curr_states=after;e.curr_timestep=1;++e.task_pool.at(1).idx_next_loc;e.goal_locations[1]={{16,0}};
   const auto prior_known=changed.stats().temporal_next_known;std::vector<Action>second;changed.plan(&e,1000,second);
   if(step(e,e.curr_states,second).empty()||e.task_pool.at(1).idx_next_loc!=1||
      changed.stats().temporal_next_known!=prior_known)
    throw std::runtime_error("own-cell service left stale next-errand metadata in the following plan");
  }
 }
 unsetenv("CGAR_GUIDE_ROUTES");setenv("CGAR_TURN_BUILD_LIMIT","32",1);setenv("CGAR_TEMPORAL_TABLE_BATCH","32",1);
 setenv("CGAR_FLOW_STRENGTH","4",1);setenv("CGAR_FLOW_COST_SCALE","4",1);setenv("CGAR_FLOW_WARMUP","1",1);
 setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","0",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","4",1);
 auto e=environment(0);e.task_pool.at(3).locations.push_back(44);
 setenv("CGAR_TEMPORAL_PREP_THREADS","1",1);Cgar serial;serial.initialize(&e,1000);
 setenv("CGAR_TEMPORAL_PREP_THREADS","4",1);Cgar parallel;parallel.initialize(&e,1000);
 int next_id=4,services=0,repeated=0;
 for(int t=0;t<80;++t){e.curr_timestep=t;std::vector<Action>a,b;serial.plan(&e,1000,a);parallel.plan(&e,1000,b);
  if(a!=b)throw std::runtime_error("next-errand serial/parallel preparation changed decisions");
  auto next=step(e,e.curr_states,a);if(next.empty())throw std::runtime_error("next-errand multistep collision");e.curr_states=next;
  // Exactly one service per tick, including consecutive equal locations.
  for(int r=0;r<4;++r){int id=e.curr_task_schedule[r];auto& task=e.task_pool.at(id);
   if(e.curr_states[r].location==task.locations[task.idx_next_loc]){
    ++services;const int prior=task.idx_next_loc;++task.idx_next_loc;
    if(task.idx_next_loc<int(task.locations.size())){repeated+=task.locations[prior]==task.locations[task.idx_next_loc];}
    else{Task fresh;fresh.task_id=next_id++;fresh.t_revealed=t+1;fresh.agent_assigned=r;
     fresh.locations={(e.curr_states[r].location+11+r*3)%49,(e.curr_states[r].location+23+r*5)%49};
     e.task_pool.erase(id);e.task_pool.emplace(fresh.task_id,fresh);e.curr_task_schedule[r]=fresh.task_id;}
   }
   const auto& current=e.task_pool.at(e.curr_task_schedule[r]);e.goal_locations[r]={{current.locations[current.idx_next_loc],0}};
  }
 }
 const auto&a=serial.stats();const auto&b=parallel.stats();
 if(!services||!repeated||!a.temporal_next_changed_choices||!a.temporal_next_unavailable||a.flow_publications<2||
    a.temporal_next_known!=b.temporal_next_known||a.temporal_next_eligible!=b.temporal_next_eligible||
    a.temporal_next_arriving_choices!=b.temporal_next_arriving_choices||a.temporal_next_changed_choices!=b.temporal_next_changed_choices||
    a.oriented_builds!=b.oriented_builds||a.flow_cache_resets!=b.flow_cache_resets||a.oriented_guided!=b.oriented_guided||a.oriented_fallback!=b.oriented_fallback)
  throw std::runtime_error("next-errand publication/cache/lifecycle replay failed");
 for(const char* key:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TURN_FIRST","CGAR_TEMPORAL_STEPS","CGAR_TEMPORAL_EQUAL_WEIGHT","CGAR_TURN_TABLE_MB",
     "CGAR_TEMPORAL_TABLE_BATCH","CGAR_TEMPORAL_PREP_THREADS","CGAR_TURN_BUILD_LIMIT","CGAR_FLOW_STRENGTH","CGAR_FLOW_COST_SCALE","CGAR_FLOW_WARMUP",
     "CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL"})unsetenv(key);
 temporal_region_adapter_regression();temporal_primary_regression();unsetenv("CGAR_TEMPORAL_NEXT_ERRAND");
 std::cout<<"TEMPORAL_NEXT_ERRAND passed independent_weighted_scores="<<arriving<<" common_heading_baseline=1 joint_fixtures=2 native_fallback_cases=7 excluded_pocket_endpoint=1 own_cell_service=1 primary_preserved=1 metadata_immutable=1 service_events="<<services<<" repeated_location_services="<<repeated<<" serial_parallel_actions=320 changing_publications=1 explicit_lookup_failure=1 protected_regional_actions=4800\n";
}

void temporal_peak_audit_regression() {
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(49,true),7,7,[]{});
 const auto& paths=geometry.paths(24,0);int deep=-1,shallow=-1;
 for(int op=1;op<129;++op)if(paths[op].valid){if(paths[op].depth==5)deep=op;if(paths[op].depth==3)shallow=op;}
 if(deep<0||shallow<0)throw std::runtime_error("missing peak-audit geometric fixture");
 // Three physically valid choices have analytically known scores0,100,1000.
 // Horizon changes plus deliberately hot annealing can visit1000 and return100.
 std::vector<std::vector<TemporalChoice>> choices{{{&paths[0],1000,0},{&paths[deep],0,deep},{&paths[shallow],900,shallow}}};
 std::vector<char> fixed{false};std::vector<double> power{1};std::vector<int> initial{2};int lost=0;
 auto same_stats=[](const TemporalStats&a,const TemporalStats&b){return
  std::tie(a.roots,a.accepted,a.recursive_calls,a.candidates,a.budget_exhausted,a.repairs,a.repairs_accepted,a.repair_batches_kept,a.repair_batches_reverted,a.max_depth)==
  std::tie(b.roots,b.accepted,b.recursive_calls,b.candidates,b.budget_exhausted,b.repairs,b.repairs_accepted,b.repair_batches_kept,b.repair_batches_reverted,b.max_depth);};
 for(int seed=0;seed<64;++seed){
  TemporalPibt ordinary(49,choices,fixed,power,8192,seed,&initial),observed(49,choices,fixed,power,8192,seed,&initial);
  TemporalRepairAudit audit;ordinary.repair(300,[]{},0,nullptr,1000000);observed.repair(300,[]{},0,nullptr,1000000,&audit);
  if(!audit.completed||audit.attempts!=300||audit.initial_score!=100||audit.returned_score!=observed.score()||
     (audit.peak_score!=100&&audit.peak_score!=1000)||audit.peak_score<std::max(audit.initial_score,audit.final_score)||
     ordinary.selections()!=observed.selections()||ordinary.score()!=observed.score()||!same_stats(ordinary.stats,observed.stats))
   throw std::runtime_error("read-only repair maximum audit changed search or score accounting");
  TemporalRegionPeaks totals;totals.observe(audit);
  if(audit.peak_score==1000&&audit.final_score==100){++lost;
   if(totals.lost_peaks!=1||totals.lost_improvements!=1||totals.peak_gain!=900||totals.discarded_gain!=900||
      (audit.peak_attempt!=129&&audit.peak_attempt!=257))throw std::runtime_error("peak audit lost known transient improvement");}
  ordinary.repair(300,[]{},0,nullptr,1000000);observed.repair(300,[]{},0,nullptr,1000000);
  if(ordinary.selections()!=observed.selections()||ordinary.score()!=observed.score()||!same_stats(ordinary.stats,observed.stats))
   throw std::runtime_error("peak audit changed subsequent RNG trajectory");
 }
 if(!lost)throw std::runtime_error("transient-peak audit fixture was vacuous");
 TemporalPibt interrupted(49,choices,fixed,power,8192,0,&initial);TemporalRepairAudit unfinished;int checks=0;bool failed=false;
 try{interrupted.repair(300,[&]{if(++checks==5)throw Timeout("peak_audit_fixture");},0,nullptr,1000000,&unfinished);}
 catch(const Timeout&){failed=true;}
 if(!failed||unfinished.completed)throw std::runtime_error("peak audit authorized an incomplete repair");
 bool incomplete_rejected=false;try{TemporalRegionPeaks total;total.observe(unfinished);}catch(const std::logic_error&){incomplete_rejected=true;}
 if(!incomplete_rejected)throw std::runtime_error("regional audit aggregated incomplete work");
 for(const char* value:{"-1","2","1"}){
  setenv("CGAR_TEMPORAL_REGION_PEAK_AUDIT",value,1);setenv("CGAR_TEMPORAL_REGIONS",value[0]=='1'?"0":"4",1);
  SharedEnvironment e;e.rows=e.cols=1;e.map={0};e.num_of_agents=0;bool rejected=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("invalid/inactive regional peak audit accepted");
 }
 unsetenv("CGAR_TEMPORAL_REGIONS");setenv("CGAR_TEMPORAL_REGION_PEAK_AUDIT","1",1);
 temporal_region_adapter_regression();unsetenv("CGAR_TEMPORAL_REGION_PEAK_AUDIT");
 std::cout<<"TEMPORAL_PEAK_AUDIT passed known_transient_peaks="<<lost<<" independent_three_score_fixture=1 unchanged_continuations=64 no_partial_audit=1 protected_actions=4800\n";
}

void temporal_group_snapshot_regression() {
 TemporalGeometry geometry;geometry.initialize(std::vector<char>(10,true),2,5,[]{});
 auto operation=[&](std::array<uint8_t,5> actions){const auto& ops=TemporalGeometry::operations();
  for(int k=0;k<int(ops.size());++k)if(ops[k]==actions)return k;
  throw std::runtime_error("missing snapshot fixture operation");};
 const int one=operation({0,3,3,3,3}),two=operation({0,0,3,3,3}),east=operation({2,0,3,3,3});
 for(int blockers:{1,2}){
  std::vector<std::vector<TemporalChoice>> choices(4);std::vector<char> fixed(4,false);std::vector<double> power(4,1);
  for(int r=0;r<4;++r)choices[r].push_back({&geometry.paths(r,r?1:0)[0],r?0:1000,0});
  const int forward=blockers==1?one:two;
  choices[0].push_back({&geometry.paths(0,0)[forward],0,forward});
  for(int r=1;r<=blockers;++r){
   choices[r].push_back({&geometry.paths(r,1)[one],100,one});
   choices[r].push_back({&geometry.paths(r,1)[east],100,east});
  }
  TemporalPibt observed(10,choices,fixed,power,8192,123),control(10,choices,fixed,power,8192,123);
  auto groups=observed.snapshot_forward_groups(1,0,50,1,[]{});
  if(groups.size()!=1||groups[0].root!=0||groups[0].trigger!=1||groups[0].robots.size()!=size_t(blockers+1))
   throw std::runtime_error("snapshot omitted a hand-counted blocker group");
  for(int r=0;r<=blockers;++r){const auto& robot=groups[0].robots[r];
   if(robot.robot!=r||robot.selected!=0||robot.power!=1||robot.fixed||robot.choices.size()!=choices[r].size())
    throw std::runtime_error("snapshot altered robot metadata or omitted choices");
   for(int t=0;t<5;++t)if(robot.choices[0].cell_owners[t]!=r)
    throw std::runtime_error("snapshot lost current self reservation");
  }
  const auto& trigger=groups[0].robots[0].choices[1];
  if(trigger.cell_owners[0]!=1||(blockers==2&&trigger.cell_owners[1]!=2)||
     groups[0].robots.back().choices[2].cell_owners[1]!=blockers+1)
   throw std::runtime_error("snapshot confused repeated, multiple, or outside owners");
  const auto selections=observed.selections();const auto score=observed.score();
  groups[0].robots[0].choices[0].path.cells[0]=999;
  if(observed.choice(0).path->cells[0]!=0||observed.selections()!=selections||observed.score()!=score)
   throw std::runtime_error("snapshot copy aliases a live plan");
  bool timed_out=false;try{observed.snapshot_forward_groups(1,0,50,1,[]{throw Timeout("snapshot_fixture");});}
  catch(const Timeout&){timed_out=true;}
  if(!timed_out||observed.selections()!=selections||observed.score()!=score)
   throw std::runtime_error("snapshot timeout altered a complete plan");
  std::vector<int> order{0,1,2,3};observed.construct(order,[]{});control.construct(order,[]{});
  observed.repair(512,[]{});control.repair(512,[]{});
  if(observed.selections()!=control.selections()||observed.score()!=control.score()||
     observed.stats.candidates!=control.stats.candidates||observed.stats.repairs_accepted!=control.stats.repairs_accepted)
   throw std::runtime_error("snapshot changed subsequent random search");
  fixed[blockers]=true;TemporalPibt protected_group(10,choices,fixed,power,8192,123);
  if(!protected_group.snapshot_forward_groups(1,0,50,1,[]{}).empty())
   throw std::runtime_error("snapshot released a fixed blocker");
 }
 // Forward paths swap an edge at slot1 without occupying the same cell.
 const int delayed=operation({3,0,3,3,3});
 std::vector<std::vector<TemporalChoice>> choices(2);
 choices[0]={{&geometry.paths(0,0)[0],1000,0},{&geometry.paths(0,0)[two],0,two}};
 choices[1]={{&geometry.paths(2,2)[0],0,0},{&geometry.paths(2,2)[delayed],0,delayed}};
 std::vector<char> fixed(2,false);std::vector<double> power(2,1);std::vector<int> initial{0,1};
 TemporalPibt edge(10,choices,fixed,power,8192,123,&initial);auto groups=edge.snapshot_forward_groups(1,0,50,1,[]{});
 if(groups.size()!=1||groups[0].robots.size()!=2||groups[0].robots[0].choices[1].edge_owners[1]!=1)
  throw std::runtime_error("snapshot missed an edge-only conflict");
 for(int owner:groups[0].robots[0].choices[1].cell_owners)if(owner>=0)
  throw std::runtime_error("edge-only fixture unexpectedly has a cell owner");
 for(auto limits:std::vector<std::array<int,3>>{{0,50,1},{33,50,1},{1,0,1},{1,50,0}}){bool rejected=false;
  try{edge.snapshot_forward_groups(limits[0],0,limits[1],limits[2],[]{});}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("snapshot accepted invalid limits");}
 for(const char* value:{"-1","33","1"}){setenv("CGAR_TEMPORAL_GROUP_SNAPSHOT_COUNT",value,1);
  SharedEnvironment e;e.rows=e.cols=1;e.num_of_agents=0;e.map={0};bool rejected=false;
  try{Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("snapshot accepted invalid/non-temporal configuration");}
 setenv("CGAR_DIAGNOSTICS","1",1);setenv("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE","31",1);setenv("CGAR_TEMPORAL_GROUP_SNAPSHOT_COUNT","1",1);
 temporal_region_adapter_regression();
 unsetenv("CGAR_TEMPORAL_GROUP_SNAPSHOT_COUNT");unsetenv("CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE");unsetenv("CGAR_DIAGNOSTICS");
 std::cout<<"TEMPORAL_GROUP_SNAPSHOT passed single_two_and_edge_owners=1 outside_owners=1 all_choices=1 deep_copy=1 fixed_protection=1 immutable_rng=1 explicit_timeout=1 serial_parallel_actions=4800\n";
}

void city_game_trick_regression() {
 using namespace cgar::tricks;
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 auto rejects=[&](auto run,const char* message){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,message);};
 long long field_values=0,oriented_states=0,control_actions=0,native_actions=0;
 for(const std::string name:{"CITY-01","CITY-02","GAME"}) {
  const auto asset=field_asset(name);SharedEnvironment base;
  base.rows=asset.rows;base.cols=asset.cols;base.num_of_agents=name=="CITY-01"?1500:name=="CITY-02"?3000:6500;
  base.map_name="unrelated-name.map";std::vector<int> free;
  for(int cell=0;cell<asset.rows*asset.cols;++cell){base.map.push_back(asset.masks[cell]=='x');if(!base.map.back())free.push_back(cell);}
  require(free.size()==size_t(asset.free_cells),"CITY/GAME field occupancy count differs");
  validate_instance(name,base.map,base.rows,base.cols,base.num_of_agents);
  rejects([&]{validate_instance(name,base.map,base.rows,base.cols,base.num_of_agents+1);},"CITY/GAME accepted another fleet size");
  auto bad=base.map;bad[free[0]]=1;
  rejects([&]{validate_instance(name,bad,base.rows,base.cols,base.num_of_agents);},"CITY/GAME accepted changed occupancy");
  rejects([&]{validate_map(name,base.map,base.rows+1,base.cols);},"CITY/GAME accepted wrong dimensions");
  rejects([&]{validate_map(name=="GAME"?"CITY-01":"GAME",base.map,base.rows,base.cols);},"CITY/GAME accepted the wrong map");
  rejects([&]{native_forward_costs(name,base.map,base.rows,base.cols,true);},"CITY/GAME accepted unavailable band surcharge");
  rejects([&]{native_forward_costs(name,base.map,base.rows,base.cols,false,true);},"CITY/GAME accepted RANDOM uniform selector");
  setenv("CGAR_TRICK_NATIVE_BANDS","1",1);rejects([&]{options(name);},"CITY/GAME accepted band configuration");unsetenv("CGAR_TRICK_NATIVE_BANDS");
  setenv("CGAR_TRICK_LANES","0",1);rejects([&]{options("");},"CITY/GAME zero selector accepted without explicit CLI");unsetenv("CGAR_TRICK_LANES");
  const auto native=native_forward_costs(name,base.map,base.rows,base.cols,false);
  const auto adapted=forward_costs(name,base.map,base.rows,base.cols);
  require(validate_native_field(native,false,name)==asset.native_nobands_fnv1a64,"CITY/GAME native fingerprint differs");
  require(std::string(native_field_hash(false,name))==asset.native_nobands_sha256&&
          std::string(occupancy_hash(name))==asset.occupancy_sha256&&std::string(lane_field_hash(name))==asset.lane_sha256,
          "CITY/GAME receipt selected a different field");
  auto corrupt=native;corrupt[free[0]*4]^=1;
  rejects([&]{validate_native_field(corrupt,false,name);},"CITY/GAME accepted corrupt costs");
  for(int cell=0;cell<int(base.map.size());++cell)for(int h=0;h<4;++h){
   const int row=cell/base.cols,col=cell%base.cols;
   const bool against=h==(row%2?0:2)||h==(col%2?1:3);
   require(native[4*cell+h]==(base.map[cell]?20:against?200:20),"CITY/GAME native parity formula mismatch");
   require(adapted[4*cell+h]==(base.map[cell]?4:against?16:4),"CITY/GAME adapted parity formula mismatch");++field_values;
  }
  // One independent full oriented heap per distinct map. Disconnected free
  // components must remain unreachable in both implementations.
  if(name!="CITY-02"){
   Certificate cert;cert.rows=base.rows;cert.cols=base.cols;
   for(int wall:base.map)cert.free.push_back(!wall);cert.core=cert.free;cert.pocket.assign(base.map.size(),-1);
   const int goal=free[free.size()/2];std::vector<int> expected(base.map.size()*4,kInf);
   using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> heap;
   for(int h=0;h<4;++h){expected[goal*4+h]=0;heap.push({0,goal*4+h});}
   while(!heap.empty()){
    const auto item=heap.top();heap.pop();if(expected[item.second]!=item.first)continue;
    const int cell=item.second/4,h=item.second%4;
    auto relax=[&](int key,int price){if(item.first+price<expected[key]){expected[key]=item.first+price;heap.push({expected[key],key});}};
    relax(cell*4+(h+1)%4,1);relax(cell*4+(h+3)%4,1);
    const int before=nb(cell,(h+2)%4,base.rows,base.cols);if(before>=0&&!base.map[before])relax(before*4+h,native[before*4+h]);
   }
   TurnDistanceOracle oracle;oracle.init(&cert,16<<20,1,true,20,200);oracle.set_forward_costs(native);
   const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(30));
   for(int cell:free)for(int h=0;h<4;++h){require(oracle.value(*table,cell,h)==expected[cell*4+h],"CITY/GAME oriented oracle differs from independent heap");++oriented_states;}
  }
  const auto certificate=build_certificate_feasible(base.map,base.rows,base.cols,base.num_of_agents);
  std::vector<int> core;for(int cell:free)if(certificate.core[cell])core.push_back(cell);
  require(core.size()>size_t(base.num_of_agents+8),"CITY/GAME fixture lacks certified task cells");
  base.curr_task_schedule.assign(base.num_of_agents,-1);base.goal_locations.resize(base.num_of_agents);
  for(int r=0;r<base.num_of_agents;++r)base.curr_states.emplace_back(core[r],0,r%4);
  for(int t=0;t<4;++t){Task task;task.task_id=t;task.t_revealed=0;task.locations={core[base.num_of_agents+t],core[t]};base.task_pool.emplace(t,task);}
  const std::vector<std::pair<const char*,const char*>> settings={{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","8"},
   {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","4"},{"CGAR_FLOW_COST_SCALE","4"},
   {"CGAR_FLOW_WARMUP","1"},{"CGAR_FLOW_MIN_SAMPLES","1"},{"CGAR_FLOW_REFRESH_INTERVAL","1"},
   {"CGAR_PICKUP_FLOW","1"},{"CGAR_PICKUP_FULL_ROBOTS","1"},{"CGAR_TURN_COMPACT","1"},{"CGAR_TURN_TABLE_MB","64"},{"CGAR_TURN_BUILD_LIMIT","8"}};
  for(auto setting:settings)setenv(setting.first,setting.second,1);
  auto install=[&](SharedEnvironment& e,const std::vector<int>& schedule){
   e.curr_task_schedule=schedule;
   for(int r=0;r<e.num_of_agents;++r)if(schedule[r]>=0){auto& task=e.task_pool.at(schedule[r]);task.agent_assigned=r;e.goal_locations[r]={{task.locations.front(),e.curr_timestep}};}
  };
  {
   auto plain=base,control=base;control.trick_instance=name;
   Cgar generic;generic.initialize(&plain,30000);
   setenv("CGAR_TRICK_LANES","0",1);Cgar ablation;ablation.initialize(&control,30000);unsetenv("CGAR_TRICK_LANES");
   for(int tick=0;tick<2;++tick){
    plain.curr_timestep=control.curr_timestep=tick;
    std::vector<int>a,b;generic.schedule(&plain,30000,a);ablation.schedule(&control,30000,b);
    require(a==b,"CITY/GAME lanes-off control changed scheduling");install(plain,a);install(control,b);
    std::vector<Action>x,y;generic.plan(&plain,30000,x);ablation.plan(&control,30000,y);
    require(x==y,"CITY/GAME lanes-off control changed actions");
    auto next=step(plain,plain.curr_states,x);require(next.size()==size_t(base.num_of_agents),"CITY/GAME generic control collision");
    plain.curr_states=control.curr_states=next;control_actions+=x.size();
   }
  }
  {
   setenv("CGAR_FLOW_COST_SCALE","20",1);setenv("CGAR_TRICK_LANES","1",1);setenv("CGAR_TRICK_NATIVE_METRIC","1",1);setenv("CGAR_TRICK_REMAINING_FLOW","1",1);
   auto active=base;active.trick_instance=name;Cgar planner;planner.initialize(&active,30000);
   std::vector<int> schedule;planner.schedule(&active,30000,schedule);require(planner.stats().pickup_full_fields>0,"CITY/GAME native pickup metric inactive");install(active,schedule);
   std::vector<Action> actions;planner.plan(&active,30000,actions);
   require(step(active,active.curr_states,actions).size()==size_t(base.num_of_agents)&&!planner.stats().flow_publications,"CITY/GAME native action invalid or field overwritten");native_actions+=actions.size();
   for(const char* key:{"CGAR_TRICK_LANES","CGAR_TRICK_NATIVE_METRIC","CGAR_TRICK_REMAINING_FLOW"})unsetenv(key);
  }
  for(auto setting:settings)unsetenv(setting.first);
 }
 std::cout<<"CITY_GAME_TRICK passed independent_field_values="<<field_values<<" independent_oriented_states="<<oriented_states
  <<" explicit_map_fleet_gate=3 generic_control_actions="<<control_actions<<" native_pickup_and_plan_actions="<<native_actions<<"\n";
}

void random_trick_regression() {
 using namespace cgar::tricks;
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 auto rejects=[&](auto run,const char* message){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,message);};
 SharedEnvironment base;base.rows=random_rows;base.cols=random_cols;base.map_name="unrelated-name.map";
 std::vector<int> free;
 for(int cell=0;cell<1024;++cell){base.map.push_back(random_masks[cell]=='x');if(!base.map.back())free.push_back(cell);}
 require(free.size()==819,"RANDOM asset lost a free cell");
 for(const std::string name:{"RANDOM-04","RANDOM-05"}) {
  const int n=name=="RANDOM-04"?700:800;
  validate_instance(name,base.map,32,32,n);
  rejects([&]{validate_instance(name,base.map,32,32,n==700?800:700);},"RANDOM trick accepted the other team size");
  auto bad=base.map;bad[free[0]]=1;
  rejects([&]{validate_instance(name,bad,32,32,n);},"RANDOM trick accepted modified occupancy");
  rejects([&]{validate_instance(name,base.map,16,64,n);},"RANDOM trick accepted altered dimensions");
  rejects([&]{forward_costs(name,base.map,32,32);},"RANDOM trick accepted legacy lane mask");
  rejects([&]{native_forward_costs(name,base.map,32,32,true);},"RANDOM trick accepted native bands");
 }
 setenv("CGAR_TRICK_RANDOM_UNIFORM","0",1);
 rejects([&]{options("");},"RANDOM zero selector silently activated without CLI");
 rejects([&]{options("WAREHOUSE");},"RANDOM selector accepted on Warehouse");
 rejects([&]{options("RANDOM-04");},"RANDOM uniform accepted without native metric");
 unsetenv("CGAR_TRICK_RANDOM_UNIFORM");
 const std::vector<std::pair<const char*,const char*>> native_options={{"CGAR_TRICK_LANES","1"},
  {"CGAR_TRICK_REMAINING_FLOW","1"},{"CGAR_TRICK_NATIVE_METRIC","1"},{"CGAR_TRICK_NATIVE_TURN_COST","6"}};
 for(auto setting:native_options)setenv(setting.first,setting.second,1);
 for(const char* value:{"","-1","2","true"}){setenv("CGAR_TRICK_RANDOM_UNIFORM",value,1);rejects([&]{options("RANDOM-04");},"RANDOM uniform malformed boolean accepted");}
 unsetenv("CGAR_TRICK_RANDOM_UNIFORM");
 require(!options("RANDOM-04").random_uniform,"RANDOM field default changed to uniform");
 setenv("CGAR_TRICK_NATIVE_BANDS","1",1);rejects([&]{options("RANDOM-04");},"RANDOM bands selector accepted");unsetenv("CGAR_TRICK_NATIVE_BANDS");
 for(auto setting:native_options)unsetenv(setting.first);
 Certificate cert;cert.rows=cert.cols=32;for(int wall:base.map)cert.free.push_back(!wall);cert.core=cert.free;cert.pocket.assign(1024,-1);
 long long compared=0;
 for(bool uniform:{false,true}) {
  const auto weights=native_forward_costs("RANDOM-04",base.map,32,32,false,uniform);
  require(weights==native_forward_costs("RANDOM-05",base.map,32,32,false,uniform),"RANDOM instances loaded different fields");
  validate_native_field(weights,false,"RANDOM-04",uniform);
  auto corrupt=weights;corrupt[0]^=1;
  rejects([&]{validate_native_field(corrupt,false,"RANDOM-05",uniform);},"RANDOM corrupted field accepted");
  rejects([&]{validate_native_field(weights,false,"RANDOM-04",!uniform);},"RANDOM field confused with uniform control");
  if(uniform)require(std::all_of(weights.begin(),weights.end(),[](int c){return c==20;}),"uniform control is not uniform");
  else require(*std::min_element(weights.begin(),weights.end())==5&&*std::max_element(weights.begin(),weights.end())==43,"RANDOM quantized bounds differ");
  for(int goal:{free.front(),free[400],free.back()}) {
   // Independent reverse heap search includes all oriented states and costs
   // below the nominal normalization unit 20.
   std::vector<int> expected(4096,kInf);
   using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> heap;
   for(int h=0;h<4;++h){expected[goal*4+h]=0;heap.push({0,goal*4+h});}
   while(!heap.empty()) {
    const auto item=heap.top();heap.pop();const int distance=item.first,key=item.second;
    if(expected[key]!=distance)continue;
    const int cell=key/4,h=key%4;
    auto relax=[&](int next,int price){if(distance+price<expected[next]){expected[next]=distance+price;heap.push({distance+price,next});}};
    relax(cell*4+(h+1)%4,6);relax(cell*4+(h+3)%4,6);
    const int previous=nb(cell,(h+2)%4,32,32);
    if(previous>=0&&!base.map[previous])relax(previous*4+h,weights[previous*4+h]);
   }
   for(bool compact:{false,true}) {
    TurnDistanceOracle oracle;oracle.init(&cert,16<<20,6,compact,1,43);oracle.set_forward_costs(weights);
    require(oracle.weighted_forward(),"RANDOM uniform control lost explicit edge costs");
    const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(30));
    for(int cell:free)for(int h=0;h<4;++h){require(oracle.value(*table,cell,h)==expected[cell*4+h],"RANDOM oriented table differs from independent heap");++compared;}
   }
  }
 }
 const std::vector<std::pair<const char*,const char*>> settings={{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","8"},
  {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","4"},{"CGAR_FLOW_COST_SCALE","4"},
  {"CGAR_FLOW_WARMUP","2"},{"CGAR_FLOW_MIN_SAMPLES","1"},{"CGAR_FLOW_REFRESH_INTERVAL","2"},
  {"CGAR_PICKUP_FLOW","1"},{"CGAR_PICKUP_FULL_ROBOTS","1"},{"CGAR_TURN_COMPACT","1"}};
 for(auto setting:settings)setenv(setting.first,setting.second,1);
 int actions_checked=0;
 for(const std::string name:{"RANDOM-04","RANDOM-05"}) {
  auto e=base;e.num_of_agents=name=="RANDOM-04"?700:800;
  e.curr_task_schedule.assign(e.num_of_agents,-1);e.goal_locations.resize(e.num_of_agents);
  for(int r=0;r<e.num_of_agents;++r)e.curr_states.push_back(State(free[r],0,r%4));
  // Capacity mode intentionally accepts only chains entirely in the certified
  // core. Use eligible tasks so both 700- and 800-robot fixtures exercise the
  // actual pickup metric rather than the empty-task admission path.
  const auto fixture_cert=build_certificate_feasible(e.map,e.rows,e.cols,e.num_of_agents);
  std::vector<int> core;for(int cell:free)if(fixture_cert.core[cell])core.push_back(cell);
  require(core.size()>8,"RANDOM fixture core too small");
  for(int t=0;t<4;++t){Task task;task.task_id=t;task.t_revealed=0;task.locations={core[core.size()-1-t],core[t]};e.task_pool.emplace(t,task);}
  auto plain=e,control=e;control.trick_instance=name;
  Cgar generic;generic.initialize(&plain,30000);
  setenv("CGAR_TRICK_LANES","0",1);Cgar ablation;ablation.initialize(&control,30000);unsetenv("CGAR_TRICK_LANES");
  for(int tick=0;tick<4;++tick) {
   plain.curr_timestep=control.curr_timestep=tick;
   std::vector<int> a,b;generic.schedule(&plain,30000,a);ablation.schedule(&control,30000,b);
   require(a==b,"RANDOM lanes-off control changed schedule");plain.curr_task_schedule=control.curr_task_schedule=a;
   for(int r=0;r<e.num_of_agents;++r)if(a[r]>=0){const int goal=plain.task_pool.at(a[r]).locations.front();plain.goal_locations[r]=control.goal_locations[r]={{goal,tick}};plain.task_pool.at(a[r]).agent_assigned=control.task_pool.at(a[r]).agent_assigned=r;}
   std::vector<Action> x,y;generic.plan(&plain,30000,x);ablation.plan(&control,30000,y);
   require(x==y,"RANDOM lanes-off control changed actions");
   auto next=step(plain,plain.curr_states,x);require(next.size()==size_t(e.num_of_agents),"RANDOM generic fixture collision");
   plain.curr_states=control.curr_states=next;actions_checked+=x.size();
  }
  setenv("CGAR_FLOW_COST_SCALE","20",1);for(auto setting:native_options)setenv(setting.first,setting.second,1);
  for(const char* uniform:{"0","1"}) {
   setenv("CGAR_TRICK_RANDOM_UNIFORM",uniform,1);auto active=e;active.trick_instance=name;active.curr_timestep=10;
   Cgar planner;planner.initialize(&active,30000);std::vector<int> schedule;planner.schedule(&active,30000,schedule);
   require(planner.stats().pickup_full_fields>0,"RANDOM static pickup field inactive");active.curr_task_schedule=schedule;
   for(int r=0;r<e.num_of_agents;++r)if(schedule[r]>=0){active.goal_locations[r]={{active.task_pool.at(schedule[r]).locations.front(),10}};active.task_pool.at(schedule[r]).agent_assigned=r;}
   std::vector<Action> actions;planner.plan(&active,30000,actions);
   require(step(active,active.curr_states,actions).size()==size_t(e.num_of_agents)&&!planner.stats().flow_publications,"RANDOM native actions invalid or static field overwritten");
  }
  unsetenv("CGAR_TRICK_RANDOM_UNIFORM");for(auto setting:native_options)unsetenv(setting.first);setenv("CGAR_FLOW_COST_SCALE","4",1);
 }
 for(auto setting:settings)unsetenv(setting.first);
 std::cout<<"RANDOM_TRICK passed independent_oriented_states="<<compared<<" generic_control_actions="<<actions_checked
  <<" exact_map_and_team=1 explicit_activation=1 uniform_control=1 corrupt_field_rejected=1 native_pickup_and_plan=4\n";
}

void sortation_trick_regression() {
 using namespace cgar::tricks;
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 auto rejects=[&](auto run,const char* message){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,message);};
 SharedEnvironment e;e.rows=sortation_rows;e.cols=sortation_cols;e.num_of_agents=1;
 for(int cell=0;cell<e.rows*e.cols;++cell)e.map.push_back(sortation_masks[cell]=='x');
 validate_name("SORTATION");validate_map("SORTATION",e.map,e.rows,e.cols);
 for(const std::string name:{"sortation","SORTATION-other","RANDOM-06",""})
  rejects([&]{validate_name(name);},"Sortation provider accepted an unknown trick name");
 auto bad=e.map;bad[0]=!bad[0];
 rejects([&]{validate_map("SORTATION",bad,e.rows,e.cols);},"Sortation accepted modified occupancy");
 rejects([&]{validate_map("SORTATION",e.map,e.rows+1,e.cols);},"Sortation accepted wrong dimensions");
 bad=e.map;bad.pop_back();
 rejects([&]{validate_map("SORTATION",bad,e.rows,e.cols);},"Sortation accepted truncated occupancy");
 std::vector<int> warehouse;for(int cell=0;cell<warehouse_rows*warehouse_cols;++cell)warehouse.push_back(warehouse_masks[cell]=='x');
 rejects([&]{validate_map("SORTATION",warehouse,e.rows,e.cols);},"Sortation accepted the same-size Warehouse map");
 rejects([&]{validate_map("WAREHOUSE",e.map,e.rows,e.cols);},"Warehouse accepted the same-size Sortation map");
 require(std::string(occupancy_hash("SORTATION"))==sortation_occupancy_sha256&&
  std::string(lane_field_hash("SORTATION"))==sortation_field_sha256,"Sortation receipt uses a different asset");
 const auto adapted=forward_costs("SORTATION",e.map,e.rows,e.cols);
 Certificate cert;cert.rows=e.rows;cert.cols=e.cols;
 for(int wall:e.map)cert.free.push_back(!wall);cert.core=cert.free;cert.pocket.assign(e.map.size(),-1);
 const int goal=std::find(e.map.begin()+e.map.size()/2,e.map.end(),0)-e.map.begin();
 require(goal<int(e.map.size()),"Sortation fixture has no central goal");
 long long field_values=0,oriented_states=0;
 for(bool bands:{false,true}) {
  const auto native=native_forward_costs("SORTATION",e.map,e.rows,e.cols,bands);
  require(validate_native_field(native,bands,"SORTATION")==
   (bands?sortation_native_bands_fnv1a64:sortation_native_nobands_fnv1a64),"Sortation native fingerprint differs");
  require(std::string(native_field_hash(bands,"SORTATION"))==
   (bands?sortation_native_bands_field_sha256:sortation_native_nobands_field_sha256),"Sortation native receipt differs");
  rejects([&]{validate_native_field(native,bands);},"Sortation field passed Warehouse fingerprint check");
  auto corrupt=native;corrupt[goal*4]^=1;
  rejects([&]{validate_native_field(corrupt,bands,"SORTATION");},"Sortation accepted corrupt native costs");
  rejects([&]{validate_native_field(std::vector<uint8_t>{20},bands,"SORTATION");},"Sortation accepted truncated native costs");
  for(int cell=0;cell<int(e.map.size());++cell)for(int h=0;h<4;++h) {
   // Independent formulas from the archived NMS constructor, without decoding
   // either generated mask. Wall entries are deliberately normalized.
   const int row=cell/e.cols,col=cell%e.cols;
   const int horizontal=((0b1001>>(row%4))&1)?0:2;
   const int vertical=((0b1100>>(col%4))&1)?1:3;
   const bool against=h==horizontal||h==vertical;
   const int extra=bands&&(row<=8||row>=130);
   require(adapted[cell*4+h]==(e.map[cell]?4:(against?16:4)),"Sortation adapted formula mismatch");
   require(native[cell*4+h]==(e.map[cell]?20:(against?200:20)+extra),"Sortation native formula mismatch");
   ++field_values;
  }
  // An independent heap search checks propagation through the new field in
  // the actual reverse oriented distance oracle, including outer bands.
  std::vector<int> expected(e.map.size()*4,kInf);
  using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> heap;
  for(int h=0;h<4;++h){expected[goal*4+h]=0;heap.push({0,goal*4+h});}
  while(!heap.empty()) {
   const auto item=heap.top();heap.pop();const int distance=item.first,key=item.second;
   if(expected[key]!=distance)continue;
   const int cell=key/4,h=key%4;
   auto relax=[&](int next,int price){if(distance+price<expected[next]){expected[next]=distance+price;heap.push({distance+price,next});}};
   relax(cell*4+(h+1)%4,1);relax(cell*4+(h+3)%4,1);
   const int previous=nb(cell,(h+2)%4,e.rows,e.cols);
   if(previous>=0&&!e.map[previous])relax(previous*4+h,native[previous*4+h]);
  }
  TurnDistanceOracle oracle;oracle.init(&cert,16<<20,1,true,20,bands?201:200);oracle.set_forward_costs(native);
  const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(30));
  for(int cell=0;cell<int(e.map.size());++cell)if(!e.map[cell])for(int h=0;h<4;++h) {
   require(oracle.value(*table,cell,h)==expected[cell*4+h],"Sortation oriented distance differs from heap oracle");++oriented_states;
  }
 }
 int start=-1;for(int cell=0;cell+12<int(e.map.size());++cell)if(cell/e.cols==(cell+11)/e.cols) {
  bool clear=true;for(int k=0;k<12;++k)clear=clear&&!e.map[cell+k];if(clear){start=cell;break;}
 }
 require(start>=0,"Sortation fixture lacks a free segment");
 e.map_name="sortation_large.map";e.curr_states={State(start,0,0)};e.curr_task_schedule={-1};e.goal_locations={{}};
 Task task;task.task_id=0;task.t_revealed=0;task.locations={start+11};e.task_pool.emplace(0,task);
 const std::vector<std::pair<const char*,const char*>> settings={{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","128"},
  {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","4"},{"CGAR_FLOW_COST_SCALE","4"},
  {"CGAR_FLOW_WARMUP","2"},{"CGAR_FLOW_MIN_SAMPLES","1"},{"CGAR_FLOW_REFRESH_INTERVAL","2"},
  {"CGAR_PICKUP_FLOW","1"},{"CGAR_PICKUP_FULL_ROBOTS","1"}};
 for(auto setting:settings)setenv(setting.first,setting.second,1);
 auto plain=e,control=e;control.trick_instance="SORTATION";
 Cgar generic;generic.initialize(&plain,30000);
 setenv("CGAR_TRICK_LANES","0",1);
 rejects([&]{options("");},"zero-valued Sortation selector accepted without CLI");
 Cgar ablation;ablation.initialize(&control,30000);unsetenv("CGAR_TRICK_LANES");
 for(int tick=0;tick<8;++tick) {
  plain.curr_timestep=control.curr_timestep=tick;
  std::vector<int> a,b;generic.schedule(&plain,30000,a);ablation.schedule(&control,30000,b);
  require(a==b,"Sortation lanes-off control changed generic scheduling");
  plain.curr_task_schedule=control.curr_task_schedule=a;
  plain.goal_locations=control.goal_locations={{{start+11,tick}}};
  plain.task_pool.at(0).agent_assigned=control.task_pool.at(0).agent_assigned=0;
  std::vector<Action> x,y;generic.plan(&plain,30000,x);ablation.plan(&control,30000,y);
  require(x==y,"Sortation lanes-off control changed generic actions");
  const auto next=step(plain,plain.curr_states,x);require(!next.empty(),"Sortation generic control has invalid actions");
  plain.curr_states=control.curr_states=next;
 }
 require(generic.stats().flow_publications==ablation.stats().flow_publications,"Sortation control changed learned publications");
 setenv("CGAR_FLOW_COST_SCALE","20",1);setenv("CGAR_TRICK_LANES","1",1);
 setenv("CGAR_TRICK_REMAINING_FLOW","1",1);setenv("CGAR_TRICK_NATIVE_METRIC","1",1);
 for(const char* bands:{"0","1"}) {
  setenv("CGAR_TRICK_NATIVE_BANDS",bands,1);auto active=e;active.trick_instance="SORTATION";active.curr_timestep=10;
  Cgar planner;planner.initialize(&active,30000);std::vector<int> schedule;planner.schedule(&active,30000,schedule);
  require(schedule==std::vector<int>{0}&&planner.stats().pickup_full_fields==1,"Sortation native pickup field inactive");
  active.curr_task_schedule=schedule;active.goal_locations={{{start+11,10}}};active.task_pool.at(0).agent_assigned=0;
  std::vector<Action> actions;planner.plan(&active,30000,actions);
  require(!step(active,active.curr_states,actions).empty()&&!planner.stats().flow_publications,"Sortation native integration invalid or overwritten");
 }
 for(auto setting:settings)unsetenv(setting.first);
 for(const char* key:{"CGAR_TRICK_LANES","CGAR_TRICK_REMAINING_FLOW","CGAR_TRICK_NATIVE_METRIC","CGAR_TRICK_NATIVE_BANDS"})unsetenv(key);
 std::cout<<"SORTATION_TRICK passed independent_field_values="<<field_values<<" independent_oriented_states="<<oriented_states
  <<" explicit_activation=1 cross_map_rejection=1 generic_control_actions=8 native_pickup_and_plan=2\n";
}

void warehouse_trick_regression() {
 using namespace cgar::tricks;
 for(const auto& name:std::vector<std::string>{"", "warehouse", "RANDOM-01", "WAREHOUSE-other"}) {
  bool rejected=false;try{validate_name(name);}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("unknown trick name accepted");
 }
 SharedEnvironment e;e.rows=warehouse_rows;e.cols=warehouse_cols;e.num_of_agents=1;
 for(int i=0;i<e.rows*e.cols;++i)e.map.push_back(warehouse_masks[i]=='x');
 validate_map("WAREHOUSE",e.map,e.rows,e.cols);
 auto bad=e.map;bad[0]=!bad[0];bool rejected=false;
 try{validate_map("WAREHOUSE",bad,e.rows,e.cols);}catch(const std::invalid_argument&){rejected=true;}
 if(!rejected)throw std::runtime_error("altered trick layout accepted");
 rejected=false;try{validate_map("WAREHOUSE",e.map,e.rows+1,e.cols);}catch(const std::invalid_argument&){rejected=true;}
 if(!rejected)throw std::runtime_error("wrong trick dimensions accepted");
 const auto costs=forward_costs("WAREHOUSE",e.map,e.rows,e.cols);
 Certificate cert;cert.rows=e.rows;cert.cols=e.cols;
 for(int wall:e.map)cert.free.push_back(!wall);
 cert.core=cert.free;cert.pocket.assign(e.map.size(),-1);
 const int goal=std::find(e.map.begin(),e.map.end(),0)-e.map.begin();
 TurnDistanceOracle oracle;oracle.init(&cert,16<<20,4,true,4);oracle.set_forward_costs(costs);
 const auto* table=oracle.table(goal,std::chrono::steady_clock::now()+std::chrono::seconds(30));
 // Independent priority-queue shortest paths on the full oriented warehouse.
 std::vector<int> expected(e.map.size()*4,kInf);
 using Node=std::pair<int,int>;std::priority_queue<Node,std::vector<Node>,std::greater<Node>> q;
 for(int d=0;d<4;++d){expected[goal*4+d]=0;q.push({0,goal*4+d});}
 while(!q.empty()){
  const auto item=q.top();q.pop();const int dist=item.first,key=item.second;
  if(expected[key]!=dist)continue;
  const int cell=key/4,d=key%4;
  auto relax=[&](int next,int price){if(dist+price<expected[next]){expected[next]=dist+price;q.push({dist+price,next});}};
  relax(cell*4+(d+1)%4,4);relax(cell*4+(d+3)%4,4);
  const int previous=nb(cell,(d+2)%4,e.rows,e.cols);
  if(previous>=0&&!e.map[previous])relax(previous*4+d,costs[previous*4+d]);
 }
 int compared=0;
 for(int cell=0;cell<int(e.map.size());++cell)if(!e.map[cell])for(int d=0;d<4;++d){
  if(oracle.value(*table,cell,d)!=expected[cell*4+d])throw std::runtime_error("warehouse trick weighted distance mismatch");
  ++compared;
 }
 // Independently replay macros under the actual static field. This checks
 // the remaining-potential score against the priority-queue oracle above and
 // witnesses changed rankings; an aggregate contraflow rate alone cannot prove
 // whether the scoring switch is active.
 long long static_scores=0,static_changed=0,static_neutral=0;
 {
  TemporalGeometry geometry;geometry.initialize(cert.free,e.rows,e.cols,[]{});
  for(int cell=goal;cell<int(e.map.size());cell+=97)if(!e.map[cell])for(int heading=0;heading<4;++heading){
   int best[2]={-1,-1};int64_t minimum[2]={INT64_MAX,INT64_MAX};
   for(int op=0;op<129;++op){const auto& path=geometry.paths(cell,heading)[op];if(!path.valid)continue;
    const auto& actions=TemporalGeometry::operations()[op];int at=cell,h=heading,paid=0,last=-1;bool served=false;
    for(int t=0;t<5;++t){int action=actions[t];
     if(action==0){if(!served)paid+=costs[at*4+h]-4;at=nb(at,h,e.rows,e.cols);}
     else if(action==1)h=(h+1)%4;else if(action==2)h=(h+3)%4;
     if(at==goal){served=true;last=t;}
    }
    int tail=expected[at*4+h];
    if(actions[4]==3){tail=std::min({tail,expected[at*4+(h+1)%4],expected[at*4+(h+3)%4]});
     if(actions[3]==3)tail=std::min(tail,expected[at*4+(h+2)%4]);}
    const int64_t pure=int64_t(last<0?tail:-last*4)*50-int64_t(op)*4;
    for(int mode=0;mode<2;++mode){
     const auto value=TemporalGeometry::flow_cost(path,op,cell,goal,4,
      [&](int c,int d){return oracle.value(*table,c,d);},
      [&](int from,int to){return costs[from*4+(to==from+1?0:to==from+e.cols?1:to==from-1?2:3)];},50,4,mode!=0);
     if(value!=pure+(mode?0:int64_t(paid)*50))throw std::runtime_error("static lane score differs from independent replay");
     if(value<minimum[mode]){minimum[mode]=value;best[mode]=op;}
    }
    static_neutral+=paid==0;++static_scores;
   }
   static_changed+=best[0]!=best[1];
  }
 }
 if(!static_scores||!static_neutral||!static_changed)throw std::runtime_error("static lane scoring fixture was vacuous");
 // Short integrated lifecycle: the static field is available for scheduling
 // after the generic mass dispatch and cannot be overwritten by publications.
 int start=goal;
 for(int cell=goal+1;cell<int(e.map.size());++cell)if(!e.map[cell]){start=cell;break;}
 e.curr_states={State(start,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);
 Task task;task.task_id=0;task.locations={goal};e.task_pool.emplace(0,task);
 e.map_name="warehouse_large.map";
 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_TEMPORAL_STEPS","0",1);setenv("CGAR_FLOW_STRENGTH","4",1);
 setenv("CGAR_FLOW_COST_SCALE","4",1);setenv("CGAR_PICKUP_FLOW","1",1);
 setenv("CGAR_FLOW_WARMUP","2",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","2",1);
 Cgar generic;generic.initialize(&e,30000);
 auto flagged=e;flagged.trick_instance="WAREHOUSE";
 setenv("CGAR_TEMPORAL_REMAINING_FLOW","1",1);rejected=false;
 try{Cgar invalid;invalid.initialize(&flagged,30000);}catch(const std::invalid_argument& error){
  rejected=std::string(error.what()).find("remaining-flow scoring")!=std::string::npos;}
 unsetenv("CGAR_TEMPORAL_REMAINING_FLOW");
 if(!rejected)throw std::runtime_error("generic remaining-flow experiment accepted a static trick");
 Cgar trick;trick.initialize(&flagged,30000);
 std::vector<int> schedule;std::vector<Action> actions;
 std::vector<int> initial_generic;
 generic.schedule(&e,30000,initial_generic);trick.schedule(&flagged,30000,schedule);
 if(schedule!=initial_generic||trick.stats().pickup_flow_warmup_calls!=1)
  throw std::runtime_error("static trick changed generic initial dispatch");
 flagged.curr_timestep=1;trick.schedule(&flagged,30000,schedule);
 if(trick.stats().pickup_flow_warmup_calls!=1)
  throw std::runtime_error("static pickup metric not activated after initial dispatch");
 for(int t=0;t<8;++t){
  e.curr_timestep=flagged.curr_timestep=t;
  generic.plan(&e,30000,actions);trick.plan(&flagged,30000,actions);
 }
 if(trick.stats().flow_publications||trick.stats().flow_cache_resets||generic.stats().flow_publications==0)
  throw std::runtime_error("static trick activation or publication isolation failed");
 // Explicit matching uses the static metric without waiting forever for a
 // learned-flow publication. The primary remains protected in this real pass.
 setenv("CGAR_TRICK_UNOPENED_MATCH","1",1);
 auto matching=flagged;matching.curr_timestep=0;matching.curr_task_schedule={0};
 matching.goal_locations={{{goal,0}}};matching.task_pool.at(0).agent_assigned=0;
 Cgar matching_policy;matching_policy.initialize(&matching,30000);matching_policy.plan(&matching,30000,actions);
 matching.curr_timestep=10;matching_policy.schedule(&matching,30000,schedule);
 if(schedule!=std::vector<int>{0}||matching_policy.stats().flow_publications||
    matching_policy.stats().match_passes!=1||matching_policy.stats().match_primary_protected!=1)
  throw std::runtime_error("static trick matching did not activate with primary protection");
 setenv("CGAR_REASSIGN_MATCH","1",1);rejected=false;
 try{Cgar invalid;invalid.initialize(&matching,30000);}catch(const std::invalid_argument&){rejected=true;}
 if(!rejected)throw std::runtime_error("generic matching selector bypassed explicit trick component");
 unsetenv("CGAR_REASSIGN_MATCH");unsetenv("CGAR_TRICK_UNOPENED_MATCH");
 setenv("CGAR_TRICK_REMAINING_FLOW","1",1);
 {
  auto potential=matching;potential.curr_timestep=0;Cgar policy;policy.initialize(&potential,30000);
  if(!options("WAREHOUSE").remaining_flow)throw std::runtime_error("explicit static remaining-flow selector lost");
  policy.plan(&potential,30000,actions);
  if(step(potential,potential.curr_states,actions).empty()||policy.stats().flow_publications)
   throw std::runtime_error("static remaining-flow integration invalid or published learned costs");
 }
 for(const auto& bad:std::vector<std::pair<const char*,const char*>>{{"CGAR_TRICK_LANES","0"},
     {"CGAR_TEMPORAL_NEXT_ERRAND","1"},
     {"CGAR_TEMPORAL_CONFLICT_AUDIT_STRIDE","1"},{"CGAR_TEMPORAL_BRANCH_WORK","1"}}){
  setenv(bad.first,bad.second,1);rejected=false;
  try{Cgar invalid;invalid.initialize(&flagged,30000);}catch(const std::invalid_argument&){rejected=true;}
  unsetenv(bad.first);if(!rejected)throw std::runtime_error("static remaining-flow accepted incompatible component");
 }
 // Static remaining-potential scoring and bounded matching share the same
 // immutable oriented metric. Exercise a real beneficial cycle in both scores,
 // including primary protection, task metadata and the one-retarget budget.
 setenv("CGAR_TRICK_UNOPENED_MATCH","1",1);
 int base=-1;
 for(int cell=0;cell<int(e.map.size());++cell)if(cell%e.cols+12<=e.cols){
  bool free=true;for(int k=0;k<12;++k)free=free&&!e.map[cell+k];
  if(free){base=cell;break;}
 }
 if(base<0)throw std::runtime_error("static matching fixture lacks free segment");
 for(int score:{0,1}){
  setenv("CGAR_TRICK_REMAINING_FLOW",score?"1":"0",1);
  auto test=flagged;test.curr_timestep=0;test.num_of_agents=3;
  test.curr_states={State(base,0,0),State(base+11,0,2),State(base+1,0,0)};
  test.curr_task_schedule={0,1,2};test.goal_locations={{{base+10,0}},{{base+1,0}},{{base+11,0}}};test.task_pool.clear();
  for(int i=0;i<3;++i){Task task;task.task_id=i;task.t_revealed=-5;task.agent_assigned=i;
   task.locations={i==0?base+10:(i==1?base+1:base+11)};test.task_pool.emplace(i,task);}
  Cgar policy;policy.initialize(&test,30000);
  for(int t=0;t<2;++t){test.curr_timestep=t;policy.plan(&test,30000,actions);
   auto next=step(test,test.curr_states,actions);
   if(next.empty())throw std::runtime_error("static score/matching setup collided");test.curr_states=next;}
  test.curr_timestep=10;policy.schedule(&test,30000,schedule);
  if(schedule!=std::vector<int>({0,2,1})||policy.stats().match_moved!=2||
     policy.stats().match_accepted_cycles!=1||policy.stats().match_saving<8||
     policy.stats().match_primary_protected!=1||policy.stats().flow_publications||
     policy.stats().match_groups>4||policy.stats().match_nodes>8192)
   throw std::runtime_error("static score/matching failed protected beneficial cycle: score="+std::to_string(score)+
    " moved="+std::to_string(policy.stats().match_moved)+" primary="+std::to_string(policy.stats().match_primary_protected));
  for(int i=0;i<3;++i)if(test.curr_task_schedule[i]!=i||test.task_pool.at(i).agent_assigned!=i||
      test.task_pool.at(i).t_revealed!=-5||test.task_pool.at(i).idx_next_loc)
   throw std::runtime_error("static score/matching changed simulator metadata");
  test.curr_task_schedule=schedule;
  for(int i=0;i<3;++i){auto& task=test.task_pool.at(schedule[i]);task.agent_assigned=i;test.goal_locations[i]={{{task.locations[0],10}}};}
  policy.plan(&test,30000,actions);
  if(step(test,test.curr_states,actions).empty())throw std::runtime_error("static score/matching post-swap plan collided");
  test.curr_timestep=20;policy.schedule(&test,30000,schedule);
  if(schedule!=test.curr_task_schedule||policy.stats().match_moved!=2)
   throw std::runtime_error("static score/matching bypassed the finite retarget protection");
 }
 unsetenv("CGAR_TRICK_UNOPENED_MATCH");
 std::cout<<"TRICK_SCORE_MATCHING passed metric_modes=2 beneficial_cycles=2 primary_protected=1 simulator_metadata_unchanged=1 bounded_retargets=1 valid_post_swap_plan=1\n";
 unsetenv("CGAR_TRICK_REMAINING_FLOW");
 // Distinguish age preference from the independent forced-oldest admission.
 // Both tasks have the same pickup, so only their chain and age differ.
 int far=goal;for(int cell=0;cell<int(e.map.size());++cell)
  if(!e.map[cell]&&std::abs(cell/e.cols-goal/e.cols)+std::abs(cell%e.cols-goal%e.cols)>50){far=cell;break;}
 if(far==goal)throw std::runtime_error("short-task fixture lacks a long chain");
 for(int lanes:{0,1})for(int short_tasks:{0,1})for(int age_preference:{0,1}){
  setenv("CGAR_TRICK_LANES",std::to_string(lanes).c_str(),1);
  setenv("CGAR_TRICK_SHORT_TASKS",std::to_string(short_tasks).c_str(),1);
  setenv("CGAR_HRRN",std::to_string(age_preference).c_str(),1);
  auto test=flagged;test.curr_timestep=100;test.curr_task_schedule={-1};test.goal_locations={{}};test.task_pool.clear();
  Task old;old.task_id=0;old.t_revealed=0;old.locations={goal,far};test.task_pool.emplace(0,old);
  Task fresh;fresh.task_id=1;fresh.t_revealed=100;fresh.locations={goal,goal};test.task_pool.emplace(1,fresh);
  Cgar policy;policy.initialize(&test,30000);
  policy.schedule(&test,30000,schedule);
  const int expected=short_tasks||!age_preference?1:0;
  if(schedule!=std::vector<int>{expected}||policy.stats().fair_assignments)
   throw std::runtime_error("short-task preference did not isolate the age term");
  policy.schedule(&test,30000,schedule);
  if(schedule!=std::vector<int>{short_tasks?1:0}||policy.stats().fair_assignments!=!short_tasks)
   throw std::runtime_error("short-task preference did not isolate oldest admission");
  test.curr_task_schedule={0};test.task_pool.at(0).agent_assigned=0;test.task_pool.at(0).idx_next_loc=1;
  test.goal_locations={{{far,0}}};policy.schedule(&test,30000,schedule);
  if(schedule!=std::vector<int>{0}||test.task_pool.at(0).idx_next_loc!=1)
   throw std::runtime_error("short-task trick redirected a started long task");
 }
 for(const char* key:{"CGAR_TRICK_LANES","CGAR_TRICK_SHORT_TASKS","CGAR_TRICK_UNOPENED_MATCH","CGAR_TRICK_REMAINING_FLOW"}){
  unsetenv("CGAR_TRICK_LANES");unsetenv("CGAR_TRICK_SHORT_TASKS");unsetenv("CGAR_TRICK_UNOPENED_MATCH");unsetenv("CGAR_TRICK_REMAINING_FLOW");setenv(key,"0",1);rejected=false;
  try{options("");}catch(const std::invalid_argument&){rejected=true;}
  if(!rejected)throw std::runtime_error("trick component activated without CLI");
  for(const char* bad:{"", "-1", "2", "true"}){setenv(key,bad,1);rejected=false;
   try{options("WAREHOUSE");}catch(const std::invalid_argument&){rejected=true;}
   if(!rejected)throw std::runtime_error("malformed trick component accepted");}
 }
 unsetenv("CGAR_TRICK_LANES");unsetenv("CGAR_TRICK_SHORT_TASKS");unsetenv("CGAR_TRICK_UNOPENED_MATCH");unsetenv("CGAR_TRICK_REMAINING_FLOW");unsetenv("CGAR_HRRN");
 for(const char* key:{"CGAR_TEMPORAL","CGAR_ORIENTATION_GUIDANCE","CGAR_TEMPORAL_STEPS","CGAR_FLOW_STRENGTH","CGAR_FLOW_COST_SCALE","CGAR_PICKUP_FLOW","CGAR_FLOW_WARMUP","CGAR_FLOW_REFRESH_INTERVAL"})unsetenv(key);
 std::cout<<"TRICK_SHORT_TASKS passed age_and_oldest_admission_independent=1 lanes_factorial_cases=8 started_long_task_protected=1 explicit_cli_required=1 invalid_components_rejected=1\n";
 std::cout<<"TRICK_REMAINING_FLOW passed static_macro_scores="<<static_scores<<" changed_rankings="<<static_changed<<" neutral_macros="<<static_neutral<<" explicit_selector=1 incompatible_components_rejected=1\n";
 std::cout<<"WAREHOUSE_TRICK passed explicit_activation=1 map_identity_rejection=1 independent_oriented_states="<<compared<<" generic_initial_dispatch=1 static_pickup_from_tick1=1 no_flow_publications=1 generic_flow_preserved=1 explicit_matching_component=1 static_matching_active=1\n";
}


void native_metric_regression() {
 using namespace cgar::tricks;
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 auto rejects=[&](auto run,const char* message){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,message);};
 auto deadline=[](){return std::chrono::steady_clock::now()+std::chrono::seconds(60);};
 SharedEnvironment e;e.rows=warehouse_rows;e.cols=warehouse_cols;
 for(int cell=0;cell<e.rows*e.cols;++cell)e.map.push_back(warehouse_masks[cell]=='x');
 Certificate cert;cert.rows=e.rows;cert.cols=e.cols;
 for(int wall:e.map)cert.free.push_back(!wall);cert.core=cert.free;cert.pocket.assign(e.map.size(),-1);
 auto neighbor=[&](int cell,int h){const int y=cell/e.cols,x=cell%e.cols;
  if((h==0&&x+1==e.cols)||(h==2&&x==0)||(h==1&&y+1==e.rows)||(h==3&&y==0))return -1;
  const int to=cell+(h==0?1:h==1?e.cols:h==2?-1:-e.cols);return e.map[to]?-1:to;};
 TemporalGeometry geometry;geometry.initialize(cert.free,e.rows,e.cols,[]{});
 long long checked_states=0,checked_macros=0,service_macros=0,prefetched_states=0,seed_macros=0;
 long long neutral_changed=0,departed_start_without_service=0;
 for(int native_turn:{1,2,4})for(bool bands:{false,true}){
  const auto weights=native_forward_costs("WAREHOUSE",e.map,e.rows,e.cols,bands);
  uint64_t hash=14695981039346656037ULL;for(uint8_t w:weights)hash=(hash^w)*1099511628211ULL;
  require(hash==(bands?warehouse_native_bands_fnv1a64:warehouse_native_nobands_fnv1a64),"native asset differs from independently verified NMS dump");
  require(validate_native_field(weights,bands)==hash,"native installed vector validation differs");
  auto corrupt=weights;corrupt[0]^=1;
  rejects([&]{validate_native_field(corrupt,bands);},"native corrupted vector accepted");
  rejects([&]{validate_native_field(std::vector<uint8_t>{20},bands);},"native short vector accepted");
  TurnDistanceOracle oracle;oracle.init(&cert,32<<20,native_turn,true,20,bands?201:200);oracle.set_forward_costs(weights);
  for(int goal:{4,1504}){
   require(!e.map[goal],"native goal fixture blocked");
   std::vector<int> expected(e.map.size()*4,kInf);
   using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> heap;
   for(int h=0;h<4;++h){expected[goal*4+h]=0;heap.push({0,goal*4+h});}
   while(!heap.empty()){
    const auto item=heap.top();heap.pop();const int cost=item.first,state=item.second;
    if(expected[state]!=cost)continue;
    const int cell=state/4,h=state%4;
    auto relax=[&](int to,int edge){if(expected[to]>cost+edge){expected[to]=cost+edge;heap.push({cost+edge,to});}};
    relax(cell*4+(h+1)%4,native_turn);relax(cell*4+(h+3)%4,native_turn);
    const int from=neighbor(cell,(h+2)%4);if(from>=0)relax(from*4+h,weights[from*4+h]);
   }
   const auto* table=oracle.table(goal,deadline());
   for(int cell=0;cell<int(e.map.size());++cell)if(!e.map[cell])for(int h=0;h<4;++h){
    require(oracle.value(*table,cell,h)==expected[cell*4+h],"native reverse Dial differs from independent heap");++checked_states;
   }
   // Scratch must use the installed 200/201 maximum, including four workers.
   std::vector<int> batch{goal};for(int cell=0;batch.size()<4;++cell)if(!e.map[cell]&&cell!=goal)batch.push_back(cell);
   for(int threads:{1,4}){
    TurnDistanceOracle parallel;parallel.init(&cert,32<<20,native_turn,true,20,bands?201:200);
    parallel.set_forward_costs(weights);parallel.prefetch(batch,threads,deadline());
    require(parallel.prefetched_builds==4&&!parallel.peek(goal),"native prefetch published before demand");
    const auto* admitted=parallel.table(goal,deadline());
    require(parallel.prefetched_hits==1,"native parallel table was not admitted from prefetch");
    for(int cell=0;cell<int(e.map.size());++cell)if(!e.map[cell])for(int h=0;h<4;++h){
     require(parallel.value(*admitted,cell,h)==expected[cell*4+h],"native parallel Dial differs from independent heap");++prefetched_states;
    }
    parallel.discard_prefetch();require(parallel.prefetched_discarded==3,"native speculative tables did not discard completely");
   }
   std::set<int> samples{goal};for(int d=0;d<4;++d){const int from=neighbor(goal,d);if(from>=0)samples.insert(from);}
   for(int cell=0;cell<int(e.map.size());cell+=997)if(!e.map[cell])samples.insert(cell);
   // Negative fixture indices enumerate op0 with every real first action.
   for(int start:samples)for(int heading=0;heading<4;++heading)for(int fixture=-3;fixture<129;++fixture){
    const int op=std::max(0,fixture);
    const auto path=fixture<=0?geometry.seed(start,heading,-fixture):geometry.paths(start,heading)[op];if(!path.valid)continue;
    // Independent action replay; do not use the stored path's endpoint or hits.
    int at=start,h=heading,last_hit=-1;
    auto actions=TemporalGeometry::operations()[op];if(fixture<=0)actions[0]=-fixture;
    for(int slot=0;slot<5;++slot){
     if(actions[slot]==0)at=neighbor(at,h);
     else if(actions[slot]==1)h=(h+1)%4;
     else if(actions[slot]==2)h=(h+3)%4;
     require(at>=0,"native independent macro replay invalid");
     if(at==goal)last_hit=slot;
    }
    int value=expected[at*4+h];
    if(actions[4]==3){value=std::min({value,expected[at*4+(h+1)%4],expected[at*4+(h+3)%4]});
     if(actions[3]==3)value=std::min(value,expected[at*4+(h+2)%4]);}
    if(last_hit>=0){value=-last_hit;++service_macros;}
    const int64_t wanted=int64_t(value)*50-op;
    require(TemporalGeometry::pure_potential_cost(path,op,goal,[&](int cell,int orientation){return oracle.value(*table,cell,orientation);})==wanted,
      "native raw-unit macro score differs from independent replay");++checked_macros;seed_macros+=fixture<=0;
    const int64_t neutral_wanted=last_hit>=0 ? -200-op : wanted;
    require(TemporalGeometry::pure_potential_cost(path,op,goal,[&](int cell,int orientation){return oracle.value(*table,cell,orientation);},true)==neutral_wanted,
      "neutral service score differs from independent action replay");
    neutral_changed+=neutral_wanted!=wanted;
    departed_start_without_service+=start==goal&&last_hit<0;
   }
  }
 }
 // Hand scores isolate last-hit service, raw ties and terminal-wait headings.
 TemporalPath path;path.cells.fill(0);path.orientation=0;
 auto heading_distance=[](int,int h){return std::array<int,4>{100,5,1,7}[h];};
 int one=-1,two=-1,none=-1;
 for(int op=0;op<129;++op){const auto& a=TemporalGeometry::operations()[op];
  if(a[4]!=3)none=op;else if(a[3]==3)two=op;else one=op;}
 require(one>=0&&two>=0&&none>=0,"native terminal-wait fixtures missing");
 require(TemporalGeometry::pure_potential_cost(path,one,99,heading_distance)==250-one,"native one-wait heading price");
 require(TemporalGeometry::pure_potential_cost(path,two,99,heading_distance)==50-two,"native two-wait heading price");
 require(TemporalGeometry::pure_potential_cost(path,none,99,heading_distance)==5000-none,"native unwaited heading price");
 path.cells={9,3,9,5,9};require(TemporalGeometry::pure_potential_cost(path,128,9,heading_distance)==-328,"native service must use raw last slot");
 require(TemporalGeometry::pure_potential_cost(path,128,-1,heading_distance)==128,"native idle tie must be raw");
 path.cells={9,3,9,5,7};
 require(TemporalGeometry::pure_potential_cost(path,128,9,heading_distance)==-228&&
  TemporalGeometry::pure_potential_cost(path,128,9,heading_distance,true)==-328,"neutral tail did not remove only the last-hit penalty");
 require(TemporalGeometry::pure_potential_cost(path,128,-1,heading_distance,true)==128,"neutral tail changed idle tie");
 path.cells.fill(0);
 require(TemporalGeometry::pure_potential_cost(path,0,99,[](int,int){return 20;})<TemporalGeometry::pure_potential_cost(path,128,99,[](int,int){return 40;}),"native full forward step must dominate op tie");
 require(TemporalGeometry::pure_potential_cost(path,0,99,[](int,int){return 1;})>TemporalGeometry::pure_potential_cost(path,51,99,[](int,int){return 2;}),"native one-turn difference may lose to raw tie");
 // Both forward searches against a separate heap oracle, with wide positive costs.
 auto next=[](int cell,int h){int x=cell%6,y=cell/6;
  if((h==0&&x==5)||(h==2&&x==0)||(h==1&&y==1)||(h==3&&y==0))return -1;
  return cell+(h==0?1:h==1?6:h==2?-1:-6);};
 auto allowed=[](int cell){return cell>=0&&cell<12;};
 CompletePickupSearch complete;OrientedPickupSearch bounded;FullPickupField output;
 for(int native_turn:{1,2,4,16})for(int limit:{16,201,255}){
  auto edge=[&](int cell,int h){return 1+(cell*53+h*37)%limit;};
  std::vector<int> expected(48,kInf);expected[0]=0;
  using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> heap;heap.push({0,0});
  while(!heap.empty()){auto item=heap.top();heap.pop();int at=item.second,cost=item.first;if(expected[at]!=cost)continue;
   int cell=at/4,h=at%4;auto offer=[&](int to,int price){if(expected[to]>cost+price){expected[to]=cost+price;heap.push({cost+price,to});}};
   offer(cell*4+(h+1)%4,native_turn);offer(cell*4+(h+3)%4,native_turn);int to=next(cell,h);if(to>=0)offer(to*4+h,edge(cell,h));}
  complete.run(12,0,0,native_turn,kInf,next,allowed,edge,[]{},output,limit);
  std::vector<int> observed(12,kInf);auto stats=bounded.run(12,0,0,65536,native_turn,next,allowed,edge,[&](int cell,int d){observed[cell]=d;return false;},[]{},limit);
  require(!stats.limited&&stats.cells==12,"native bounded pickup did not finish small oracle fixture");
  for(int cell=0;cell<12;++cell){int best=*std::min_element(expected.begin()+4*cell,expected.begin()+4*cell+4);
   require(output.distance[cell]==best&&observed[cell]==best,"native pickup distances differ from independent heap");}
 }
 for(int bad:{0,256}){
  auto edge=[&](int,int){return bad;};
  rejects([&]{complete.run(12,0,0,1,kInf,next,allowed,edge,[]{},output,255);},"native complete pickup accepted invalid edge");
  rejects([&]{bounded.run(12,0,0,100,1,next,allowed,edge,[](int,int){return false;},[]{},255);},"native bounded pickup accepted invalid edge");
  rejects([&]{complete.run(12,0,0,1,kInf,next,allowed,[](int,int){return 1;},[]{},output,bad);},"native complete pickup accepted invalid capability");
 }
 rejects([&]{complete.run(12,0,0,1,kInf,next,allowed,[](int,int){return 20;},[]{},output);},"default complete pickup lost16bound");
 rejects([&]{bounded.run(12,0,0,100,1,next,allowed,[](int,int){return 20;},[](int,int){return false;},[]{});},"default bounded pickup lost16bound");
 // Large directed distance must use the lossless wide table, including eviction.
 Certificate corridor;corridor.rows=1;corridor.cols=401;corridor.free=corridor.core=std::vector<char>(401,true);corridor.pocket.assign(401,-1);
 TurnDistanceOracle wide;const size_t bytes=401*4*sizeof(int);wide.init(&corridor,bytes,1,true,20,201);
 std::vector<uint8_t> heavy(401*4,200);wide.set_forward_costs(heavy);
 auto* far=wide.table(400,deadline());require(!far->is_compact()&&wide.value(*far,0,0)==80000&&wide.wide_fallback_tables==1&&wide.capacity()==1,"native compact overflow corrupted distance or budget");
 wide.table(0,deadline());wide.trim();require(!wide.peek(400)&&wide.peek(0)&&wide.wide_fallback_tables==2,"native wide-table eviction differs");
 wide.set_forward_costs(std::vector<uint8_t>(401*4,20));require(!wide.peek(0),"native stale metric table survived");
 far=wide.table(400,deadline());require(far->is_compact()&&wide.value(*far,0,0)==8000&&wide.wide_fallback_tables==2,"native compact fallback did not recover losslessly");
 TurnDistanceOracle legacy;legacy.init(&corridor,bytes,1,true,1);
 rejects([&]{legacy.set_forward_costs(heavy);},"default reverse oracle lost16bound");
 // Real CGAR initialization, complete pickup fields and a legal native plan.
 const std::vector<std::pair<const char*,const char*>> settings={{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","128"},
  {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","4"},{"CGAR_FLOW_COST_SCALE","20"},{"CGAR_PICKUP_FLOW","1"},
  {"CGAR_PICKUP_FULL_ROBOTS","1"},{"CGAR_TRICK_LANES","1"},{"CGAR_TRICK_REMAINING_FLOW","1"},{"CGAR_TRICK_NATIVE_METRIC","1"}};
 for(auto setting:settings)setenv(setting.first,setting.second,1);
 e.trick_instance="WAREHOUSE";e.num_of_agents=1;e.curr_timestep=10;e.curr_states={State(1504,10,0)};e.curr_task_schedule={-1};e.goal_locations={{}};
 Task task;task.task_id=0;task.t_revealed=0;task.locations={1515};e.task_pool.emplace(0,task);
 for(const char* bands:{"0","1"}){setenv("CGAR_TRICK_NATIVE_BANDS",bands,1);Cgar planner;planner.initialize(&e,30000);
  std::vector<int> schedule;planner.schedule(&e,30000,schedule);require(schedule==std::vector<int>{0}&&planner.stats().pickup_full_fields==1,"native scheduling did not use complete field");
  auto active=e;active.curr_task_schedule=schedule;active.goal_locations={{{1515,10}}};active.task_pool.at(0).agent_assigned=0;
  std::vector<Action> actions;planner.plan(&active,30000,actions);require(!step(active,active.curr_states,actions).empty()&&!planner.stats().flow_publications,"native integrated plan invalid or learned metric published");}
 // Matching's acceptance scale remains explicit; old callers retain limit16.
 const std::vector<int> cheap={4,2,2,4},small={3,2,2,3};
 for(const auto& base:{cheap,small})for(int unit:{1,4,20,255}){
  auto matrix=base;for(int& value:matrix)value*=unit;
  const auto permutation=minimum_pickup_permutation(matrix,2,kInf,[]{});
  const auto cycles=pickup_permutation_cycles(matrix,permutation,unit,[]{},255);
  require(cycles.size()==1&&cycles[0].accepted==(base==cheap),"native matching gain threshold lost scale invariance");
 }
 auto permutation=minimum_pickup_permutation(cheap,2,kInf,[]{});
 rejects([&]{pickup_permutation_cycles(cheap,permutation,20,[]{});},"default matching lost16unit bound");
 for(int bad:{0,256})rejects([&]{pickup_permutation_cycles(cheap,permutation,20,[]{},bad);},"native matching accepted invalid capability");
 rejects([&]{pickup_permutation_cycles(cheap,permutation,21,[]{},20);},"native matching exceeded explicit unit limit");
 // Real native matching: beneficial cycle, primary and finite retarget guards.
 int base=-1;for(int cell=0;cell+12<int(e.map.size());++cell)if(cell/e.cols==(cell+11)/e.cols){
  bool free=true;for(int k=0;k<12;++k)free=free&&!e.map[cell+k];if(free){base=cell;break;}}
 require(base>=0,"native matching fixture lacks free segment");
 setenv("CGAR_TRICK_UNOPENED_MATCH","1",1);setenv("CGAR_DIAGNOSTICS","1",1);
 std::map<std::string,std::vector<long long>> matching_traces;
 for(const char* bands:{"0","1"})for(const char* short_tasks:{"0","1"})for(int audit:{0,10}){
  setenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE",std::to_string(audit).c_str(),1);
  setenv("CGAR_TRICK_SHORT_TASKS",short_tasks,1);
  setenv("CGAR_TRICK_NATIVE_BANDS",bands,1);auto test=e;test.curr_timestep=0;test.num_of_agents=3;
  test.curr_states={State(base,0,0),State(base+11,0,2),State(base+1,0,0)};
  test.curr_task_schedule={0,1,2};test.goal_locations={{{base+10,0}},{{base+1,0}},{{base+11,0}}};test.task_pool.clear();
  for(int i=0;i<3;++i){Task task;task.task_id=i;task.t_revealed=-5;task.agent_assigned=i;
   task.locations={i==0?base+10:(i==1?base+1:base+11)};test.task_pool.emplace(i,task);}
  Cgar policy;policy.initialize(&test,30000);std::vector<Action> actions;std::vector<int> schedule;
  for(int tick=0;tick<2;++tick){test.curr_timestep=tick;policy.plan(&test,30000,actions);auto next=step(test,test.curr_states,actions);
   require(!next.empty(),"native matching setup collided");test.curr_states=next;}
  test.curr_timestep=10;policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>({0,2,1})&&policy.stats().match_moved==2&&policy.stats().match_accepted_cycles==1&&
    policy.stats().match_saving>=80&&policy.stats().match_primary_protected==1&&!policy.stats().flow_publications&&
    policy.stats().match_groups<=4&&policy.stats().match_nodes<=8192,"native matching failed protected beneficial cycle");
  for(int i=0;i<3;++i)require(test.curr_task_schedule[i]==i&&test.task_pool.at(i).agent_assigned==i&&test.task_pool.at(i).idx_next_loc==0&&
   test.task_pool.at(i).t_revealed==-5,"native matching changed simulator metadata");
  test.curr_task_schedule=schedule;for(int i=0;i<3;++i){auto& task=test.task_pool.at(schedule[i]);task.agent_assigned=i;test.goal_locations[i]={{{task.locations[0],10}}};}
  policy.plan(&test,30000,actions);require(!step(test,test.curr_states,actions).empty(),"native matching post-swap collided");
  test.curr_timestep=20;policy.schedule(&test,30000,schedule);
  require(schedule==test.curr_task_schedule&&policy.stats().match_moved==2,"native matching bypassed retarget protection");
  // Controlled later state: the two once-moved tasks again admit a profitable
  // cycle, but the real one-retarget limit must still prohibit that cycle.
  test.curr_states[1]=State(base+1,29,0);test.curr_states[2]=State(base+11,29,2);
  test.curr_timestep=29;policy.plan(&test,30000,actions);
  require(!step(test,test.curr_states,actions).empty(),"budget shadow setup produced conflicting actions");
  test.curr_timestep=30;policy.schedule(&test,30000,schedule);
  require(schedule==test.curr_task_schedule&&policy.stats().match_moved==2,"budget shadow committed a second retarget");
  if(audit){const auto& a=policy.match_budget_shadow();
   require(a.witness_cycles==1&&a.witness_rows==2&&a.witness_budget_rows==2&&a.witness_saving>=80&&
    a.fully_protected_cycles==1&&a.fully_protected_rows==2&&a.fully_protected_saving==a.witness_saving&&
    a.work.match_budget_protected>=2&&a.work.match_primary_protected>0,"budget shadow failed to find a protected beneficial cycle");}
  const auto saving=policy.match_budget_shadow().witness_saving;
  test.curr_timestep=40;policy.schedule(&test,30000,schedule);
  require(schedule==test.curr_task_schedule&&policy.stats().match_moved==2,"repeated budget shadow changed assignments");
  if(audit)require(policy.match_budget_shadow().witness_cycles==1&&policy.match_budget_shadow().duplicate_cycles>=1&&
   policy.match_budget_shadow().witness_saving==saving&&policy.match_budget_shadow().fully_protected_saving==saving,"budget shadow double-counted a task witness");
  auto& started=test.task_pool.at(test.curr_task_schedule[1]);started.locations.push_back(base+8);started.idx_next_loc=1;
  const auto eligible=policy.match_budget_shadow().work.match_eligible;
  test.curr_timestep=50;policy.schedule(&test,30000,schedule);
  require(schedule==test.curr_task_schedule&&started.idx_next_loc==1&&policy.stats().match_moved==2,"budget shadow redirected a started task");
  if(audit)require(policy.match_budget_shadow().work.match_eligible==eligible+1&&policy.match_budget_shadow().witness_saving==saving,
   "budget shadow included a started holder");
  else require(policy.match_budget_shadow().work.match_passes==0,"disabled budget shadow performed work");
  std::vector<long long> trace{policy.stats().match_moved,policy.stats().match_saving,policy.stats().match_groups,
   policy.stats().match_selected,policy.stats().match_matrix_entries,policy.stats().match_primary_protected,
   policy.stats().oriented_builds,policy.stats().temporal_cold_worker_runs,policy.primary()};
  for(int value:schedule)trace.push_back(value);for(auto value:actions)trace.push_back(int(value));
  const std::string key=std::string(bands)+short_tasks;
  if(!audit)matching_traces[key]=trace;else require(matching_traces.at(key)==trace,"budget shadow changed real matching/planner work or actions");
 }
 unsetenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE");
 unsetenv("CGAR_TRICK_UNOPENED_MATCH");unsetenv("CGAR_TRICK_SHORT_TASKS");
 // Native closed-loop service semantics, including repeated locations. Serial
 // and parallel preparation must yield identical schedules/actions/cache work.
 setenv("CGAR_TURN_PREFETCH_THREADS","4",1);setenv("CGAR_TURN_TABLE_MB","64",1);setenv("CGAR_TURN_COMPACT","1",1);
 setenv("CGAR_PICKUP_FULL_ROBOTS","4",1);setenv("CGAR_PICKUP_FULL_THREADS","4",1);
 struct NativeTrace{std::vector<int> values;Stats stats;MatchBudgetShadowStats shadow;FreshPickupShadowStats fresh;int services=0,repeated=0,completed=0,rotations=0;};
 auto episode=[&](const char* bands,int preparation,int audit=0,int fresh=0,int interval=10){
  setenv("CGAR_REASSIGN_MATCH_INTERVAL",std::to_string(interval).c_str(),1);
  setenv("CGAR_FRESH_PICKUP_AUDIT",std::to_string(fresh).c_str(),1);
  setenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE",std::to_string(audit).c_str(),1);
  setenv("CGAR_TRICK_NATIVE_BANDS",bands,1);setenv("CGAR_TEMPORAL_PREP_THREADS",std::to_string(preparation).c_str(),1);
  auto test=e;test.curr_timestep=0;test.num_of_agents=24;test.curr_states.clear();test.task_pool.clear();
  test.curr_task_schedule.assign(24,-1);test.goal_locations.assign(24,{});
  std::vector<int> cells;for(int cell=0;cells.size()<128;++cell)if(!test.map[cell])cells.push_back(cell);
  for(int r=0;r<24;++r)test.curr_states.emplace_back(cells[r*5],0,r%4);
  int next_id=0;auto add=[&](){Task task;task.task_id=next_id++;task.t_revealed=test.curr_timestep;
   const int pickup=cells[(task.task_id*17+3)%128];task.locations={pickup,pickup,cells[(task.task_id*29+31)%128]};test.task_pool.emplace(task.task_id,task);};
  for(int k=0;k<72;++k)add();Cgar policy;policy.initialize(&test,30000);NativeTrace trace;
  std::map<int,int> first_holder,retarget_count;
  for(int tick=0;tick<128;++tick){
   test.curr_timestep=tick;std::vector<int> schedule;policy.schedule(&test,30000,schedule);std::set<int> assigned;
   require(schedule.size()==24,"native episode omitted schedule rows");
   for(int r=0;r<24;++r){const int old=test.curr_task_schedule[r];
    require(schedule[r]>=0&&assigned.insert(schedule[r]).second,"native episode omitted or duplicated tasks");
    if(old>=0&&test.task_pool.at(old).idx_next_loc>0)require(schedule[r]==old,"native episode redirected started task");
    auto holder=first_holder.emplace(schedule[r],r);
    if(!holder.second&&holder.first->second!=r){require(++retarget_count[schedule[r]]<=1,"native episode exceeded one retarget");holder.first->second=r;}}
   for(auto& item:test.task_pool)item.second.agent_assigned=-1;
   for(int r=0;r<24;++r){test.curr_task_schedule[r]=schedule[r];auto& task=test.task_pool.at(schedule[r]);task.agent_assigned=r;
    test.goal_locations[r]={{{task.locations[task.idx_next_loc],0}}};trace.values.push_back(schedule[r]);}
   std::vector<Action> actions;policy.plan(&test,30000,actions);auto next=step(test,test.curr_states,actions);
   require(!next.empty(),"native episode collided or left the map");test.curr_states=next;
   for(int r=0;r<24;++r){trace.values.push_back(int(actions[r]));auto& task=test.task_pool.at(test.curr_task_schedule[r]);
    trace.rotations+=actions[r]==Action::CR||actions[r]==Action::CCR;
    // Exactly one errand may complete for this robot on each physical tick.
    if(test.curr_states[r].location==task.locations[task.idx_next_loc]){
     ++trace.services;trace.repeated+=task.idx_next_loc>0&&task.locations[task.idx_next_loc]==task.locations[task.idx_next_loc-1];++task.idx_next_loc;
     if(task.idx_next_loc==int(task.locations.size())){const int id=task.task_id;++trace.completed;test.curr_task_schedule[r]=-1;test.goal_locations[r].clear();test.task_pool.erase(id);add();}
     else test.goal_locations[r]={{{task.locations[task.idx_next_loc],0}}};
    }
   }
  }
  trace.stats=policy.stats();trace.shadow=policy.match_budget_shadow();trace.fresh=policy.fresh_pickup_shadow();
  require(trace.services>0&&trace.repeated>0&&trace.completed>0,"native service episode was vacuous");
  if(trace.stats.flow_publications||trace.stats.flow_cache_resets||trace.stats.temporal_prepared_robots!=24*128||
     trace.stats.temporal_cold_worker_runs!=128||
     (!options(test.trick_instance).native_neutral_tail&&trace.stats.temporal_seed_rotations==0))
   throw std::runtime_error("native episode counters: publications="+std::to_string(trace.stats.flow_publications)+
    " resets="+std::to_string(trace.stats.flow_cache_resets)+" prepared="+std::to_string(trace.stats.temporal_prepared_robots)+
    " workers="+std::to_string(trace.stats.temporal_cold_worker_runs)+" seed_rotations="+std::to_string(trace.stats.temporal_seed_rotations)+
    " service_choices="+std::to_string(trace.stats.native_service_choices)+" changed_choices="+std::to_string(trace.stats.native_service_changed_choices));
  return trace;
 };
 int episode_services=0,episode_repeated=0,episode_rotations=0;
 std::map<std::string,NativeTrace> native_original_episodes;
 for(const char* bands:{"0","1"}){auto serial=episode(bands,1),parallel=episode(bands,4);
  require(serial.values==parallel.values&&serial.stats.oriented_builds==parallel.stats.oriented_builds&&
   serial.stats.pickup_full_pops==parallel.stats.pickup_full_pops&&serial.services==parallel.services,"native serial/parallel closed-loop behavior differs");
  episode_services+=serial.services;episode_repeated+=serial.repeated;episode_rotations+=serial.stats.temporal_seed_rotations;
  require(!serial.stats.native_service_choices&&!parallel.stats.native_service_changed_choices,"disabled neutral tail performed diagnostic work");
  native_original_episodes.emplace(bands,serial);
 }
 // Explicit native turn1 exactly retains the absent-selector trace. New
 // route/pickup prices2/4 must remain legal and independent of preparation order.
 int changed_turn_episodes=0;
 for(const char* bands:{"0","1"}) {
  setenv("CGAR_TRICK_NATIVE_TURN_COST","1",1);
  require(episode(bands,4).values==native_original_episodes.at(bands).values,"explicit native turn1 changed baseline");
  for(const char* cost:{"2","4"}) {
   setenv("CGAR_TRICK_NATIVE_TURN_COST",cost,1);
   const auto serial=episode(bands,1),parallel=episode(bands,4);
   require(serial.values==parallel.values&&serial.services==parallel.services&&
    serial.stats.oriented_builds==parallel.stats.oriented_builds&&serial.stats.pickup_full_pops==parallel.stats.pickup_full_pops,
    "native turn price depends on preparation order");
   changed_turn_episodes+=serial.values!=native_original_episodes.at(bands).values;
  }
 }
 unsetenv("CGAR_TRICK_NATIVE_TURN_COST");
 require(changed_turn_episodes>0,"native turn-price service fixtures were vacuous");
 std::cout<<"NATIVE_TURN_COST_EPISODES passed pairs=4 independent_actions=24576 explicit_default_exact=1 serial_parallel_exact=1 started_protection=1 one_retarget=1 changed_traces="<<changed_turn_episodes<<"\n";
 // Exercise the changed scorer in real service/rotation episodes. Every
 // physical action is checked independently, with serial/parallel equality.
 setenv("CGAR_TRICK_NATIVE_NEUTRAL_TAIL","1",1);
 int neutral_trace_changes=0;
 for(const char* bands:{"0","1"}){
  auto serial=episode(bands,1),parallel=episode(bands,4);
  require(serial.values==parallel.values&&serial.stats.native_service_choices==parallel.stats.native_service_choices&&
   serial.stats.native_service_changed_choices==parallel.stats.native_service_changed_choices&&
   serial.stats.oriented_builds==parallel.stats.oriented_builds&&serial.stats.pickup_full_pops==parallel.stats.pickup_full_pops,
   "neutral tail depends on preparation thread order");
  require(serial.rotations>0&&serial.rotations==parallel.rotations,"neutral episode missed physical rotations or changed them across threads");
  require(serial.stats.native_service_changed_choices>0&&serial.stats.native_service_choices>=serial.stats.native_service_changed_choices,
   "neutral tail production episode never repriced a serviced departure");
  neutral_trace_changes+=serial.values!=native_original_episodes.at(bands).values;
 }
 unsetenv("CGAR_TRICK_NATIVE_NEUTRAL_TAIL");
 require(neutral_trace_changes>0&&neutral_changed>0&&departed_start_without_service>0,"neutral service fixtures were vacuous");
 std::cout<<"NATIVE_NEUTRAL_TAIL passed replayed_macros="<<checked_macros<<" changed_macros="<<neutral_changed
  <<" departed_start_without_service="<<departed_start_without_service<<" changed_episode_traces="<<neutral_trace_changes
  <<" independent_actions=12288 serial_parallel_exact=1 repeated_errands=1 started_protection=1 fixed_work=1\n";
 // Complete native scheduling episodes: the diagnostic cannot alter actions,
 // matching, horizon calibration or cache work, including a configured horizon.
 setenv("CGAR_TRICK_UNOPENED_MATCH","1",1);setenv("CGAR_TRICK_SHORT_TASKS","0",1);setenv("CGAR_HRRN","1",1);
 setenv("CGAR_PICKUP_FULL_ROBOTS","64",1);
 for(int horizon:{0,128})for(const char* bands:{"0","1"}){
  setenv("CGAR_TRICK_KNOWN_HORIZON",std::to_string(horizon).c_str(),1);
  setenv("CGAR_TRICK_HORIZON_MARGIN",horizon?"1":"0",1);
  setenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE",horizon?"90":"0",1);
  const auto baseline=episode(bands,4,0,0),shadow=episode(bands,4,0,1);
  require(baseline.values==shadow.values&&baseline.services==shadow.services&&baseline.fresh.passes==0&&
   shadow.fresh.passes>0&&shadow.fresh.eligible>0,"fresh pickup native episode was inactive or changed a trace");
  for(auto member:{&Stats::assignments,&Stats::fair_assignments,&Stats::oriented_builds,&Stats::pickup_full_pops,&Stats::pickup_full_scans,
   &Stats::temporal_cold_worker_runs,&Stats::match_moved,&Stats::match_saving,&Stats::horizon_rank_changes,&Stats::horizon_margin_rank_changes})
   require(baseline.stats.*member==shadow.stats.*member,"fresh pickup native episode changed real work or calibration decisions");
 }
 unsetenv("CGAR_FRESH_PICKUP_AUDIT");unsetenv("CGAR_TRICK_KNOWN_HORIZON");unsetenv("CGAR_TRICK_HORIZON_MARGIN");
 unsetenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE");setenv("CGAR_PICKUP_FULL_ROBOTS","4",1);
 std::cout<<"FRESH_PICKUP_NATIVE_EPISODES passed pairs=4 independent_actions=24576 horizon_p90=1 source_fields=complete trace_and_work_exact=1\n";
 // Five-step matching keeps fixed work and real service semantics on both
 // native fields, including the p90 cutoff model. Thread order cannot change it.
 setenv("CGAR_TRICK_KNOWN_HORIZON","128",1);setenv("CGAR_TRICK_HORIZON_MARGIN","1",1);
 setenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE","90",1);
 for(const char* bands:{"0","1"}){
  const auto serial=episode(bands,1,0,0,5),parallel=episode(bands,4,0,0,5);
  require(serial.values==parallel.values&&serial.services==parallel.services&&serial.stats.match_passes==26&&
   parallel.stats.match_passes==26&&serial.stats.match_moved==parallel.stats.match_moved&&
   serial.stats.match_saving==parallel.stats.match_saving&&serial.stats.oriented_builds==parallel.stats.oriented_builds&&
   serial.stats.pickup_full_pops==parallel.stats.pickup_full_pops,"five-step native matching depends on preparation order or missed prescribed passes");
 }
 unsetenv("CGAR_REASSIGN_MATCH_INTERVAL");unsetenv("CGAR_TRICK_KNOWN_HORIZON");unsetenv("CGAR_TRICK_HORIZON_MARGIN");
 unsetenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE");
 std::cout<<"MATCH_CADENCE_NATIVE_EPISODES passed pairs=2 interval=5 prescribed_passes=26 horizon_p90=1 independent_actions=12288 serial_parallel_exact=1 started_protection=1 one_retarget=1\n";
 // A whole-cycle horizon veto still executes complete, legal native plans.
 // Missing and explicit-zero are identical; enabled mode is deterministic
 // across preparation threads and retains started-task/one-retarget protection.
 setenv("CGAR_TRICK_KNOWN_HORIZON","128",1);setenv("CGAR_TRICK_HORIZON_MARGIN","1",1);
 setenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE","90",1);
 long long guarded_cycles=0,guarded_rejections=0;
 for(const char* bands:{"0","1"}){
  unsetenv("CGAR_TRICK_MATCH_HORIZON");const auto baseline=episode(bands,4);
  setenv("CGAR_TRICK_MATCH_HORIZON","0",1);const auto zero=episode(bands,4);
  require(baseline.values==zero.values&&!zero.stats.match_horizon_cycles,"disabled matching horizon changed trace or did work");
  setenv("CGAR_TRICK_MATCH_HORIZON","1",1);
  const auto serial=episode(bands,1),parallel=episode(bands,4);
  require(serial.values==parallel.values&&serial.services==parallel.services&&
   serial.stats.match_horizon_cycles==parallel.stats.match_horizon_cycles&&
   serial.stats.match_horizon_rejected==parallel.stats.match_horizon_rejected&&
   serial.stats.match_horizon_worse_rows==parallel.stats.match_horizon_worse_rows&&
   serial.stats.oriented_builds==parallel.stats.oriented_builds&&serial.stats.pickup_full_pops==parallel.stats.pickup_full_pops,
   "matching horizon depends on preparation thread order");
  require(serial.stats.match_horizon_cycles==serial.stats.match_accepted_cycles+serial.stats.match_horizon_rejected&&
   serial.stats.match_horizon_rows>=2*serial.stats.match_horizon_cycles&&
   serial.stats.match_horizon_worse_rows>=serial.stats.match_horizon_rejected,
   "matching horizon accepted/rejected cycle accounting differs");
  guarded_cycles+=serial.stats.match_horizon_cycles;guarded_rejections+=serial.stats.match_horizon_rejected;
 }
 require(guarded_cycles>0,"matching horizon native episode never checked an accepted cycle");
 setenv("CGAR_HRRN","0",1);rejects([&]{Cgar bad;bad.initialize(&e,30000);},"matching horizon accepted disabled fairness");setenv("CGAR_HRRN","1",1);
 setenv("CGAR_TRICK_SHORT_TASKS","1",1);rejects([&]{Cgar bad;bad.initialize(&e,30000);},"matching horizon accepted short preference");setenv("CGAR_TRICK_SHORT_TASKS","0",1);
 unsetenv("CGAR_TRICK_MATCH_HORIZON");unsetenv("CGAR_REASSIGN_MATCH_INTERVAL");
 unsetenv("CGAR_TRICK_KNOWN_HORIZON");unsetenv("CGAR_TRICK_HORIZON_MARGIN");unsetenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE");
 std::cout<<"MATCH_HORIZON_NATIVE_EPISODES passed pairs=2 independent_actions=24576 exact_disabled=1 serial_parallel_exact=1 started_protection=1 one_retarget=1 checked_cycles="<<guarded_cycles<<" rejected_cycles="<<guarded_rejections<<"\n";
 // Dedicated closed-loop shadow neutrality with native+short+matching together.
 setenv("CGAR_TRICK_UNOPENED_MATCH","1",1);setenv("CGAR_TRICK_SHORT_TASKS","1",1);
 for(const char* bands:{"0","1"}){auto baseline=episode(bands,4,0),shadow=episode(bands,4,10);
  require(baseline.values==shadow.values&&baseline.services==shadow.services&&baseline.stats.oriented_builds==shadow.stats.oriented_builds&&
   baseline.stats.pickup_full_pops==shadow.stats.pickup_full_pops&&baseline.stats.temporal_cold_worker_runs==shadow.stats.temporal_cold_worker_runs&&
   baseline.stats.match_moved==shadow.stats.match_moved&&baseline.stats.match_saving==shadow.stats.match_saving&&
   baseline.shadow.work.match_passes==0&&shadow.shadow.work.match_passes==13,"native matching budget shadow changed a closed-loop trace/work");
 }
 for(const char* bad:{"-1","1","15","5010"}){setenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE",bad,1);
  rejects([&]{Cgar invalid;invalid.initialize(&e,30000);},"matching budget audit accepted invalid stride");}
 setenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE","10",1);setenv("CGAR_DIAGNOSTICS","0",1);
 rejects([&]{Cgar invalid;invalid.initialize(&e,30000);},"matching budget audit accepted disabled diagnostics");
 setenv("CGAR_DIAGNOSTICS","1",1);setenv("CGAR_TRICK_UNOPENED_MATCH","0",1);
 rejects([&]{Cgar invalid;invalid.initialize(&e,30000);},"matching budget audit accepted disabled matching");
 unsetenv("CGAR_MATCH_BUDGET_AUDIT_STRIDE");unsetenv("CGAR_TRICK_UNOPENED_MATCH");unsetenv("CGAR_TRICK_SHORT_TASKS");unsetenv("CGAR_DIAGNOSTICS");
 std::cout<<"MATCH_BUDGET_SHADOW passed protected_beneficial_cycles=4 task_disjoint_dedup=1 no_commit=1 started_primary_cooldown_protected=1 real_work_exact=1 closed_loop_pairs=2 independent_action_checks=12288 guards=6\n";
 unsetenv("CGAR_FRESH_PICKUP_AUDIT");unsetenv("CGAR_REASSIGN_MATCH_INTERVAL");
 unsetenv("CGAR_TURN_PREFETCH_THREADS");unsetenv("CGAR_TURN_TABLE_MB");unsetenv("CGAR_TURN_COMPACT");
 unsetenv("CGAR_TEMPORAL_PREP_THREADS");unsetenv("CGAR_PICKUP_FULL_THREADS");setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);
 std::cout<<"NATIVE_EPISODES passed robots=24 ticks_per_episode=128 profiles=4 services="<<episode_services<<" repeated="<<episode_repeated
  <<" seed_rotations="<<episode_rotations<<" independent_action_checks=12288 started_protection=1 fixed_work=1 serial_parallel_exact=1 no_learned_publications=1\n";
 unsetenv("CGAR_TRICK_NATIVE_BANDS");
 for(auto bad:std::vector<std::pair<const char*,const char*>>{{"CGAR_TRICK_LANES","0"},{"CGAR_TRICK_REMAINING_FLOW","0"},
  {"CGAR_TEMPORAL_REMAINING_FLOW","1"},
  {"CGAR_FLOW_COST_SCALE","4"},{"CGAR_TURN_COST","2"},{"CGAR_TURN_SURCHARGE","1"},{"CGAR_TEMPORAL_DISTANCE_SCALE","51"}}){
  std::string old=std::getenv(bad.first)?std::getenv(bad.first):"";bool existed=std::getenv(bad.first);
  setenv(bad.first,bad.second,1);rejects([&]{Cgar invalid;invalid.initialize(&e,30000);},"native incompatible profile accepted");
  if(existed)setenv(bad.first,old.c_str(),1);else unsetenv(bad.first);
 }
 for(const char* good:{"1","2","4","16"}) {
  setenv("CGAR_TRICK_NATIVE_TURN_COST",good,1);
  require(options("WAREHOUSE").native_turn_cost==std::stoi(good),"native turn selector lost value");
 }
 for(const char* bad:{"","0","-1","17","1000000000000000000","2x","1.5"}) {
  setenv("CGAR_TRICK_NATIVE_TURN_COST",bad,1);
  rejects([&]{options("WAREHOUSE");},"invalid native turn cost accepted");
 }
 unsetenv("CGAR_TRICK_NATIVE_TURN_COST");
 for(auto setting:settings)unsetenv(setting.first);
 setenv("CGAR_TRICK_NATIVE_BANDS","1",1);rejects([&]{Cgar invalid;invalid.initialize(&e,30000);},"native bands accepted without native metric");unsetenv("CGAR_TRICK_NATIVE_BANDS");
 setenv("CGAR_TRICK_NATIVE_NEUTRAL_TAIL","1",1);
 rejects([&]{options("WAREHOUSE");},"neutral tail accepted without native metric");unsetenv("CGAR_TRICK_NATIVE_NEUTRAL_TAIL");
 setenv("CGAR_TRICK_NATIVE_TURN_COST","1",1);
 rejects([&]{options("WAREHOUSE");},"native turn selector accepted without native metric");
 rejects([&]{options("");},"native turn selector accepted without explicit CLI");
 unsetenv("CGAR_TRICK_NATIVE_TURN_COST");
 for(const char* key:{"CGAR_TRICK_NATIVE_METRIC","CGAR_TRICK_NATIVE_BANDS","CGAR_TRICK_NATIVE_NEUTRAL_TAIL"}){
  setenv(key,"0",1);rejects([&]{options("");},"native selector accepted without CLI");
  for(const char* bad:{"", "-1", "2", "true"}){setenv(key,bad,1);rejects([&]{options("WAREHOUSE");},"native malformed selector accepted");}
  unsetenv(key);
 }
 require(service_macros>0&&checked_macros>1000,"native score fixtures did not cover service");
 std::cout<<"NATIVE_METRIC passed independent_states="<<checked_states<<" macro_scores="<<checked_macros<<" service_macros="<<service_macros
  <<" prefetched_states="<<prefetched_states<<" seed_macros="<<seed_macros<<" native_asset_hashes=2 corrupted_field_rejected=1 pickup_oracles=3 positive_bounds=1 default_bounds=1 wide_fallback=1 eviction=1 raw_score_units=1 integrated_profiles=2 native_matching_cycles=8 short_on_off_cycles=1 matching_scale_invariance=8 explicit_cli=1 incompatible_gates=1\n";
}

// Explicit short preference changes only the two admission/age mechanisms.
// Native prices and remaining-potential scoring must not weaken task protection.
void native_short_preference_regression() {
 using namespace cgar::tricks;
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 const std::vector<std::pair<const char*,const char*>> settings={{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","128"},
  {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","4"},{"CGAR_PICKUP_FLOW","1"},
  {"CGAR_PICKUP_FULL_ROBOTS","1"},{"CGAR_TRICK_LANES","1"},{"CGAR_TRICK_REMAINING_FLOW","1"}};
 for(auto setting:settings)setenv(setting.first,setting.second,1);
 SharedEnvironment base;base.trick_instance="WAREHOUSE";base.rows=warehouse_rows;base.cols=warehouse_cols;base.num_of_agents=1;
 for(int cell=0;cell<base.rows*base.cols;++cell)base.map.push_back(warehouse_masks[cell]=='x');
 base.curr_timestep=1000000;base.curr_states={State(1504,1000000,0)};base.curr_task_schedule={-1};base.goal_locations={{}};
 const int goal=1515;int far=-1;for(int cell=0;cell<int(base.map.size());++cell)
  if(!base.map[cell]&&std::abs(cell/base.cols-goal/base.cols)+std::abs(cell%base.cols-goal%base.cols)>50){far=cell;break;}
 require(far>=0&&!base.map[1504]&&!base.map[goal],"native short fixture invalid");
 int cases=0;
 for(int metric:{0,1,2})for(int short_tasks:{0,1})for(int age:{0,1})for(int weight:{1,5}){
  setenv("CGAR_TRICK_NATIVE_METRIC",metric?"1":"0",1);setenv("CGAR_TRICK_NATIVE_BANDS",metric==2?"1":"0",1);
  setenv("CGAR_FLOW_COST_SCALE",metric?"20":"4",1);setenv("CGAR_TRICK_SHORT_TASKS",short_tasks?"1":"0",1);
  setenv("CGAR_HRRN",age?"1":"0",1);setenv("CGAR_PICKUP_WEIGHT",std::to_string(weight).c_str(),1);
  auto test=base;Task old;old.task_id=0;old.t_revealed=0;old.locations={goal,far};test.task_pool.emplace(0,old);
  Task fresh;fresh.task_id=1;fresh.t_revealed=test.curr_timestep;fresh.locations={goal,goal};test.task_pool.emplace(1,fresh);
  Cgar policy;policy.initialize(&test,30000);std::vector<int> schedule;
  policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{short_tasks||!age?1:0}&&!policy.stats().fair_assignments,"native short preference failed independent age test");
  policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{short_tasks?1:0}&&policy.stats().fair_assignments==!short_tasks,"native short preference failed independent oldest-admission test");
  require(test.curr_task_schedule==std::vector<int>{-1}&&test.task_pool.at(0).agent_assigned==-1&&test.task_pool.at(1).agent_assigned==-1&&
   test.task_pool.at(0).idx_next_loc==0&&test.task_pool.at(1).idx_next_loc==0,"native short preference changed simulator metadata");
  test.curr_task_schedule={0};test.task_pool.at(0).agent_assigned=0;test.task_pool.at(0).idx_next_loc=1;test.goal_locations={{{far,0}}};
  policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{0}&&test.task_pool.at(0).idx_next_loc==1&&test.task_pool.at(0).t_revealed==0,"native short preference redirected or changed a started long task");
  require(!policy.stats().flow_publications,"native short preference published learned field");++cases;
 }
 for(auto setting:settings)unsetenv(setting.first);
 for(const char* key:{"CGAR_TRICK_NATIVE_METRIC","CGAR_TRICK_NATIVE_BANDS","CGAR_FLOW_COST_SCALE","CGAR_TRICK_SHORT_TASKS","CGAR_HRRN","CGAR_PICKUP_WEIGHT"})unsetenv(key);
 std::cout<<"NATIVE_SHORT_PREFERENCE passed metric_profiles=3 age_admission_weight_cases="<<cases<<" age_and_oldest_independent=1 started_long_task_protected=1 simulator_metadata_unchanged=1 no_learned_publications=1\n";
}

// A configured run horizon is an explicit trick, never simulator/future-task data.
void known_horizon_regression() {
 using namespace cgar::tricks;
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 auto rejects=[&](auto run,const char* message){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,message);};
 setenv("CGAR_TRICK_KNOWN_HORIZON","0",1);rejects([&]{options("");},"known horizon bypassed explicit CLI");
 for(const char* bad:{"","-1","1000001","5000x","1.5","true","999999999999999999","+5"}){
  setenv("CGAR_TRICK_KNOWN_HORIZON",bad,1);rejects([&]{options("WAREHOUSE");},"malformed known horizon accepted");
 }
 unsetenv("CGAR_TRICK_KNOWN_HORIZON");
 const std::vector<std::pair<const char*,const char*>> settings={{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","128"},
  {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","4"},{"CGAR_PICKUP_FLOW","1"},
  {"CGAR_PICKUP_FULL_ROBOTS","1"},{"CGAR_PICKUP_FULL_THREADS","1"},{"CGAR_TRICK_LANES","1"},
  {"CGAR_TRICK_REMAINING_FLOW","1"},{"CGAR_PICKUP_WEIGHT","5"},{"CGAR_HRRN","1"}};
 for(auto setting:settings)setenv(setting.first,setting.second,1);
 SharedEnvironment base;base.trick_instance="WAREHOUSE";base.rows=warehouse_rows;base.cols=warehouse_cols;base.num_of_agents=1;
 for(int cell=0;cell<base.rows*base.cols;++cell)base.map.push_back(warehouse_masks[cell]=='x');
 base.curr_timestep=100;base.curr_states={State(1504,100,0)};base.curr_task_schedule={-1};base.goal_locations={{}};
 const int goal=1515;int far=-1;for(int cell=0;cell<int(base.map.size());++cell)
  if(!base.map[cell]&&std::abs(cell/base.cols-goal/base.cols)+std::abs(cell%base.cols-goal%base.cols)>50){far=cell;break;}
 for(int cell=1504;cell<=goal;++cell)require(!base.map[cell],"horizon straight-line service fixture obstructed");
 require(far>=0,"horizon fixture lacks long task");
 int cases=0;
 for(int metric:{0,1})for(int tables:{0,128})for(int horizon:{0,99,100,111,112,113,1000000}){
  setenv("CGAR_TRICK_NATIVE_METRIC",metric?"1":"0",1);setenv("CGAR_TRICK_NATIVE_BANDS",metric?"1":"0",1);
  setenv("CGAR_FLOW_COST_SCALE",metric?"20":"4",1);setenv("CGAR_SCHED_TABLES",std::to_string(tables).c_str(),1);
  setenv("CGAR_TRICK_KNOWN_HORIZON",std::to_string(horizon).c_str(),1);
  auto test=base;Task old;old.task_id=0;old.t_revealed=0;old.locations={goal,far};test.task_pool.emplace(0,old);
  Task fresh;fresh.task_id=1;fresh.t_revealed=100;fresh.locations={goal,goal};test.task_pool.emplace(1,fresh);
  Cgar policy;policy.initialize(&test,30000);std::vector<int> schedule;policy.schedule(&test,30000,schedule);
  const bool demotes=horizon==112||horizon==113;
  require(schedule==std::vector<int>{demotes?1:0},"horizon tier broke exact boundary, ordinary order or expiry");
  require(!policy.stats().fair_assignments,"horizon changed first ordinary admission");
  require((policy.stats().horizon_first_rank_change==100)==demotes,"horizon first changed comparison incorrect");
  require((policy.stats().horizon_pairs>0)==(horizon>100),"horizon ran outside configured interval");
  // The independent oldest admission must still take the old, impossible task.
  policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{0}&&policy.stats().fair_assignments==1,"horizon changed oldest fair admission");
  require(test.curr_task_schedule==std::vector<int>{-1}&&test.task_pool.at(0).agent_assigned==-1&&test.task_pool.at(1).idx_next_loc==0,
   "horizon changed simulator task metadata");
  test.curr_task_schedule={0};test.task_pool.at(0).agent_assigned=0;test.goal_locations={{{goal,0}}};
  for(int started:{0,1}){test.task_pool.at(0).idx_next_loc=started;test.goal_locations={{{started?far:goal,0}}};
   policy.schedule(&test,30000,schedule);require(schedule==std::vector<int>{0}&&test.task_pool.at(0).idx_next_loc==started,
    "horizon redirected a held or started task");}
  ++cases;
 }
 // One service per tick: three copies of a location need one more tick than two.
 setenv("CGAR_TRICK_NATIVE_METRIC","1",1);setenv("CGAR_TRICK_NATIVE_BANDS","1",1);setenv("CGAR_FLOW_COST_SCALE","20",1);
 setenv("CGAR_SCHED_TABLES","128",1);setenv("CGAR_TRICK_KNOWN_HORIZON","112",1);
 for(int copies:{1,2,3}){
  auto test=base;Task old;old.task_id=0;old.t_revealed=0;old.locations={goal,far};test.task_pool.emplace(0,old);
  Task fresh;fresh.task_id=1;fresh.t_revealed=100;fresh.locations.assign(copies,goal);test.task_pool.emplace(1,fresh);
  Cgar policy;policy.initialize(&test,30000);std::vector<int> schedule;policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{copies<=2?1:0},"horizon failed repeated-service lower bound");
 }
 // Already at the pickup: two same-cell errands need two ticks, not zero.
 for(int horizon:{101,102}){
  setenv("CGAR_TRICK_KNOWN_HORIZON",std::to_string(horizon).c_str(),1);auto test=base;test.curr_states[0].location=goal;
  Task old;old.task_id=0;old.t_revealed=0;old.locations={goal,far};test.task_pool.emplace(0,old);
  Task fresh;fresh.task_id=1;fresh.t_revealed=100;fresh.locations={goal,goal};test.task_pool.emplace(1,fresh);
  Cgar policy;policy.initialize(&test,30000);std::vector<int> schedule;policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{horizon==102?1:0},"horizon ignored first same-cell service tick");
 }
 // A real shelf blocks the four-step Manhattan route. The six-step route
 // above it is free; grid parity proves the spatial shortest distance is six.
 for(int cell:{10008,10009,10010})require(base.map[cell],"horizon detour obstacle fixture changed");
 for(int cell:{10007,9507,9508,9509,9510,9511,10011})require(!base.map[cell],"horizon detour route fixture changed");
 setenv("CGAR_TRICK_KNOWN_HORIZON","105",1);
 for(int tables:{0,128}){
  setenv("CGAR_SCHED_TABLES",std::to_string(tables).c_str(),1);auto test=base;test.curr_states[0].location=10007;
  Task old;old.task_id=0;old.t_revealed=0;old.locations={10011,10011};test.task_pool.emplace(0,old);
  Task fresh;fresh.task_id=1;fresh.t_revealed=100;fresh.locations={10007};test.task_pool.emplace(1,fresh);
  Cgar policy;policy.initialize(&test,30000);std::vector<int> schedule;policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{tables?1:0},"resident BFS did not change horizon tier across a shelf detour");
  require((policy.stats().horizon_first_rank_change==100)==bool(tables),"detour tier did not use the available spatial bound");
 }
 // Both pickups lie beyond the fixed2048-node discovery radius. The oldest
 // fallback is impossible; sampled replenishment must retain the feasible pair.
 setenv("CGAR_SCHED_TABLES","128",1);setenv("CGAR_PICKUP_FLOW","0",1);setenv("CGAR_PICKUP_FULL_ROBOTS","0",1);
 setenv("CGAR_TRICK_KNOWN_HORIZON","547",1);
 {
  auto test=base;for(int cell=1504;cell<=1950;++cell)require(!test.map[cell],"horizon fallback corridor fixture changed");
  Task old;old.task_id=0;old.t_revealed=0;old.locations={1950,1504};test.task_pool.emplace(0,old);
  Task fresh;fresh.task_id=1;fresh.t_revealed=100;fresh.locations={1950,1950};test.task_pool.emplace(1,fresh);
  Cgar policy;policy.initialize(&test,30000);std::vector<int> schedule;policy.schedule(&test,30000,schedule);
  require(schedule==std::vector<int>{1}&&policy.stats().empty_searches>0&&policy.stats().fallback_assignments==1&&
   policy.stats().sample_evaluations==2&&policy.stats().improved_fallbacks==1&&policy.stats().horizon_first_rank_change==100,
   "empty-shortlist fallback sampling bypassed active horizon tier");
 }
 setenv("CGAR_PICKUP_FLOW","1",1);setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);
 setenv("CGAR_PICKUP_FULL_COST_KEY","1",1);rejects([&]{auto test=base;Cgar policy;policy.initialize(&test,30000);},"horizon accepted a different retention baseline");unsetenv("CGAR_PICKUP_FULL_COST_KEY");
 setenv("CGAR_REFINE_CHAIN_COSTS","1",1);rejects([&]{auto test=base;Cgar policy;policy.initialize(&test,30000);},"horizon accepted unreviewed refined chain basis");unsetenv("CGAR_REFINE_CHAIN_COSTS");
 for(auto setting:settings)unsetenv(setting.first);
 for(const char* key:{"CGAR_TRICK_KNOWN_HORIZON","CGAR_TRICK_NATIVE_METRIC","CGAR_TRICK_NATIVE_BANDS","CGAR_FLOW_COST_SCALE","CGAR_SCHED_TABLES"})unsetenv(key);
 std::cout<<"KNOWN_HORIZON passed metric_table_boundary_cases="<<cases<<" repeated_service_cases=5 active_shelf_detour_cases=2 empty_shortlist_sampled_fallback=1 first_rank_change=1 at_and_past_horizon_ordinary=1 oldest_admission_unchanged=1 held_started_protected=1 task_metadata_unchanged=1 explicit_cli=1 malformed_and_basis_guards=11\n";
}

void horizon_percentile_regression() {
 auto require=[](bool x,const char* message){if(!x)throw std::runtime_error(message);};
 auto rejects=[&](auto run){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,"horizon percentile activation guard failed");};
 const char* option="CGAR_TRICK_HORIZON_MARGIN_PERCENTILE";
 setenv(option,"0",1);rejects([&]{tricks::options("");});
 setenv(option,"75",1);rejects([&]{tricks::options("WAREHOUSE");});
 setenv("CGAR_TRICK_HORIZON_MARGIN","1",1);rejects([&]{tricks::options("WAREHOUSE");});
 setenv("CGAR_TRICK_KNOWN_HORIZON","5000",1);
 for(const char* bad:{"","-1","101","1000","1.5","true"," 75"}){setenv(option,bad,1);rejects([&]{tricks::options("WAREHOUSE");});}
 for(int q:{0,1,50,75,90,100}){const auto value=std::to_string(q);setenv(option,value.c_str(),1);require(tricks::options("WAREHOUSE").horizon_margin_percentile==q,"horizon percentile option changed its value");}
 unsetenv(option);unsetenv("CGAR_TRICK_HORIZON_MARGIN");unsetenv("CGAR_TRICK_KNOWN_HORIZON");
 for(int bad:{-1,101})rejects([&]{HorizonMargins m;m.configure_percentile(bad);});
 int checked=0;
 // Train through the real prospective ledger on skewed, duplicate observations.
 // Offline sorted samples give nearest ranks1,3,4,5 of [0,1,1,4,20].
 for(int q:{0,1,50,75,90,100}){
  HorizonMargins model;model.configure_percentile(q);
  SharedEnvironment e;e.num_of_agents=26;e.curr_timestep=0;e.curr_task_schedule.assign(26,-1);e.curr_states.assign(26,State(0,0,0));model.observe(e);
  std::vector<int> bounds,finish,proposal;
  for(int bound:{1,50,100,200,400})for(int excess:{0,1,1,4,20}){const int id=bounds.size();bounds.push_back(bound);finish.push_back(bound+excess);proposal.push_back(id);Task t;t.task_id=id;t.locations={0};e.task_pool.emplace(id,t);}
  bounds.push_back(1);finish.push_back(10000);proposal.push_back(25);Task pending;pending.task_id=25;pending.locations={0};e.task_pool.emplace(25,pending);
  model.proposed(e,proposal,[&](int,int id){return bounds[id];});e.curr_task_schedule=proposal;for(int id=0;id<26;++id)e.task_pool.at(id).agent_assigned=id;
  const auto cold=model.snapshot();
  for(int tick=1;tick<=420;++tick){e.curr_timestep=tick;for(int id=0;id<26;++id)if(finish[id]==tick){e.task_pool.erase(id);e.curr_task_schedule[id]=-1;}model.observe(e);model.observe(e);}
  const auto learned=model.snapshot();require(learned.percentile==q&&model.tracked()==1,"percentile mode or pending ledger was lost");
  for(int k=0;k<5;++k){const int bound=bounds[k*5];const int cutoff=q==0?6:q==1?0:q==50?1:q==75?4:20;
   require(learned.count[k]==5&&learned.excess[k]==26,"percentile trained on unfinished task or repeated observation");
   require(!q||learned.cutoff[k]==cutoff,"empirical nearest-rank cutoff differs from sorted samples");
   require(learned.tier(bound,bound+cutoff)==0,"percentile equality must be feasible");
   require(learned.tier(bound,bound-1)==2,"percentile weakened the physical bound");
   if(cutoff)require(learned.tier(bound,bound+cutoff-1)==1,"percentile risk boundary lost a tick");
   require(cold.count[k]==0&&cold.tier(bound,bound)==0,"percentile snapshot changed after training");++checked;
  }
  bool rejected=false;try{model.configure_percentile(q);}catch(const std::logic_error&){rejected=true;}require(rejected,"trained percentile can be silently reconfigured");
 }
 // Retargeted tasks remain excluded even when the histogram is enabled.
 {HorizonMargins model;model.configure_percentile(75);SharedEnvironment e;e.num_of_agents=2;e.curr_timestep=0;e.curr_states.assign(2,State(0,0,0));e.curr_task_schedule={-1,-1};model.observe(e);
  Task t;t.task_id=0;t.locations={0};e.task_pool.emplace(0,t);model.proposed(e,{0,-1},[](int,int){return 1;});e.curr_task_schedule={0,-1};e.task_pool.at(0).agent_assigned=0;
  e.curr_timestep=1;model.observe(e);model.proposed(e,{-1,0},[](int,int){return 1;});e.curr_task_schedule={-1,0};e.task_pool.at(0).agent_assigned=1;
  e.curr_timestep=2;model.observe(e);e.curr_timestep=3;e.task_pool.clear();e.curr_task_schedule={-1,-1};model.observe(e);
  const auto m=model.snapshot();require(m.count[0]==0&&m.cutoff[0]==0&&model.invalidated==1&&model.excluded_completions==1,"retargeted duration entered percentile histogram");}
 std::cout<<"HORIZON_PERCENTILE passed bucket_rank_checks="<<checked<<" skewed_samples=1 duplicates=1 exact_equality=1 prospective_only=1 immutable_snapshot=1 retarget_excluded=1 configuration_guards=1\n";
}

void horizon_margin_regression() {
 auto require=[](bool x,const char* message){if(!x)throw std::runtime_error(message);};
 auto rejects=[&](auto run){bool rejected=false;try{run();}catch(const std::invalid_argument&){rejected=true;}require(rejected,"horizon margin activation guard failed");};
 setenv("CGAR_TRICK_HORIZON_MARGIN","0",1);rejects([&]{cgar::tricks::options("");});
 setenv("CGAR_TRICK_HORIZON_MARGIN","1",1);rejects([&]{cgar::tricks::options("WAREHOUSE");});
 setenv("CGAR_TRICK_KNOWN_HORIZON","26",1);
 for(const char* bad:{"","-1","2","true"}){setenv("CGAR_TRICK_HORIZON_MARGIN",bad,1);rejects([&]{cgar::tricks::options("WAREHOUSE");});}
 unsetenv("CGAR_TRICK_HORIZON_MARGIN");unsetenv("CGAR_TRICK_KNOWN_HORIZON");
 auto empty=[](int robots){SharedEnvironment e;e.num_of_agents=robots;e.curr_timestep=0;e.curr_task_schedule.assign(robots,-1);e.curr_states.assign(robots,State(0,0,0));return e;};
 int buckets=0;
 for(int bound:{1,50,100,200,400}){
  HorizonMargins model;auto e=empty(1);model.observe(e);
  Task t;t.task_id=0;t.locations={0};e.task_pool.emplace(0,t);
  model.proposed(e,{0},[&](int robot,int id){return bound;});
  e.task_pool.at(0).agent_assigned=0;e.curr_task_schedule={0};
  const auto cold=model.snapshot();require(cold.tier(bound,bound)==0,"cold margin did not retain physical equality");
  for(int tick=1;tick<bound+3;++tick){e.curr_timestep=tick;model.observe(e);require(model.snapshot().count[HorizonMargins::Snapshot::bucket(bound)]==0,"margin used unfinished duration");}
  e.curr_timestep=bound+3;e.task_pool.clear();e.curr_task_schedule={-1};model.observe(e);model.observe(e);
  const auto warm=model.snapshot();const int bin=HorizonMargins::Snapshot::bucket(bound);
  require(warm.count[bin]==1&&warm.excess[bin]==3,"prospective completion duration or duplicate-observation accounting failed");
  require(warm.tier(bound,bound+3)==0&&warm.tier(bound,bound+2)==1&&warm.tier(bound,bound-1)==2,"margin inclusive tier boundary failed");
  require(cold.count[bin]==0&&cold.tier(bound,bound)==0,"margin snapshot changed during a comparison batch");++buckets;
 }
 // Rational means are compared without rounding down: (3+4)/2 needs four ticks.
 {HorizonMargins::Snapshot s;s.count[0]=2;s.excess[0]=7;require(s.tier(10,13)==1&&s.tier(10,14)==0,"fractional mean lost a tick");}
 // Retarget and drop/readmit histories must never re-enter calibration.
 for(bool drop:{false,true}){
  HorizonMargins model;auto e=empty(2);model.observe(e);Task task;task.task_id=0;task.locations={0};e.task_pool.emplace(0,task);
  model.proposed(e,{0,-1},[](int,int){return 1;});e.curr_task_schedule={0,-1};e.task_pool.at(0).agent_assigned=0;
  e.curr_timestep=1;model.observe(e);model.proposed(e,drop?std::vector<int>{-1,-1}:std::vector<int>{-1,0},[](int,int){return 1;});
  e.curr_task_schedule=drop?std::vector<int>{-1,-1}:std::vector<int>{-1,0};e.task_pool.at(0).agent_assigned=drop?-1:1;
  e.curr_timestep=2;model.observe(e);if(drop){model.proposed(e,{0,-1},[](int,int){return 1;});e.curr_task_schedule={0,-1};e.task_pool.at(0).agent_assigned=0;}
  e.curr_timestep=3;e.task_pool.clear();e.curr_task_schedule={-1,-1};model.observe(e);
  require(model.snapshot().count[0]==0&&model.invalidated==1&&model.excluded_completions==1,"retarget/drop trained the single-holder margin");
 }
 // A first proposal finalized on robot1 is a genuine single-holder admission.
 {HorizonMargins model;auto e=empty(2);model.observe(e);Task task;task.task_id=0;task.locations={0};e.task_pool.emplace(0,task);
  model.proposed(e,{-1,0},[](int r,int){return r==1?1:9;});e.curr_task_schedule={-1,0};e.task_pool.at(0).agent_assigned=1;
  e.curr_timestep=1;model.observe(e);e.curr_timestep=2;e.task_pool.clear();e.curr_task_schedule={-1,-1};model.observe(e);
  require(model.snapshot().count[0]==1&&model.snapshot().excess[0]==1,"margin recorded an intermediate proposal holder");}
 // Missing an observation cannot manufacture an exact completion timestamp.
 {HorizonMargins model;auto e=empty(1);model.observe(e);Task task;task.task_id=0;task.locations={0};e.task_pool.emplace(0,task);
  model.proposed(e,{0},[](int,int){return 1;});e.curr_timestep=2;e.task_pool.clear();model.observe(e);
  require(model.snapshot().count[0]==0&&model.excluded_completions==1,"gapped observations trained a guessed finish time");}
 // A sound physical bound must never exceed the observed single-holder duration.
 {HorizonMargins model;auto e=empty(1);model.observe(e);Task task;task.task_id=0;task.locations={0};e.task_pool.emplace(0,task);
  model.proposed(e,{0},[](int,int){return 2;});e.curr_timestep=1;e.task_pool.clear();bool failed=false;
  try{model.observe(e);}catch(const std::logic_error&){failed=true;}
  require(failed&&model.bound_violations==1&&model.snapshot().count[0]==0,"physical-bound violation was silently discarded");}
 // Real scheduler integration: bound11 task completes after14 ticks. The
 // learned +3 margin demotes an old bound12 task at R12, but admits bound1.
 const std::vector<std::pair<const char*,const char*>> settings={{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","128"},
  {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","4"},{"CGAR_PICKUP_FLOW","1"},
  {"CGAR_PICKUP_FULL_ROBOTS","1"},{"CGAR_PICKUP_FULL_THREADS","1"},{"CGAR_TRICK_LANES","1"},
  {"CGAR_TRICK_REMAINING_FLOW","1"},{"CGAR_PICKUP_WEIGHT","5"},{"CGAR_HRRN","1"},
  {"CGAR_TRICK_NATIVE_METRIC","1"},{"CGAR_TRICK_NATIVE_BANDS","1"},{"CGAR_FLOW_COST_SCALE","20"},
  {"CGAR_TRICK_KNOWN_HORIZON","26"},{"CGAR_SCHED_TABLES","128"}};
 for(auto setting:settings)setenv(setting.first,setting.second,1);
 for(int mode:{0,1,50,75,90}){
  const int margin=mode!=0,percentile=mode>=50?mode:0;const auto q=std::to_string(percentile);
  setenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE",q.c_str(),1);
  setenv("CGAR_TRICK_HORIZON_MARGIN",margin?"1":"0",1);SharedEnvironment e;e.trick_instance="WAREHOUSE";
  e.rows=cgar::tricks::warehouse_rows;e.cols=cgar::tricks::warehouse_cols;e.num_of_agents=2;
  for(int cell=0;cell<e.rows*e.cols;++cell)e.map.push_back(cgar::tricks::warehouse_masks[cell]=='x');
  for(int cell=1504;cell<=1526;++cell)require(!e.map[cell],"margin corridor fixture changed");
  e.curr_states={State(1504,0,0),State(1500,0,0)};e.curr_task_schedule={-1,99};e.goal_locations={{},{{1950,0}}};
  Task held;held.task_id=99;held.locations={1950};held.agent_assigned=1;e.task_pool.emplace(99,held);
  Task train;train.task_id=0;train.locations={1515};e.task_pool.emplace(0,train);
  Cgar policy;policy.initialize(&e,30000);std::vector<int> schedule;policy.schedule(&e,30000,schedule);
  require(schedule==std::vector<int>({0,99}),"margin training admission failed");e.task_pool.at(0).agent_assigned=0;e.curr_task_schedule=schedule;
  for(int tick=1;tick<14;++tick){e.curr_timestep=tick;e.curr_states[0].location=1504+std::max(0,tick-3);policy.schedule(&e,30000,schedule);require(schedule==std::vector<int>({0,99}),"margin changed held work while training");}
  e.curr_timestep=14;e.curr_states[0].location=1515;e.curr_task_schedule={-1,99};e.task_pool.erase(0);
  Task old;old.task_id=1;old.t_revealed=0;old.locations={1526,1526};e.task_pool.emplace(1,old);
  Task fresh;fresh.task_id=2;fresh.t_revealed=14;fresh.locations={1515};e.task_pool.emplace(2,fresh);
  policy.schedule(&e,30000,schedule);require(schedule==std::vector<int>({margin?2:1,99}),"observed margin did not change the feasible near-boundary admission");
  require(!policy.stats().fair_assignments,"margin activated oldest admission early");
  if(margin){const auto m=policy.horizon_margins().snapshot();require(m.count[0]==1&&m.excess[0]==3&&m.percentile==percentile&&(!percentile||m.cutoff[0]==3)&&policy.stats().horizon_margin_first_rank_change==14,"real margin observation or reversal timestamp failed");}
  // Fair admission still overrides the tiers on the next admission wave.
  policy.schedule(&e,30000,schedule);require(schedule==std::vector<int>({1,99})&&policy.stats().fair_assignments==1,"margin changed oldest fair admission");
  require(e.curr_task_schedule==std::vector<int>({-1,99})&&e.task_pool.at(1).agent_assigned==-1,"margin mutated simulator metadata");
 }
 setenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE","75",1);
 // Real native matching: an initially free robot receives a fresh task,
 // then matching hands it to another holder before TaskManager accepts it.
 setenv("CGAR_TRICK_HORIZON_MARGIN","1",1);setenv("CGAR_TRICK_KNOWN_HORIZON","10000",1);
 setenv("CGAR_TRICK_UNOPENED_MATCH","1",1);
 {
  SharedEnvironment e;e.trick_instance="WAREHOUSE";e.rows=cgar::tricks::warehouse_rows;e.cols=cgar::tricks::warehouse_cols;e.num_of_agents=3;
  for(int cell=0;cell<e.rows*e.cols;++cell)e.map.push_back(cgar::tricks::warehouse_masks[cell]=='x');
  const int base=1504;for(int k=0;k<=24;++k)require(!e.map[base+k],"margin matching corridor fixture changed");
  e.curr_states={State(base+24,0,0),State(base+11,0,2),State(base+1,0,0)};
  e.curr_task_schedule={0,-1,2};e.goal_locations={{{base+10,0}},{},{{base+1,0}}};
  for(int id=0;id<3;++id){Task task;task.task_id=id;task.t_revealed=-5;task.locations={id==0?base+10:base+1};task.agent_assigned=id==1?-1:id;e.task_pool.emplace(id,task);}
  Cgar policy;policy.initialize(&e,30000);std::vector<int> schedule;std::vector<Action> actions;
  policy.schedule(&e,30000,schedule);require(schedule==std::vector<int>({0,1,2}),"margin matching initial fresh admission failed");
  e.curr_task_schedule=schedule;e.task_pool.at(1).agent_assigned=1;e.goal_locations[1]={{{base+1,0}}};
  for(int tick=0;tick<10;++tick){e.curr_timestep=tick;if(tick)policy.schedule(&e,30000,schedule);
   if(tick<2){policy.plan(&e,30000,actions);auto next=step(e,e.curr_states,actions);require(!next.empty(),"margin matching warmup collided");e.curr_states=next;}}
  e.curr_timestep=10;e.curr_states[2].location=base+1;e.curr_task_schedule[2]=-1;e.goal_locations[2].clear();e.task_pool.erase(2);
  Task fresh;fresh.task_id=3;fresh.t_revealed=10;fresh.locations={base+10};e.task_pool.emplace(3,fresh);
  const int holder_cell=e.curr_states[1].location;
  std::vector<int> distance(e.map.size(),-1),queue{base+10};distance[base+10]=0;
  for(size_t h=0;h<queue.size();++h)for(int dir=0;dir<4;++dir){int v=nb(queue[h],dir,e.rows,e.cols);if(v>=0&&!e.map[v]&&distance[v]<0){distance[v]=distance[queue[h]]+1;queue.push_back(v);}}
  const int pickup=distance[holder_cell],bound=std::max(1,pickup);
  require(pickup==std::abs(holder_cell/e.cols-(base+10)/e.cols)+std::abs(holder_cell%e.cols-(base+10)%e.cols),"margin matching final-holder fixture has a hidden detour");
  policy.schedule(&e,30000,schedule);
  require(schedule==std::vector<int>({0,3,1})&&policy.stats().match_moved==2&&policy.horizon_margins().invalidated==1,"real matching did not invalidate displaced holder or move fresh admission");
  e.curr_task_schedule=schedule;for(int r=0;r<3;++r){auto& task=e.task_pool.at(schedule[r]);task.agent_assigned=r;e.goal_locations[r]={{{task.locations[0],10}}};}
  const int duration=pickup?bound+2:1;
  for(int dt=1;dt<=duration;++dt){e.curr_timestep=10+dt;
   if(dt==1){e.task_pool.erase(1);e.curr_task_schedule[2]=-1;e.goal_locations[2].clear();}
   if(dt==duration){e.curr_states[1].location=base+10;e.task_pool.erase(3);e.curr_task_schedule[1]=-1;e.goal_locations[1].clear();}
   policy.schedule(&e,30000,schedule);
  }
  const auto learned=policy.horizon_margins().snapshot();
  require(learned.count[0]==1&&learned.excess[0]==duration-bound&&learned.percentile==75&&learned.cutoff[0]==duration-bound&&policy.horizon_margins().excluded_completions==2&&policy.horizon_margins().bound_violations==0,
   "real matching trained displaced task or used fresh robot's pre-matching bound");
 }
 unsetenv("CGAR_TRICK_UNOPENED_MATCH");
 setenv("CGAR_TRICK_HORIZON_MARGIN","1",1);setenv("CGAR_REASSIGN_POOL","1",1);
 {SharedEnvironment e;e.trick_instance="WAREHOUSE";e.rows=cgar::tricks::warehouse_rows;e.cols=cgar::tricks::warehouse_cols;e.num_of_agents=1;
  for(int cell=0;cell<e.rows*e.cols;++cell)e.map.push_back(cgar::tricks::warehouse_masks[cell]=='x');e.curr_states={State(1504,0,0)};e.curr_task_schedule={-1};e.goal_locations={{}};
  rejects([&]{Cgar p;p.initialize(&e,30000);});}
 unsetenv("CGAR_TRICK_HORIZON_MARGIN_PERCENTILE");unsetenv("CGAR_REASSIGN_POOL");unsetenv("CGAR_TRICK_HORIZON_MARGIN");for(auto setting:settings)unsetenv(setting.first);
 std::cout<<"HORIZON_MARGIN passed bucket_boundaries="<<buckets<<" fractional_mean=1 prospective_only=1 no_double_count=1 immutable_snapshot=1 retarget_and_drop_excluded=1 final_proposal_holder=1 gap_excluded=1 bound_violation_fails=1 real_fresh_matching_ledger=1 real_scheduler_margin=1 fair_and_held_unchanged=1 activation_guards=7\n";
}

void chain_flow_pricing_regression() {
 auto check=[](bool condition,const char* message){if(!condition)throw std::runtime_error(message);};
 auto deadline=[](){return std::chrono::steady_clock::now()+std::chrono::seconds(5);};
 Certificate cert;cert.rows=1;cert.cols=4;cert.free=cert.core=std::vector<char>(4,true);cert.pocket.assign(4,-1);
 TurnDistanceOracle oracle;oracle.init(&cert,2*4*4*sizeof(int),4,true,4);
 std::vector<uint8_t> costs(16,4);costs[1*4+2]=12;oracle.set_forward_costs(costs);
 Task task;task.task_id=17;task.locations={3,0};
 check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::MissingTable,"chain priced absent table");
 oracle.table(0,deadline());
 auto quote=resident_chain_price(task,oracle,cert);
 check(quote.status==ChainPriceStatus::Covered&&quote.cost==20,"chain toll hand count differs");
 task.locations={3,0,2};check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::MissingTable,"partial chain treated as complete");
 oracle.table(2,deadline());quote=resident_chain_price(task,oracle,cert);
 check(quote.cost==28,"relaxed two-leg chain hand count differs");
 // A read of goal0 must not rescue it from eviction behind the newer goal2.
 task.locations={3,0};resident_chain_price(task,oracle,cert);oracle.table(3,deadline());oracle.trim();
 check(!oracle.peek(0)&&oracle.peek(2)&&oracle.peek(3),"chain pricing changed LRU/admission");
 oracle.table(0,deadline());costs[1*4+2]=4;check(oracle.set_forward_costs(costs),"chain refresh fixture metric did not change");
 check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::MissingTable,"stale chain quote survived metric update");
 oracle.table(0,deadline());check(resident_chain_price(task,oracle,cert).cost==12,"rebuilt chain retained old toll");
 oracle.clear_tables();task.locations={0,0,0};check(resident_chain_price(task,oracle,cert).cost==0,"repeated stops required a table");
 for(const auto& stops:std::vector<std::vector<int>>{{},{-1,0},{0,4},{0,0,-1}}){task.locations=stops;check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::Invalid,"malformed chain index did not fall back");}
 task.locations={0};task.idx_next_loc=-1;check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::Invalid,"negative chain stop accepted");
 task.idx_next_loc=1;check(resident_chain_price(task,oracle,cert).status==ChainPriceStatus::Invalid,"past-end chain stop accepted");task.idx_next_loc=0;
 auto outside=cert;outside.core[3]=false;outside.pocket[3]=7;task.locations={3,0};
 check(resident_chain_price(task,oracle,outside).status==ChainPriceStatus::OutsideDomain,"excluded pocket leg used a chain quote");
 outside=cert;outside.free[0]=false;check(resident_chain_price(task,oracle,outside).status==ChainPriceStatus::OutsideDomain,"obstacle stop accepted");
 Certificate split;split.rows=1;split.cols=5;split.free=split.core={1,1,0,1,1};split.pocket.assign(5,-1);
 TurnDistanceOracle disconnected;disconnected.init(&split,1<<20);disconnected.table(4,deadline());task.locations={0,4};
 check(resident_chain_price(task,disconnected,split).status==ChainPriceStatus::Unreachable,"disconnected resident leg accepted");
 ResidentChainPrice covered{42,ChainPriceStatus::Covered},missing;
 check(selected_chain_price(2,12,covered,3,2)==42&&selected_chain_price(3,12,covered,3,2)==18&&selected_chain_price(4,12,covered,3,2)==12,"ratio control leaked individual price");
 check(selected_chain_price(1,12,missing,3,2)==12&&selected_chain_price(2,12,missing,3,2)==18&&imputed_chain_price(5,3,2)==8,"whole-chain imputation or ceiling incorrect");
 check(imputed_chain_price(12,0,0)==12&&imputed_chain_price(kInf-1,9223372036854775807LL,1)==kInf-1,"imputation fallback or saturation incorrect");
 for(auto bad:std::vector<std::array<long long,3>>{{-1,1,1},{1,-1,1},{1,1,-1}}){bool rejected=false;try{imputed_chain_price(int(bad[0]),bad[1],bad[2]);}catch(const std::invalid_argument&){rejected=true;}check(rejected,"negative ratio accepted");}

 setenv("CGAR_TEMPORAL","1",1);setenv("CGAR_TEMPORAL_STEPS","128",1);setenv("CGAR_ORIENTATION_GUIDANCE","1",1);
 setenv("CGAR_PICKUP_FLOW","1",1);setenv("CGAR_PICKUP_FULL_ROBOTS","1",1);setenv("CGAR_PICKUP_FULL_THREADS","1",1);
 setenv("CGAR_FLOW_WARMUP","1",1);setenv("CGAR_FLOW_MIN_SAMPLES","1",1);setenv("CGAR_FLOW_MIN_MARGIN_PERCENT","0",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","0",1);
 auto blank=[](){SharedEnvironment e;e.rows=5;e.cols=7;e.num_of_agents=1;e.map.assign(35,0);e.curr_states={State(17,0,0)};e.curr_task_schedule={-1};e.goal_locations.resize(1);return e;};
 long long production_cases=0;
 setenv("CGAR_SCHED_TABLES","0",1);
 for(int refine:{0,1})for(int scale:{1,4})for(int coverage:{0,1,2})for(int mode:{0,1,2,3,4}){
  setenv("CGAR_REFINE_CHAIN_COSTS",std::to_string(refine).c_str(),1);
  setenv("CGAR_FLOW_COST_SCALE",std::to_string(scale).c_str(),1);setenv("CGAR_FLOW_STRENGTH",std::to_string(scale).c_str(),1);
  setenv("CGAR_CHAIN_FLOW_PRICING",std::to_string(mode).c_str(),1);auto e=blank();Cgar c;c.initialize(&e,1000);
  std::vector<Action> offered;const std::array<Action,3> observed{Action::FW,Action::CR,Action::CR};
  for(int tick=0;tick<3;++tick){e.curr_timestep=tick;c.plan(&e,1000,offered);auto next=step(e,e.curr_states,{observed[tick]});check(!next.empty(),"chain observation fixture collision");e.curr_states=next;}
  for(int k=0;k<coverage;++k){e.curr_timestep=3+k;e.goal_locations={{{k?11:17,0}}};c.plan(&e,1000,offered);auto next=step(e,e.curr_states,offered);check(!next.empty(),"chain table-preparation action collided");e.curr_states=next;}
  check(c.stats().flow_publications==1&&c.stats().flow_penalized_edges>0,"chain assignment fixture lacked active toll");
  e.curr_timestep=5;e.goal_locations={{}};
  for(int id=0;id<2;++id){Task t;t.task_id=id;t.t_revealed=5;t.locations={18,id?11:17};e.task_pool.emplace(id,t);}
  const auto builds=c.stats().oriented_builds;std::vector<int> proposed;c.schedule(&e,1000,proposed);
  const int wanted=(coverage==2&&(mode==1||mode==2))||(coverage==1&&mode==1)?1:0;
  check(proposed==std::vector<int>{wanted},"chain pricing production choice disagrees with hand costs");
  check(c.stats().oriented_builds==builds&&e.curr_task_schedule==std::vector<int>{-1}&&e.task_pool.at(0).agent_assigned==-1,"chain pricing changed cache builds or simulator metadata");
  if(mode){check(c.stats().chain_price_outcomes[0]==coverage&&c.stats().chain_price_outcomes[1]==2-coverage,"chain coverage counters differ from fixture");
   check(c.stats().chain_price_numerator==(coverage==2?3*scale:coverage==1?2*scale:0)&&c.stats().chain_price_denominator==coverage*scale,"production snapshot ratio differs from hand count");
   check(c.stats().chain_price_basis==std::array<long long,4>{coverage,0,0,2-coverage},"native chain basis differs from resident/approximate fixture");
   check(c.stats().chain_price_assignments==1&&c.stats().chain_price_ratio_calls==(coverage>0),"chain diagnostic denominators incorrect");}
  if(mode==4){check(c.stats().chain_shadow_queries==1&&c.stats().chain_shadow_changed2==(coverage==2)&&c.stats().chain_shadow_changed3==0&&c.stats().chain_shadow_specific==(coverage==2),"shadow shortlist ranking differs from policy hand count");
   check(c.stats().chain_shadow_choices==std::array<long long,5>{coverage!=2,coverage==2,0,0,0}&&c.stats().chain_shadow_small==0,"shadow choice categories differ from hand count");}
  c.schedule(&e,1000,proposed);check(proposed==std::vector<int>{0}&&c.stats().fair_assignments==1,"chain pricing bypassed oldest-task admission");
  e.curr_task_schedule={0};e.task_pool.at(0).agent_assigned=0;e.task_pool.at(0).idx_next_loc=1;e.goal_locations={{{17,0}}};
  c.schedule(&e,1000,proposed);check(proposed==std::vector<int>{0}&&e.task_pool.at(0).idx_next_loc==1,"chain pricing changed a started task");++production_cases;
 }
 // Exercise the complete scheduler across a real field publication. Valid
 // externally executed moves cancel flow on the priced edge; a goal-free
 // publication cannot immediately rebuild the flushed destination tables.
 // Quotes must fall back as a whole, then reflect the new metric after normal
 // planner table preparation. No cache or planner internals are modified.
 long long refresh_cases=0;
 setenv("CGAR_FLOW_REFRESH_INTERVAL","8",1);
 for(int refine:{0,1})for(int scale:{1,4})for(int mode:{0,1,2,3,4}){
  setenv("CGAR_REFINE_CHAIN_COSTS",std::to_string(refine).c_str(),1);
  setenv("CGAR_FLOW_COST_SCALE",std::to_string(scale).c_str(),1);setenv("CGAR_FLOW_STRENGTH",std::to_string(scale).c_str(),1);
  setenv("CGAR_CHAIN_FLOW_PRICING",std::to_string(mode).c_str(),1);auto e=blank();Cgar c;c.initialize(&e,1000);
  auto observe=[&](int tick,int goal,Action executed){
   e.curr_timestep=tick;e.goal_locations={{}};if(goal>=0)e.goal_locations[0].push_back({goal,0});
   std::vector<Action> offered;c.plan(&e,1000,offered);auto next=step(e,e.curr_states,{executed});
   check(!next.empty(),"chain refresh observation fixture collision");e.curr_states=next;
  };
  observe(0,-1,Action::FW);observe(1,-1,Action::CR);observe(2,-1,Action::CR);
  observe(3,17,Action::FW);observe(4,11,Action::W);
  check(c.stats().flow_publications==1&&c.stats().flow_cache_resets==1&&c.stats().flow_penalized_edges==1,"chain refresh fixture missed first tolled publication");
  e.curr_timestep=5;e.goal_locations={{}};
  for(int id=0;id<2;++id){Task t;t.task_id=id;t.t_revealed=5;t.locations={18,id?11:17};e.task_pool.emplace(id,t);}
  std::vector<int> proposed;auto builds=c.stats().oriented_builds;c.schedule(&e,1000,proposed);
  check(proposed==std::vector<int>{mode==1||mode==2?1:0}&&c.stats().oriented_builds==builds,"pre-refresh chain choice or table work differs from hand count");
  if(mode&&!(c.stats().chain_price_outcomes[0]==2&&c.stats().chain_price_numerator==3*scale&&c.stats().chain_price_denominator==2*scale))
   throw std::runtime_error("pre-refresh production quotes differ: refine="+std::to_string(refine)+" scale="+std::to_string(scale)+" mode="+std::to_string(mode)+
    " covered="+std::to_string(c.stats().chain_price_outcomes[0])+" numerator="+std::to_string(c.stats().chain_price_numerator)+" denominator="+std::to_string(c.stats().chain_price_denominator));
  observe(5,-1,Action::FW);for(int tick=6;tick<=9;++tick)observe(tick,-1,Action::W);
  check(c.stats().flow_publications==2&&c.stats().flow_cache_resets==2&&c.stats().flow_penalized_edges==1&&c.stats().oriented_builds==builds,"chain refresh did not flush into an unrebuilt changed field");
  e.curr_timestep=10;const auto before=c.stats();c.schedule(&e,1000,proposed);
  check(proposed==std::vector<int>{0}&&c.stats().oriented_builds==builds,"uncovered production fallback changed native choice or built tables");
  if(mode)check(c.stats().chain_price_outcomes[0]==before.chain_price_outcomes[0]&&c.stats().chain_price_outcomes[1]==before.chain_price_outcomes[1]+2&&
    c.stats().chain_price_numerator==0&&c.stats().chain_price_denominator==0&&c.stats().chain_price_changed==before.chain_price_changed,"production pricing retained stale quotes or failed native fallback after refresh");
  observe(10,17,Action::CR);observe(11,17,Action::CR);observe(12,17,Action::FW);observe(13,11,Action::W);
  e.curr_timestep=14;e.goal_locations={{}};builds=c.stats().oriented_builds;const auto rebuilt=c.stats();c.schedule(&e,1000,proposed);
  check(proposed==std::vector<int>{0}&&c.stats().oriented_builds==builds,"rebuilt neutral chain choice or scheduling table work differs from native");
  if(mode)check(c.stats().chain_price_outcomes[0]==rebuilt.chain_price_outcomes[0]+2&&c.stats().chain_price_numerator==2*scale&&c.stats().chain_price_denominator==2*scale&&
    c.stats().chain_price_changed==rebuilt.chain_price_changed,"rebuilt production chain quotes retained the previous toll");
  ++refresh_cases;
 }
 setenv("CGAR_FLOW_REFRESH_INTERVAL","0",1);
 unsetenv("CGAR_SCHED_TABLES");
 setenv("CGAR_FLOW_COST_SCALE","1",1);setenv("CGAR_FLOW_STRENGTH","1",1);setenv("CGAR_CHAIN_FLOW_PRICING","4",1);setenv("CGAR_PICKUP_FULL_ROBOTS","0",1);
 for(const auto& bad:std::vector<std::pair<const char*,const char*>>{{"CGAR_CHAIN_FLOW_PRICING","-1"},{"CGAR_CHAIN_FLOW_PRICING","5"},{"CGAR_PICKUP_FLOW","0"},{"CGAR_REASSIGN","1"},{"CGAR_REASSIGN_POOL","1"},{"CGAR_TEMPORAL_REMAINING_FLOW","1"}}){
  setenv(bad.first,bad.second,1);bool rejected=false;try{auto e=blank();Cgar c;c.initialize(&e,1000);}catch(const std::invalid_argument& error){rejected=std::string(error.what()).find("chain flow pricing")!=std::string::npos;}
  check(rejected,"incompatible chain pricing configuration accepted");unsetenv(bad.first);setenv("CGAR_PICKUP_FLOW","1",1);setenv("CGAR_CHAIN_FLOW_PRICING","4",1);
 }
 // Closed-loop shadow and default must execute exactly the same assignments,
 // actions, table-build work and publications, including repeated refreshes.
 setenv("CGAR_FLOW_WARMUP","4",1);setenv("CGAR_FLOW_REFRESH_INTERVAL","16",1);setenv("CGAR_PICKUP_FULL_ROBOTS","4",1);setenv("CGAR_PICKUP_FULL_THREADS","4",1);
 struct Trace {std::vector<int> values;Stats stats;};
 auto run=[&](int mode,int refine){
  setenv("CGAR_CHAIN_FLOW_PRICING",std::to_string(mode).c_str(),1);setenv("CGAR_REFINE_CHAIN_COSTS",std::to_string(refine).c_str(),1);
  SharedEnvironment e;e.rows=7;e.cols=9;e.map.assign(63,0);e.num_of_agents=12;e.curr_task_schedule.assign(12,-1);e.goal_locations.resize(12);
  for(int r=0;r<12;++r)e.curr_states.emplace_back(r*5,0,r%4);
  int next_id=0;auto add_task=[&](){Task t;t.task_id=next_id++;t.t_revealed=e.curr_timestep;t.locations={(t.task_id*11+17)%63,(t.task_id*23+8)%63};e.task_pool.emplace(t.task_id,t);};
  for(int k=0;k<36;++k)add_task();Cgar c;c.initialize(&e,1000);Trace trace;
  for(int tick=0;tick<96;++tick){e.curr_timestep=tick;std::vector<int> proposed;c.schedule(&e,1000,proposed);
   std::set<int> assigned;for(int r=0;r<12;++r){check(proposed[r]>=0&&assigned.insert(proposed[r]).second,"chain shadow omitted or duplicated assignment");const int old=e.curr_task_schedule[r];if(old>=0&&e.task_pool.at(old).idx_next_loc>0)check(proposed[r]==old,"chain shadow redirected started task");}
   for(auto& item:e.task_pool)item.second.agent_assigned=-1;
   for(int r=0;r<12;++r){e.curr_task_schedule[r]=proposed[r];auto& task=e.task_pool.at(proposed[r]);task.agent_assigned=r;e.goal_locations[r]={{{task.locations[task.idx_next_loc],0}}};trace.values.push_back(proposed[r]);}
   std::vector<Action> actions;c.plan(&e,1000,actions);auto next=step(e,e.curr_states,actions);check(!next.empty(),"chain shadow trajectory collided");e.curr_states=next;
   for(int r=0;r<12;++r){trace.values.push_back(int(actions[r]));auto& task=e.task_pool.at(e.curr_task_schedule[r]);if(e.curr_states[r].location==task.locations[task.idx_next_loc]){
     ++task.idx_next_loc;if(task.idx_next_loc==int(task.locations.size())){const int id=task.task_id;e.curr_task_schedule[r]=-1;e.goal_locations[r].clear();e.task_pool.erase(id);add_task();}
     else e.goal_locations[r]={{{task.locations[task.idx_next_loc],0}}};}}
  }
  trace.stats=c.stats();return trace;
 };
 for(int refine:{0,1}){const auto control=run(0,refine),shadow=run(4,refine);
  check(control.values==shadow.values&&control.stats.oriented_builds==shadow.stats.oriented_builds&&control.stats.flow_publications==shadow.stats.flow_publications&&control.stats.estimated_chain_cost==shadow.stats.estimated_chain_cost,"chain shadow changed closed-loop trajectory or cache work");
  check(shadow.stats.chain_shadow_queries>0&&shadow.stats.flow_publications>1&&shadow.stats.flow_penalized_edges>0&&shadow.stats.chain_price_changed==0&&
        shadow.stats.chain_price_outcomes[0]>0&&shadow.stats.chain_price_outcomes[1]>0&&shadow.stats.chain_price_ratio_calls>0,"chain shadow closed-loop fixture was vacuous");
  for(auto member:{&Stats::assignments,&Stats::fair_assignments,&Stats::pickup_full_fields,&Stats::pickup_full_pops,&Stats::pickup_full_scans,
                   &Stats::pickup_flow_cached_estimates,&Stats::pickup_flow_approximate_estimates,&Stats::route_manhattan,&Stats::estimated_pickup_cost})
   check(control.stats.*member==shadow.stats.*member,"chain shadow changed baseline work counters");
  check(std::accumulate(shadow.stats.chain_shadow_choices.begin(),shadow.stats.chain_shadow_choices.end(),0LL)==shadow.stats.chain_shadow_queries,"shadow category counts do not conserve queries");}
 for(const char* key:{"CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_ORIENTATION_GUIDANCE","CGAR_PICKUP_FLOW","CGAR_PICKUP_FULL_ROBOTS","CGAR_PICKUP_FULL_THREADS","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL","CGAR_FLOW_COST_SCALE","CGAR_FLOW_STRENGTH","CGAR_CHAIN_FLOW_PRICING","CGAR_REFINE_CHAIN_COSTS"})unsetenv(key);
 std::cout<<"CHAIN_FLOW_PRICING passed production_cases="<<production_cases<<" production_refresh_cases="<<refresh_cases<<" hand_tolls=1 partial_fallback=1 ratio_only_purity=1 cache_lru_unchanged=1 refresh_invalidation=1 fairness_started_protected=1 shadow_exact_robot_steps=2304 shadow_collision_checks=4608 refined_and_legacy_native=1\n";
}


void match_horizon_guard_regression() {
 auto require=[](bool value,const char* message){if(!value)throw std::runtime_error(message);};
 const std::vector<int> costs{10,0,1,10};
 const auto permutation=minimum_pickup_permutation(costs,2,100000,[]{});
 auto run=[&](const std::vector<int>& tiers){
  auto cycles=pickup_permutation_cycles(costs,permutation,1,[]{});
  auto stats=guard_pickup_cycle_tiers(cycles,permutation,[&](int r,int c){return tiers.at(r*2+c);},[]{});
  return std::make_pair(cycles,stats);
 };
 const auto keep=run({0,1,0,2}),veto=run({0,1,1,2});
 require(keep.first.size()==1&&keep.first[0].accepted&&keep.second.cycles==1&&keep.second.rows==2&&keep.second.rejected==0,
  "matching horizon compared a new task with the row's old task instead of its old holder");
 require(!veto.first[0].accepted&&veto.second.rejected==1&&veto.second.worse_rows==1&&
  veto.first[0].before==20&&veto.first[0].after==1,"matching horizon failed whole-cycle veto or changed cost");
 AssignmentPermutation two;two.column={1,0,3,2};
 std::vector<PickupPermutationCycle> cycles{{{0,1},20,1,true},{{2,3},20,1,true}};
 const auto stats=guard_pickup_cycle_tiers(cycles,two,[](int row,int col){return int(row==1&&col==0);},[]{});
 require(!cycles[0].accepted&&cycles[1].accepted&&stats.cycles==2&&stats.rows==4&&stats.rejected==1,
  "matching horizon veto affected an independent valid cycle");
 cycles={{{0,1},20,19,false}};
 const auto skipped=guard_pickup_cycle_tiers(cycles,permutation,[](int,int)->int{throw std::runtime_error("unaccepted cycle quoted");},[]{});
 require(skipped.cycles==0&&skipped.rows==0&&!cycles[0].accepted,"matching horizon quoted a threshold-rejected cycle");
 bool threw=false;cycles={{{0,1},20,1,true}};
 try{guard_pickup_cycle_tiers(cycles,permutation,[](int,int){return 0;},[]{throw Timeout("match_horizon_fixture");});}
 catch(const Timeout&){threw=true;}
 require(threw&&cycles[0].accepted,"matching horizon swallowed deadline or partially changed the first cycle");
 threw=false;try{run({0,3,0,2});}catch(const std::invalid_argument&){threw=true;}
 require(threw,"matching horizon accepted invalid tiers");
 auto rejects=[&](auto f){bool failed=false;try{f();}catch(const std::invalid_argument&){failed=true;}require(failed,"matching horizon activation guard failed");};
 const char* key="CGAR_TRICK_MATCH_HORIZON";
 setenv(key,"0",1);rejects([&]{tricks::options("");});
 setenv(key,"1",1);rejects([&]{tricks::options("WAREHOUSE");});
 setenv("CGAR_TRICK_KNOWN_HORIZON","128",1);rejects([&]{tricks::options("WAREHOUSE");});
 setenv("CGAR_TRICK_UNOPENED_MATCH","1",1);
 for(const char* value:{"","-1","2","true"}){setenv(key,value,1);rejects([&]{tricks::options("WAREHOUSE");});}
 setenv(key,"1",1);require(tricks::options("WAREHOUSE").match_horizon,"matching horizon did not activate");
 setenv(key,"0",1);require(!tricks::options("WAREHOUSE").match_horizon,"matching horizon zero did not preserve default");
 unsetenv(key);require(!tricks::options("WAREHOUSE").match_horizon,"matching horizon missing did not preserve default");
 unsetenv("CGAR_TRICK_KNOWN_HORIZON");unsetenv("CGAR_TRICK_UNOPENED_MATCH");
 std::cout<<"MATCH_HORIZON_GUARD passed task_column_reference=1 whole_cycle_veto=1 independent_cycle=1 threshold_rejected_skipped=1 deadline=1 invalid_tiers=1 activation_guards=7\n";
}

void fresh_pickup_audit_regression() {
 auto require=[](bool ok,const char* text){if(!ok)throw std::runtime_error(text);};
 const int inf=1000000;
 const std::vector<int> costs{10,0,1,10};
 auto clear=audit_fresh_pickup_permutation(costs,{0,1,0,2},2,inf,1,16,[]{});
 require(clear.horizon_excluded_pairs==0&&clear.guarded.size()==1&&clear.guarded[0].accepted&&
  clear.guarded[0].before-clear.guarded[0].after==19,"fresh audit compared feasibility with the receiving row instead of the task's original holder");
 auto blocked=audit_fresh_pickup_permutation(costs,{0,1,1,2},2,inf,1,16,[]{});
 require(blocked.horizon_excluded_pairs==1&&blocked.unrestricted.size()==1&&blocked.guarded.empty(),
  "fresh audit allowed a task's feasibility tier to worsen");
 const std::vector<int> cycle{20,1,50,50,20,1,1,50,20};
 auto three=audit_fresh_pickup_permutation(cycle,{},3,inf,1,16,[]{});
 require(three.guarded.size()==1&&three.guarded[0].rows.size()==3&&three.guarded[0].accepted&&
  three.guarded[0].before-three.guarded[0].after==57,"fresh audit missed an improving three-way cycle");
 require(audit_fresh_pickup_permutation({0,2,2,0},{},2,inf,1,16,[]{}).guarded.empty(),"fresh audit moved an optimal identity");
 bool timed_out=false;int checks=0;
 try{audit_fresh_pickup_permutation(cycle,{},3,inf,1,16,[&]{if(++checks==5)throw Timeout("fresh_audit_test");});}
 catch(const Timeout&){timed_out=true;}
 require(timed_out&&costs==std::vector<int>({10,0,1,10})&&cycle==std::vector<int>({20,1,50,50,20,1,1,50,20}),
  "fresh audit failed deadline propagation or mutated input costs");
 for(const std::vector<int> invalid:std::vector<std::vector<int>>{{0},{0,3,0,2}}){
  bool rejected=false;try{audit_fresh_pickup_permutation(costs,invalid,2,inf,1,16,[]{});}catch(const std::invalid_argument&){rejected=true;}
  require(rejected,"fresh audit accepted an invalid tier matrix");
 }
 for(const auto& setting:std::vector<std::pair<const char*,const char*>>{
  {"CGAR_DIAGNOSTICS","1"},{"CGAR_TEMPORAL","1"},{"CGAR_TEMPORAL_STEPS","128"},
  {"CGAR_ORIENTATION_GUIDANCE","1"},{"CGAR_FLOW_STRENGTH","1"},{"CGAR_FLOW_COST_SCALE","1"},
  {"CGAR_FLOW_WARMUP","1"},{"CGAR_FLOW_MIN_SAMPLES","1"},{"CGAR_FLOW_MIN_MARGIN_PERCENT","0"},
  {"CGAR_FLOW_REFRESH_INTERVAL","0"},{"CGAR_PICKUP_FLOW","1"},{"CGAR_HRRN","1"},
  {"CGAR_PICKUP_WEIGHT","1"},{"CGAR_PICKUP_FULL_ROBOTS","2"},{"CGAR_PICKUP_FULL_THREADS","1"},
  {"CGAR_REASSIGN_MATCH","1"}})setenv(setting.first,setting.second,1);
 auto blank=[](){SharedEnvironment e;e.rows=7;e.cols=41;e.num_of_agents=4;e.map.assign(e.rows*e.cols,0);
  e.curr_states={State(3*41+5,0,0),State(3*41+13,0,0),State(41+5,0,0),State(5*41+5,0,0)};
  e.curr_task_schedule={-1,-1,102,103};e.goal_locations.resize(4);
  for(int r:{2,3}){Task task;task.task_id=100+r;task.t_revealed=0;task.locations={e.curr_states[r].location,e.curr_states[r].location+25};
   task.idx_next_loc=1;task.agent_assigned=r;e.task_pool.emplace(task.task_id,task);e.goal_locations[r]={{{task.locations[1],0}}};}
  return e;};
 struct Trace{std::vector<int> schedule;Stats stats;FreshPickupShadowStats shadow;};
 auto run=[&](bool audit,int quota){
  setenv("CGAR_FRESH_PICKUP_AUDIT",audit?"1":"0",1);setenv("CGAR_PICKUP_FULL_ROBOTS",std::to_string(quota).c_str(),1);
  auto e=blank();Cgar policy;policy.initialize(&e,5000);std::vector<Action> actions;
  const std::array<Action,3> observed{Action::FW,Action::CR,Action::CR};
  for(int tick=0;tick<3;++tick){e.curr_timestep=tick;policy.plan(&e,5000,actions);
   auto next=step(e,e.curr_states,std::vector<Action>(4,observed[tick]));require(!next.empty(),"fresh pickup warmup trace collided");e.curr_states=next;}
  require(policy.stats().flow_publications>0,"fresh pickup test never published its metric");
  e.curr_timestep=3;
  for(int id=0;id<2;++id){Task task;task.task_id=id;task.t_revealed=3;task.locations={3*41+(id?26:11)};e.task_pool.emplace(id,task);}
  std::vector<int> proposed;policy.schedule(&e,5000,proposed);
  require(proposed==std::vector<int>({1,0,102,103}),"fresh pickup fixture did not expose the intended greedy crossing");
  const auto first=policy.fresh_pickup_shadow();
  if(audit&&quota==2)require(first.eligible==2&&first.groups==1&&first.witness_cycles==1&&first.witness_rows==2&&first.witness_saving>=4,
   "fresh pickup actual forward-field fixture missed a useful cycle");
  if(audit&&quota==1)require(first.missing==1&&first.eligible==1&&first.groups==0,"fresh pickup audit used an absent forward field");
  require(e.curr_task_schedule==std::vector<int>({-1,-1,102,103})&&e.task_pool.at(0).agent_assigned==-1&&e.task_pool.at(1).agent_assigned==-1&&
   e.task_pool.at(102).idx_next_loc==1&&e.task_pool.at(103).idx_next_loc==1,"fresh pickup audit changed metadata or a started task");
  policy.schedule(&e,5000,proposed);
  if(audit&&quota==2)require(policy.fresh_pickup_shadow().witness_saving==first.witness_saving&&
   policy.fresh_pickup_shadow().duplicate_cycles==1,"fresh pickup audit double-counted task witnesses");
  return Trace{proposed,policy.stats(),policy.fresh_pickup_shadow()};
 };
 for(int quota:{1,2}){const auto baseline=run(false,quota),shadow=run(true,quota);
  require(baseline.schedule==shadow.schedule&&baseline.shadow.passes==0,"fresh pickup diagnostic changed actual assignments or ran disabled");
  for(auto member:{&Stats::assignments,&Stats::fair_assignments,&Stats::oriented_builds,&Stats::flow_publications,
   &Stats::pickup_full_fields,&Stats::pickup_full_pops,&Stats::pickup_full_scans,&Stats::pickup_full_estimates,
   &Stats::pickup_flow_cached_estimates,&Stats::pickup_flow_approximate_estimates,&Stats::estimated_pickup_cost,
   &Stats::estimated_chain_cost,&Stats::route_queries,&Stats::match_moved,&Stats::match_saving})
   require(baseline.stats.*member==shadow.stats.*member,"fresh pickup diagnostic changed real work or caches");
 }
 for(const auto& invalid:std::vector<std::pair<const char*,const char*>>{{"CGAR_FRESH_PICKUP_AUDIT","-1"},{"CGAR_FRESH_PICKUP_AUDIT","2"},
  {"CGAR_PICKUP_FULL_ROBOTS","0"},{"CGAR_PICKUP_FULL_ROBOTS","65"},{"CGAR_DIAGNOSTICS","0"},{"CGAR_HRRN","0"},{"CGAR_REASSIGN_MATCH","0"}}){
  setenv("CGAR_FRESH_PICKUP_AUDIT","1",1);setenv("CGAR_PICKUP_FULL_ROBOTS","2",1);setenv("CGAR_DIAGNOSTICS","1",1);
  setenv("CGAR_HRRN","1",1);setenv("CGAR_REASSIGN_MATCH","1",1);setenv(invalid.first,invalid.second,1);
  bool rejected=false;try{auto e=blank();Cgar policy;policy.initialize(&e,5000);}catch(const std::invalid_argument& error){rejected=std::string(error.what()).find("fresh pickup audit")!=std::string::npos;}
  require(rejected,"fresh pickup diagnostic accepted an unsupported configuration");
 }
 for(const char* key:{"CGAR_DIAGNOSTICS","CGAR_TEMPORAL","CGAR_TEMPORAL_STEPS","CGAR_ORIENTATION_GUIDANCE","CGAR_FLOW_STRENGTH",
  "CGAR_FLOW_COST_SCALE","CGAR_FLOW_WARMUP","CGAR_FLOW_MIN_SAMPLES","CGAR_FLOW_MIN_MARGIN_PERCENT","CGAR_FLOW_REFRESH_INTERVAL",
  "CGAR_PICKUP_FLOW","CGAR_HRRN","CGAR_PICKUP_WEIGHT","CGAR_PICKUP_FULL_ROBOTS","CGAR_PICKUP_FULL_THREADS","CGAR_REASSIGN_MATCH","CGAR_FRESH_PICKUP_AUDIT"})unsetenv(key);
 std::cout<<"FRESH_PICKUP_AUDIT passed actual_forward_field_cycle=1 task_column_tier_guard=1 three_way=1 identity=1 missing_field=1 started_protected=1 task_disjoint=1 deadline=1 real_work_exact=1 config_guards=7\n";
}

int main(){try{city_game_trick_regression();temporal_after_turn_promise_regression();random_trick_regression();temporal_priority_portfolio_regression();sortation_trick_regression();temporal_region_budget_regression();turn_prewarm_regression();match_horizon_guard_regression();fresh_pickup_audit_regression();native_metric_regression();native_short_preference_regression();known_horizon_regression();horizon_percentile_regression();horizon_margin_regression();chain_flow_pricing_regression();warehouse_trick_regression();temporal_remaining_flow_regression();temporal_group_snapshot_regression();temporal_peak_audit_regression();temporal_next_errand_regression();temporal_service_audit_regression();fractional_turn_scheduler_regression();temporal_mixed_start_regression();oriented_pickup_search_regression();pickup_flow_scheduler_regression();complete_pickup_scheduler_regression();temporal_table_batch_regression();turn_build_limit_regression();temporal_transaction_safety_regression();pool_exchange_regression();pool_exchange_fair_admission();temporal_transaction_regression();temporal_preparation_regression();temporal_forward_audit_regression();guide_window_regression();guide_routes_regression();guide_reconnect_regression();guide_refine_regression();flow_margin_regression();flow_refresh_regression();flow_cache_only_regression();flow_cost_scale_regression();temporal_wait_turn_regression();temporal_warm_start_regression();for(const char* temperature:{"100","0"}){setenv("CGAR_TEMPORAL_REGION_TEMPERATURE_PPM",temperature,1);temporal_region_adapter_regression();}unsetenv("CGAR_TEMPORAL_REGION_TEMPERATURE_PPM");temporal_distance_scale_regression();flow_guidance_regression();temporal_turn_progress_regression();temporal_region_adapter_regression();compact_turn_tables();turn_prefetch_regression();temporal_regions_regression();setenv("CGAR_TURN_COST","4",1);temporal_primary_regression();temporal_parallel_regression();unsetenv("CGAR_TURN_COST");initialization_failure_recovery();temporal_idle_blocker();global_task_candidates();temporal_parallel_regression();temporal_kernel_on_thread();temporal_primary_regression();oriented_distances();movement_diagnostics();assignment_permutation_regression();unopened_matching_production();unopened_reassignment();reassignment_primary_and_commitments();reassignment_recovery_protection();reassignment_fair_admission();weighted_pickup_assignment();cache_and_chain_consistency();consistent_progress_basis();certificates();pocket_case();pocket_case(20);persistent_primary();capacity_bootstrap();scheduler_case();fair_sparse_schedule();sparse_fallback_quality();replenish_taken_candidate();bounded_scheduler_work();compact_distances();bounded_distance_work();std::cout<<"All CGAR regression checks passed\n";}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
