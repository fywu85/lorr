# Keep log writes off the shared filesystem during timed entries

RANDOM02delay3(sourcec302547d) failed at planner tick199, elapsed2632.087ms,
planning_complete. Its phase timings before diagnostics sum to about355ms;
log_movement() then flushes stdout synchronously before the final deadline check.
The old runner writes stdout directly to the shared archive. Filesystem delay is
a plausible cause, not proven by these samples; scheduling stalls remain possible.
The failed full run is preserved and has no accepted partial score.

Optional `--local-log-spool` in the matrix/runner writes stdout and stderr to a
unique file under `/tmp` on the allocated compute node. It archives the full log
after process exit, including failure exits, and retains normal error propagation.
Solver binary, random stream, fixed work, diagnostics, deadline and memory cap stay
unchanged. Runner hash and sink mode are declared in matrix spec and suite metadata
and verified before comparison. Whole-process wall time includes final archiving;
per-entry deadlines still include all actual solver logging. The option is off
unless requested. Byte-for-byte smoke checks cover exits0/124/17and launch errors.

Declared verification: repeat full RANDOM02controlanddelay3(seed2), same frozen
binary, strict1000ms and four physical cores, with local logs. This determines
its completed throughput if valid; it does not retroactively erase the first
timeout or conclusively identify its cause. Future experiments use this option
to reduce avoidable shared-filesystem exposure during timed entry calls.
