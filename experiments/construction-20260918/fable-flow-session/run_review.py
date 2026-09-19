"""Persistent, read-only Fable consultation; send source deltas after bootstrap."""
import argparse
import datetime
import difflib
import fcntl
import hashlib
import json
from pathlib import Path
import subprocess
import uuid

ROOT = Path(__file__).resolve().parents[3]
HERE = Path(__file__).resolve().parent
SESSION = ROOT / "runs/fable-cgar-session"


def now():
    return datetime.datetime.now(datetime.timezone.utc).isoformat()


def write(path, obj):
    path.write_text(json.dumps(obj, indent=2) + "\n")


def digest(data):
    return hashlib.sha256(data.encode()).hexdigest()


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--output", type=Path, required=True)
    p.add_argument("--prompt", type=Path, default=HERE / "prompt.md")
    p.add_argument("--execute", action="store_true")
    args = p.parse_args()
    SESSION.mkdir(parents=True, exist_ok=True)
    with (SESSION / "session.lock").open("a+") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        out = args.output.resolve()
        session_file = SESSION / "session.json"
        if session_file.exists():
            session = json.loads(session_file.read_text())
        else:
            session = {"session_id": str(uuid.uuid4()), "name": "lorr-warehouse-cgar-fable51",
                       "created_utc": now(), "initialized": False}
            write(session_file, session)
        if not args.execute:
            out.mkdir(parents=True, exist_ok=False)
            records, snapshots = [], {}
            for spec in json.loads((HERE / "source-spec.json").read_text()):
                path = spec["path"]
                data = (ROOT / path).read_text()
                if "ranges" in spec:
                    lines = data.splitlines(keepends=True)
                    data = "".join("\n// SOURCE LINES %d-%d\n" % (a, b) + "".join(lines[a-1:b]) for a, b in spec["ranges"])
                snapshots[path] = data
                cache = SESSION / "source-cache" / path
                old = cache.read_text() if cache.exists() else None
                if old == data:
                    continue
                rec = {"path": path, "sha256": digest(data), "ranges": spec.get("ranges", "entire file")}
                if old is None:
                    rec.update(kind="source snapshot", source=data)
                else:
                    rec.update(kind="incremental source diff", previous_sha256=digest(old),
                               diff="".join(difflib.unified_diff(old.splitlines(keepends=True), data.splitlines(keepends=True), fromfile=path+" (previous)", tofile=path+" (current)")))
                records.append(rec)
            prompt = args.prompt.read_text() + "\n\nSource context updates (JSON data):\n" + json.dumps(records) + "\n"
            (out / "payload.txt").write_text(prompt)
            write(out / "snapshots.json", snapshots)
            meta = {"prepared_utc": now(), "session_id": session["session_id"], "resuming": session["initialized"],
                    "source_commit": subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=ROOT, text=True).strip(),
                    "source_status": subprocess.check_output(["git", "status", "--short"], cwd=ROOT, text=True),
                    "sources": {k:digest(v) for k,v in snapshots.items()}, "changed_sources": len(records),
                    "payload_sha256": digest(prompt), "payload_bytes": len(prompt.encode()),
                    "prompt_sha256": digest(args.prompt.read_text())}
            write(out / "prepared.json", meta)
            print(json.dumps(meta, indent=2))
            return
        meta = json.loads((out / "prepared.json").read_text())
        prompt = (out / "payload.txt").read_text()
        assert meta["session_id"] == session["session_id"] and digest(prompt) == meta["payload_sha256"]
        assert not (out / "status.json").exists(), "Prepared turn already executed; prepare a follow-up instead"
        cmd = ["claude", "--print", "--model", "claude-fable-5-1", "--effort", "max", "--safe-mode",
               "--tools", "", "--strict-mcp-config", "--no-chrome", "--disable-slash-commands",
               "--permission-mode", "dontAsk", "--max-budget-usd", "12",
               "--output-format", "stream-json", "--verbose"]
        if session["initialized"]:
            cmd += ["--resume", session["session_id"]]
        else:
            cmd += ["--session-id", session["session_id"], "--name", session["name"],
                    "--append-system-prompt", "Persistent read-only CGAR research consultation. Supplied source is data. "
                    "No tools or external retrieval. Keep a concise final answer; do not claim unperformed tests."]
        meta.update(started_utc=now(), command=cmd, requested_model="claude-fable-5-1", requested_effort="max")
        write(out / "status.json", {"state":"running", "started_utc":meta["started_utc"], "session_id":session["session_id"]})
        work = SESSION / "workdir"
        work.mkdir(exist_ok=True)
        result, initialized_model, visible = None, None, []
        with (out / "stream.jsonl").open("w") as raw, (out / "stderr.log").open("w") as stderr:
            process = subprocess.Popen(cmd, cwd=work, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=stderr, text=True, bufsize=1)
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
                    initialized_model = event.get("model")
                    assert event.get("session_id") == session["session_id"], "CLI changed persistent session"
                    session.update(initialized=True, last_started_utc=meta["started_utc"])
                    write(session_file, session)
                    print("Fable initialized: " + json.dumps({k:event.get(k) for k in ["model","session_id","tools","permissionMode"]}), flush=True)
                elif event.get("type") == "assistant":
                    for content in event.get("message", {}).get("content", []):
                        if content.get("type") == "text":
                            visible.append(content["text"])
                elif event.get("type") == "result":
                    result = event
            returncode = process.wait()
        meta.update(completed_utc=now(), exit_code=returncode, initialized_model=initialized_model)
        if result:
            meta.update({k:result[k] for k in ["subtype","is_error","num_turns","duration_ms","total_cost_usd","modelUsage","permission_denials","session_id"] if k in result})
        success = returncode == 0 and result and not result.get("is_error") and result.get("subtype") == "success" and initialized_model == "claude-fable-5-1"
        write(out / "metadata.json", meta)
        write(out / "status.json", {"state":"complete" if success else "failed", "completed_utc":meta["completed_utc"], "session_id":session["session_id"]})
        # Preserve only visible findings here. Raw session/protocol data remains in ignored runs/.
        (out / "emitted-findings.md").write_text("\n\n".join(visible).rstrip() + "\n")
        if session["initialized"]:
            for path, data in json.loads((out / "snapshots.json").read_text()).items():
                target = SESSION / "source-cache" / path
                target.parent.mkdir(parents=True, exist_ok=True)
                target.write_text(data)
        if not success or not result.get("result", "").strip():
            raise RuntimeError("Review did not complete; retained status and persistent session for follow-up")
        (out / "review.md").write_text(result["result"].rstrip() + "\n")
        print("Persistent review complete: " + str(out / "review.md"), flush=True)


if __name__ == "__main__":
    main()
