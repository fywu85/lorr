"""Build an isolated diagnostic binary; production CGAR sources stay unchanged."""
from pathlib import Path
import hashlib,json,shlex,shutil,subprocess
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
BUILD=HERE/'build'/'v2';BUILD.mkdir(parents=True,exist_ok=True)
s=(ROOT/'cgar/cgar_planner/cgar.cpp').read_text()
base_hash=hashlib.sha256(s.encode()).hexdigest()
if base_hash != "96a4399c5f1e35e4b661fb2aacb07e6e7318f0041b44a748ba53efe48096e37f":
 raise RuntimeError("This diagnostic patch requires the recorded production source revision")
def replace(old,new,count=1):
 global s
 assert old in s,old[:100]
 s=s.replace(old,new,count)
replace('namespace cgar {','''namespace cgar {
struct DiagnosticStats {
 long long calls=0, assigned=0, local_assigned=0, fallback_assigned=0;
 long long dense_calls=0, sparse_calls=0, nodes=0, empty_limit=0, empty_deadline=0;
 long long pickup_cost=0, chain_cost=0, route_queries=0, manhattan=0, chain_upgrades=0;
};
static DiagnosticStats diagnostic;
static std::unordered_set<int> diagnostic_exact_chains;
''')
replace('    if (it != chain_cost_.end()) return it->second;','''    if (it != chain_cost_.end()) {
        bool upgrade = env_int("CGAR_DIAG_UPGRADE_CHAIN", 0) && !diagnostic_exact_chains.count(task_id);
        if (upgrade) {
            const Task& task = env_->task_pool.at(task_id);
            for (size_t k = task.idx_next_loc + 1; k < task.locations.size(); ++k)
                upgrade &= oracle_.has(task.locations[k]);
        }
        if (!upgrade) return it->second;
        ++diagnostic.chain_upgrades;
        diagnostic_exact_chains.insert(task_id);
    }''')
replace('int Cgar::route_h(int i, int cell) {','int Cgar::route_h(int i, int cell) {\n    ++diagnostic.route_queries;')
replace('if (table_budget_ <= 0) return oracle_.manhattan(cell, a.goal);','if (table_budget_ <= 0) { ++diagnostic.manhattan; return oracle_.manhattan(cell, a.goal); }')
replace('if (t == nullptr) { table_budget_ = 0; return oracle_.manhattan(cell, a.goal); }','if (t == nullptr) { table_budget_ = 0; ++diagnostic.manhattan; return oracle_.manhattan(cell, a.goal); }')
legacy=(HERE/'draft_scheduler.inc').read_text()
legacy=legacy.replace('    proposed = env->curr_task_schedule;', '    deadline_ = distance_deadline_ = deadline;\n    prepare_capacity_mode();\n    if (capacity_mode_ && !parking_ready_) return;\n    proposed = env->curr_task_schedule;')
legacy=legacy.replace('entry.second.agent_assigned == -1)', 'entry.second.agent_assigned == -1 && eligible_task(entry.second))')
legacy=legacy.replace('proposed[i] == -1)', 'proposed[i] == -1 && !parked_[i])')
legacy=legacy.replace('table = &oracle_.table(first);','table = oracle_.try_table(first, deadline);')
legacy=legacy.replace('(*table)[from]','oracle_.value(*table, from)').replace('(*table)[exit]','oracle_.value(*table, exit)')
legacy=legacy.replace('++stats_.assignments;', '++stats_.assignments;\n        ++diagnostic.assigned;\n        diagnostic.pickup_cost += p.cost - chain_cost_.at(p.task);\n        diagnostic.chain_cost += chain_cost_.at(p.task);')
head='void Cgar::schedule(SharedEnvironment* env, int time_limit_ms, std::vector<int>& proposed) {'
replace(head,head+'\n    ++diagnostic.calls;\n    if (env_int("CGAR_DIAG_DRAFT_SCHED", 0)) {\n'+legacy+'\n        return;\n    }')
# All following replacements are unique to the current sparse scheduler.
replace('        proposed[p.robot] = tasks[p.task].id;','''        proposed[p.robot] = tasks[p.task].id;
        ++diagnostic.assigned;
        diagnostic.pickup_cost += p.cost - tasks[p.task].chain;
        diagnostic.chain_cost += tasks[p.task].chain;''')
replace('    auto candidates = [&](int r, int limit) {','''    const int node_limit = std::max(1, env_int("CGAR_DIAG_NODE_LIMIT", 2048));
    auto candidates = [&](int r, int limit) {''')
replace('    auto candidates = [&](int r, int limit) {', '\n'.join([
    '    const int global_samples = std::max(0, env_int("CGAR_DIAG_GLOBAL_SAMPLES", 0));',
    '    const auto candidate_deadline = deadline_ - std::chrono::milliseconds(global_samples ? std::max(1, time_limit_ms / 4) : 0);',
    '    auto candidates = [&](int r, int limit) {']))
replace('head < 2048; ++head)', 'head < static_cast<size_t>(node_limit); ++head)')
replace('            const int u = queue[head];\n            for (int t : at_cell[u])', '            ++diagnostic.nodes;\n            const int u = queue[head];\n            for (int t : at_cell[u])')
replace('        return result;\n    };\n    const int per_robot', '''        if (result.empty()) {
            if (Clock::now() >= deadline_) ++diagnostic.empty_deadline;
            else ++diagnostic.empty_limit;
        }
        return result;
    };
    const int per_robot''')
replace('std::min<long long>(16, max_pairs_ / robots.size())','std::min<long long>(env_int("CGAR_DIAG_CANDIDATES", 16), max_pairs_ / robots.size())')
old='''    for (int r : robots) {
        if (Clock::now() >= deadline_ || static_cast<long long>(pairs.size()) >= max_pairs_) break;
        auto local = candidates(r, std::min<int>(per_robot, static_cast<int>(max_pairs_ - pairs.size())));
        pairs.insert(pairs.end(), local.begin(), local.end());
    }'''
new='''    const bool dense = env_int("CGAR_DIAG_HYBRID", 0) &&
        robots.size() * tasks.size() <= static_cast<size_t>(max_pairs_);
    if (dense) {
        ++diagnostic.dense_calls;
        for (int r : robots) {
            for (size_t t = 0; t < tasks.size(); ++t) {
                if ((pairs.size() & 1023) == 0 && Clock::now() >= deadline_) break;
                pairs.push_back(estimate(r, static_cast<int>(t)));
            }
            if (Clock::now() >= deadline_) break;
        }
    } else {
        ++diagnostic.sparse_calls;
'''+old+'''\n    }'''
replace(old,new)
replace('        assign(p);\n        ++regular_admissions_;\n    }\n    for (int r : robots)', '        assign(p);\n        ++diagnostic.local_assigned;\n        ++regular_admissions_;\n    }\n    for (int r : robots)')
replace('        const Pair best = local.empty() ? estimate(r, fallback)', '''        if (local.empty()) ++diagnostic.fallback_assigned;
        else ++diagnostic.local_assigned;
        const Pair best = local.empty() ? estimate(r, fallback)''')
replace('    std::vector<char> robot_used(n_, 0), task_used(tasks.size(), 0);', '\n'.join([
    '    std::vector<char> robot_used(n_, 0), task_used(tasks.size(), 0);',
    '    std::vector<int> available(tasks.size()), available_position(tasks.size());',
    '    for (size_t t = 0; t < tasks.size(); ++t) available[t] = available_position[t] = static_cast<int>(t);']))
replace('        task_used[p.task] = 1;', '\n'.join([
    '        task_used[p.task] = 1;',
    '        const int slot = available_position[p.task], last = available.back();',
    '        available[slot] = last;',
    '        available_position[last] = slot;',
    '        available.pop_back();',
    '        available_position[p.task] = -1;']))
current_start=s.index('    proposed[p.robot] = tasks[p.task].id;')
begin=s.index('    auto candidates = ', current_start)
end=s.index('\n}  // namespace cgar',begin)
part=s[begin:end].replace('Clock::now() >= deadline_', 'Clock::now() >= candidate_deadline')
s=s[:begin]+part+s[end:]
replace('        const Pair best = local.empty() ? estimate(r, fallback) : *std::min_element(local.begin(), local.end(), better);', '\n'.join([
    '        if (local.empty() && env_int("CGAR_DIAG_DEFER_FALLBACK", 0)) continue;',
    '        Pair best = local.empty() ? estimate(r, fallback) : *std::min_element(local.begin(), local.end(), better);',
    '        if (local.empty() && global_samples) {',
    '            const uint64_t base = (static_cast<uint64_t>(r) + 1) * 2654435761ULL +',
    '                (static_cast<uint64_t>(now) + 1) * 2246822519ULL;',
    '            for (int k = 0; k < global_samples; ++k) {',
    '                const int t = available[(base + static_cast<uint64_t>(k) * 3266489917ULL) % available.size()];',
    '                if (task_used[t]) continue;',
    '                const Pair candidate = estimate(r, t);',
    '                if (better(candidate, best)) best = candidate;',
    '            }',
    '        }']))
log_start=s.index('void Cgar::log_summary()');log_end=s.index('// ─── scheduler',log_start)
part=s[log_start:log_end]
part=part.replace('    std::fflush(stdout);','''    std::printf("[diag] t=%d calls=%lld assigned=%lld local=%lld fallback=%lld dense=%lld sparse=%lld nodes=%lld empty_limit=%lld empty_deadline=%lld pickup_cost=%lld chain_cost=%lld route_queries=%lld manhattan=%lld upgrades=%lld\\n",
       env_->curr_timestep, diagnostic.calls, diagnostic.assigned, diagnostic.local_assigned, diagnostic.fallback_assigned,
       diagnostic.dense_calls, diagnostic.sparse_calls, diagnostic.nodes, diagnostic.empty_limit, diagnostic.empty_deadline,
       diagnostic.pickup_cost, diagnostic.chain_cost, diagnostic.route_queries, diagnostic.manhattan, diagnostic.chain_upgrades);
    std::fflush(stdout);''')
s=s[:log_start]+part+s[log_end:]
(BUILD/'cgar.cpp').write_text(s)
shutil.copy2(str(ROOT/'cgar/cgar_planner/cgar.hpp'),str(BUILD/'cgar.hpp'))
flags={}
for line in (ROOT/'cgar/build/CMakeFiles/lifelong.dir/flags.make').read_text().splitlines():
 if line.startswith('CXX_') and ' = ' in line:
  k,v=line.split(' = ',1);flags[k]=shlex.split(v)
link=shlex.split((ROOT/'cgar/build/CMakeFiles/lifelong.dir/link.txt').read_text())
obj=BUILD/'cgar.cpp.o'
compile_cmd=[link[0]]+flags['CXX_DEFINES']+flags['CXX_INCLUDES']+flags['CXX_FLAGS']+['-c',str(BUILD/'cgar.cpp'),'-o',str(obj)]
subprocess.run(compile_cmd,check=True)
for i,arg in enumerate(link):
 if arg=='CMakeFiles/lifelong.dir/cgar_planner/cgar.cpp.o':link[i]=str(obj)
 if arg=='-o':link[i+1]=str(BUILD/'lifelong')
subprocess.run(link,cwd=str(ROOT/'cgar/build'),check=True)
(BUILD/'metadata.json').write_text(json.dumps({'production_source_sha256':base_hash,'probe_source_sha256':hashlib.sha256(s.encode()).hexdigest(),'binary_sha256':hashlib.sha256((BUILD/'lifelong').read_bytes()).hexdigest()},indent=2)+'\n')
print('Diagnostic executable:',BUILD/'lifelong',flush=True)
