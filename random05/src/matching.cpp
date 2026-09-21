#include "engine.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <limits>
#include <stdexcept>

namespace r05 {
// Independent capacitated epsilon-auction. Identical optional and mandatory
// idle slots form multi-unit objects, avoiding repeated bids between copies of
// the same idle choice. Each object's price is its lowest retained bid once
// full, and zero while it has unused capacity. Bidders and ties are deterministic.
std::vector<int> auction_assignment(const std::vector<float>& matrix,int nr,int nc,
    double epsilon,uint64_t bid_limit,int dummy_columns,int optional_columns,uint64_t* bids) {
    if(nr<0 || nc<nr || matrix.size()!=size_t(nr)*nc || !std::isfinite(epsilon) || epsilon<=0 ||
       dummy_columns<0 || dummy_columns>nr || optional_columns<0 || optional_columns>nc-dummy_columns)
        throw std::invalid_argument("invalid capacitated auction problem");
    if(bids)*bids=0;
    if(!nr)return {};
    double minimum=0;
    for(float cost:matrix) {
        if(!std::isfinite(cost))throw std::invalid_argument("auction requires finite assignment prices");
        minimum=std::min(minimum,double(cost));
    }
    const int real=nc-optional_columns-dummy_columns;
    for(int row=0;row<nr;++row) {
        for(int j=real+1;j<real+optional_columns;++j)
            if(matrix[size_t(row)*nc+j]!=matrix[size_t(row)*nc+real])
                throw std::invalid_argument("auction optional slots must be identical within each row");
        for(int j=nc-dummy_columns;j<nc;++j)
            if(matrix[size_t(row)*nc+j]!=matrix[nc-1])
                throw std::invalid_argument("auction mandatory slots must share one price");
    }
    // Filling every mandatory slot enforces the already-declared admission cap.
    // Strengthening their common constant does not change the objective among
    // cap-feasible assignments. The gap exceeds the n*epsilon auction tolerance.
    const double mandatory_price=minimum-(nr+1)*epsilon-1;
    struct Bid {double value;int agent;};
    auto greater=[](const Bid& a,const Bid& b){return a.value!=b.value?a.value>b.value:a.agent>b.agent;};
    struct Object {int start,capacity;double price=0;std::vector<Bid> owners;};
    std::vector<Object> objects;objects.reserve(real+2);
    for(int j=0;j<real;++j)objects.push_back({j,1,0,{}});
    if(optional_columns)objects.push_back({real,optional_columns,0,{}});
    if(dummy_columns)objects.push_back({nc-dummy_columns,dummy_columns,0,{}});
    for(auto& object:objects)object.owners.reserve(object.capacity);
    auto cost=[&](int row,const Object& object) {
        return dummy_columns && object.start==nc-dummy_columns?mandatory_price:double(matrix[size_t(row)*nc+object.start]);
    };
    std::deque<int> pending;for(int row=0;row<nr;++row)pending.push_back(row);
    std::vector<int> assigned(nr,-1);uint64_t count=0;
    while(!pending.empty() && count<bid_limit) {
        const int row=pending.front();pending.pop_front();
        int best=-1;double first=std::numeric_limits<double>::infinity(),second=first;
        for(int j=0;j<int(objects.size());++j) {
            const double value=cost(row,objects[j])+objects[j].price;
            if(value<first){second=first;first=value;best=j;}
            else if(value<second)second=value;
        }
        if(best<0)throw std::runtime_error("auction lost all admissible objects");
        if(!std::isfinite(second))second=first;
        auto& object=objects[best];const double offer=object.price+second-first+epsilon;
        if(!std::isfinite(offer) || offer<=object.price)
            throw std::runtime_error("auction bid increment lost numeric precision");
        object.owners.push_back({offer,row});std::push_heap(object.owners.begin(),object.owners.end(),greater);
        assigned[row]=best;
        if(int(object.owners.size())>object.capacity) {
            std::pop_heap(object.owners.begin(),object.owners.end(),greater);
            const int displaced=object.owners.back().agent;object.owners.pop_back();
            assigned[displaced]=-1;pending.push_back(displaced);
        }
        object.price=int(object.owners.size())==object.capacity?object.owners.front().value:0;
        ++count;
    }
    if(bids)*bids=count;
    if(!pending.empty())return {}; // discard the incomplete assignment
    if(dummy_columns && int(objects.back().owners.size())!=dummy_columns)
        throw std::runtime_error("completed auction did not enforce the admission cap");
    // Certify epsilon-complementary slackness before exposing an assignment.
    // This also guards the rectangular, partially filled capacity case.
    for(int row=0;row<nr;++row) {
        double best=std::numeric_limits<double>::infinity();
        for(const auto& object:objects)best=std::min(best,cost(row,object)+object.price);
        const auto& chosen=objects.at(assigned[row]);
        if(cost(row,chosen)+chosen.price>best+epsilon+1e-8)
            throw std::runtime_error("completed auction failed its price certificate");
    }
    std::vector<int> next(objects.size()),result(nr,-1);
    for(int row=0;row<nr;++row) {
        const int object=assigned[row];
        if(next[object]>=objects[object].capacity)throw std::runtime_error("auction exceeded object capacity");
        result[row]=objects[object].start+next[object]++;
    }
    return result;
}
}
