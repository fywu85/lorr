#!/usr/bin/env python3
import unittest
from audit_branch_ranking import screening


class RankingDiagnostics(unittest.TestCase):
    def test_dropped_tied_winner_can_still_have_zero_value_loss(self):
        scores = [[10, 10, 0, 0], [9, 9, 9, 9], [8, 8, 10, 10], [0, 0, 0, 0]]
        rows = [dict(candidate=i, scores=s, value=sum(s) / 4) for i, s in enumerate(scores)]
        one = screening(rows, 2, 1)
        self.assertFalse(one['winner_survives'])
        self.assertEqual(one['full_value_loss'], 4)
        self.assertEqual(one['winner_prefix_rank'], 3)
        two = screening(rows, 2, 2)
        self.assertFalse(two['winner_survives'])
        self.assertEqual(two['full_value_loss'], 0)
        self.assertEqual(two['top8_recall'], .5)

    def test_anchor_changes_screening_and_respects_retention_budget(self):
        rows = [dict(candidate=i, scores=[i] * 4, value=i) for i in range(4)]
        only_anchor = screening(rows, 1, 1)
        self.assertEqual(only_anchor['full_value_loss'], 3)
        anchor_and_best = screening(rows, 1, 2)
        self.assertTrue(anchor_and_best['winner_survives'])
        self.assertEqual(anchor_and_best['top8_recall'], .5)

    def test_equal_score_acceptance_selects_the_correct_tie(self):
        rows = [dict(candidate=i, scores=[1] * 4, value=1) for i in range(4)]
        self.assertFalse(screening(rows, 1, 1, True)['winner_survives'])
        self.assertTrue(screening(rows, 1, 1, False)['winner_survives'])


if __name__ == '__main__':
    unittest.main()
