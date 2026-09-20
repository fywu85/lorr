The first saved audit failed after its fixtures because its assertion treated
an initially impossible robot-task pair as impossible after later retargeting.
This is a diagnostic-assumption error, not a solver error. No report or policy
result was accepted from this job8899649. Raw frozen sources/log retained.
The revised audit counts completed initial-pair rescues and checks the FINAL
holder lower bound instead. All bounds are conditional on keeping the holder.
