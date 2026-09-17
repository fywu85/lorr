#!/usr/bin/env bash
# Run one 2024 LoRR entry on one instance.
#   usage: ./run.sh <nms|kk> <instance.json relative to the team dir> <steps> [extra lifelong args...]
# Output JSON and log land in runs/. The planner is run from its own root because
# both teams load their config and weight files by relative path.
set -eo pipefail
here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
team="$1"; inst="$2"; steps="$3"; shift 3
source "$here/activate.sh"  # conda's activation hooks are not `set -u` clean
set -u

# NMS hard-codes 32 threads (Solution/settings.hpp); KK follows OpenMP.
export OMP_NUM_THREADS="${OMP_NUM_THREADS_RUN:-16}"

name="${team}_$(basename "$inst" .json)_s${steps}"
mkdir -p "$here/runs"
cd "$here/$team"
start=$(date +%s)
./build/lifelong -i "$inst" -o "$here/runs/$name.json" -s "$steps" "$@" \
    > "$here/runs/$name.log" 2>&1 && status=0 || status=$?
echo "wall_seconds=$(( $(date +%s) - start )) exit=$status" >> "$here/runs/$name.log"
python3 - "$here/runs/$name.json" "$name" "$status" <<'EOF'
import json, sys
path, name, status = sys.argv[1], sys.argv[2], sys.argv[3]
try:
    d = json.load(open(path))
except Exception as e:
    print(f"{name}: exit={status}, no readable output ({e})"); sys.exit()
keys = ["teamSize", "numTaskFinished", "AllValid", "numPlannerErrors",
        "numScheduleErrors", "numEntryTimeouts", "makespan"]
print(name, f"exit={status}", " ".join(f"{k}={d.get(k)}" for k in keys))
EOF
