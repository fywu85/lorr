"""Read executed horizon independently of LoRR's assigned-work makespan metric."""


def executed_steps(data):
    paths = data['actualPaths']
    assert len(paths) == data['teamSize'] and paths, 'missing robot trajectories'
    assert all(isinstance(path, str) for path in paths), 'invalid trajectory encoding'
    lengths = {path.count(',') + 1 if path else 0 for path in paths}
    assert len(lengths) == 1, 'robots have different executed horizons'
    steps = lengths.pop()
    assert 0 <= data['makespan'] <= steps, 'assigned-work cost exceeds executed horizon'
    if 'entryComputeTimes' in data:
        assert len(data['entryComputeTimes']) == steps, 'entry sample count differs from executed horizon'
        if 'entryComputeSamples' in data:
            assert data['entryComputeSamples'] == steps, 'declared entry sample count mismatch'
    return steps


def summary_steps(summary):
    lengths = summary['actual_path_steps']
    assert len(lengths) == 2 and lengths[0] == lengths[1], 'unequal or missing full paths'
    steps = lengths[0]
    assert isinstance(steps, int) and steps >= 0
    result = summary['result']
    assert 0 <= result['makespan'] <= steps
    if result.get('executedSteps') is not None:
        assert result['executedSteps'] == steps
    if result.get('entryComputeSamples') is not None:
        assert result['entryComputeSamples'] == steps
    return steps
