# Persistent Fable research consultation

The user explicitly authorized passing project information through Claude Code
and asked that this review retain one consistent session. The runner initializes
`claude-fable-5-1 --effort max` once, then resumes the same UUID and supplies only
changed source excerpts as unified diffs plus the next prompt. Fixed working
directory and state live under ignored `runs/fable-cgar-session/`. CLI session
persistence is enabled. Earlier guide reviews disabled persistence, so a new
bootstrap is necessary; this is not a continuation of their missing transcripts.

The consultation has no tools and cannot edit the repository. Each payload is
prepared and hashed before execution. A lock prevents concurrent turns on the
same session. The initialized model and session ID are checked. The configured
CLI budget is $12 per call; a prior review exceeded this setting before reporting
a budget failure, so it must not be described as an exact spending cap.
Only visible findings and usage/status metadata may be copied into this directory;
raw protocol data and private reasoning stay excluded from version control.

Initial payload: six relevant source files/excerpts, completed matched-seed
findings, current negative guide results, and the two pending flow interventions.
See prompt.md and source-spec.json. No throughput conclusion is based on this
review before independent validation.

For subsequent turns, prepare a new output directory with a new `--prompt` file,
then execute that prepared directory. The runner automatically uses `--resume`
and unchanged source snapshots are omitted. Do not delete the persistent state
or pass `--no-session-persistence`.
