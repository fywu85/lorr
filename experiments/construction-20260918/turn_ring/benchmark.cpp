// Paired benchmark of mechanically extracted production compute() bodies.
// Minimal state mirrors the immutable Warehouse topology; full integration and
// certificate coverage belong to cgar_regression and the trajectory controls.
#include "warehouse_lanes.hpp"
#include "warehouse_native.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>
constexpr int kInf=1<<29;
thread_local size_t deadline_checks=0;
void check_deadline(std::chrono::steady_clock::time_point deadline,const char*) {
 ++deadline_checks;
 if(std::chrono::steady_clock::now()>=deadline)throw std::runtime_error("benchmark deadline");
}
struct Topology {std::vector<int> pocket;std::vector<char> core;};
class BenchOracle {
 public:
 const Topology* cert_;std::vector<int> cells_,index_,backward_;std::vector<uint8_t> forward_costs_;
 int turn_cost_=1,forward_base_=1;
 int forward_cost(int cell,int dir)const{return forward_costs_.empty()?forward_base_:forward_costs_[cell*4+dir];}
 std::vector<int> before(int,std::chrono::steady_clock::time_point,std::vector<int>&,std::vector<std::vector<int>>&)const;
 std::vector<int> after(int,std::chrono::steady_clock::time_point,std::vector<int>&,std::vector<std::vector<int>>&)const;
};
#include "turn-before.inc"
#include "turn-after.inc"
struct Sample {double cpu,wall;};
Sample timed(const std::function<void()>& run){timespec a,b;clock_gettime(CLOCK_THREAD_CPUTIME_ID,&a);auto w=std::chrono::steady_clock::now();run();
 auto wall=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-w).count();clock_gettime(CLOCK_THREAD_CPUTIME_ID,&b);
 return{1000.*(b.tv_sec-a.tv_sec)+1e-6*(b.tv_nsec-a.tv_nsec),wall};}
double median(std::vector<double> x){std::sort(x.begin(),x.end());return(x[x.size()/2-1]+x[x.size()/2])/2;}
int main(){try{
 using namespace cgar::tricks;const int rows=warehouse_rows,cols=warehouse_cols,cells=rows*cols;
 Topology topology;topology.pocket.assign(cells,-1);topology.core.assign(cells,0);BenchOracle oracle;oracle.cert_=&topology;oracle.index_.assign(cells,-1);
 for(int cell=0;cell<cells;++cell)if(warehouse_masks[cell]!='x'){topology.core[cell]=1;oracle.index_[cell]=oracle.cells_.size();oracle.cells_.push_back(cell);}
 auto neighbor=[&](int cell,int dir){int y=cell/cols,x=cell%cols;
  if((dir==0&&x+1==cols)||(dir==2&&x==0)||(dir==1&&y+1==rows)||(dir==3&&y==0))return-1;
  return cell+(dir==0?1:dir==1?cols:dir==2?-1:-cols);};
 oracle.backward_.assign(oracle.cells_.size()*4,-1);
 for(size_t u=0;u<oracle.cells_.size();++u)for(int d=0;d<4;++d){int from=neighbor(oracle.cells_[u],(d+2)%4);
  if(from>=0&&oracle.index_[from]>=0)oracle.backward_[u*4+d]=oracle.index_[from]*4+d;}
 auto hex=[](char c){return c<='9'?c-'0':c-'a'+10;};
 const char* names[]={"uniform1_bfs_control","legacy4_16_turn4","native20_200_turn1","native20_200_bands_turn1"};
 std::cout<<std::setprecision(10)<<"{\"profiles\":[";
 for(int profile=0;profile<4;++profile){
  oracle.forward_base_=profile==0?1:profile==1?4:20;oracle.turn_cost_=profile==1?4:1;
  int cap=profile==0?1:profile==1?16:profile==2?200:201;std::vector<uint8_t> weights(cells*4,oracle.forward_base_);
  for(int cell:oracle.cells_){int mask=hex(warehouse_masks[cell]);int band=profile==3?((hex(warehouse_native_band_hex[cell/4])>>(cell%4))&1):0;
   for(int d=0;d<4;++d)weights[cell*4+d]=profile==0?1:((mask&(1<<d))?(profile==1?16:200):oracle.forward_base_)+band;}
  if(profile>=2){uint64_t hash=14695981039346656037ULL;for(auto w:weights)hash=(hash^w)*1099511628211ULL;
   if(hash!=(profile==2?warehouse_native_nobands_fnv1a64:warehouse_native_bands_fnv1a64))throw std::runtime_error("native field mismatch");}
  oracle.forward_costs_=profile?weights:std::vector<uint8_t>{};
  std::vector<int> old_queue,new_queue,old_dist,new_dist;std::vector<std::vector<int>> old_buckets(cap+1),new_buckets(cap+1);
  old_queue.reserve(oracle.cells_.size()*4);new_queue.reserve(oracle.cells_.size()*4);
  std::vector<double> old_cpu,new_cpu,old_wall,new_wall;uint64_t checksum=0,total_states=0,checks=0;
  for(int repeat=-2;repeat<24;++repeat){int index=repeat+2,goal=oracle.cells_[(index*997+12)%oracle.cells_.size()];size_t old_checks=0,new_checks=0;
   auto old_run=[&]{deadline_checks=0;old_dist=oracle.before(goal,std::chrono::steady_clock::time_point::max(),old_queue,old_buckets);old_checks=deadline_checks;};
   auto new_run=[&]{deadline_checks=0;new_dist=oracle.after(goal,std::chrono::steady_clock::time_point::max(),new_queue,new_buckets);new_checks=deadline_checks;};
   Sample old_sample,new_sample;if(index%2){new_sample=timed(new_run);old_sample=timed(old_run);}else{old_sample=timed(old_run);new_sample=timed(new_run);}
   if(old_dist!=new_dist||old_checks!=new_checks)throw std::runtime_error("changed distances or deadline-check count");
   if(repeat>=0){old_cpu.push_back(old_sample.cpu);new_cpu.push_back(new_sample.cpu);old_wall.push_back(old_sample.wall);new_wall.push_back(new_sample.wall);
    total_states+=new_dist.size();checksum+=std::accumulate(new_dist.begin(),new_dist.end(),uint64_t(0));checks+=new_checks;}
  }
  if(profile)std::cout<<',';
  std::cout<<"{\"profile\":\""<<names[profile]<<"\",\"pairs\":24,\"identical_distances_deadline_checks\":true,\"compared_states\":"<<total_states<<",\"deadline_checks\":"<<checks<<",\"checksum\":"<<checksum
   <<",\"before_cpu_median_ms\":"<<median(old_cpu)<<",\"after_cpu_median_ms\":"<<median(new_cpu)<<",\"cpu_ratio\":"<<median(new_cpu)/median(old_cpu)
   <<",\"before_wall_median_ms\":"<<median(old_wall)<<",\"after_wall_median_ms\":"<<median(new_wall)<<'}';
 }
 std::cout<<"],\"scope\":\"Mechanically extracted production compute bodies, immutable Warehouse topology/fields, all free cells eligible as core. One bound core, alternating order, two warmup pairs then24pairs/profile. Thread CPU and wall time, full distances and deadline-check-count equality. Kernel only: cache/compression excluded, no whole-entry or throughput claim. Full production integration separately regression-tested.\"}\n";
 }catch(const std::exception& e){std::cerr<<e.what()<<'\n';return 1;}}
