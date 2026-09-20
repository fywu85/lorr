"""Read-only audit evidence checks shared by replay and full-run analyses."""
import hashlib
import json
import re


def fields(line):
    return dict(word.split('=', 1) for word in line.split()[1:] if '=' in word)


def canonical(data):
    digest = hashlib.sha256()
    encoder = json.JSONEncoder(separators=(',', ':'))
    for key in ['actualPaths', 'actualSchedule', 'events', 'tasks']:
        digest.update(key.encode())
        for chunk in encoder.iterencode(data[key]):
            digest.update(chunk.encode())
    return digest.hexdigest()


def prefix(data, steps):
    digest = hashlib.sha256()
    digest.update(json.dumps(data['start'], separators=(',', ':')).encode())
    for path in data['actualPaths']:
        digest.update(path[:max(0, 2 * steps - 1)].encode()); digest.update(b'\n')
    for line in data['actualSchedule']:
        digest.update(','.join(e for e in line.split(',') if e and int(e.split(':')[0]) <= steps).encode())
        digest.update(b'\n')
    digest.update(json.dumps([e for e in data['events'] if e[0] <= steps], separators=(',', ':')).encode())
    return digest.hexdigest()


def audit_samples(lines, enabled, horizon, unit):
    receipt = [fields(x) for x in lines if x.startswith('[cgar-fresh-pickup-config] ')]
    expected = dict(read_only='1', after_real_match='1', fresh_only='1', group_size='32', groups='2',
                    cap='64', metric='complete_forward', horizon='nonworsening_per_task', task_disjoint='1')
    assert receipt == ([expected] if enabled else []), receipt
    raw = [fields(x) for x in lines if x.startswith('[cgar-fresh-pickup] ')]
    assert [int(x['step']) for x in raw] == (list(range(200, horizon + 1, 200)) if enabled else [])
    samples = [{k:int(v) for k,v in x.items()} for x in raw]
    for sample in samples:
        assert sample['read_only'] == 1 and sample['unit'] == unit
        assert all(v >= 0 for v in sample.values())
        assert sample['passes'] > 0 and sample['eligible'] > 0 and sample['groups'] > 0
        assert sample['groups'] <= 2 * sample['passes']
        assert sample['matrix_entries'] <= 1024 * sample['groups']
        assert sample['accepted_saving'] <= sample['positive_saving']
        assert sample['guarded_saving'] <= sample['positive_saving']
        assert sample['witness_saving'] <= sample['guarded_saving']
        assert sample['unique_tasks'] == sample['witness_rows']
        assert sample['ordinary_witness_saving'] + sample['match_tick_witness_saving'] == sample['witness_saving']
        assert sample['late_witness_saving'] <= sample['witness_saving']
        assert sample['witness_cycles'] + sample['duplicate_cycles'] == sample['guarded_cycles']
        assert sample['accepted_cycles'] <= sample['positive_cycles']
    for before, after in zip(samples, samples[1:]):
        assert before.keys() == after.keys()
        assert all(after[k] >= before[k] for k in before)
    return samples


def real_diagnostics(lines):
    """Retain real decisions/work counters; discard only explicitly timed fields."""
    result = []
    for line in lines:
        if not line.startswith(('[cgar', '[CGAR_TRICK')) or line.startswith('[cgar-fresh-pickup'):
            continue
        if line.startswith('[cgar] certificate '):
            line = re.sub(r' secs=[0-9.]+', '', line)
        if line.startswith('[cgar-temporal-timing] '):
            line = '[cgar-temporal-timing] ' + ' '.join(k+'='+v for k,v in fields(line).items()
                   if k not in ('candidates', 'global', 'regions', 'validation'))
        result.append(line)
    return result
