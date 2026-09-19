#!/usr/bin/env python3
"""Apply narrow, checked diagnostic changes to a frozen copy of NMS."""
from pathlib import Path
import shutil

HERE = Path(__file__).resolve().parent


def replace(path, old, new, count=1):
    text = path.read_text()
    assert text.count(old) == count, (str(path), old, text.count(old), count)
    path.write_text(text.replace(old, new))


def instrument(source):
    solution = source / 'Solution'
    shutil.copy2(HERE / 'nms_probe.hpp', solution / 'nms_probe.hpp')
    settings = solution / 'settings.hpp'
    replace(settings, '#include <cstdint>', '#include <cstdint>\n#include "nms_probe.hpp"')
    replace(settings, 'static constexpr uint32_t THREADS = 32;',
            'static const uint32_t THREADS = std::max(1u, nms_probe_uint("NMS_HELPERS", 32));')
    replace(settings, 'static constexpr uint32_t PIBTS_STEPS = -1;',
            'static const uint32_t PIBTS_STEPS = nms_probe_uint("NMS_STEPS", UINT32_MAX);')
    # Preprocessing can use the reserved allocation while decisions use one core.
    hm = solution / 'Objects/Environment/heuristic_matrix.cpp'
    replace(hm, 'HeuristicMatrix::HeuristicMatrix(const Graph &graph) {',
            'HeuristicMatrix::HeuristicMatrix(const Graph &graph) {\n'
            '    const uint32_t THREADS = std::max(1u, nms_probe_uint("NMS_PREPROCESS_THREADS", 32));')
    header = solution / 'Planner/PIBT/pibts.hpp'
    replace(header, '#pragma once', '#pragma once\n#include "nms_probe.hpp"')
    replace(header, '    uint32_t step = 0;', '    NmsConstructionTrace trace;\n    uint32_t step = 0;')
    cpp = solution / 'Planner/PIBT/pibts.cpp'
    text = cpp.read_text()
    start = text.index('uint32_t PIBTS::build(uint32_t r, uint32_t depth, uint32_t &counter) {')
    end = text.index('bool PIBTS::build(uint32_t r) {', start)
    body = text[start:end]
    body = body.replace('    if (counter == -1',
                        '    ++trace.recursive_calls;\n'
                        '    trace.max_depth = std::max(trace.max_depth, depth);\n'
                        '    if (counter == -1', 1)
    body = body.replace('        counter = -1;',
                        '        if (counter != UINT32_MAX) trace.clock_cut = true;\n'
                        '        counter = -1;', 1)
    body = body.replace('    for (uint32_t desired: robot_desires[r][current_depth]) {',
                        '    for (uint32_t desired: robot_desires[r][current_depth]) {\n'
                        '        ++trace.candidates;', 1)
    text = text[:start] + body + text[end:]
    cpp.write_text(text)
    replace(cpp, 'void PIBTS::solve(uint64_t seed) {\n    rnd = Randomizer(seed);',
            'void PIBTS::solve(uint64_t seed) {\n'
            '    const double solve_started = nms_probe_seconds();\n'
            '    trace.robots = robots.size(); trace.helpers = THREADS;\n'
            '    trace.start_cpu = sched_getcpu();\n'
            '    cpu_set_t mask; CPU_ZERO(&mask);\n'
            '    if (sched_getaffinity(0, sizeof(mask), &mask)) throw std::runtime_error("get affinity");\n'
            '    for (int cpu = 0; cpu < CPU_SETSIZE; ++cpu) if (CPU_ISSET(cpu, &mask)) trace.affinity.push_back(cpu);\n'
            '    rnd = Randomizer(seed);')
    replace(cpp, '''    temp = 0;
    for (uint32_t r: order) {
        if (get_now() >= end_time) {
            break;
        }
        if (desires[r] != 0) {
            continue;
        }
        build(r);
    }

    best_desires = desires;''', '''    temp = 0;
    const double construction_started = nms_probe_seconds();
    for (uint32_t r: order) {
        if (get_now() >= end_time) {
            trace.clock_cut = true;
            break;
        }
        ++trace.order_visited;
        if (desires[r] != 0) {
            continue;
        }
        ++trace.roots;
        if (build(r)) ++trace.roots_accepted;
    }
    trace.construction_complete = trace.order_visited == order.size() && !trace.clock_cut;
    trace.construction_score = cur_score;
    trace.construction_seconds = nms_probe_seconds() - construction_started;

    best_desires = desires;''')
    replace(cpp, '\n}\n\nstd::vector<Action> PIBTS::get_actions() const {',
            '\n    trace.final_score = best_score; trace.search_steps = step;\n'
            '    trace.solve_seconds = nms_probe_seconds() - solve_started;\n'
            '    trace.end_cpu = sched_getcpu();\n'
            '}\n\nstd::vector<Action> PIBTS::get_actions() const {')
    planner = solution / 'Planner/eplanner.cpp'
    replace(planner, 'constexpr uint32_t THR = THREADS;',
            'const uint32_t THR = std::max(1u, nms_probe_uint("NMS_WORKERS", THREADS));\n'
            '        nms_probe_traces().assign(THR, {});')
    replace(planner, '        PIBTS main_pibt_solver(get_robots_handler().get_robots(), end_time);',
            '        const double prepare_started = nms_probe_seconds();\n'
            '        PIBTS main_pibt_solver(get_robots_handler().get_robots(), end_time);\n'
            '        nms_probe_prepare_seconds() = nms_probe_seconds() - prepare_started;')
    replace(planner, '            pibt.solve(seed);',
            '            pibt.solve(seed);\n'
            '            pibt.trace.worker = thr; nms_probe_traces()[thr] = pibt.trace;')
    replace(planner, '        static Randomizer rnd;',
            '        static Randomizer rnd(nms_probe_uint("NMS_SEED", 202));')


def snapshot_driver(original):
    text = original.read_text()
    text = text.replace('#include <array>', '#include <array>\n#include <sched.h>\n#include <cstdlib>\n#ifdef SNAPSHOT_NMS\n#include "nms_probe.hpp"\n#endif')
    needle = '        std::array<long long, 4> actions_all{}, actions_pending{};'
    assert text.count(needle) == 1
    text = text.replace(needle, '''        if (const char* cpu = std::getenv("PROBE_PLAN_CPU")) {
            cpu_set_t mask; CPU_ZERO(&mask); CPU_SET(std::stoi(cpu), &mask);
            if (sched_setaffinity(0, sizeof(mask), &mask)) throw std::runtime_error("set affinity");
        }
        cpu_set_t plan_mask; CPU_ZERO(&plan_mask);
        if (sched_getaffinity(0, sizeof(plan_mask), &plan_mask)) throw std::runtime_error("get affinity");
        result["plan_affinity"] = json::array();
        for (int cpu = 0; cpu < CPU_SETSIZE; ++cpu) if (CPU_ISSET(cpu, &plan_mask)) result["plan_affinity"].push_back(cpu);
        std::array<long long, 4> actions_all{}, actions_pending{};''')
    needle = '            times.push_back(seconds);'
    assert text.count(needle) == 1
    text = text.replace(needle, '''            times.push_back(seconds);
#ifdef SNAPSHOT_NMS
            json traces = json::array();
            for (const auto& x : nms_probe_traces()) traces.push_back({
                {"worker", x.worker}, {"order_visited", x.order_visited}, {"robots", x.robots},
                {"roots", x.roots}, {"roots_accepted", x.roots_accepted},
                {"recursive_calls", x.recursive_calls}, {"candidates", x.candidates},
                {"max_depth", x.max_depth}, {"search_steps", x.search_steps}, {"helpers", x.helpers},
                {"clock_cut", x.clock_cut}, {"construction_complete", x.construction_complete},
                {"construction_score", x.construction_score}, {"final_score", x.final_score},
                {"construction_seconds", x.construction_seconds}, {"solve_seconds", x.solve_seconds},
                {"start_cpu", x.start_cpu}, {"end_cpu", x.end_cpu}, {"affinity", x.affinity}});
            result["construction"].push_back({{"step", t}, {"prepare_seconds", nms_probe_prepare_seconds()}, {"workers", traces}});
#endif''')
    return text
