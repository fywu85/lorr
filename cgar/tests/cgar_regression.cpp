#include "cgar.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <queue>
#include <set>
#include <stdexcept>
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
int main(){try{certificates();pocket_case();pocket_case(20);persistent_primary();capacity_bootstrap();scheduler_case();fair_sparse_schedule();sparse_fallback_quality();replenish_taken_candidate();bounded_scheduler_work();compact_distances();bounded_distance_work();std::cout<<"All CGAR regression checks passed\n";}catch(const std::exception& e){std::cerr<<e.what()<<"\n";return 1;}}
