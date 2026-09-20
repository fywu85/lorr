    // Exact unweighted spatial bounds on first admissions, using the independent
    // complete BFS queries above. This is stronger than production's cache-miss
    // Manhattan fallback. Report stock witnesses, never a throughput upper bound.
    const int horizon = report.at("steps").get<int>();
    std::vector<long long> chain_service(tasks.size());
    for (size_t i = 0; i < tasks.size(); ++i) {
        chain_service[i] = tasks[i].chain;
        for (size_t k = 1; k < tasks[i].stops.size(); ++k)
            chain_service[i] += tasks[i].stops[k] == tasks[i].stops[k-1];
    }
    std::vector<int> order;
    for (size_t i = 0; i < tasks.size(); ++i) if (!tasks[i].assignments.empty()) order.push_back(i);
    std::sort(order.begin(), order.end(), [&](int a, int b) {
        const auto& x = tasks[a].assignments.front(); const auto& y = tasks[b].assignments.front();
        return std::make_tuple(x.step,x.robot,tasks[a].id) < std::make_tuple(y.step,y.robot,tasks[b].id);
    });
    json cohorts;
    std::vector<int> thresholds{std::max(1,horizon-1000),std::max(1,horizon-400),std::max(1,horizon-250),std::max(1,horizon-100)};
    std::sort(thresholds.begin(),thresholds.end());thresholds.erase(std::unique(thresholds.begin(),thresholds.end()),thresholds.end());
    for (int threshold : thresholds) {
        long long completed=0,impossible=0,slack=0;
        for (int i : order) {
            const auto& t=tasks[i];const int step=std::max(1,t.assignments.front().step);
            if(step<threshold)continue;
            const long long bound=std::max(1,t.first_pickup_distance)+chain_service[i];
            const int remaining=horizon-step+1;
            if(t.finished>=0){
                if(bound>remaining)throw std::runtime_error("completed task violates exact spatial service lower bound");
                ++completed;
            }else if(bound>remaining)++impossible;else ++slack;
        }
        cohorts[std::to_string(threshold)]={{"completed",completed},{"uncompleted_provably_impossible",impossible},{"uncompleted_lb_feasible",slack}};
    }
    std::vector<char> used(tasks.size(),false);
    long long exposed=0,overlap_skipped=0,witnesses=0,searches=0,candidates=0;
    json examples=json::array();
    for (int i : order) {
        const auto& t=tasks[i];const auto& assignment=t.assignments.front();const int step=std::max(1,assignment.step);
        if(step<thresholds.front()||t.finished>=0)continue;
        const long long bound=std::max(1,t.first_pickup_distance)+chain_service[i];const int remaining=horizon-step+1;
        if(bound<=remaining)continue;
        ++exposed;if(used[i]){++overlap_skipped;continue;}
        std::vector<int> possible;
        for (size_t j=0;j<tasks.size();++j) {
            const auto& alt=tasks[j];
            // Conservatively omit every task actually assigned in this same
            // batch, as well as previously assigned or not-yet-revealed tasks.
            if(used[j]||alt.revealed>=step||(!alt.assignments.empty()&&alt.assignments.front().step<=step)||chain_service[j]+1>remaining)continue;
            const int first=alt.stops.front();
            const int manhattan=std::abs(first/cols-assignment.cell/cols)+std::abs(first%cols-assignment.cell%cols);
            if(std::max(1,manhattan)+chain_service[j]<=remaining)possible.push_back(j);
        }
        candidates+=possible.size();if(possible.empty())continue;
        ++searches;std::fill(distance.begin(),distance.end(),-1);distance[assignment.cell]=0;queue.assign(1,assignment.cell);
        for(size_t head=0;head<queue.size();++head){const int from=queue[head];
            for(int direction=0;direction<4;++direction){const int to=neighbor(from,direction);
                if(to>=0&&free[to]&&distance[to]<0){distance[to]=distance[from]+1;queue.push_back(to);}}}
        int best=-1;long long best_bound=0;
        for(int j:possible){const int d=distance[tasks[j].stops.front()];if(d<0)continue;
            const long long b=std::max(1,d)+chain_service[j];if(b>remaining)continue;
            if(best<0||std::make_pair(b,tasks[j].id)<std::make_pair(best_bound,tasks[best].id)){best=j;best_bound=b;}}
        if(best<0)continue;
        used[i]=used[best]=true;++witnesses;
        if(examples.size()<20)examples.push_back({{"assignment_step",step},{"decision_tick",step-1},{"robot",assignment.robot},
            {"actual_task",t.id},{"actual_bound",bound},{"remaining",remaining},{"alternative_task",tasks[best].id},{"alternative_bound",best_bound}});
    }
    report["known_horizon_bounds"]={{"configured_horizon",horizon},{"cohorts",cohorts},
        {"chronological_witnesses",witnesses},{"exposed_impossible_unfinished_admissions",exposed},{"overlapping_actual_tasks_skipped",overlap_skipped},
        {"bfs_searches",searches},{"manhattan_candidate_exposures",candidates},{"examples",examples},
        {"limits","Exact spatial bounds, stronger than production cache misses. Sequential task-disjoint witnesses against the unchanged pool; same-batch assignments conservatively excluded. Not a counterfactual rollout, complete opportunity count, throughput estimate or upper bound. Reveals and later assignments would change under a new policy."}};
