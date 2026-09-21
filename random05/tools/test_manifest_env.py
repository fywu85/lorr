#!/usr/bin/env python3
import tempfile
import unittest
from pathlib import Path
from grid import validate_r05_case


class ManifestOptions(unittest.TestCase):
    def test_frozen_options_and_runtime_environment(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'source/src'
            source.mkdir(parents=True)
            (source / 'engine.cpp').write_text('c.predict=integer("R05_SCHED_PREDICT",0);')
            case = dict(name='test', binary=str(root / 'build/lifelong'),
                        env=dict(R05_SCHED_PREDICT='1', OMP_PROC_BIND='true'))
            validate_r05_case(case)
            case['env']['R05_PREDICT_MATCHING'] = '1'
            with self.assertRaises(ValueError):
                validate_r05_case(case)
            # Even a disabled misspelling is rejected rather than silently ignored.
            case['env']['R05_PREDICT_MATCHING'] = '0'
            with self.assertRaises(ValueError):
                validate_r05_case(case)

    def test_rejects_the_failed_8192_eight_branch_budget_before_submission(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'source/src'
            source.mkdir(parents=True)
            keys = ['R05_K','R05_FIRST_K','R05_CONTINUATIONS','R05_SCREEN_BRANCHES','R05_SCREEN_KEEP','R05_GENERATIONS']
            text = ' '.join('"'+k+'"' for k in keys)
            text += " each generation's K must divide into complete screening groups"
            (source / 'engine.cpp').write_text(text)
            case = dict(name='eight-branch', binary=str(root / 'build/lifelong'),
                        env=dict(zip(keys, ['8192','4096','8','2','4','4'])))
            with self.assertRaisesRegex(ValueError, 'multiple of 56'):
                validate_r05_case(case)
            case['env'].update(R05_K='8064', R05_FIRST_K='4032')
            validate_r05_case(case)
            case['env']['R05_FIRST_K'] = '4096'
            with self.assertRaises(ValueError):
                validate_r05_case(case)

    def test_missing_frozen_source_rejected(self):
        with self.assertRaises(ValueError):
            validate_r05_case(dict(name='missing', binary='/missing/build/lifelong', env=dict(R05_K='16')))

    def test_nms_is_not_validated_against_pilot_options(self):
        validate_r05_case(dict(name='nms', team='nms', binary='/missing/nms', env=dict(OMP_NUM_THREADS='32')))


if __name__ == '__main__':
    unittest.main()
