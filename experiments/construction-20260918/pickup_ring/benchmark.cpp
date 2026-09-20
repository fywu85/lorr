// Read-only kernel benchmark: before/after complete pickup fields, identical work.
#define FullPickupField BeforeFullPickupField
#define CompletePickupSearch BeforeCompletePickupSearch
#include "pickup-before.hpp"
#undef FullPickupField
#undef CompletePickupSearch
#define FullPickupField OriginalFullPickupField
#define CompletePickupSearch OriginalCompletePickupSearch
#include "pickup-original.hpp"
#undef FullPickupField
#undef CompletePickupSearch
#include "pickup-after.hpp"
#include "warehouse_lanes.hpp"
#include "warehouse_native.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string>
using namespace cgar;
struct Sample { double cpu,wall; };
Sample timed(const std::function<void()>& run){
 timespec a,b;clock_gettime(CLOCK_THREAD_CPUTIME_ID,&a);auto wall=std::chrono::steady_clock::now();run();
 auto elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-wall).count();clock_gettime(CLOCK_THREAD_CPUTIME_ID,&b);
 return {1000.*(b.tv_sec-a.tv_sec)+1e-6*(b.tv_nsec-a.tv_nsec),elapsed};
}
double median(std::vector<double> v){std::sort(v.begin(),v.end());return(v[v.size()/2-1]+v[v.size()/2])/2;}
int main(){try{
 using namespace tricks;const int rows=warehouse_rows,cols=warehouse_cols,cells=rows*cols;
 std::vector<int> free;for(int i=0;i<cells;++i)if(warehouse_masks[i]!='x')free.push_back(i);
 auto allowed=[&](int cell){return warehouse_masks[cell]!='x';};
 auto neighbor=[&](int cell,int dir){const int y=cell/cols,x=cell%cols;
  if((dir==0&&x+1==cols)||(dir==2&&x==0)||(dir==1&&y+1==rows)||(dir==3&&y==0))return-1;
  return cell+(dir==0?1:dir==1?cols:dir==2?-1:-cols);};
 auto hex=[](char c){return c<='9'?c-'0':c-'a'+10;};
 const char* names[]={"uniform1","legacy4_16_turn4","native20_200_turn1","native20_200_bands_turn1"};
 BeforeCompletePickupSearch before;CompletePickupSearch after;OriginalCompletePickupSearch original;
 BeforeFullPickupField b;FullPickupField a;OriginalFullPickupField o;
 std::cout<<std::setprecision(10)<<"{\"profiles\":[";
 for(int profile=0;profile<4;++profile){
  const int base=profile==0?1:profile==1?4:20,cap=profile<2?16:profile==2?200:201,turn=profile==1?4:1;
  std::vector<uint8_t> weights(cells*4,base);
  for(int cell:free){const int mask=hex(warehouse_masks[cell]);const int band=profile==3?((hex(warehouse_native_band_hex[cell/4])>>(cell%4))&1):0;
   for(int h=0;h<4;++h)weights[cell*4+h]=profile==0?1:(mask&(1<<h)?(profile==1?16:200):base)+band;}
  if(profile>=2){uint64_t hash=14695981039346656037ULL;for(auto w:weights)hash=(hash^w)*1099511628211ULL;
   if(hash!=(profile==2?warehouse_native_nobands_fnv1a64:warehouse_native_bands_fnv1a64))throw std::runtime_error("microbenchmark field mismatch");}
  auto edge=[&](int cell,int h){return weights[cell*4+h];};
  std::vector<double> old_cpu,new_cpu,original_cpu,old_wall,new_wall;long long pops=0,states=0;uint64_t checksum=0;
  for(int repeat=-2;repeat<24;++repeat){const int index=repeat+2,start=free[(index*997+12)%free.size()],heading=index%4;
   auto old_run=[&]{before.run(cells,start,heading,turn,1<<29,neighbor,allowed,edge,[]{},b,cap);};
   auto new_run=[&]{after.run(cells,start,heading,turn,1<<29,neighbor,allowed,edge,[]{},a,cap);};
   auto original_run=[&]{original.run(cells,start,heading,turn,1<<29,neighbor,allowed,edge,[]{},o);};
   std::array<Sample,3> samples{};std::array<std::function<void()>,3> calls{old_run,new_run,original_run};const int count=profile<2?3:2;
   for(int k=0;k<count;++k){const int which=(index+k)%count;samples[which]=timed(calls[which]);}
   if(a.distance!=b.distance||a.pops!=b.pops||a.states!=b.states)throw std::runtime_error("ring differs from dynamic-modulo result/work");
   if(profile<2&&(a.distance!=o.distance||a.pops!=o.pops||a.states!=o.states))throw std::runtime_error("ring differs from original constant-modulo result/work");
   if(repeat>=0){old_cpu.push_back(samples[0].cpu);new_cpu.push_back(samples[1].cpu);old_wall.push_back(samples[0].wall);new_wall.push_back(samples[1].wall);
    if(profile<2)original_cpu.push_back(samples[2].cpu);pops+=a.pops;states+=a.states;checksum+=std::accumulate(a.distance.begin(),a.distance.end(),uint64_t(0));}
  }
  if(profile)std::cout<<',';
  std::cout<<"{\"profile\":\""<<names[profile]<<"\",\"pairs\":24,\"identical_distances_pops_states\":true,\"total_pops\":"<<pops<<",\"total_states\":"<<states<<",\"checksum\":"<<checksum
   <<",\"before_cpu_median_ms\":"<<median(old_cpu)<<",\"after_cpu_median_ms\":"<<median(new_cpu)<<",\"cpu_ratio\":"<<median(new_cpu)/median(old_cpu)
   <<",\"before_wall_median_ms\":"<<median(old_wall)<<",\"after_wall_median_ms\":"<<median(new_wall);
  if(profile<2)std::cout<<",\"original_constant_cpu_median_ms\":"<<median(original_cpu)<<",\"ratio_to_original\":"<<median(new_cpu)/median(original_cpu);
  std::cout<<'}';
 }
 std::cout<<"],\"scope\":\"One bound core; alternating order; two warmup pairs then24pairs per profile. Thread CPU time and wall time. Exact distance/pops/state equality. Kernel timings only, no throughput or whole-entry speed claim.\"}\n";
 }catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 1;}}
