#!/usr/bin/env python3
"""Independent checks for strict and explicitly incomplete diagnostic handling."""
import importlib.util
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

HERE=Path(__file__).resolve().parent
spec=importlib.util.spec_from_file_location('motion_under_test',HERE/'analyze.py')
motion=importlib.util.module_from_spec(spec);spec.loader.exec_module(motion)
spec=importlib.util.spec_from_file_location('sequence_under_test',HERE.parent/'sequences-20260918/analyze.py')
sequence=importlib.util.module_from_spec(spec);spec.loader.exec_module(sequence)

class DiagnosticCompleteness(unittest.TestCase):
    def setUp(self):
        self.tmp=tempfile.TemporaryDirectory()
        self.path=Path(self.tmp.name)/'WAREHOUSE.json'
        self.item=('fixture',self.path)
        self.row={'steps':200,'empty_forward_actions':4,'empty_turn_actions':3,'empty_wait_actions':2}
    def tearDown(self):
        self.tmp.cleanup()
    def log(self,steps=(200,200,200),bad_forward=False):
        lines=[]
        for phase,step in enumerate(steps):
            fw,cr,ccr,wait=(4,1,2,2) if phase==1 else (0,0,0,0)
            closer=fw-int(bad_forward and phase==1)
            lines.append('[cgar-movement] steps=%d phase=%d fw=%d cr=%d ccr=%d wait=%d planned_wait=%d blocked_forward=0 safety_cancel=0 closer=%d farther=0 equal=0 unknown=0' % (step,phase,fw,cr,ccr,wait,wait,closer))
        self.path.with_suffix('.log').write_text('\n'.join(lines)+'\n')
    def read(self,require_complete=True):
        with patch.object(motion,'base',return_value=dict(self.row)):
            return motion.get_metrics(self.item,require_complete=require_complete)
    def test_complete_crosschecked(self):
        self.log();row=self.read()
        self.assertTrue(row['movement_diagnostics']['complete'])
        self.assertEqual(row['movement_diagnostics']['observed_steps'],[200])
        self.assertIn('movement_phases',row)
        self.assertNotIn('partial_movement_phases',row)
    def test_missing_final_sample_stays_strict(self):
        self.log((180,180,180))
        with self.assertRaises(AssertionError):self.read()
        row=self.read(False)
        self.assertFalse(row['movement_diagnostics']['complete'])
        self.assertEqual(row['movement_diagnostics']['observed_steps'],[180])
        self.assertNotIn('movement_phases',row)
        self.assertEqual(row['empty_forward_actions'],4)
        self.assertIn('partial_movement_phases',row)
    def test_mixed_samples_are_partial(self):
        self.log((200,180,200))
        with self.assertRaises(AssertionError):self.read()
        self.assertEqual(self.read(False)['movement_diagnostics']['observed_steps'],[180,200])
    def test_missing_log_counters_are_explicit(self):
        self.path.with_suffix('.log').write_text('other diagnostic\n')
        with self.assertRaises(AssertionError):self.read()
        row=self.read(False)
        self.assertFalse(row['movement_diagnostics']['complete'])
        self.assertEqual(row['partial_movement_phases'],{})
    def test_optional_does_not_hide_corrupt_counters(self):
        self.log((180,180,180),bad_forward=True)
        with self.assertRaises(AssertionError):self.read(False)
    def test_optional_complete_still_checks_actions(self):
        self.log();self.row['empty_forward_actions']=5
        with self.assertRaises(AssertionError):self.read(False)
    def test_partial_sequences_are_not_full_metrics(self):
        self.path.with_suffix('.log').write_text('[cgar-sequences] steps=180 calls=100\n')
        with patch.object(sequence,'base',return_value={'steps':200}):
            with self.assertRaises(AssertionError):sequence.get_metrics(self.item)
        with patch.object(sequence,'base',return_value={'steps':200}):
            row=sequence.get_metrics_allow_incomplete(self.item)
        self.assertFalse(row['sequence_diagnostics_complete'])
        self.assertEqual(row['partial_sequences']['steps'],180)
        self.assertNotIn('sequences',row)

if __name__=='__main__':unittest.main()
