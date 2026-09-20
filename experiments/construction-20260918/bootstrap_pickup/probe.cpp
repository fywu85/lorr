// OFFLINE diagnostic only. No proposed assignment is consumed by a solver.
#include "pickup_full.hpp"
#include "assignment_permutation.hpp"
#include "tricks.hpp"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <thread>
using json = nlohmann::ordered_json;
using Clock = std::chrono::steady_clock;
constexpr int INF = 1000000000;
void require(bool value, const char* message) { if (!value) throw std::runtime_error(message); }

int main(int argc, char** argv) {
    require(argc == 5 && std::string(argv[1]) == "--trick" && std::string(argv[2]) == "WAREHOUSE",
            "offline probe requires --trick WAREHOUSE input.json output.json");
    std::ifstream input(argv[3]); json data; input >> data;
    const int rows=data.at("rows"), cols=data.at("cols"), cells=rows*cols;
    const std::vector<int> map=data.at("map"); const std::vector<std::array<int,5>> agents=data.at("agents");
    require(data.at("schema")==1 && agents.size()==10000, "invalid initial-assignment fixture");
    auto field=cgar::tricks::native_forward_costs("WAREHOUSE",map,rows,cols,true);
    const auto fingerprint=cgar::tricks::validate_native_field(field,true);
    auto neighbor=[&](int cell,int d) {
        const int r=cell/cols,c=cell%cols;
        if ((d==0&&c+1==cols)||(d==1&&r+1==rows)||(d==2&&c==0)||(d==3&&r==0)) return -1;
        return cell+(d==0?1:d==1?cols:d==2?-1:-cols);
    };
    auto allowed=[&](int cell) {return !map[cell];};
    auto check=[]{};
    std::vector<int> remaining; std::set<int> occupied,tasks;
    int already_at_pickup=0;
    for (int i=0;i<int(agents.size());++i) {
        const auto& a=agents[i];
        require(a[0]==i && a[1]>=0&&a[1]<cells&&a[2]>=0&&a[2]<4&&a[4]>=0&&a[4]<cells,
                "invalid initial row");
        require(allowed(a[1])&&allowed(a[4])&&occupied.insert(a[1]).second&&tasks.insert(a[3]).second,
                "duplicate or obstructed initial holder");
        already_at_pickup += a[1]==a[4];
        // Conservative, explicit exclusions. This is not a reconstruction of
        // arbitrary live recovery state: all rows are the time-zero dispatch.
        if (i!=0 && a[1]!=a[4]) remaining.push_back(i);
    }
    std::array<cgar::CompletePickupSearch,4> searches;
    json reports=json::array(); long long total_before=0,total_after=0,total_guarded=0,accepted_rows=0;
    long long physical_before=0,physical_after=0,physical_optimum=0,accepted_physical_delta=0;
    long long pops=0,states=0; int independent_fields=0;
    auto started=Clock::now();
    // Independent heap Dijkstra checks every distance in the first two native
    // and first two physical fields, including the original heading convention.
    auto reference_field=[&](int source,int heading,bool weighted) {
        std::vector<int> distance(cells*4,INF),best(cells,INF);
        using Item=std::pair<int,int>; std::priority_queue<Item,std::vector<Item>,std::greater<Item>> queue;
        distance[source*4+heading]=0;queue.push({0,source*4+heading});
        while (!queue.empty()) {
            const auto item=queue.top();queue.pop();const int cost=item.first,s=item.second;
            if (distance[s]!=cost) continue;
            const int cell=s/4,d=s%4;best[cell]=std::min(best[cell],cost);
            auto offer=[&](int next,int step) {if(cost+step<distance[next]){distance[next]=cost+step;queue.push({cost+step,next});}};
            offer(cell*4+(d+1)%4,1);offer(cell*4+(d+3)%4,1);
            const int next=neighbor(cell,d);if(next>=0&&allowed(next))offer(next*4+d,weighted?field[cell*4+d]:1);
        }
        return best;
    };
    while (!remaining.empty()) {
        const int anchor=agents[remaining.front()][1];
        auto near=[&](int robot) {const int cell=agents[robot][1];return std::abs(cell/cols-anchor/cols)+std::abs(cell%cols-anchor%cols);};
        std::sort(remaining.begin(),remaining.end(),[&](int a,int b){const int x=near(a),y=near(b);return x!=y?x<y:a<b;});
        const int n=std::min<int>(32,remaining.size());std::vector<int> group(remaining.begin(),remaining.begin()+n);
        remaining.erase(remaining.begin(),remaining.begin()+n);
        std::vector<int> costs(n*n),physical(n*n);
        std::array<std::exception_ptr,4> errors{};std::array<long long,4> row_pops{},row_states{};
        std::array<int,4> checked{};std::vector<std::thread> workers;
        const bool first=reports.empty();
        for(int w=0;w<4;++w)workers.emplace_back([&,w]{try {
            cgar::FullPickupField result;
            for(int r=w;r<n;r+=4)for(int metric=0;metric<2;++metric){
                const auto& agent=agents[group[r]];
                searches[w].run(cells,agent[1],agent[2],1,INF,neighbor,allowed,
                    [&](int cell,int d){return metric?1:int(field[cell*4+d]);},check,result,metric?1:201);
                row_pops[w]+=result.pops;row_states[w]+=result.states;
                if(first&&r<2){require(result.distance==reference_field(agent[1],agent[2],!metric),"heap and Dial distances disagree");++checked[w];}
                auto& target=metric?physical:costs;
                for(int c=0;c<n;++c){target[r*n+c]=result.distance[agents[group[c]][4]];require(target[r*n+c]<INF,"unreachable initial pickup");}
            }
        }catch(...){errors[w]=std::current_exception();}});
        for(auto& worker:workers)worker.join();
        for(int w=0;w<4;++w){if(errors[w])std::rethrow_exception(errors[w]);pops+=row_pops[w];states+=row_states[w];independent_fields+=checked[w];}
        const auto match=cgar::minimum_pickup_permutation(costs,n,INF,check);
        const auto best_physical=cgar::minimum_pickup_permutation(physical,n,INF,check);
        const auto cycles=cgar::pickup_permutation_cycles(costs,match,20,check,20);
        long long accepted=0,accepted_n=0,phys_before=0,phys_after=0,accepted_phys_gain=0;
        int accepted_cycles=0;
        for(int r=0;r<n;++r){phys_before+=physical[r*n+r];phys_after+=physical[r*n+match.column[r]];}
        for(const auto& cycle:cycles)if(cycle.accepted){accepted+=cycle.before-cycle.after;accepted_n+=cycle.rows.size();++accepted_cycles;
            for(int r:cycle.rows)accepted_phys_gain+=physical[r*n+r]-physical[r*n+match.column[r]];
        }
        require(match.before>=match.after&&best_physical.before==phys_before&&best_physical.after<=phys_after,
                "pickup accounting inconsistent");
        reports.push_back({{"group",reports.size()},{"robots",group},{"native_before",match.before},{"native_after",match.after},
            {"native_positive_saving",match.before-match.after},{"native_changed_rows",match.changed},
            {"accepted_saving",accepted},{"accepted_rows",accepted_n},{"accepted_cycles",accepted_cycles},
            {"physical_before",phys_before},{"physical_after_native_match",phys_after},{"physical_after_own_match",best_physical.after},
            {"accepted_physical_lower_bound_delta",accepted_phys_gain}});
        total_before+=match.before;total_after+=match.after;total_guarded+=accepted;accepted_rows+=accepted_n;
        physical_before+=phys_before;physical_after+=phys_after;physical_optimum+=best_physical.after;accepted_physical_delta+=accepted_phys_gain;
        if(reports.size()%50==0)std::cerr<<"BOOTSTRAP_GROUPS "<<reports.size()<<"\n";
    }
    require(independent_fields==4,"independent field checks missing");
    json result={{"offline_only",true},{"trick","WAREHOUSE"},{"native_field_fnv1a64",fingerprint},
        {"scope","Same initial selected task set, disjoint local groups <=32, exclude robot0 and already-at-pickup. Not a global matching bound, live-policy feasibility proof, physical saving or throughput prediction."},
        {"groups",reports},{"already_at_pickup",already_at_pickup},{"conservatively_excluded_robot",0},
        {"native_before",total_before},{"native_after",total_after},{"native_saving",total_before-total_after},
        {"accepted_native_saving",total_guarded},{"accepted_rows",accepted_rows},
        {"physical_before",physical_before},{"physical_after_native_match",physical_after},
        {"physical_after_own_match",physical_optimum},{"accepted_physical_lower_bound_delta",accepted_physical_delta},
        {"complete_field_pops",pops},{"complete_field_states",states},{"independent_heap_fields",independent_fields},
        {"diagnostic_wall_seconds",std::chrono::duration<double>(Clock::now()-started).count()}};
    std::ofstream output(argv[4]);output<<result.dump(2)<<"\n";require(bool(output),"failed to save diagnostic");
    std::cout<<"BOOTSTRAP_MATCHING_MEASURED native="<<total_before<<"->"<<total_after<<" accepted_saving="<<total_guarded<<" physical="<<physical_before<<"->"<<physical_after<<"\n";
}
