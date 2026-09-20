// Standalone feasibility probe; no live solver consumes its tables.
#include "cgar.hpp"
#include "tricks.hpp"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <sys/resource.h>
using json=nlohmann::ordered_json;
using Clock=std::chrono::steady_clock;
int main(int argc,char**argv) {
 if(argc!=4)throw std::runtime_error("usage: probe MAP OUTPUT THREADS");
 const int threads=std::stoi(argv[3]);
 if(threads<1||threads>32)throw std::runtime_error("threads outside [1,32]");
 std::ifstream input(argv[1]);std::string word;int rows=0,cols=0;
 while(input>>word&&word!="map"){if(word=="height")input>>rows;else if(word=="width")input>>cols;}
 std::vector<int> map;
 for(int r=0;r<rows;++r){input>>word;if(int(word.size())!=cols)throw std::runtime_error("bad map");for(char c:word)map.push_back(c=='@'||c=='T');}
 auto cert=cgar::build_certificate_feasible(map,rows,cols,10000);
 const auto field=cgar::tricks::native_forward_costs("WAREHOUSE",map,rows,cols,true);
 const auto fingerprint=cgar::tricks::validate_native_field(field,true);
 std::vector<int> goals;for(int cell=0;cell<int(map.size());++cell)if(!map[cell])goals.push_back(cell);
 cgar::TurnDistanceOracle oracle;
 constexpr size_t cache_limit=size_t(25)<<30;
 oracle.init(&cert,cache_limit,1,true,20,201);oracle.set_forward_costs(field);
 if(oracle.capacity()<goals.size())throw std::runtime_error("logical cache cannot hold all goals");
 const auto began=Clock::now();const auto cpu_began=std::clock();
 const auto deadline=began+std::chrono::minutes(5);
 long long stored=0;json checkpoints=json::array();
 for(size_t first=0;first<goals.size();first+=32){
  const std::vector<int> batch(goals.begin()+first,goals.begin()+std::min(goals.size(),first+32));
  oracle.prefetch(batch,threads,deadline);
  for(int goal:batch){const auto* t=oracle.table(goal,deadline);stored+=t->storage_bytes();
   for(int h=0;h<4;++h)if(oracle.value(*t,goal,h)!=0)throw std::runtime_error("self distance not zero");}
  oracle.trim();
  if(first%4096==0||first+32>=goals.size())checkpoints.push_back({{"goals",std::min(goals.size(),first+32)},
    {"wall_seconds",std::chrono::duration<double>(Clock::now()-began).count()},{"stored_table_bytes",stored}});
 }
 const double wall=std::chrono::duration<double>(Clock::now()-began).count();
 const double cpu=double(std::clock()-cpu_began)/CLOCKS_PER_SEC;
 for(int goal:goals)if(!oracle.peek(goal))throw std::runtime_error("prewarmed goal was evicted");
 if(oracle.prefetched_builds!=int(goals.size())||oracle.prefetched_hits!=int(goals.size())||oracle.prefetched_discarded)
  throw std::runtime_error("prefetch work differs from prescribed all-goal work");
 // Independent heap oracle for four spread-out goals, outside timed work.
 long long compared=0;
 auto neighbor=[&](int c,int h){if((h==0&&c%cols+1==cols)||(h==1&&c/cols+1==rows)||(h==2&&c%cols==0)||(h==3&&c/cols==0))return -1;return c+(h==0?1:h==1?cols:h==2?-1:-cols);};
 for(size_t pick:{size_t(0),goals.size()/3,goals.size()/2,goals.size()-1}){
  int goal=goals[pick];std::vector<int> distances(map.size()*4,cgar::kInf);
  using Item=std::pair<int,int>;std::priority_queue<Item,std::vector<Item>,std::greater<Item>> heap;
  for(int h=0;h<4;++h){distances[4*goal+h]=0;heap.push({0,4*goal+h});}
  while(!heap.empty()){
   const auto x=heap.top();heap.pop();const int d=x.first,s=x.second,c=s/4,h=s%4;if(distances[s]!=d)continue;
   auto relax=[&](int to,int cost){if(d+cost<distances[to]){distances[to]=d+cost;heap.push({d+cost,to});}};
   relax(4*c+(h+1)%4,1);relax(4*c+(h+3)%4,1);
   const int from=neighbor(c,(h+2)%4);
   if(from>=0&&!map[from]&&(cert.core[from]||cert.pocket[from]==cert.pocket[goal]))relax(4*from+h,field[4*from+h]);
  }
  const auto* t=oracle.peek(goal);
  for(int c:goals)for(int h=0;h<4;++h){if(oracle.value(*t,c,h)!=distances[4*c+h])throw std::runtime_error("prewarmed value differs from heap");++compared;}
 }
 struct rusage usage{};getrusage(RUSAGE_SELF,&usage);
 const long long peak=usage.ru_maxrss*1024LL;
 if(peak>=32000000000LL)throw std::runtime_error("probe exceeds decimal32GB");
 json result={{"all_valid",true},{"offline_only",true},{"goals",goals.size()},{"threads",threads},{"logical_capacity",oracle.capacity()},
  {"logical_cache_limit_bytes",cache_limit},{"stored_table_bytes",stored},{"peak_rss_bytes",peak},{"prewarm_wall_seconds",wall},
  {"prewarm_cpu_seconds",cpu},{"prefetched_builds",oracle.prefetched_builds},{"wide_fallback_tables",oracle.wide_fallback_tables},
  {"independent_heap_states",compared},{"field_fnv1a64",fingerprint},{"checkpoints",checkpoints},
  {"under30_seconds_in_this_probe",wall<30},{"limits","Isolated complete-table preparation, excluding CGAR geometry, scheduling, task revelation and full initialization. Shared-host timing is not a preprocessing guarantee. No throughput improvement or whole-solver memory claim."}};
 std::ofstream(argv[2])<<result.dump(2)<<'\n';std::cout<<result.dump(2)<<'\n';
}
