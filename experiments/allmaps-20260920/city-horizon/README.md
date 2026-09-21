# CITY-01 known-horizon admission transfer

Declared full3000-step seed2 comparison on the selected8440 profile: no known
horizon, Manhattan-plus-service bound, observed mean completion margin, observed
90th-percentile margin. These are existing explicit tricks and require
--trick CITY-01. They rank new assignments only; do not drop held/started tasks,
change the oldest-admission policy, or inspect future tasks. Complete fixed work,
strict1000ms entries,4 bound physical cores,32decimalGB and node-local logs.

The CITY counters show little stationary waiting and matching groups mostly
smaller than32, so more assignment width is not the next lever. This asks whether
avoiding new chains unlikely to finish before the known end can improve the
boundary. The related CITY-02 experiment already lost and is not being repeated.
This is a transfer test, not a prior claim of benefit. Source and binary are the
qualified matching-width build7fd2cfc6, with no new production changes.

All four complete and valid. Control8440, bound8410, mean8427, p90 8357; maxima756.89/755.34/762.74/770.28ms. Control repeats the full selected trajectory. No promotion; keep the horizon trick disabled.
