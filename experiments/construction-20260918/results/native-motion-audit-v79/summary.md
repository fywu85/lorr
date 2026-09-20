# TRICK: native costs reduce loaded motion overhead

Eight saved full runs are independently replayed:400million physical robot steps.
All input/trajectory hashes, base fixtures, lane-phase fixture and bad-field
rejection pass. Lane counts describe direction alignment; they are not native
weighted-cost totals or a separate band-cell visitation analysis.

| Policy / seed | Tasks | Shortest completed chain | Empty steps/task | Loaded overhead/task | Loaded turns/task | Loaded waits/task |
|---|---:|---:|---:|---:|---:|---:|
|trick_native_bands-s0-r0|151443|242.283|62.498|16.370|5.696|7.690|
|trick_native_bands-s2-r0|151630|242.340|62.430|16.113|5.683|7.449|
|trick_native_control_paid-s0-r0|146659|242.244|67.180|21.924|5.943|11.172|
|trick_native_control_paid-s2-r0|146566|242.187|67.299|22.035|5.968|11.254|
|trick_native_control_potential-s0-r0|147422|242.223|65.781|21.624|6.637|10.906|
|trick_native_control_potential-s2-r0|147502|242.251|65.678|21.528|6.612|10.885|
|trick_native_no_bands-s0-r0|149915|242.294|63.791|18.337|6.154|8.850|
|trick_native_no_bands-s2-r0|149871|242.334|63.844|18.361|6.144|8.885|

The native band policy lowers loaded overhead from21.624/21.528 to16.370/16.113
steps per completed task versus remaining-potential controls. Empty work falls
from65.781/65.678 to62.498/62.430. Completed shortest-chain length remains~242.3.
This supports a motion-related interpretation of the gain, with both reduced
waiting and fewer detours/turns. It is not a causal partition: routing, assignment
and completed cohorts all change together.

The archived unmatched NMS diagnostic had234.794chain,62.021empty and16.755loaded
overhead. Its different resources/cohort and one timeout preclude a certified
matched comparison. These numbers suggest task selection is worth testing next;
they do not promise that changing fairness will recover the numerical chain gap.
The general reference remains unchanged, and short preference is still OFF.

[Accounting](accounting.json), [full quality/resource verification](../native-metric-full-v73/summary.md).
