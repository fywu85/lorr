#pragma once
// Generic temporal PIBT construction and fixed-count repair, adapted from the
// archived No Man's Sky PIBTS implementation. See LICENSE_NMS in this directory.
// Geometry and candidate scores are supplied independently of the search policy.
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <random>
#include <stdexcept>
#include <vector>

namespace cgar {
constexpr int kTemporalHorizon = 5;

struct TemporalPath {
    std::array<int, kTemporalHorizon> cells{};
    std::array<int, kTemporalHorizon> edges{};  // undirected edge, or -1 for a stationary action
    uint8_t orientation = 0, first_action = 3, depth = 3;
    bool valid = false;
};

struct TemporalChoice {
    const TemporalPath* path = nullptr;
    int64_t cost = 0;
    int operation = 0;
};

struct TemporalForwardPartition {
    int no_lower_forward = 0, unblocked = 0;
    int one_movable = 0, two_movable = 0, many_movable = 0, protected_blocker = 0;
};

// A read-only snapshot of the finished reservation table, not rejection history.
struct TemporalForwardAudit {
    int stationary = 0, no_lower_forward = 0, unblocked = 0;
    int one_movable = 0, two_movable = 0, many_movable = 0, protected_blocker = 0;
    TemporalForwardPartition easiest, physical;
};

// Read-only candidate footprints for offline joint-repair probes.
// Owner arrays preserve self and every relevant outside reservation.
struct TemporalSnapshotChoice {
    TemporalPath path;
    int64_t cost = 0;
    int operation = 0;
    std::array<int, kTemporalHorizon> cell_owners{}, edge_owners{};
};
struct TemporalSnapshotRobot {
    int robot = -1, selected = -1;
    double power = 0;
    bool fixed = false;
    std::vector<TemporalSnapshotChoice> choices;
};
struct TemporalGroupSnapshot {
    int root = -1, trigger = -1;
    std::vector<TemporalSnapshotRobot> robots;
};

// Post-service reservations are observed slots, not assumed removable blockers.
struct TemporalServiceBucket {
    int robots = 0, selected_wait = 0, selected_turn = 0, selected_forward = 0;
    int candidate_forward = 0;
    int64_t physical_gain = 0;
};
struct TemporalServiceAudit {
    int eligible = 0, no_improving = 0, unblocked = 0, other_blocker = 0;
    int arriving = 0, arriving_known_next = 0;
    // Tags of the cheapest candidate in the easiest improving blocker class:
    // only owners with known next errands, only finishing owners, or both.
    std::array<TemporalServiceBucket, 3> post_service{};
};

// Read-only observations at complete repair-attempt boundaries. A thrown
// deadline leaves completed=false; no partial search result is authorized.
struct TemporalRepairAudit {
    bool completed = false;
    double initial_score = 0, peak_score = 0, final_score = 0, returned_score = 0;
    long long attempts = 0, peak_updates = 0;
    int peak_attempt = 0;
};

struct TemporalStats {
    long long roots = 0, accepted = 0, recursive_calls = 0, candidates = 0;
    long long budget_exhausted = 0, repairs = 0, repairs_accepted = 0;
    long long repair_batches_kept = 0, repair_batches_reverted = 0, repair_peaks_restored = 0;
    int max_depth = 0;
};

class TemporalPibt {
public:
    TemporalPibt(int cells, const std::vector<std::vector<TemporalChoice>>& choices,
                 const std::vector<char>& fixed, const std::vector<double>& power,
                 int displacement_limit, uint64_t seed, const std::vector<int>* initial = nullptr,
                 const std::vector<std::vector<int>>* choice_regions = nullptr, int region = -1)
        : choices_(choices), fixed_(fixed), power_(power), limit_(displacement_limit), rng_(seed),
          selected_(initial ? *initial : std::vector<int>(choices.size(), 0)), visited_(choices.size(), 0),
          used_cells_(cells), used_edges_(2 * cells), choice_regions_(choice_regions), region_(region) {
        if (selected_.size() != choices.size()) throw std::logic_error("invalid temporal initial size");
        for (auto& x : used_cells_) x.fill(-1);
        for (auto& x : used_edges_) x.fill(-1);
        for (int r = 0; r < static_cast<int>(choices_.size()); ++r) {
            if (selected_[r] < 0 || selected_[r] >= static_cast<int>(choices_[r].size()) ||
                !choice(r).path->valid) throw std::logic_error("invalid temporal seed");
            if (blocker(*choice(r).path) != -1) throw std::logic_error("conflicting temporal seed");
            add(r);
        }
        if (choice_regions_) {
            size_t count = 0;
            for (int r = 0; r < static_cast<int>(choices_.size()); ++r)
                if (!fixed_[r]) count += choices_[r].size() - 1;
            candidate_order_.reserve(count * 3);
            candidate_offsets_.resize(choices_.size());
            for (int r = 0; r < static_cast<int>(choices_.size()); ++r) {
                for (int horizon = 3; horizon <= 5; ++horizon) {
                    candidate_offsets_[r][horizon - 3] = candidate_order_.size();
                    if (fixed_[r]) continue;
                    for (int k = 1; k < static_cast<int>(choices_[r].size()); ++k)
                        if (choices_[r][k].path->depth <= horizon && (*choice_regions_)[r][k] == region_)
                            candidate_order_.push_back(k);
                }
                candidate_offsets_[r][3] = candidate_order_.size();
            }
        }
    }

    template<class Deadline>
    void construct(const std::vector<int>& order, Deadline check) {
        for (int r : order) {
            check();
            if (fixed_[r] || selected_[r] != 0) continue;
            ++stats.roots;
            if (attempt(r, false, check)) ++stats.accepted;
        }
    }

    template<class Deadline>
    void repair(int steps, Deadline check, long long candidate_limit = 0, const std::vector<int>* roots = nullptr,
                int temperature_ppm = 1000, TemporalRepairAudit* audit = nullptr, bool keep_peak = false) {
        if (temperature_ppm < 0 || temperature_ppm > 1000000)
            throw std::invalid_argument("temporal repair temperature must be in [0,1000000] ppm");
        if (audit) {
            *audit = {};
            audit->initial_score = audit->peak_score = audit->final_score = audit->returned_score = score_;
        }
        if (roots && roots->empty()) { if (audit) audit->completed = true; return; }
        const double initial_score = score_;
        double best_score = initial_score;
        auto best = selected_;
        temperature_ = static_cast<double>(temperature_ppm) / 1000000.0;
        // A positive limit prescribes a deterministic amount of candidate work,
        // including construction. Check only between complete repair attempts:
        // the last attempt may overshoot, and every root has already finished.
        for (int k = 0; k < steps && (!candidate_limit || stats.candidates < candidate_limit); ++k) {
            check();
            if (k && k % 128 == 0) horizon_ = random_int(3, 5);
            const int draw = random_int(0, static_cast<int>(roots ? roots->size() : choices_.size()) - 1);
            const int r = roots ? (*roots)[draw] : draw;
            ++stats.repairs;
            if (!fixed_[r] && attempt(r, true, check)) ++stats.repairs_accepted;
            temperature_ *= 0.999;
            // Save only complete reservation states. This does not change the
            // search stream or its prescribed stopping point; later attempts
            // continue from the current annealing state, not this snapshot.
            if (keep_peak && score_ > best_score + 1e-6) {
                best_score = score_; best = selected_;
            }
            if (audit) {
                ++audit->attempts;
                if (score_ > audit->peak_score + 1e-6) {
                    audit->peak_score = score_; audit->peak_attempt = k + 1; ++audit->peak_updates;
                }
            }
        }
        if (audit) audit->final_score = score_;
        // Default: preserve NMS's final-versus-initial acceptance rule exactly.
        // Optional retention chooses a strictly better visited plan only after
        // all fixed work has completed. A deadline exception still propagates.
        const bool restore_peak = keep_peak && best_score > score_ + 1e-6;
        if (restore_peak || score_ <= initial_score + 1e-6) {
            if (restore_peak) ++stats.repair_peaks_restored;
            if (keep_peak && best_score > initial_score + 1e-6) ++stats.repair_batches_kept;
            else ++stats.repair_batches_reverted;
            for (int r = 0; r < static_cast<int>(choices_.size()); ++r) remove(r);
            selected_ = std::move(best); score_ = 0;
            for (int r = 0; r < static_cast<int>(choices_.size()); ++r) add(r);
        } else ++stats.repair_batches_kept;
        if (audit) { audit->returned_score = score_; audit->completed = true; }
    }

    const TemporalChoice& choice(int r) const { return choices_[r][selected_[r]]; }
    int selected(int r) const { return selected_[r]; }
    const std::vector<int>& selections() const { return selected_; }
    double score() const { return score_; }
    // Preserve the best-cost classification and also report the least-conflicted
    // improving alternative. The physical partition requires one full unit of
    // distance/turn cost after removing operation tie terms. Fixed owners rank
    // after every non-fixed class. This is const: no search, cache or RNG changes.
    TemporalForwardAudit audit_forward_blockers(int distance_scale = 50, int unit_cost = 1) const {
        if (distance_scale <= 0 || unit_cost <= 0) throw std::invalid_argument("invalid forward audit cost units");
        TemporalForwardAudit out;
        constexpr int protected_class = 2 * kTemporalHorizon + 1, absent = protected_class + 1;
        auto tally = [](auto& counts, int category) {
            if (category == absent) ++counts.no_lower_forward;
            else if (category == protected_class) ++counts.protected_blocker;
            else if (!category) ++counts.unblocked;
            else if (category == 1) ++counts.one_movable;
            else if (category == 2) ++counts.two_movable;
            else ++counts.many_movable;
        };
        for (int r = 0; r < static_cast<int>(choices_.size()); ++r) {
            const int start = choices_[r][0].path->cells[0];
            if (fixed_[r] || power_[r] <= 0 || choice(r).path->cells[0] != start) continue;
            ++out.stationary;
            int best = -1, best_class = absent, easiest = absent, physical = absent;
            const int64_t old_physical = choice(r).cost + int64_t(choice(r).operation) * unit_cost;
            for (int k = 1; k < static_cast<int>(choices_[r].size()); ++k) {
                const auto& candidate = choices_[r][k];
                if (candidate.path->cells[0] == start || candidate.cost >= choice(r).cost) continue;
                std::array<int, 2 * kTemporalHorizon> owners{};
                int count = 0;
                auto take = [&](int owner) {
                    if (owner < 0 || owner == r) return;
                    for (int i = 0; i < count; ++i) if (owners[i] == owner) return;
                    owners[count++] = owner;
                };
                const auto& path = *candidate.path;
                for (int t = 0; t < kTemporalHorizon; ++t) {
                    take(used_cells_[path.cells[t]][t]);
                    if (path.edges[t] >= 0) take(used_edges_[path.edges[t]][t]);
                }
                int category = count;
                for (int i = 0; i < count; ++i) if (fixed_[owners[i]]) category = protected_class;
                easiest = std::min(easiest, category);
                const int64_t candidate_physical = candidate.cost + int64_t(candidate.operation) * unit_cost;
                if (old_physical - candidate_physical >= int64_t(distance_scale) * unit_cost)
                    physical = std::min(physical, category);
                if (best < 0 || candidate.cost < choices_[r][best].cost ||
                    (candidate.cost == choices_[r][best].cost && candidate.operation < choices_[r][best].operation)) {
                    best = k; best_class = category;
                }
            }
            tally(out, best_class); tally(out.easiest, easiest); tally(out.physical, physical);
        }
        return out;
    }
    template<class Deadline>
    std::vector<TemporalGroupSnapshot> snapshot_forward_groups(int limit_per_class,
            uint64_t sample_index, int distance_scale, int unit_cost, Deadline check) const {
        if (limit_per_class < 1 || limit_per_class > 32 || distance_scale <= 0 || unit_cost <= 0)
            throw std::invalid_argument("invalid temporal group snapshot limits");
        std::vector<TemporalGroupSnapshot> out;
        const int count = static_cast<int>(choices_.size());
        if (!count) return out;
        std::array<int, 3> collected{};
        const int start_robot = ((sample_index % count) * 997) % count;
        for (int offset = 0; offset < count; ++offset) {
            if ((offset & 31) == 0) check();
            if (collected[1] == limit_per_class && collected[2] == limit_per_class) break;
            const int r = (start_robot + offset) % count;
            const int start = choices_[r][0].path->cells[0];
            if (fixed_[r] || power_[r] <= 0 || choice(r).path->cells[0] != start) continue;
            const int64_t old_physical = choice(r).cost + int64_t(choice(r).operation) * unit_cost;
            int best = -1, best_count = 2 * kTemporalHorizon + 1;
            std::vector<int> best_owners;
            for (int k = 1; k < static_cast<int>(choices_[r].size()); ++k) {
                const auto& candidate = choices_[r][k];
                if (candidate.path->cells[0] == start || candidate.cost >= choice(r).cost ||
                    old_physical - candidate.cost - int64_t(candidate.operation) * unit_cost < int64_t(distance_scale) * unit_cost)
                    continue;
                std::vector<int> owners;
                auto take = [&](int owner) {
                    if (owner >= 0 && owner != r && std::find(owners.begin(), owners.end(), owner) == owners.end())
                        owners.push_back(owner);
                };
                for (int t = 0; t < kTemporalHorizon; ++t) {
                    take(used_cells_[candidate.path->cells[t]][t]);
                    if (candidate.path->edges[t] >= 0) take(used_edges_[candidate.path->edges[t]][t]);
                }
                if (std::any_of(owners.begin(), owners.end(), [&](int owner) { return fixed_[owner]; })) continue;
                const int n = static_cast<int>(owners.size());
                if (n < best_count || (n == best_count && (best < 0 || candidate.cost < choices_[r][best].cost ||
                    (candidate.cost == choices_[r][best].cost && candidate.operation < choices_[r][best].operation)))) {
                    best = k; best_count = n; best_owners = std::move(owners);
                }
            }
            if (best_count < 1 || best_count > 2 || collected[best_count] == limit_per_class) continue;
            std::sort(best_owners.begin(), best_owners.end());
            best_owners.insert(best_owners.begin(), r);
            TemporalGroupSnapshot group; group.root = r; group.trigger = best;
            for (int owner : best_owners) {
                check();
                TemporalSnapshotRobot robot;
                robot.robot = owner; robot.selected = selected_[owner]; robot.power = power_[owner]; robot.fixed = fixed_[owner];
                for (const auto& candidate : choices_[owner]) {
                    TemporalSnapshotChoice record; record.path = *candidate.path;
                    record.cost = candidate.cost; record.operation = candidate.operation;
                    for (int t = 0; t < kTemporalHorizon; ++t) {
                        record.cell_owners[t] = used_cells_[record.path.cells[t]][t];
                        record.edge_owners[t] = record.path.edges[t] < 0 ? -1 : used_edges_[record.path.edges[t]][t];
                    }
                    robot.choices.push_back(std::move(record));
                }
                group.robots.push_back(std::move(robot));
            }
            out.push_back(std::move(group)); ++collected[best_count];
        }
        check(); return out;
    }
    // Const snapshot, with no search/RNG/cache changes. Every examined candidate
    // improves the distance/turn objective by a full unit after removing op ties.
    // A conflict is post-service only strictly after its owner's first goal hit;
    // arrival-slot conflicts and all fixed owners remain in the other class.
    template<class Deadline>
    TemporalServiceAudit audit_post_service(const std::vector<int>& goals,
            const std::vector<char>& known_next, int distance_scale, int unit_cost,
            Deadline check) const {
        const int count = static_cast<int>(choices_.size());
        if (goals.size() != choices_.size() || known_next.size() != choices_.size() ||
            distance_scale <= 0 || unit_cost <= 0)
            throw std::invalid_argument("invalid post-service audit inputs");
        TemporalServiceAudit out;
        std::vector<int> arrival(count, -1);
        for (int r = 0; r < count; ++r) {
            if ((r & 63) == 0) check();
            if (goals[r] < 0) continue;
            for (int t = 0; t < kTemporalHorizon; ++t) if (choice(r).path->cells[t] == goals[r]) {
                arrival[r] = t; ++out.arriving; out.arriving_known_next += known_next[r] != 0; break;
            }
        }
        for (int r = 0; r < count; ++r) {
            if ((r & 63) == 0) check();
            if (fixed_[r] || goals[r] < 0 || power_[r] <= 0) continue;
            ++out.eligible;
            const int64_t old_physical = choice(r).cost + int64_t(choice(r).operation) * unit_cost;
            int best = -1, best_class = 3, best_tags = 0;
            int64_t best_gain = 0;
            for (int k = 0; k < static_cast<int>(choices_[r].size()); ++k) {
                const auto& candidate = choices_[r][k];
                const int64_t gain = old_physical - candidate.cost - int64_t(candidate.operation) * unit_cost;
                if (gain < int64_t(distance_scale) * unit_cost) continue;
                bool blocked = false, real = false;
                int tags = 0;
                auto take = [&](int owner, int t) {
                    if (owner < 0 || owner == r) return;
                    blocked = true;
                    if (fixed_[owner] || arrival[owner] < 0 || t <= arrival[owner]) real = true;
                    else tags |= known_next[owner] ? 1 : 2;
                };
                const auto& path = *candidate.path;
                for (int t = 0; t < kTemporalHorizon; ++t) {
                    take(used_cells_[path.cells[t]][t], t);
                    if (path.edges[t] >= 0) take(used_edges_[path.edges[t]][t], t);
                }
                const int category = !blocked ? 0 : real ? 2 : 1;
                if (category < best_class || (category == best_class &&
                    (best < 0 || candidate.cost < choices_[r][best].cost ||
                     (candidate.cost == choices_[r][best].cost && candidate.operation < choices_[r][best].operation)))) {
                    best = k; best_class = category; best_tags = tags; best_gain = gain;
                }
            }
            if (best < 0) ++out.no_improving;
            else if (best_class == 0) ++out.unblocked;
            else if (best_class == 2) ++out.other_blocker;
            else {
                if (best_tags < 1 || best_tags > 3) throw std::logic_error("missing post-service owner tag");
                auto& part = out.post_service[best_tags - 1];
                ++part.robots; part.physical_gain += best_gain;
                const int action = choice(r).path->first_action;
                if (action == 0) ++part.selected_forward;
                else if (action == 3) ++part.selected_wait;
                else ++part.selected_turn;
                part.candidate_forward += choices_[r][best].path->first_action == 0;
            }
        }
        check();
        return out;
    }
    TemporalStats stats;

private:
    double random_real() { return static_cast<double>(std::uniform_int_distribution<uint64_t>{}(rng_)) / UINT64_MAX; }
    int random_int(int a, int b) { return static_cast<int>(std::uniform_int_distribution<int64_t>(a, b)(rng_)); }
    int blocker(const TemporalPath& path) const {
        int answer = -1;
        auto take = [&](int other) {
            if (other >= 0) {
                if (answer >= 0 && answer != other) return false;
                answer = other;
            }
            return true;
        };
        for (int t = 0; t < kTemporalHorizon; ++t)
            if (!take(used_cells_[path.cells[t]][t])) return -2;
        for (int t = 0; t < kTemporalHorizon; ++t)
            if (path.edges[t] >= 0 && !take(used_edges_[path.edges[t]][t])) return -2;
        return answer;
    }
    void reserve(int r, bool insert) {
        const auto& path = *choice(r).path;
        for (int t = 0; t < kTemporalHorizon; ++t) {
            used_cells_[path.cells[t]][t] = insert ? r : -1;
            if (path.edges[t] >= 0) used_edges_[path.edges[t]][t] = insert ? r : -1;
        }
        const auto gain = choices_[r][0].cost - choice(r).cost;
        score_ += (insert ? 1 : -1) * static_cast<double>(gain) * power_[r];
    }
    void add(int r) { reserve(r, true); }
    void remove(int r) { reserve(r, false); }
    bool consider() {
        if (old_score_ - 1e-6 <= score_) return true;
        const double draw = random_real();  // construction also consumes this draw
        return temperature_ > 0 && old_score_ > 0 &&
               draw < std::exp(-((old_score_ - score_) / old_score_) / temperature_);
    }
    template<bool Indexed, class Deadline>
    int build(int r, int depth, int& counter, bool repairing, Deadline& check) {
        ++stats.recursive_calls;
        stats.max_depth = std::max(stats.max_depth, depth);
        // construct/repair checked immediately before this root attempt.
        // Keep the existing checks every16 recursive displacements.
        if (counter && (counter & 15) == 0) check();
        if (counter > (repairing ? std::min(limit_, 1000) : limit_)) { ++stats.budget_exhausted; return 2; }
        visited_[r] = generation_;
        const int old = selected_[r];
        // Choices are sorted by the native terminal score, excluding the wait
        // seed at index zero. A failed ordinary wait remains displaceable.
        const size_t end = Indexed ? candidate_offsets_[r][horizon_ - 2] : choices_[r].size();
        for (size_t offset = Indexed ? candidate_offsets_[r][horizon_ - 3] : 1; offset < end; ++offset) {
            const int k = Indexed ? candidate_order_[offset] : static_cast<int>(offset);
            if constexpr (!Indexed) {
                if (choices_[r][k].path->depth > horizon_) continue;
            }
            ++stats.candidates;
            const int other = blocker(*choices_[r][k].path);
            if (other == -1) {
                selected_[r] = k;
                add(r);
                if (consider()) return 1;
                remove(r); selected_[r] = old; return 2;
            }
            if (other < 0 || fixed_[other]) continue;
            if (repairing) {
                if (visited_[other] == generation_ || random_real() < 0.2) continue;
            } else {
                if (counter > 3000 && depth >= 6) continue;
                if (selected_[other] != 0 && random_real() < 0.8) continue;
            }
            remove(other); selected_[r] = k; add(r);
            const int result = build<Indexed>(other, depth + 1, ++counter, repairing, check);
            if (result == 1) return 1;
            remove(r); add(other);
            if (result == 2) { selected_[r] = old; return 2; }
        }
        selected_[r] = old; visited_[r] = 0; return 0;
    }
    template<class Deadline>
    bool attempt(int r, bool repairing, Deadline& check) {
        ++generation_; old_score_ = score_; remove(r);
        int counter = 0;
        const int result = choice_regions_ ? build<true>(r, 0, counter, repairing, check) :
                                           build<false>(r, 0, counter, repairing, check);
        if (result != 1) add(r);
        return result == 1;
    }
    const std::vector<std::vector<TemporalChoice>>& choices_;
    const std::vector<char>& fixed_;
    const std::vector<double>& power_;
    int limit_, horizon_ = 3;
    std::mt19937_64 rng_;
    std::vector<int> selected_, candidate_order_;
    std::vector<std::array<size_t, 4>> candidate_offsets_;
    std::vector<uint64_t> visited_;
    std::vector<std::array<int, kTemporalHorizon>> used_cells_, used_edges_;
    const std::vector<std::vector<int>>* choice_regions_ = nullptr;
    int region_ = -1;
    uint64_t generation_ = 0;
    double score_ = 0, old_score_ = 0, temperature_ = 0;
};

}  // namespace cgar
