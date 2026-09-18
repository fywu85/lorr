#include "pibt.hpp"
#include "trace_io.hpp"
#include <iostream>

// A generated header adds only this friend declaration. pibt.cpp stays byte-for-byte
// upstream. The native driver below also checks this stepping wrapper against run().
class PibtStepHarness {
    PIBT planner;
    std::vector<PIBT::Agent> agents;
    PIBT::Agents ordered;
public:
    explicit PibtStepHarness(MAPF_Instance* problem) : planner(problem) {
        planner.createDistanceTable();
        agents.reserve(problem->getNum());
        for (int i = 0; i < problem->getNum(); ++i)
            agents.push_back({i, problem->getStart(i), nullptr, problem->getGoal(i), 0,
                              planner.pathDist(i), getRandomFloat(0, 1, problem->getMT())});
        for (auto& a : agents) ordered.push_back(&a);
    }
    std::vector<int> step(Graph* graph, const std::vector<int>& positions, bool initial) {
        std::fill(planner.occupied_now.begin(), planner.occupied_now.end(), nullptr);
        std::fill(planner.occupied_next.begin(), planner.occupied_next.end(), nullptr);
        for (size_t i = 0; i < agents.size(); ++i) {
            auto& a = agents[i]; a.v_now = graph->getNode(positions[i]); a.v_next = nullptr;
            if (!initial) a.elapsed = a.v_now == a.g ? 0 : a.elapsed + 1;
            planner.occupied_now[positions[i]] = &a;
        }
        std::sort(ordered.begin(), ordered.end(), [](const auto* a, const auto* b) {
            if (a->elapsed != b->elapsed) return a->elapsed > b->elapsed;
            if (a->init_d != b->init_d) return a->init_d > b->init_d;
            return a->tie_breaker > b->tie_breaker;
        });
        for (auto* a : ordered) if (a->v_next == nullptr) planner.funcPIBT(a);
        std::vector<int> next;
        for (const auto& a : agents) next.push_back(a.v_next->id);
        return next;
    }
};

std::string scenario(const traceio::Input& in, const std::string& output) {
    std::ofstream map(output + ".map");
    map << "type octile\nheight " << in.rows << "\nwidth " << in.cols << "\nmap\n";
    for (int r = 0; r < in.rows; ++r) {
        for (int c = 0; c < in.cols; ++c) map << (in.map[r * in.cols + c] ? '@' : '.');
        map << '\n';
    }
    std::ofstream spec(output + ".scen");
    spec << "map_file=" << output << ".map\nagents=" << in.goals.size() << "\nseed=" << in.seed
         << "\nrandom_problem=0\nmax_timestep=" << in.steps << "\nmax_comp_time=1800000\n";
    for (size_t i = 0; i < in.goals.size(); ++i)
        spec << in.positions[i] % in.cols << ',' << in.positions[i] / in.cols << ','
             << in.goals[i] % in.cols << ',' << in.goals[i] / in.cols << '\n';
    return output + ".scen";
}
int main(int argc, char** argv) {
    if (argc != 3) return 2;
    traceio::Json result;
    try {
        traceio::Input in(argv[1]);
        // pibt2 initializes missing distances to max_timestep, so use a cap
        // exceeding every possible finite shortest path in the stepping wrapper.
        auto problem_input = in; problem_input.steps = std::max(in.steps, in.rows * in.cols + 1);
        MAPF_Instance problem(scenario(problem_input, argv[2]));
        const auto begin = traceio::Clock::now(); PibtStepHarness planner(&problem);
        result["preprocess_seconds"] = std::chrono::duration<double>(traceio::Clock::now() - begin).count();
        auto positions = in.positions; auto orientations = in.orientations;
        result["positions"] = {positions}; result["orientations"] = {orientations};
        for (int step = 0; step < in.steps; ++step) {
            const auto start = traceio::Clock::now();
            auto targets = planner.step(problem.getG(), positions, step == 0);
            const double seconds = std::chrono::duration<double>(traceio::Clock::now() - start).count();
            if (seconds >= 30) throw std::runtime_error("upstream step timeout");
            result["decision_seconds"].push_back(seconds);
            traceio::validate(in, positions, targets); result["targets"].push_back(targets);
            if (in.rotation) {
                auto actions = traceio::rotate(in, positions, orientations, targets);
                positions = traceio::apply(in, positions, orientations, actions);
                result["actions"].push_back(actions);
            } else positions = targets;
            result["positions"].push_back(positions); result["orientations"].push_back(orientations);
            if (positions == in.goals) break;
        }
        if (!in.rotation) {
            // Independent full upstream driver validates our per-step exposure,
            // including priority updates, RNG consumption, and stopping behavior.
            MAPF_Instance original(scenario(in, std::string(argv[2]) + ".original"));
            PIBT unmodified(&original); unmodified.solve();
            if (unmodified.overCompTime()) throw std::runtime_error("upstream native timeout");
            const auto full = unmodified.getSolution();
            if (full.size() != static_cast<int>(result["positions"].size())) throw std::runtime_error("wrapper length differs from upstream run");
            for (int t = 0; t < full.size(); ++t) for (int i = 0; i < original.getNum(); ++i)
                if (full.get(t, i)->id != result["positions"][t][i]) throw std::runtime_error("wrapper differs from upstream run");
            result["matches_unmodified_run"] = true;
        }
        result["steps"] = result["positions"].size() - 1;
        result["valid"] = true; result["solved"] = positions == in.goals;
        traceio::write(argv[2], result); return 0;
    } catch (const std::exception& e) {
        result["valid"] = false; result["error"] = e.what(); traceio::write(argv[2], result);
        std::cerr << e.what() << '\n'; return 1;
    }
}
