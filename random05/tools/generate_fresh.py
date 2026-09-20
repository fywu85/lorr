#!/usr/bin/env python3
"""Generate declared RANDOM-05 validation inputs using the archived map helper."""
import argparse
import datetime
import hashlib
import importlib.util
import json
from pathlib import Path
import shutil
import numpy as np

ROOT = Path(__file__).resolve().parents[2]
GENERATOR = ROOT / 'Code-Archive/2024 Competition/Team_Kitty_Knight/c8bfed41a2d7aa8435d3ee79a8b0eb2c4f6942cc/scripts/map.py'
MAP = ROOT / 'mr24/random.domain/maps/random-32-32-20.map'


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--seeds', type=int, nargs='+', default=[50001, 50002])
    args = parser.parse_args()
    args.output.mkdir(parents=True, exist_ok=False)
    spec = importlib.util.spec_from_file_location('archived_lorr_map', str(GENERATOR))
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    records = []
    for seed in args.seeds:
        directory = args.output / ('seed' + str(seed))
        directory.mkdir()
        shutil.copy2(str(MAP), str(directory / MAP.name))
        graph = module.Map(str(MAP))
        assert graph.num_empty_locs == 819
        graph.generate_agent_files(800, str(directory / 'agents.agents'), seed + 10086)
        graph.generate_task_files(100000, str(directory / 'tasks.tasks'), [2, 5], seed + 25784)
        case = dict(mapFile=MAP.name, agentFile='agents.agents', taskFile='tasks.tasks',
                    teamSize=800, numTasksReveal=1.5, version='2024 LoRR')
        (directory / 'RANDOM-05.json').write_text(json.dumps(case, indent=2) + '\n')
        agents = list(map(int, (directory / 'agents.agents').read_text().splitlines()))
        tasks = (directory / 'tasks.tasks').read_text().splitlines()
        free = set(np.flatnonzero(graph.graph.reshape(-1) == 0).tolist())
        assert agents[0] == 800 and len(agents) == 801 and len(set(agents[1:])) == 800
        assert set(agents[1:]) <= free
        assert int(tasks[0]) == 100000 and len(tasks) == 100001
        for line in tasks[1:]:
            stops = list(map(int, line.split(',')))
            assert 2 <= len(stops) <= 5 and set(stops) <= free
        records.append(dict(seed=seed, agent_seed=seed+10086, task_seed=seed+25784,
                            input=str((directory/'RANDOM-05.json').resolve()),
                            hashes={p.name:digest(p) for p in sorted(directory.iterdir())}))
    metadata = dict(created_utc=datetime.datetime.now(datetime.timezone.utc).isoformat(),
                    generator=str(GENERATOR.relative_to(ROOT)), generator_sha256=digest(GENERATOR),
                    numpy_version=np.__version__, records=records,
                    caveat='New validation inputs; not the colleague\'s private instances or claimed seeds.')
    (args.output/'generation.json').write_text(json.dumps(metadata, indent=2)+'\n')
    print('Generated and validated {} inputs.'.format(len(records)))


if __name__ == '__main__':
    main()
