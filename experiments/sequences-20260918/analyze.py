#!/usr/bin/env python3
"""Extend movement analysis with the fixed action-sequence work counters."""
import importlib.util
from pathlib import Path

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('motion_analysis', HERE.parent / 'motion-20260918/analyze.py')
motion = importlib.util.module_from_spec(spec)
spec.loader.exec_module(motion)
base = motion.get_metrics


def get_metrics(item):
    row = base(item)
    for line in item[1].with_suffix('.log').read_text().splitlines():
        if line.startswith('[cgar-sequences] '):
            row['sequences'] = {k: int(v) for k, v in (field.split('=', 1) for field in line.split()[1:])}
    if 'sequences' in row:
        assert row['sequences']['steps'] == row['steps']
    return row


if __name__ == '__main__':
    motion.assignment.analyze_matrix.get_metrics = get_metrics
    motion.assignment.analyze_matrix.main()
