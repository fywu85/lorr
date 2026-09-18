# Fable 5.1 Max review through Claude Code — 2026-09-18

This is the additional independent review requested by the user, performed through the installed Claude Code CLI. The initial review used the session's separate agent tool; its model list did not include Fable. The earlier statement that Fable was unavailable overlooked the CLI and has been corrected.

- [Exact review prompt](prompt.md)
- [Reproducible CLI driver](run_review.py)
- [Earlier measured diagnosis and prototypes](../throughput-20260917/README.md)

The invocation explicitly selected `claude-fable-5-1` with `--effort max`, using Claude Code 2.1.275. The running CLI session has confirmed `claude-fable-5-1`; the completed review and returned usage metadata will be added when it finishes. Available tools were limited to `Read`, `Grep`, and `Glob`, with additional instructions to inspect the source, challenge earlier conclusions, preserve the corrected progress mechanisms, and propose only map-agnostic changes. No production source changes or new benchmarks were requested in this pass.

The review is source analysis and interpretation of the existing experiments. A proposed improvement or newly identified risk is not itself a measured throughput result. Raw CLI events remain in ignored `runs/fable-review-20260918/`; the prompt and driver are retained here, with the final review and compact provenance to follow.

To repeat from the workspace root with an authenticated Claude Code installation:

```sh
python3 experiments/throughput-20260918-fable/run_review.py --output runs/fable-review-repeat
```

The output directory must be new. The driver verifies the initialized model, checks successful completion, and saves the returned review. It does not enable permission bypass or select an automatic fallback model.
