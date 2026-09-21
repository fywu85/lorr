#!/usr/bin/env python3
"""Check that fresh-audit allocation declarations cannot silently change scale."""
import copy
import unittest
from audit_fresh import verify_allocation

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

    def test_legacy_four_core_default(self):
        case = dict(self.case, cores=4, smt=1, env={'R05_THREADS': '4'})
        resources = dict(self.resources, physical_cores_visible=4)
        self.assertEqual(verify_allocation(case, resources, {})['workers'], 4)

if __name__ == '__main__':unittest.main()
