#!/usr/bin/env python3
"""Exercise terminal allocation rejection without submitting or running a job."""
import importlib.util
import json
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[2]
spec = importlib.util.spec_from_file_location('r05_grid_test', str(Path(__file__).with_name('grid.py')))
grid = importlib.util.module_from_spec(spec)
spec.loader.exec_module(grid)


class PreflightRejection(unittest.TestCase):
    def test_rejected_allocations_are_terminal_without_solver_launch(self):
        original_root = grid.ROOT
        try:
            for kind, changes in [('cpu_model', {'cpu_model': 'wrong processor'}),
                                  ('core_binding', {'physical_cores_visible': 64}),
                                  ('cpu_quota', {'effective_cpu_quota': 2})]:
                with self.subTest(kind=kind), tempfile.TemporaryDirectory(prefix='r05-preflight-') as tmp:
                    out = Path(tmp)
                    (out / 'must-not-launch').mkdir()
                    frozen = dict(kind='benchmark', repo=str(ROOT), physical=4, slots=8,
                                  required_cpu_model='expected processor', cases=[dict(
                                      name='must-not-launch', steps=2000, binary_sha256='unused')])
                    (out / 'spec.json').write_text(json.dumps(frozen))
                    allocation = dict(cpu_model='expected processor', physical_cores_visible=4,
                                      effective_cpu_quota=None, logical_cpus_by_physical_core=[[0, 4], [1, 5], [2, 6], [3, 7]])
                    allocation.update(changes)
                    args = type('Arguments', (), {'output': out})()
                    # The mocked allocation is the external condition. Launching
                    # any child process would fail this test immediately.
                    with patch.object(grid, 'cpu_resources', return_value=allocation), \
                         patch.object(grid.subprocess, 'run', side_effect=AssertionError('solver was launched')):
                        self.assertEqual(grid.execute(args), 1)
                    completion = json.loads((out / 'completion.json').read_text())
                    rows = json.loads((out / 'summary.json').read_text())
                    self.assertFalse(completion['valid'])
                    self.assertFalse(completion['solver_started'])
                    self.assertEqual(completion['failure']['kind'], kind)
                    self.assertEqual(len(rows), 1)
                    self.assertFalse(rows[0]['valid'])
                    self.assertNotIn('result', rows[0])
                    self.assertEqual(rows[0], json.loads((out / 'must-not-launch' / 'summary.json').read_text()))
                    # The existing batch collector must report the failed case
                    # as terminal, while retaining the invalid overall status.
                    batch = out / 'batch'
                    batch.mkdir()
                    (batch / 'batch.json').write_text(json.dumps(dict(jobs=[dict(output=str(out))])))
                    proc = subprocess.run([sys.executable, str(Path(__file__).with_name('split_grid.py')),
                                           'collect', '--output', str(batch)],
                                          stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
                    self.assertEqual(proc.returncode, 0, proc.stderr)
                    self.assertIn('1/1 cases complete', proc.stdout)
                    self.assertFalse(json.loads((batch / 'completion.json').read_text())['valid'])
        finally:
            grid.ROOT = original_root


if __name__ == '__main__':
    unittest.main()
