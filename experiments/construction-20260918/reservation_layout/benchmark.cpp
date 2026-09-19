#include "temporal_geometry.hpp"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <numeric>
#include <tuple>
using namespace cgar;
using Clock=std::chrono::steady_clock;
static uint64_t checksum=1469598103934665603ULL;
void mix(uint64_t value){checksum^=value;checksum*=1099511628211ULL;}
int main(int argc,char**argv){
 const int mode=argc>1?std::stoi(argv[1]):0,rows=45,cols=45,cells=rows*cols,count=1000;
 if(mode<0||mode>2)return 2;
 TemporalGeometry geometry;std::vector<char> free(cells,true);geometry.initialize(free,rows,cols,[]{});
 std::mt19937_64 rng(81473);std::vector<int> locations(cells),order(count),goals(count);
 std::iota(locations.begin(),locations.end(),0);std::shuffle(locations.begin(),locations.end(),rng);locations.resize(count);
 std::iota(order.begin(),order.end(),0);std::shuffle(order.begin(),order.end(),rng);
 std::vector<double> power(count,1);std::vector<char> fixed(count,false);
 std::vector<TemporalPath> seeds(count);std::vector<std::vector<TemporalChoice>> choices(count);
 auto region=[&](int u){return (u/cols>=rows/2)*2+(u%cols>=cols/2);};
 std::vector<std::vector<int>> regions(count);std::vector<int> roots;
 for(int r=0;r<count;++r){
  const int ori=rng()%4,goal=rng()%cells;goals[r]=goal;
  auto distance=[&](int u,int d){
   int dx=goal%cols-u%cols,dy=goal/cols-u/cols;if(!dx&&!dy)return 0;int turn=2;
   for(int dir=0;dir<4;++dir)if((dir==0&&dx>0)||(dir==1&&dy>0)||(dir==2&&dx<0)||(dir==3&&dy<0)){
    int a=(dir-d+4)%4;turn=std::min(turn,std::min(a,4-a));}
   return std::abs(dx)+std::abs(dy)+turn;
  };
  if(mode==2)power[r]=double(count-r)/(count+1);
  seeds[r]=geometry.seed(locations[r],ori,3);
  choices[r].push_back({&seeds[r],TemporalGeometry::cost(seeds[r],0,goal,1,distance),0});
  const auto& paths=geometry.paths(locations[r],ori);
  for(int op=1;op<129;++op)if(paths[op].valid)choices[r].push_back({&paths[op],TemporalGeometry::cost(paths[op],op,goal,1,distance),op});
  std::sort(choices[r].begin()+1,choices[r].end(),[](const auto&a,const auto&b){return std::tie(a.cost,a.operation)<std::tie(b.cost,b.operation);});
  if(mode==1){fixed[r]=region(locations[r])!=0;if(!fixed[r])roots.push_back(r);
   for(const auto& choice:choices[r]){int reg=region(locations[r]);for(int u:choice.path->cells)if(region(u)!=reg){reg=-1;break;}regions[r].push_back(reg);}
  }
 }
 double init_time=0,construct_time=0,repair_time=0,destroy_time=0;long long candidates=0,recursion=0,repairs=0;
 std::vector<int> warm;
 if(mode==2){TemporalPibt previous(cells,choices,fixed,power,8192,1729);previous.construct(order,[]{});previous.repair(1000,[]{});warm=previous.selections();}
 for(int trial=0;trial<4;++trial){
  auto begin=Clock::now();
  auto run=std::make_unique<TemporalPibt>(cells,choices,fixed,power,8192,177+trial,
      warm.empty()?nullptr:&warm,mode==1?&regions:nullptr,mode==1?0:-1);
  auto initialized=Clock::now();run->construct(order,[]{});auto constructed=Clock::now();
  run->repair(1000000,[]{},4000000,mode==1?&roots:nullptr);auto repaired=Clock::now();
  const auto&s=run->stats;
  for(long long v:{s.roots,s.accepted,s.recursive_calls,s.candidates,s.budget_exhausted,s.repairs,s.repairs_accepted,s.repair_batches_kept,s.repair_batches_reverted})mix(v);
  mix(s.max_depth);double score=run->score();uint64_t bits;std::memcpy(&bits,&score,sizeof(bits));mix(bits);
  std::vector<std::array<int,5>> used_cells(cells),used_edges(2*cells);
  for(auto&x:used_cells)x.fill(-1);for(auto&x:used_edges)x.fill(-1);
  for(int r=0;r<count;++r){mix(run->selected(r));const auto& path=*run->choice(r).path;
   for(int t=0;t<5;++t){if(used_cells[path.cells[t]][t]>=0)throw std::runtime_error("invalid vertex reservation");used_cells[path.cells[t]][t]=r;
    if(path.edges[t]>=0){if(used_edges[path.edges[t]][t]>=0)throw std::runtime_error("invalid edge reservation");used_edges[path.edges[t]][t]=r;}}
  }
  candidates+=s.candidates;recursion+=s.recursive_calls;repairs+=s.repairs;
  auto destroying=Clock::now();run.reset();auto destroyed=Clock::now();
  auto seconds=[](auto a,auto b){return std::chrono::duration<double>(b-a).count();};
  init_time+=seconds(begin,initialized);construct_time+=seconds(initialized,constructed);repair_time+=seconds(constructed,repaired);destroy_time+=seconds(destroying,destroyed);
 }
 std::cout<<std::setprecision(12)<<"{\"mode\":"<<mode<<",\"robots\":"<<count<<",\"trials\":4,\"checksum\":"<<checksum
  <<",\"candidates\":"<<candidates<<",\"recursion\":"<<recursion<<",\"repairs\":"<<repairs
  <<",\"initialization_seconds\":"<<init_time<<",\"construction_seconds\":"<<construct_time<<",\"repair_seconds\":"<<repair_time
  <<",\"destruction_seconds\":"<<destroy_time<<",\"kernel_seconds\":"<<init_time+construct_time+repair_time+destroy_time<<"}\n";
}
