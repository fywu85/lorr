#!/usr/bin/env python3
"""Run one command and record its own Linux wait4 resource usage."""
import argparse
import json
import os
from pathlib import Path
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("command", nargs=argparse.REMAINDER)
    args = parser.parse_args()
    command = args.command[1:] if args.command[:1] == ["--"] else args.command
    if not command:
        parser.error("a command is required")
    started = time.monotonic()
    pid = os.fork()
    if pid == 0:
        try:
            os.execvpe(command[0], command, os.environ)
        except OSError as error:
            os.write(2, (str(error) + "\n").encode())
            os._exit(127)
    _, status, usage = os.wait4(pid, 0)
    code = os.WEXITSTATUS(status) if os.WIFEXITED(status) else 128 + os.WTERMSIG(status)
    args.output.write_text(json.dumps({"user_seconds": usage.ru_utime,
        "system_seconds": usage.ru_stime, "peak_rss_kib": usage.ru_maxrss,
        "wall_seconds": time.monotonic() - started, "exit": code}, indent=2) + "\n")
    return code


if __name__ == "__main__":
    raise SystemExit(main())
