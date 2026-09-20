    // Compare the old holder's CURRENT position with the new holder at the
    // same accepted decision. Comparing their first-admission positions would
    // credit movement before the swap to matching and is deliberately avoided.
    const int horizon = report.at("steps").get<int>();
    std::vector<long long> service(tasks.size());
    std::vector<int> retargets, completions;
    for (int i = 0; i < int(tasks.size()); ++i) {
        const auto& t = tasks[i];
        service[i] = t.chain;
        for (size_t k = 1; k < t.stops.size(); ++k) service[i] += t.stops[k] == t.stops[k - 1];
        if (t.assignments.size() == 2) retargets.push_back(i);
        if (t.finished >= 0 && t.assignments.size() == 1) completions.push_back(i);
    }
    std::sort(retargets.begin(), retargets.end(), [&](int a, int b) {
        return std::make_pair(tasks[a].assignments[1].step, tasks[a].id) <
               std::make_pair(tasks[b].assignments[1].step, tasks[b].id);
    });
    std::sort(completions.begin(), completions.end(), [&](int a, int b) {
        return std::make_pair(tasks[a].finished, tasks[a].id) < std::make_pair(tasks[b].finished, tasks[b].id);
    });
    auto bucket = [](long long bound) { return bound < 50 ? 0 : bound < 100 ? 1 : bound < 200 ? 2 : bound < 400 ? 3 : 4; };
    std::array<std::map<long long, long long>, 5> histograms;
    std::array<long long, 5> samples{}, p90{};
    size_t consumed = 0;
    json witnesses = json::array(), cohorts;
    std::vector<int> thresholds{1, std::max(1, horizon - 1000), std::max(1, horizon - 400), std::max(1, horizon - 100)};
    std::sort(thresholds.begin(), thresholds.end());
    thresholds.erase(std::unique(thresholds.begin(), thresholds.end()), thresholds.end());
    for (int threshold : thresholds) {
        cohorts[std::to_string(threshold)] = {
            {"observed_retargets", 0LL}, {"completed", 0LL}, {"picked_unfinished", 0LL}, {"unpicked", 0LL},
            {"physical_pickup_distance_change_sum", 0LL}, {"physical_distance_shorter", 0LL},
            {"physical_distance_equal", 0LL}, {"physical_distance_longer", 0LL},
            {"physical_feasible_to_impossible", 0LL}, {"physical_impossible_to_feasible", 0LL},
            {"physical_feasible_to_impossible_completed", 0LL}, {"physical_impossible_to_feasible_completed", 0LL},
            {"diagnostic_p90_tier_worse", 0LL}, {"diagnostic_p90_tier_better", 0LL},
            {"diagnostic_p90_tier_worse_completed", 0LL}, {"diagnostic_p90_tier_better_completed", 0LL},
            {"diagnostic_p90_transitions", std::array<std::array<long long, 3>, 3>{}}
        };
    }
    for (int i : retargets) {
        const auto& task = tasks[i];
        const auto& previous = task.departures[0];
        const auto& next = task.assignments[1];
        const int step = std::max(1, next.step), remaining = horizon - step + 1;
        bool updated = false;
        while (consumed < completions.size() && tasks[completions[consumed]].finished < step) {
            const int j = completions[consumed++];
            const auto& done = tasks[j];
            const long long bound = std::max(1, done.first_pickup_distance) + service[j];
            const long long elapsed = done.finished - std::max(1, done.assignments[0].step) + 1;
            if (elapsed < bound) throw std::runtime_error("single-holder completion beats exact physical bound");
            const int b = bucket(bound);
            ++samples[b]; ++histograms[b][elapsed - bound]; updated = true;
        }
        if (updated) for (int b = 0; b < 5; ++b) {
            const long long rank = (samples[b] * 90 + 99) / 100;
            long long count = 0;
            for (const auto& h : histograms[b]) if ((count += h.second) >= rank) { p90[b] = h.first; break; }
        }
        const long long before = std::max(1, task.departure_pickup_distance) + service[i];
        const long long after = std::max(1, task.last_pickup_distance) + service[i];
        auto tier = [&](long long bound) { return bound > remaining ? 2 : bound + p90[bucket(bound)] > remaining ? 1 : 0; };
        const int old_tier = tier(before), new_tier = tier(after);
        if (task.finished >= 0 && after > remaining)
            throw std::runtime_error("completed final holder violates physical bound");
        for (int threshold : thresholds) if (step >= threshold) {
            auto& c = cohorts[std::to_string(threshold)];
            auto add = [&](const char* key, long long value) { c[key] = c[key].get<long long>() + value; };
            add("observed_retargets", 1); add("completed", task.finished >= 0);
            add("picked_unfinished", task.finished < 0 && task.pickup >= 0); add("unpicked", task.pickup < 0);
            add("physical_pickup_distance_change_sum", task.last_pickup_distance - task.departure_pickup_distance);
            add("physical_distance_shorter", task.last_pickup_distance < task.departure_pickup_distance);
            add("physical_distance_equal", task.last_pickup_distance == task.departure_pickup_distance);
            add("physical_distance_longer", task.last_pickup_distance > task.departure_pickup_distance);
            add("physical_feasible_to_impossible", before <= remaining && after > remaining);
            add("physical_impossible_to_feasible", before > remaining && after <= remaining);
            add("physical_feasible_to_impossible_completed", before <= remaining && after > remaining && task.finished >= 0);
            add("physical_impossible_to_feasible_completed", before > remaining && after <= remaining && task.finished >= 0);
            add("diagnostic_p90_tier_worse", new_tier > old_tier);
            add("diagnostic_p90_tier_better", new_tier < old_tier);
            add("diagnostic_p90_tier_worse_completed", new_tier > old_tier && task.finished >= 0);
            add("diagnostic_p90_tier_better_completed", new_tier < old_tier && task.finished >= 0);
            auto& n = c["diagnostic_p90_transitions"][old_tier][new_tier]; n = n.get<long long>() + 1;
        }
        if ((old_tier != new_tier || before <= remaining && after > remaining) && witnesses.size() < 40)
            witnesses.push_back({{"task", task.id}, {"step", step}, {"decision_tick", step - 1},
                {"old_robot", previous.robot}, {"new_robot", next.robot}, {"old_cell", previous.cell}, {"new_cell", next.cell},
                {"remaining", remaining}, {"old_pickup", task.departure_pickup_distance}, {"new_pickup", task.last_pickup_distance},
                {"chain_service_bound", service[i]}, {"old_bound", before}, {"new_bound", after},
                {"old_diagnostic_tier", old_tier}, {"new_diagnostic_tier", new_tier},
                {"old_bucket_count", samples[bucket(before)]}, {"new_bucket_count", samples[bucket(after)]},
                {"old_margin", p90[bucket(before)]}, {"new_margin", p90[bucket(after)]},
                {"pickup_step", task.pickup}, {"finish_step", task.finished}});
    }
    report["rematch_cutoff_audit"] = {{"cohorts", cohorts}, {"examples", witnesses},
        {"calibration_completions_observed", consumed}, {"calibration_samples", samples},
        {"limits", "Only visible accepted holder changes, at most one per task. Fresh task permutations within a single scheduling entry are not observable. Physical distances use exact unweighted BFS and compare both holders at the same decision. The prospective p90 uses exact physical first-admission bounds and completed single-holder tasks before the decision, so it is not the production cached/mixed estimator. No counterfactual movement, completion gain, complete matching-cycle account or throughput upper bound."}};
