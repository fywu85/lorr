// Offline objective experiment. No production policy is changed by this file.
#include "temporal_geometry.hpp"
#include <iostream>
#include <limits>
using namespace cgar;
struct Model {
 int rows,cols,U,T,tolls,n;TemporalGeometry geometry;std::vector<std::vector<int>> dist;
 Model(int r,int c,int unit,int extra,int toll):rows(r),cols(c),U(unit),T(unit+extra),tolls(toll),n(r*c){
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
  bool arrived=false;for(int c:p.cells)arrived|=c==goal;
  if(!arrived||!mode||next==goal||(mode==1&&next<0))return native(p,op,start,goal);
  const auto& actions=TemporalGeometry::operations()[op];int paid=0,from=start;
  for(int t=0;t<5;++t){int a=op==0&&t==0?p.first_action:actions[t];if(a==1||a==2)paid+=T-U;
   int to=p.cells[t];if(to!=from)paid+=forward(from,direction(from,to))-U;
   from=to;if(mode==2&&to==goal)break;
  }
  int d=-4*U;
  if(mode==1){int tail=value(next,p.cells[4],p.orientation);
   if(actions[4]==3){tail=std::min({tail,value(next,p.cells[4],(p.orientation+1)%4)+T-U,value(next,p.cells[4],(p.orientation+3)%4)+T-U});
    if(actions[3]==3)tail=std::min(tail,value(next,p.cells[4],(p.orientation+2)%4)+2*(T-U));}
   d+=tail-baseline(goal,next);
  }
  return int64_t(d+paid)*50-int64_t(op)*U;
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
bool compatible(const TemporalPath&a,const TemporalPath&b){for(int t=0;t<5;++t)if(a.cells[t]==b.cells[t]||(a.edges[t]>=0&&a.edges[t]==b.edges[t]))return false;return true;}
int main(){try{
 long long arriving=0,nonarriving=0;
 for(int U:{1,4,8})for(int extra:{0,1,2})for(int toll:{0,1}){
  Model m(3,7,U,extra,toll);
  for(int goal:{8,10,11}){int next=(goal+7)%m.n;
   for(int start=0;start<m.n;++start)for(int h=0;h<4;++h){const auto& paths=m.geometry.paths(start,h);
    for(int op=0;op<129;++op){const auto&p=paths[op];if(!p.valid)continue;bool hit=false;for(int c:p.cells)hit|=c==goal;
     auto score=m.proposed(p,op,start,goal,next,1);
     if(hit){int64_t physical=(score+int64_t(op)*U)/50+4*U+m.baseline(goal,next);
      if(physical!=m.oracle_tail(p,op,start,h,next))throw std::runtime_error("common-baseline score disagrees with independent paid-action replay");++arriving;
     }else{if(score!=m.native(p,op,start,goal))throw std::runtime_error("non-arriving score changed");++nonarriving;}
    }
   }
  }
 }
 Model m(3,7,4,0,0);int start=14,h=3,goal=8,next=13;int pa=m.operation("FRFFF"),pb=m.operation("RFCFW");
 const auto&a=m.geometry.paths(start,h)[pa];const auto&b=m.geometry.paths(start,h)[pb];
 auto physical=[&](const TemporalPath&p,int op){return (m.proposed(p,op,start,goal,next,1)+op*m.U)/50;};
 auto bad=[&](const TemporalPath&p,int op){int orientation=h;const auto& actions=TemporalGeometry::operations()[op];for(int t=0;t<5;++t){if(actions[t]==1)orientation=(orientation+1)%4;else if(actions[t]==2)orientation=(orientation+3)%4;if(p.cells[t]==goal)break;}return physical(p,op)-(m.value(next,goal,orientation)-m.baseline(goal,next));};
 if(physical(b,pb)-physical(a,pa)!=8||bad(b,pb)-bad(a,pa)!=4)throw std::runtime_error("hand-derived heading pair did not reproduce");
 std::cout<<"HEADING_PAIR common_physical="<<physical(a,pa)<<","<<physical(b,pb)<<" candidate_baseline_physical="<<bad(a,pa)<<","<<bad(b,pb)<<"\n";
 for(int fixture=0;fixture<2;++fixture){int sa=fixture?12:11,ha=2,ga=fixture?11:10,na=fixture?4:3,sb=5,hb=1,gb=fixture?7:8;
  const auto& A=m.geometry.paths(sa,ha);const auto&B=m.geometry.paths(sb,hb);
  for(int mode=0;mode<3;++mode){int ai=-1,bi=-1;int64_t best=std::numeric_limits<int64_t>::max();
   for(int i=0;i<129;++i)if(A[i].valid)for(int j=0;j<129;++j)if(B[j].valid&&compatible(A[i],B[j])){
    int64_t cost=m.proposed(A[i],i,sa,ga,na,mode)+m.proposed(B[j],j,sb,gb,-1,mode==2?2:0);
    if(cost<best){best=cost;ai=i;bi=j;}}
   if(ai<0||!compatible(A[ai],B[bi]))throw std::runtime_error("no joint solution");
   if(mode==0&&best!=(fixture?-840:-1040))throw std::runtime_error("native fixture changed");
   if(mode==1&&(A[ai].first_action!=0||B[bi].first_action!=0))throw std::runtime_error("continuation did not yield both first-forward in toy fixture");
   std::cout<<"JOINT fixture="<<fixture<<" mode="<<mode<<" score="<<best<<" Aop="<<ai<<" Bop="<<bi<<" Afirst="<<int(A[ai].first_action)<<" Bfirst="<<int(B[bi].first_action)<<"\n";
  }
 }
 std::cout<<"CONTINUATION_SURROGATE_ORACLE passed arriving="<<arriving<<" nonarriving="<<nonarriving<<" units=1,4,8 turn_extras=0,1,2 directed_tolls=0,1 explicit_terminal_wait_variants=1\n";
}catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}}
