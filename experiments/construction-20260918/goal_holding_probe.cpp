// Offline finite counterexample search using the production action pool and score.
// No simulation or warehouse-performance claim; see GOAL_HOLDING.md.
#include "temporal_geometry.hpp"
#include <iostream>
#include <queue>
#include <map>
using namespace cgar;
int main(int argc,char**argv){
 const bool fable=argc==2&&std::string(argv[1])=="--fable-fixture";
 const int rows=3,cols=fable?9:7,n=rows*cols,U=4;std::vector<char> free(n,1);TemporalGeometry geo;geo.initialize(free,rows,cols,[]{});
 auto neighbor=[&](int c,int d){int r=c/cols,k=c%cols;if(d==0)++k;else if(d==1)++r;else if(d==2)--k;else --r;return r<0||r>=rows||k<0||k>=cols?-1:r*cols+k;};
 std::vector<std::vector<int>> ds(n,std::vector<int>(4*n,1000000));
 for(int goal=0;goal<n;++goal){auto& d=ds[goal];std::queue<int> q;for(int h=0;h<4;++h){d[4*goal+h]=0;q.push(4*goal+h);}while(!q.empty()){int z=q.front();q.pop();int c=z/4,h=z%4;std::array<int,3> pred{4*c+(h+1)%4,4*c+(h+3)%4,-1};int p=neighbor(c,(h+2)%4);if(p>=0)pred[2]=4*p+h;for(int u:pred)if(u>=0&&d[u]>d[z]+U){d[u]=d[z]+U;q.push(u);}}}
 auto choices=[&](int start,int h,int goal){std::vector<TemporalChoice> out;const auto& paths=geo.paths(start,h);for(int op=0;op<129;++op)if(paths[op].valid){auto cost=TemporalGeometry::cost(paths[op],op,goal,U,[&](int c,int o){return ds[goal][4*c+o];},50,U);out.push_back({&paths[op],cost,op});}return out;};
 auto compatible=[&](const TemporalPath&a,const TemporalPath&b){for(int t=0;t<5;++t)if(a.cells[t]==b.cells[t]||(a.edges[t]>=0&&a.edges[t]==b.edges[t]))return false;return true;};
 std::mt19937 rng(7123);int tested=0,found=0;
 for(int sample=0;sample<(fable?1:1500)&&found<4;++sample){
  int sa=fable?11:rng()%n,ha=fable?0:rng()%4,ga=neighbor(sa,ha);if(ga<0)continue;
  int sb=fable?10:rng()%n,hb=fable?0:rng()%4,gb=fable?24:rng()%n;if(sa==sb||sb==gb||sb==ga)continue;
  auto a=choices(sa,ha,ga),b=choices(sb,hb,gb);int ai=-1,bi=-1;int64_t best=INT64_MAX;
  for(int i=0;i<(int)a.size();++i)for(int j=0;j<(int)b.size();++j)if(a[i].cost+b[j].cost<best&&compatible(*a[i].path,*b[j].path)){ai=i;bi=j;best=a[i].cost+b[j].cost;}
  ++tested;if(ai<0)throw std::runtime_error("missing collision-free joint wait");
  if(fable)std::cout<<"FABLE_FIXTURE native A op="<<a[ai].operation<<" first="<<int(a[ai].path->first_action)<<" native B op="<<b[bi].operation<<" first="<<int(b[bi].path->first_action)<<" jointcost="<<best<<"\n";
  if(a[ai].path->cells[0]!=ga||b[bi].path->cells[0]!=sb)continue;
  for(int j=0;j<(int)b.size()&&found<4;++j){if(b[j].path->cells[0]==sb)continue;
   if(b[bi].cost+int64_t(b[bi].operation)*U-(b[j].cost+int64_t(b[j].operation)*U)<50*U)continue;
   int conflicts=0;bool holds=true;
   for(int t=0;t<5;++t)if(a[ai].path->cells[t]==b[j].path->cells[t]||(a[ai].path->edges[t]>=0&&a[ai].path->edges[t]==b[j].path->edges[t])){++conflicts;if(t==0||a[ai].path->cells[t]!=ga||a[ai].path->edges[t]>=0)holds=false;}
   if(!conflicts||!holds)continue;
   for(int i=0;i<(int)a.size();++i)if(a[i].path->cells[0]==ga&&a[i].path->cells[4]!=ga&&compatible(*a[i].path,*b[j].path)){
    ++found;std::cout<<"CASE "<<found<<" A start="<<sa<<" heading="<<ha<<" goal="<<ga<<" known_next="<<a[i].path->cells[4]<<" B start="<<sb<<" heading="<<hb<<" goal="<<gb<<"\n";
    auto show=[&](const char*tag,const TemporalChoice&c){std::cout<<tag<<" op="<<c.operation<<" score="<<c.cost<<" actions=";for(int k:TemporalGeometry::operations()[c.operation])std::cout<<"FRCW"[k];std::cout<<" cells=";for(int x:c.path->cells)std::cout<<x<<",";std::cout<<"\n";};
    show("native A",a[ai]);show("native B",b[bi]);show("alternative A",a[i]);show("alternative B",b[j]);std::cout<<"native joint cost="<<best<<" alternative="<<a[i].cost+b[j].cost<<" future_hold_conflict_slots="<<conflicts<<"\n";break;
   }
  }
 }
 std::cout<<"EXHAUSTIVE_PAIR_CHOICES_CHECKED cases="<<tested<<" examples="<<found<<"\n";return fable?0:found?0:2;
}
