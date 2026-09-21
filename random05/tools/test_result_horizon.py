#!/usr/bin/env python3
import copy
import unittest
from result_horizon import executed_steps, summary_steps


class HorizonTests(unittest.TestCase):
    def setUp(self):
        self.data = dict(teamSize=2, makespan=2, actualPaths=['F,W,W,W', 'W,W,W,W'],
                         entryComputeTimes=[.1] * 4, entryComputeSamples=4)

    def test_unassigned_steps_are_still_executed(self):
        self.assertEqual(executed_steps(self.data), 4)
        self.assertEqual(self.data['makespan'], 2)

    def test_different_robot_horizons_rejected(self):
        self.data['actualPaths'][1] = 'W,W,W'
        with self.assertRaises(AssertionError):
            executed_steps(self.data)

    def test_missing_entry_work_rejected(self):
        self.data['entryComputeTimes'].pop()
        with self.assertRaises(AssertionError):
            executed_steps(self.data)

    def test_missing_robot_rejected(self):
        self.data['actualPaths'].pop()
        with self.assertRaises(AssertionError):
            executed_steps(self.data)

    def test_legacy_reference_without_entry_instrumentation(self):
        del self.data['entryComputeTimes']
        del self.data['entryComputeSamples']
        self.assertEqual(executed_steps(self.data), 4)

    def test_summary_requires_equal_paths_and_sample_count(self):
        summary = dict(actual_path_steps=[4, 4], result=dict(makespan=2, entryComputeSamples=4))
        self.assertEqual(summary_steps(summary), 4)
        for changed in (dict(actual_path_steps=[3, 4]), dict(result=dict(makespan=2, entryComputeSamples=3))):
            broken = copy.deepcopy(summary)
            broken.update(changed)
            with self.assertRaises(AssertionError):
                summary_steps(broken)


if __name__ == '__main__':
    unittest.main()
