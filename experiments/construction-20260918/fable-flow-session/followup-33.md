Continue the SAME persistent Fable 5.1/max session. Read-only, no tools/tests. The user explicitly authorizes continued source sharing through Claude Code. CGAR remains the framework; do not replace it with WPPL.

Goal: any valid full Warehouse 5000-step seed >=154795, general or explicitly --trick WAREHOUSE, 32GB. Fixed work, explicit timeout failure. Shared-host5s development deadlines permitted, aim roughly subsecond on exclusive cores. Best overall146659 static adapted lanes, generic peak145083, generic six-seed reference143941.17.

Since turn33: reproduced/fixed the stale-ticket next-primary defect; also found/fixed missing fair_tasks_ insertion for matching-only mandatory admissions. Production tests now cover both plus fresh ordinary holders; V62 full cancelled, no partial quality score. Fixed group quota4/64 added, defaultOFF. V64 800-step paired seeds0/2: OFF18954/18952, groups4 18999/18908, groups64 18958/18967. All valid, exact OFF fingerprints, no full quality claim. Full OFF/4/64 x0/2 is running now. At t2200 quota64 seed0 cumulative groups9974, selected294623, moved13157, weighted saving256487; eligible338512/resident327935/missing10577/budget_protected21676. These are exposure samples, not whole-run or realized savings.

V65 explicit --trick WAREHOUSE + CGAR_TRICK_UNOPENED_MATCH=1 added, static metric gate fixed, components/field receipts verified. General selector remains rejected under TRICK. Full lanes fixedON x shortOFF/ON x matchingOFF/64 xseeds0/2 is running. 200-step screen allvalid with exact controls. At t1200 seed0 lanes+match cumulative moved9628/saving203584; lanes+short+match moved27292/saving1337368 and budget_protected120378. Do NOT extrapolate full scores from these samples.

V66 optional mixed grouping implemented and full regressions pass: CGAR_REASSIGN_MATCH_PICKUP_GROUPS=1 admits up to16 nearby robots and16 eligible holders whose pickups are nearby, using two spatial indexes and the SAME BFS <=2048 nodes/group, <=32 total participants, <=64groups/pass, every10steps. No extra builds, same one-retarget/task/cooldown20 and cycle thresholds, defaultOFF. A production fixture shows a distant holder outside both local BFS neighborhoods now making a beneficial two-cycle; mode0 unchanged. Current paired800 screen is running, full only after validation. Please flag actual correctness issues in the delta if any.

A wider question while these tests run: our only NMS lane experiment uses forward4/16, turn4, no coordinate-band surcharge, and pays traversed contraflow extras. Native NMS uses forward20/200 (+1 in two warehouse bands), turn1, and remaining-distance-only score with op tie term. We have NOT tested that native cost ratio/cheap turns/bands or remaining-flow combined with static lanes. Generic remaining-flow averaged only+0.08267% over six seeds with4wins2losses, so not promoted. Static lane trial itself was +~1.6% on0/2, largely startup; late rates roughly equal. Current archived NMS diagnostic overhead remains substantially lower but cohorts/resources differ and are NOT causal savings/ceilings.

Please choose the next bounded experiment if the full matching factorial loses or is small. Compare (a) remaining-flow + static lanes, (b) stronger contraflow ratio/cheap turns/bands closer to actual NMS, (c) removing one-retarget limits only as an explicit TRICK, versus another concrete higher-value change. Explain why your choice discriminates a structural mismatch rather than more parameter tweaking. Identify minimal code/metric consistency safeguards and what measured result would make us reject it. In particular, CGAR cost() charges turn_cost-unit_cost and the terminal-wait rule; simply setting turn below unit can create negative extras. Current oracle validates <=16 edge weights and uses uint8, lossless wide-distance fallback. Native20/200 may need bounds/units changes, not a naive knob. No throughput promises or source-free upper bounds. A concise actionable final under1000words is enough.

Relevant unchanged native NMS excerpts follow; CGAR deltas supplied by wrapper.

Planner/PIBT/pibts.cpp
121:int64_t PIBTS::get_smart_dist_IMPL(uint32_t r, uint32_t desired) const {
122:    const auto &op = get_operations()[desired];
123:    const auto &path = get_omap().get_nodes_path(robots[r].node, desired);
124:
125:    const uint32_t target = robots[r].target;
126:
127:    int64_t dist =
128:#ifdef ENABLE_DHMR
129:            get_dhmr().get(r, path.back());
130:#else
131:            get_hm().get(path.back(), target);
132:#endif
133:
134:    if (op.back() == Action::W) {
135:        uint32_t node = path[path.size() - 2];
136:        {
137:            uint32_t to = get_graph().get_to_node(node, 1);
138:            dist = std::min(dist, static_cast<int64_t>(
139:#ifdef ENABLE_DHMR
140:                                          get_dhmr().get(r, to)
141:#else
142:                                          get_hm().get(to, target)
143:#endif
144:                                                  ));
145:        }
146:        {
147:            uint32_t to = get_graph().get_to_node(node, 2);
148:            dist = std::min(dist, static_cast<int64_t>(
149:#ifdef ENABLE_DHMR
150:                                          get_dhmr().get(r, to)
151:#else
152:                                          get_hm().get(to, target)
153:#endif
154:                                                  ));
155:        }
156:
157:        if (op[op.size() - 2] == Action::W) {
158:            uint32_t to = node;
159:            to = get_graph().get_to_node(to, 1);
160:            to = get_graph().get_to_node(to, 1);
161:            dist = std::min(dist, static_cast<int64_t>(
162:#ifdef ENABLE_DHMR
163:                                          get_dhmr().get(r, to)
164:#else
165:                                          get_hm().get(to, target)
166:#endif
167:                                                  ));
168:        }
169:    }
170:
171:    // [KEK]: если мы проходим по таргету, то мы должны это делать как можно раньше. 7200 -> 7297
172:    for (uint32_t d = 0; d < DEPTH; d++) {
173:        if (get_graph().get_pos(path[d]).get_pos() == target) {
174:            dist = d;
175:            dist = -dist;
176:        }
177:    }
178:
179:    dist = dist * 50 - desired;
180:
181:    // стой и никому не мешай
182:    if (robots[r].is_disable()) {
183:        dist = desired;
184:    }
185:    return dist;

Objects/Environment/graph_guidance.cpp
69:void GraphGuidance::set_warehouse() {
70:    set_grid();
71:
72:    uint32_t msk = 0b10100101;
73:    int bit = 0;
74:    for (uint32_t y = 0; y < cols; y++) {
75:        if ((msk >> bit) & 1) {
76:            set(0, y, rows - 1, y, 1, 0, PENALTY_WEIGHT);
77:            set(0, y, rows - 1, y, 3, 0, OK_WEIGHT);
78:        } else {
79:            set(0, y, rows - 1, y, 3, 0, PENALTY_WEIGHT);
80:            set(0, y, rows - 1, y, 1, 0, OK_WEIGHT);
81:        }
82:        bit = (bit + 1) % 8;
83:    }
84:
85:    // [KEK]: повышает вес верхней и нижней плашки, что уменьшает загруженность агентов там
86:    // реально улучшает
87:    // 36754 -> 37548
88:    {
89:        add(3, 0, 10, cols - 1, 0, 0, 1);
90:        add(3, 0, 10, cols - 1, 1, 0, 1);
91:        add(3, 0, 10, cols - 1, 2, 0, 1);
92:        add(3, 0, 10, cols - 1, 3, 0, 1);
93:
94:        add(125, 0, 137, cols - 1, 0, 0, 1);
95:        add(125, 0, 137, cols - 1, 1, 0, 1);
96:        add(125, 0, 137, cols - 1, 2, 0, 1);
97:        add(125, 0, 137, cols - 1, 3, 0, 1);
98:    }
99:}
