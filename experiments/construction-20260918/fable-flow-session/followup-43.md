Continue our persistent CGAR Warehouse review session, Fable5.1/max. Read-only.

V92 implemented both requested turn43 hardenings: calibration lower-bound failures throw and count explicitly; actual fresh native matching fixture trains at the final proposal holder and excludes a displaced older task. Complete suite and exact startup/generic controls passed. Full seed0/2 x marginOFF/ON H5000 is still running; no margin quality claim. Best is153614, target154795. Do not propose changes to that frozen running experiment.

A bounded independent general implementation optimization is being measured OUTSIDE production. TemporalPibt::build currently writes selected_[r]=k on every candidate, including rejected paths. The attached patch makes blocker take the candidate path explicitly and delays that store until immediately before add(r), after conflict/fixed/visited/random rejections. It changes no candidate order, score operations, RNG calls, recursion/work budget or deadline checks.

Please audit semantic equivalence, especially construction recursion that revisits an ancestor (repair has a visited gate but construction does not), rollback, constructor validation, reference aliasing and exceptions. Give a concrete counterexample if one exists. Distinguish any timeout diagnostic-only difference from successful fixed-work behavior. Do not assert tests were run. If safe, state the key invariant and the minimum meaningful production checks; avoid inventing an entire new testing project.

The existing independent component harness covers two randomized fixtures x cold global/region-filtered/warm unequal-weight/clocked-region modes, eight alternating pairs each,20complete4M-candidate trials. It checks exact plan/score-bit/work-counter checksums and vertex/edge validity. Preliminary106of128observations have exact paired checksums and CPU median reductions roughly0.7–1.6%; final result pending. This is component evidence only. No cache redesign: extrapolated oriented-table kernel cost is only~115CPUseconds out of~3300processCPUseconds/full run.

Prototype patch:
```diff
--- a/temporal_pibt.hpp
+++ b/temporal_pibt.hpp
@@ -102,7 +102,7 @@
         for (int r = 0; r < static_cast<int>(choices_.size()); ++r) {
             if (selected_[r] < 0 || selected_[r] >= static_cast<int>(choices_[r].size()) ||
                 !choice(r).path->valid) throw std::logic_error("invalid temporal seed");
-            if (blocker(r) != -1) throw std::logic_error("conflicting temporal seed");
+            if (blocker(*choice(r).path) != -1) throw std::logic_error("conflicting temporal seed");
             add(r);
         }
         if (choice_regions_) {
@@ -370,7 +370,7 @@
 private:
     double random_real() { return static_cast<double>(std::uniform_int_distribution<uint64_t>{}(rng_)) / UINT64_MAX; }
     int random_int(int a, int b) { return static_cast<int>(std::uniform_int_distribution<int64_t>(a, b)(rng_)); }
-    int blocker(int r) const {
+    int blocker(const TemporalPath& path) const {
         int answer = -1;
         auto take = [&](int other) {
             if (other >= 0) {
@@ -379,7 +379,6 @@
             }
             return true;
         };
-        const auto& path = *choice(r).path;
         for (int t = 0; t < kTemporalHorizon; ++t)
             if (!take(used_cells_[path.cells[t]][t])) return -2;
         for (int t = 0; t < kTemporalHorizon; ++t)
@@ -422,9 +421,9 @@
                 if (choices_[r][k].path->depth > horizon_) continue;
             }
             ++stats.candidates;
-            selected_[r] = k;
-            const int other = blocker(r);
+            const int other = blocker(*choices_[r][k].path);
             if (other == -1) {
+                selected_[r] = k;
                 add(r);
                 if (consider()) return 1;
                 remove(r); selected_[r] = old; return 2;
@@ -436,7 +435,7 @@
                 if (counter > 3000 && depth >= 6) continue;
                 if (selected_[other] != 0 && random_real() < 0.8) continue;
             }
-            remove(other); add(r);
+            remove(other); selected_[r] = k; add(r);
             const int result = build<Indexed>(other, depth + 1, ++counter, repairing, check);
             if (result == 1) return 1;
             remove(r); add(other);
```

Actual current constructor context:
```cpp
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
    long long repair_batches_kept = 0, repair_batches_reverted = 0;
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
            if (blocker(r) != -1) throw std::logic_error("conflicting temporal seed");
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

```

Actual private search context:
```cpp
private:
    double random_real() { return static_cast<double>(std::uniform_int_distribution<uint64_t>{}(rng_)) / UINT64_MAX; }
    int random_int(int a, int b) { return static_cast<int>(std::uniform_int_distribution<int64_t>(a, b)(rng_)); }
    int blocker(int r) const {
        int answer = -1;
        auto take = [&](int other) {
            if (other >= 0) {
                if (answer >= 0 && answer != other) return false;
                answer = other;
            }
            return true;
        };
        const auto& path = *choice(r).path;
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
            selected_[r] = k;
            const int other = blocker(r);
            if (other == -1) {
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
            remove(other); add(r);
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
```
