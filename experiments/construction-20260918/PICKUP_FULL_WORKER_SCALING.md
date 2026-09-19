# Additional complete planning workers

The first two full seeds improved with four4M workers. Check whether eight or
sixteen complete alternatives add useful throughput. This is a fixed-work policy
comparison, with every alternative finished before score selection. The four-worker
policy is still undergoing six-seed confirmation, and larger counts may regress.

Screen8898975 completed4/8/16workers with16 physical cores per process. All three
200-step prefixes meet one second and the four-worker control exactly matches its
earlier four-core trajectory. Maxima are902.5/918.8/968.9ms; peakRSS5.283GB.
[Screen and exact control](results/pickup-full-worker-scaling-screen-v43/).

Full8898978/analysis8898979 runs8/16workers, seeds0/2, four cases simultaneously on
64 physical cores (16 per process),64GiB aggregate reservation and32decimalGB per
process. Frozen source remains v43/1661176. Four-worker paired scores come from
results/pickup-full-workers-full-v43; these had four cores per process. The same
CPU family and fixed-work contract permit a quality comparison; different core
allocations preclude an equal-resource runtime-speedup claim. Analysis's automatic
paired table uses8workers as its within-matrix control. The final interpretation
must also include the existing4worker and confirmed1worker baselines.
[Full configuration](pickup-full-worker-scaling-full-variants.json).

Passing a prefix does not guarantee all5000 steps meet the deadline or RAM cap.
Any full timeout is an explicit failure with no accepted score. Only six-seed
confirmation can establish a new repeatable reference.
