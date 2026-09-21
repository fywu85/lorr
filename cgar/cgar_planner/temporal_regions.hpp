#pragma once
// Deterministic parallel repair of disjoint portions of one temporal plan.
// Partitions depend only on dimensions; no map identities or lane templates.
#include "temporal_pibt.hpp"
#include <atomic>
#include <exception>
#include <memory>
#include <thread>

namespace cgar {
struct TemporalRegionOptions {
    int parts = 4, rounds = 2, steps = 25000, threads = 4;
    int temperature_ppm = 1000;
    // Per region and round; zero preserves the original attempt-only budget.
    // Stop between complete attempts, so the last attempt can exceed this count.
    long long candidate_limit = 0;
    bool audit_peaks = false, keep_peak = false;
};

struct TemporalRegionPeaks {
    long long batches = 0, attempts = 0, peak_updates = 0, lost_peaks = 0, lost_improvements = 0;
    long long peak_attempt_sum = 0;
    int max_peak_attempt = 0;
    double peak_gain = 0, final_gain = 0, discarded_gain = 0;
    void observe(const TemporalRepairAudit& a) {
        if (!a.completed) throw std::logic_error("incomplete regional peak audit");
        ++batches; attempts += a.attempts; peak_updates += a.peak_updates;
        lost_peaks += a.peak_score > a.returned_score + 1e-6;
        lost_improvements += a.peak_score > a.initial_score + 1e-6 && a.final_score <= a.initial_score + 1e-6;
        peak_attempt_sum += a.peak_attempt; max_peak_attempt = std::max(max_peak_attempt, a.peak_attempt);
        peak_gain += a.peak_score - a.initial_score; final_gain += a.final_score - a.initial_score;
        discarded_gain += std::max(0.0, a.peak_score - a.returned_score);
    }
    void merge(const TemporalRegionPeaks& b) {
        batches += b.batches; attempts += b.attempts; peak_updates += b.peak_updates;
        lost_peaks += b.lost_peaks; lost_improvements += b.lost_improvements;
        peak_attempt_sum += b.peak_attempt_sum; max_peak_attempt = std::max(max_peak_attempt, b.max_peak_attempt);
        peak_gain += b.peak_gain; final_gain += b.final_gain; discarded_gain += b.discarded_gain;
    }
};

struct TemporalRegionStats {
    long long active_robots = 0, candidates = 0, repairs = 0, accepted = 0;
    long long kept_regions = 0, reverted_regions = 0, frozen_crossers = 0;
    long long candidate_limited_batches = 0, max_batch_candidates = 0, peaks_restored = 0;
    double score_before = 0, score_after = 0;
    std::vector<double> round_scores;
    TemporalRegionPeaks peaks;
};

template<class Deadline>
std::unique_ptr<TemporalPibt> repair_temporal_regions(
        int rows, int cols, const std::vector<int>& locations,
        const std::vector<std::vector<TemporalChoice>>& choices,
        const std::vector<char>& protected_robots, const std::vector<double>& power,
        int displacement_limit, const TemporalPibt& initial,
        const TemporalRegionOptions& options, std::mt19937_64& rng,
        TemporalRegionStats& stats, Deadline check) {
    if (options.parts < 1 || options.parts > 32 || options.rounds < 1 ||
        options.steps < 0 || options.candidate_limit < 0 || options.threads < 1 || options.threads > options.parts ||
        options.temperature_ppm < 0 || options.temperature_ppm > 10000)
        throw std::invalid_argument("invalid temporal region work limits");
    const int count = static_cast<int>(choices.size()), cells = rows * cols;
    int row_parts = 1, col_parts = options.parts;
    double aspect = 1e100;
    for (int rp = 1; rp <= options.parts; ++rp) if (options.parts % rp == 0) {
        const int cp = options.parts / rp;
        const double candidate = std::abs(std::log(double(rows) * cp / (double(cols) * rp)));
        if (candidate < aspect) { aspect = candidate; row_parts = rp; col_parts = cp; }
    }
    std::vector<int> selected = initial.selections();
    // Compare fresh sums on both sides, avoiding accumulated round-0 drift
    // when unequal robot weights produce nonintegral scores.
    auto merged = std::make_unique<TemporalPibt>(cells, choices, protected_robots, power,
        displacement_limit, 0, &selected);
    stats.score_before = merged->score();
    for (int round = 0; round < options.rounds; ++round) {
        check();
        const int phase = round % 4;
        const int row_shift = phase == 1 || phase == 2 ? rows / (2 * row_parts) : 0;
        const int col_shift = phase == 1 || phase == 3 ? cols / (2 * col_parts) : 0;
        std::vector<int> owner(cells);
        for (int u = 0; u < cells; ++u)
            owner[u] = ((u / cols + row_shift) % rows) * row_parts / rows * col_parts +
                       ((u % cols + col_shift) % cols) * col_parts / cols;
        std::vector<std::vector<int>> choice_regions(count), roots(options.parts);
        for (int r = 0; r < count; ++r) {
            if ((r & 63) == 0) check();
            auto& allowed = choice_regions[r]; allowed.resize(choices[r].size(), -1);
            for (size_t k = 0; k < choices[r].size(); ++k) {
                int region = owner[locations[r]];
                for (int u : choices[r][k].path->cells)
                    if (owner[u] != region) { region = -1; break; }
                allowed[k] = region;
            }
            // A robot whose existing path crosses a boundary stays fixed for
            // this round. Its complete reservations remain visible everywhere.
            const int region = allowed[selected[r]];
            if (region < 0) ++stats.frozen_crossers;
            if (!protected_robots[r] && region >= 0) roots[region].push_back(r);
        }
        std::vector<uint64_t> seeds(options.parts);
        for (auto& seed : seeds) seed = rng();
        // Declare masks first so they outlive the searches during destruction.
        std::vector<std::vector<char>> fixed(options.parts, std::vector<char>(count, true));
        std::vector<std::unique_ptr<TemporalPibt>> results(options.parts);
        std::vector<std::exception_ptr> errors(options.parts);
        std::vector<TemporalRepairAudit> audits(options.audit_peaks ? options.parts : 0);
        for (int region = 0; region < options.parts; ++region)
            for (int r : roots[region]) fixed[region][r] = false;
        std::atomic<int> next{0};
        auto work = [&] {
            for (;;) {
                const int region = next.fetch_add(1);
                if (region >= options.parts) return;
                if (roots[region].empty()) continue;
                try {
                    check();
                    auto search = std::make_unique<TemporalPibt>(cells, choices, fixed[region], power,
                        displacement_limit, seeds[region], &selected, &choice_regions, region);
                    search->repair(options.steps, check, options.candidate_limit, &roots[region], options.temperature_ppm,
                                   options.audit_peaks ? &audits[region] : nullptr, options.keep_peak);
                    check(); results[region] = std::move(search);
                } catch (...) { errors[region] = std::current_exception(); }
            }
        };
        std::vector<std::thread> threads;
        try {
            for (int t = 1; t < options.threads; ++t) threads.emplace_back(work);
        } catch (...) {
            for (auto& thread : threads) thread.join();
            throw;
        }
        work(); for (auto& thread : threads) thread.join();
        for (const auto& error : errors) if (error) std::rethrow_exception(error);
        const double before = merged->score();
        const auto previous = selected;
        for (int region = 0; region < options.parts; ++region) if (results[region]) {
            for (int r = 0; r < count; ++r) {
                if (fixed[region][r]) {
                    if (results[region]->selected(r) != previous[r])
                        throw std::logic_error("regional search changed a protected robot");
                } else selected[r] = results[region]->selected(r);
            }
            stats.active_robots += roots[region].size();
            const auto& observed = results[region]->stats;
            stats.candidates += observed.candidates; stats.repairs += observed.repairs;
            stats.accepted += observed.repairs_accepted;
            stats.max_batch_candidates = std::max(stats.max_batch_candidates, observed.candidates);
            stats.candidate_limited_batches += options.candidate_limit &&
                observed.candidates >= options.candidate_limit && observed.repairs < options.steps;
            stats.peaks_restored += observed.repair_peaks_restored;
            stats.kept_regions += observed.repair_batches_kept;
            stats.reverted_regions += observed.repair_batches_reverted;
            if (options.audit_peaks) stats.peaks.observe(audits[region]);
        }
        // Reconstruct all reservations together. The constructor checks every
        // occupied cell and edge, so an invalid merge cannot escape the layer.
        merged = std::make_unique<TemporalPibt>(cells, choices, protected_robots, power,
            displacement_limit, 0, &selected);
        stats.score_after = merged->score(); stats.round_scores.push_back(stats.score_after);
        if (merged->score() + 1e-6 < before)
            throw std::logic_error("regional repair reduced the complete-plan score");
        check();
    }
    return merged;
}
}  // namespace cgar
