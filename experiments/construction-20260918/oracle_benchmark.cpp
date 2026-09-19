// Isolated complete-table timing; this measures runtime, not lifelong throughput.
#include "cgar.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>

int main(int argc, char** argv) {
    try {
        if (argc != 3) throw std::runtime_error("usage: oracle_benchmark MAP MODE");
        std::ifstream input(argv[1]);
        std::string word, type;
        int rows, cols;
        if (!(input >> word >> type) || word != "type" ||
            !(input >> word >> rows) || word != "height" ||
            !(input >> word >> cols) || word != "width" ||
            !(input >> word) || word != "map") throw std::runtime_error("invalid map header");
        std::vector<int> map;
        std::vector<int> goals;
        for (int r = 0; r < rows; ++r) {
            if (!(input >> word) || int(word.size()) != cols) throw std::runtime_error("invalid map row");
            for (char symbol : word) {
                if (std::string(".@TSE").find(symbol) == std::string::npos)
                    throw std::runtime_error("unsupported map symbol");
                if (symbol != '@' && symbol != 'T') goals.push_back(map.size());
                map.push_back(symbol == '@' || symbol == 'T');
            }
        }
        const auto cert = cgar::build_certificate(map, rows, cols, 0);
        std::mt19937_64 rng(917);
        std::shuffle(goals.begin(), goals.end(), rng);
        goals.resize(std::min<size_t>(128, goals.size()));
        const int mode = std::stoi(argv[2]);
        if (mode < 0 || mode > 2) throw std::runtime_error("invalid mode");
        using Clock = std::chrono::steady_clock;
        cgar::TurnDistanceOracle oracle;
        const auto init_start = Clock::now();
        oracle.init(&cert, 0, mode == 1 ? 4 : 1, true);
        if (mode == 2) {
            std::vector<uint8_t> costs(map.size() * 4, 1);
            for (size_t i = 0; i < costs.size(); ++i) costs[i] += (i * 11 + i / 7) % 4;
            oracle.set_forward_costs(std::move(costs));
        }
        const double init_seconds = std::chrono::duration<double>(Clock::now() - init_start).count();
        double table_seconds = 0;
        uint64_t checksum = 14695981039346656037ULL;
        for (int goal : goals) {
            const auto start = Clock::now();
            const auto* table = oracle.table(goal, Clock::time_point::max());
            table_seconds += std::chrono::duration<double>(Clock::now() - start).count();
            for (size_t i = 0; i < table->size(); ++i) {
                checksum ^= uint64_t((*table)[i]);
                checksum *= 1099511628211ULL;
            }
            oracle.trim();
            if (oracle.has(goal)) throw std::runtime_error("benchmark unexpectedly reused a table");
        }
        std::cout << std::setprecision(12) << "{\"mode\":" << mode
                  << ",\"tables\":" << goals.size() << ",\"init_seconds\":" << init_seconds
                  << ",\"table_seconds\":" << table_seconds << ",\"checksum\":\"" << checksum << "\"}\n";
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
