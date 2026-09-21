#!/usr/bin/env python3
"""Verify recorded starts and task chains against the actual benchmark files."""
import argparse
import datetime
import hashlib
import json
from pathlib import Path


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def records(path):
    return [s for s in (line.strip() for line in path.read_text().splitlines()) if s and not s.startswith("#")]


def audit(input_path, result_path):
    specification = json.loads(input_path.read_text())
    data = json.loads(result_path.read_text())
    directory = input_path.parent
    map_path = directory / specification['mapFile']
    columns = int(map_path.read_text().splitlines()[2].split()[-1])
    agent_path = directory / specification['agentFile']
    agents = [int(s) for s in records(agent_path)]
    assert agents[0] == len(agents)-1
    n = specification['teamSize']
    assert len(data['start']) == data['teamSize'] == n and agents[0] >= n
    assert [row[0]*columns+row[1] for row in data['start']] == agents[1:n+1]
    assert all(row[2] == 'E' for row in data['start'])
    task_path = directory / specification['taskFile']
    lines = records(task_path)
    count = int(lines[0])
    assert count > 0 and len(lines) == count+1
    ids = [row[0] for row in data['tasks']]
    assert sorted(ids) == list(range(len(ids))), 'missing or duplicate revealed task ids'
    for task in data['tasks']:
        original = [int(value) for value in lines[task[0] % count + 1].split(',')]
        recorded = task[2]
        assert len(recorded) == 2*len(original)
        assert [recorded[k]*columns+recorded[k+1] for k in range(0,len(recorded),2)] == original, task[0]
    return dict(input=str(input_path), result=str(result_path), result_sha256=sha(result_path),
                input_hashes={str(p):sha(p) for p in (input_path,map_path,agent_path,task_path)},
                starts_checked=n, task_chains_checked=len(ids), all_match=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', type=Path, required=True)
    parser.add_argument('--result', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    args = parser.parse_args()
    result = audit(args.input, args.result)
    result['checked_utc'] = datetime.datetime.now(datetime.timezone.utc).isoformat()
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2)+'\n')
    print('Verified',result['starts_checked'],'starts and',result['task_chains_checked'],'task chains against input files.')


if __name__ == '__main__':
    main()
