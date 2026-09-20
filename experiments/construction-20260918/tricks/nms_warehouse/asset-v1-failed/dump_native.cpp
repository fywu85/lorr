// Execute the archived NMS constructor unchanged; no copied lane-mask logic.
#include <Objects/Environment/graph_guidance.hpp>
#include <Objects/Environment/map.hpp>
#include <Objects/Environment/info.hpp>
#include <Objects/Basic/position.hpp>
#include <fstream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv) {
    if (argc != 2) throw std::invalid_argument("usage: dump_native MAP");
    std::ifstream input(argv[1]);
    std::string word, row;
    SharedEnvironment env;
    input >> word >> word >> word >> env.rows >> word >> env.cols >> word;
    if (!input || env.rows != 140 || env.cols != 500 || word != "map")
        throw std::runtime_error("unexpected warehouse map header");
    for (int r = 0; r < env.rows; ++r) {
        input >> row;
        if (row.size() != size_t(env.cols)) throw std::runtime_error("map row width");
        for (char cell : row) {
            if (cell != '.' && cell != '@' && cell != 'T')
                throw std::runtime_error("unrecognized map cell");
            env.map.push_back(cell == '@' || cell == 'T');
        }
    }
    get_map() = Map(env);
    get_map_type() = MapType::WAREHOUSE;
    GraphGuidance guidance(env);
    // Verify actual NMS direction semantics, independently of the field values.
    const int delta[] = {1, env.cols, -1, -env.cols};
    for (int d = 0; d < 4; ++d) {
        Position p(20, 20, d);
        if (int(p.move_forward().get_pos()) != int(p.get_pos()) + delta[d])
            throw std::runtime_error("NMS direction convention mismatch");
    }
    std::cout << env.rows << ' ' << env.cols << '\n';
    for (int cell = 0; cell < env.rows * env.cols; ++cell) {
        for (int d = 0; d < 4; ++d) {
            const auto cost = guidance.get(cell + 1, d, 0);
            for (int a = 1; a < 4; ++a)
                if (guidance.get(cell + 1, d, a) != (env.map[cell] ? 0u : 1u))
                    throw std::runtime_error("unexpected native non-forward cost");
            if (env.map[cell] ? cost != 0 : (cost != 20 && cost != 21 && cost != 200 && cost != 201))
                throw std::runtime_error("unexpected native forward cost");
            std::cout << cost << (d == 3 ? '\n' : ' ');
        }
    }
}
