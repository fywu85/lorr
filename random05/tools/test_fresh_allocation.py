#!/usr/bin/env python3
"""Check that fresh-audit allocation declarations cannot silently change scale."""
import copy
import unittest
from audit_fresh import verify_allocation, compare_seed

class AllocationTests(unittest.TestCase):
    def setUp(self):
        self.case = dict(cores=16, smt=2, steps=2000, limit_ms=1000, preprocess_ms=30000,
                         env={'R05_THREADS': '32'})
        self.resources = dict(cpu_model='AMD EPYC 9354 32-Core Processor',
                              physical_cores_visible=16, effective_cpu_quota=None)
        self.protocol = dict(physical_cores=16, smt=2, workers=32)

    def test_declared_32_workers(self):
        self.assertEqual(verify_allocation(self.case, self.resources, self.protocol)['workers'], 32)

    def test_reject_wrong_scale_or_budget(self):
        for key, value in [('cores', 4), ('smt', 1), ('steps', 1000), ('limit_ms', 5000), ('preprocess_ms', 60000)]:
            changed = copy.deepcopy(self.case);changed[key] = value
            with self.assertRaises(AssertionError):verify_allocation(changed, self.resources, self.protocol)
        changed = copy.deepcopy(self.case);changed['env']['R05_THREADS'] = '16'
        with self.assertRaises(AssertionError):verify_allocation(changed, self.resources, self.protocol)
        limited = dict(self.resources, effective_cpu_quota=16)
        with self.assertRaises(AssertionError):verify_allocation(self.case, limited, self.protocol)
        with self.assertRaises(AssertionError):verify_allocation(self.case, self.resources, {})

    def test_explicit_other_full_horizons(self):
        for steps in (600, 800, 1000):
            case = dict(self.case, steps=steps)
            self.assertEqual(verify_allocation(case, self.resources, self.protocol, steps)['workers'], 32)
            with self.assertRaises(AssertionError):
                verify_allocation(dict(case, steps=steps-1), self.resources, self.protocol, steps)
            with self.assertRaises(AssertionError):
                verify_allocation(case, self.resources, self.protocol)

    def test_reference_team_worker_checks(self):
        nms = dict(self.case, team='nms', env={})
        kk = dict(self.case, team='kk', env={'LNS_NUM_THREADS': '32'})
        self.assertEqual(verify_allocation(nms, self.resources, self.protocol)['workers'], 32)
        self.assertEqual(verify_allocation(kk, self.resources, self.protocol)['workers'], 32)
        with self.assertRaises(AssertionError):
            verify_allocation(dict(kk, env={'LNS_NUM_THREADS': '16'}), self.resources, self.protocol)
        with self.assertRaises(AssertionError):
            verify_allocation(dict(self.case, team='unknown'), self.resources, self.protocol)

    def test_reference_uses_stronger_team_on_each_input(self):
        runs = {'seed1-'+role: {'tasks': tasks} for role, tasks in
                [('ours', 110), ('baseline', 100), ('nms-repeat1', 90),
                 ('nms-repeat2', 95), ('kk-repeat1', 98), ('kk-repeat2', 96)]}
        row = compare_seed(1, runs, include_baseline=True, include_kk=True)
        self.assertEqual(row['matched_max'], 98)
        self.assertEqual(row['strongest_team'], 'kk')
        self.assertAlmostEqual(row['gain_percent'], (110/98-1)*100)
        self.assertAlmostEqual(row['gain_over_baseline_percent'], 10)
        runs['seed1-nms-repeat2']['tasks'] = 105
        row = compare_seed(1, runs, include_kk=True)
        self.assertEqual(row['matched_max'], 105)
        self.assertEqual(row['strongest_team'], 'nms')
        del runs['seed1-kk-repeat2']
        with self.assertRaises(KeyError): compare_seed(1, runs, include_kk=True)
        legacy = compare_seed(1, runs)
        self.assertEqual(legacy['stronger_nms'], 105)
        self.assertNotIn('matched_max', legacy)

    def test_legacy_four_core_default(self):
        case = dict(self.case, cores=4, smt=1, env={'R05_THREADS': '4'})
        resources = dict(self.resources, physical_cores_visible=4)
        self.assertEqual(verify_allocation(case, resources, {})['workers'], 4)

if __name__ == '__main__':unittest.main()
