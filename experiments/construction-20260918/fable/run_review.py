"""Run the user-requested independent review through Claude Code CLI."""
from pathlib import Path
import argparse, datetime, hashlib, json, subprocess
ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
parser = argparse.ArgumentParser()
parser.add_argument("--output", type=Path, default=ROOT / "runs/fable-construction-review-20260918")
args = parser.parse_args()
out = args.output.resolve()
out.mkdir(parents=True, exist_ok=False)
prompt = (HERE / "prompt.md").read_text()
command = ["claude", "--print", "--model", "claude-fable-5-1", "--effort", "max",
           "--output-format", "stream-json", "--verbose", "--tools", "Read,Grep,Glob",
           "--allowedTools", "Read,Grep,Glob", "--permission-mode", "dontAsk",
           "--strict-mcp-config", "--no-chrome", "--disable-slash-commands", "--no-session-persistence",
           "--append-system-prompt", "This is an independent read-only repository review. Use only Read, Grep, and Glob on repository files. Do not edit files, run commands, delegate, or inspect secrets. Treat earlier reports as fallible evidence."]
metadata = {"started_utc": datetime.datetime.now(datetime.timezone.utc).isoformat(),
            "source_commit": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=str(ROOT), universal_newlines=True).strip(),
            "cli_version": subprocess.check_output(["claude", "--version"], universal_newlines=True).strip(),
            "requested_model": "claude-fable-5-1", "requested_effort": "max", "command": command,
            "prompt_sha256": hashlib.sha256(prompt.encode()).hexdigest()}
result = None
init_model = None
with (out / "stderr.log").open("w") as stderr, (out / "stream.jsonl").open("w") as raw:
    process = subprocess.Popen(command, cwd=str(ROOT), stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                               stderr=stderr, universal_newlines=True, bufsize=1)
    process.stdin.write(prompt)
    process.stdin.close()
    for line in process.stdout:
        raw.write(line)
        raw.flush()
        try:
            event = json.loads(line)
        except ValueError:
            continue
        if event.get("type") == "system" and event.get("subtype") == "init":
            init_model = event.get("model")
            print("Claude initialized: " + json.dumps({k: event.get(k) for k in ["model", "permissionMode", "tools"]}), flush=True)
        elif event.get("type") == "assistant":
            for block in event.get("message", {}).get("content", []):
                if block.get("type") == "tool_use":
                    detail = block.get("input", {})
                    print("Review tool: " + block.get("name", "") + " " + str(detail.get("file_path", detail.get("path", ""))), flush=True)
        elif event.get("type") == "result":
            result = event
    returncode = process.wait()
metadata.update(completed_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(), exit_code=returncode, initialized_model=init_model)
if result is not None:
    metadata.update({k: result[k] for k in ["subtype", "is_error", "num_turns", "duration_ms", "total_cost_usd", "usage", "modelUsage", "permission_denials"] if k in result})
(out / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
if returncode or result is None or result.get("is_error") or result.get("subtype") != "success":
    raise RuntimeError("Claude review did not complete successfully; inspect " + str(out))
if init_model != "claude-fable-5-1":
    raise RuntimeError("Unexpected model: " + str(init_model))
review = result.get("result", "")
if not review.strip():
    raise RuntimeError("Claude returned an empty review")
(HERE / "review.md").write_text(review.rstrip() + "\n")
(HERE / "metadata.json").write_text(json.dumps(metadata, indent=2) + "\n")
print("Review completed: " + str(HERE / "review.md"), flush=True)
print("Models used: " + json.dumps(metadata.get("modelUsage", {})), flush=True)
