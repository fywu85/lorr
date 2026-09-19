#!/usr/bin/env python3
"""Extend movement analysis with the fixed action-sequence work counters."""
import argparse
import importlib.util
from pathlib import Path
import sys

HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('motion_analysis', HERE.parent / 'motion-20260918/analyze.py')
motion = importlib.util.module_from_spec(spec)
spec.loader.exec_module(motion)
base = motion.get_metrics


def get_metrics(item, require_complete=True):
    row = base(item, require_complete=require_complete)
    for line in item[1].with_suffix('.log').read_text().splitlines():
        if line.startswith('[cgar-sequences] '):
            row['sequences'] = {k: int(v) for k, v in (field.split('=', 1) for field in line.split()[1:])}
    if 'sequences' in row:
        complete = row['sequences']['steps'] == row['steps']
        assert complete or not require_complete, 'incomplete final sequence diagnostics'
        row['sequence_diagnostics_complete'] = complete
        if not complete:
            row['partial_sequences'] = row.pop('sequences')
    return row


def get_metrics_allow_incomplete(item):
    return get_metrics(item, require_complete=False)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument('--allow-incomplete-movement', action='store_true')
    options, remaining = parser.parse_known_args()
    sys.argv = [sys.argv[0]] + remaining
    motion.assignment.analyze_matrix.get_metrics = get_metrics_allow_incomplete if options.allow_incomplete_movement else get_metrics
    motion.assignment.analyze_matrix.main()
