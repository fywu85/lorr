"""Patch only a frozen KittyKnight experimental copy; native archives stay intact."""
from pathlib import Path


def apply(source):
    source = Path(source)
    p = source / 'wppl_planner/src/scheduler.cpp'
    native = p.read_text()
    begin = native.index('void MyScheduler::schedule_plan_greedy_matching_warehouse(')
    end = native.index('\nvoid MyScheduler::schedule_plan(', begin)
    section = native[begin:end]
    def replace(old, new):
        nonlocal section
        assert section.count(old) == 1, old
        section = section.replace(old, new)
    replace('    proposed_schedule = env->curr_task_schedule;', '''    proposed_schedule = env->curr_task_schedule;
    // Diagnostic intervention only: preserve every existing assignment when OFF.
    static const bool rematch_unopened = [] {
        const char* value = std::getenv("LORR_KK_REMATCH_UNOPENED");
        if (!value || std::string(value) == "1") return true;
        if (std::string(value) == "0") return false;
        throw std::invalid_argument("LORR_KK_REMATCH_UNOPENED must be 0 or 1");
    }();''')
    replace('|| task.idx_next_loc==0)', '|| (rematch_unopened && task.idx_next_loc==0))')
    replace('if (task.idx_next_loc == 0)', 'if (rematch_unopened && task.idx_next_loc == 0)')
    replace('    if (assignable_agents_indices.size()>assignable_tasks_indices.size())', '''    if (env->curr_timestep % 200 == 0) {
        std::cout << "[kk-rematch-diag] step=" << env->curr_timestep
                  << " rematch=" << rematch_unopened
                  << " agents=" << assignable_agents_indices.size()
                  << " tasks=" << assignable_tasks_indices.size() << std::endl;
    }

    if (assignable_agents_indices.size()>assignable_tasks_indices.size())''')
    p.write_text('#include <cstdlib>\n#include <stdexcept>\n' + native[:begin] + section + native[end:])
    p = source / 'wppl_planner/src/LNS/LNSSolver.cpp'
    text = p.read_text()
    marker = '    bool succ=lns->run(time_limiter);'
    assert text.count(marker) == 1
    p.write_text(text.replace(marker, '''    const double diagnostic_lns_remaining = time_limiter.get_remaining_time();
''' + marker + '''
    // Post-search observations shared by every ablation arm. These are merged
    // iteration records, not a count of every speculative worker search.
    std::cout << "[kk-lns-diag] step=" << env.curr_timestep
              << " limit_s=" << time_limiter.time_limit
              << " remaining_before_s=" << diagnostic_lns_remaining
              << " success=" << succ
              << " iteration_records=" << lns->iteration_stats.size()
              << " failures=" << lns->num_of_failures
              << " initial_cost=" << lns->initial_sum_of_costs
              << " final_cost=" << lns->sum_of_costs << std::endl;
'''))
    p = source / 'src/Entry.cpp'
    text = p.read_text()
    marker = '    scheduler->plan(time_limit,proposed_schedule);'
    assert text.count(marker) == 1
    text = text.replace(marker, '''    const auto diagnostic_scheduler_begin = std::chrono::steady_clock::now();
''' + marker + '''
    const auto diagnostic_scheduler_end = std::chrono::steady_clock::now();''')
    marker = '    planner->plan(time_limit,plan);'
    assert text.count(marker) == 1
    text = text.replace(marker, marker + '''
    const auto diagnostic_entry_end = std::chrono::steady_clock::now();
    std::cout << "[kk-entry-diag] step=" << env->curr_timestep
              << " scheduler_ms=" << std::chrono::duration<double, std::milli>(
                     diagnostic_scheduler_end - diagnostic_scheduler_begin).count()
              << " elapsed_before_diag_ms=" << std::chrono::duration<double, std::milli>(
                     diagnostic_entry_end - env->plan_start_time).count() << std::endl;
''')
    p.write_text(text)
    return native
