#!/bin/bash
set -eu
exec /usr/bin/python3 /shared/share_mala/fw2449/lorr2024/experiments/sequences-20260918/analyze.py --input /shared/share_mala/fw2449/lorr2024/runs/cgar-guide-window-full-v26-20260919 --output /shared/share_mala/fw2449/lorr2024/experiments/construction-20260918/results/guide-window-full-v26 --control equal_work4m_compact --workers 1
