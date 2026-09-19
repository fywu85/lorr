#pragma once
// Complete deterministic preparation before temporal search. All workers join,
// including when preparation or thread creation fails; no partial result wins.
#include <exception>
#include <stdexcept>
#include <thread>
#include <vector>

namespace cgar {
template<class Prepare>
void run_temporal_preparation(int threads, Prepare prepare) {
    if (threads < 1 || threads > 32) throw std::invalid_argument("invalid temporal preparation thread count");
    std::vector<std::exception_ptr> errors(threads);
    auto work = [&](int worker) {
        try { prepare(worker); }
        catch (...) { errors[worker] = std::current_exception(); }
    };
    std::vector<std::thread> workers;
    try {
        for (int worker = 1; worker < threads; ++worker) workers.emplace_back(work, worker);
    } catch (...) {
        for (auto& worker : workers) worker.join();
        throw;
    }
    work(0);
    for (auto& worker : workers) worker.join();
    for (const auto& error : errors) if (error) std::rethrow_exception(error);
}
} // namespace cgar
