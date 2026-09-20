#!/usr/bin/env python3
"""Read-only coverage check for topology-derived straight degree-two pooling."""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path


def coverage(free, rows, cols):
    assert len(free) == rows * cols
    def adjacent(u):
        r, c = divmod(u, cols)
        return [v for valid, v in [(c + 1 < cols, u + 1), (r + 1 < rows, u + cols),
                                  (c > 0, u - 1), (r > 0, u - cols)] if valid and free[v]]
    neighbors = {u: adjacent(u) for u, yes in enumerate(free) if yes}
    straight = {u for u, ns in neighbors.items() if len(ns) == 2 and
                ((ns[0] // cols == ns[1] // cols == u // cols) or
                 (ns[0] % cols == ns[1] % cols == u % cols))}
    unseen, lengths = set(straight), []
    while unseen:
        stack = [unseen.pop()]
        length = 0
        while stack:
            u = stack.pop(); length += 1
            for v in neighbors[u]:
                if v in unseen:
                    unseen.remove(v); stack.append(v)
        lengths.append(length)
    undirected_edges = sum(map(len, neighbors.values())) // 2
    incident_edges = sum(1 for u, ns in neighbors.items() for v in ns
                         if u < v and (u in straight or v in straight))
    return {'free_cells': len(neighbors), 'undirected_edges': undirected_edges,
            'straight_degree2_cells': len(straight),
            'straight_cell_fraction': len(straight) / len(neighbors) if neighbors else 0,
            'chains': len(lengths), 'chain_length_histogram': dict(sorted(Counter(lengths).items())),
            'incident_edges': incident_edges,
            'incident_edge_fraction': incident_edges / undirected_edges if undirected_edges else 0}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--map', type=Path, required=True)
    p.add_argument('--output', type=Path, required=True)
    a = p.parse_args()
    for free, rows, cols, cells, chains, edges in [
            ([True] * 5, 1, 5, 3, 1, 4),
            ([True] * 4, 2, 2, 0, 0, 0),
            ([False, True, False, True, True, True, False, True, False], 3, 3, 0, 0, 0),
            ([False] * 4, 2, 2, 0, 0, 0)]:
        result = coverage(free, rows, cols)
        assert (result['straight_degree2_cells'], result['chains'], result['incident_edges']) == (cells, chains, edges)
    text = a.map.read_text().splitlines()
    rows, cols = int(text[1].split()[1]), int(text[2].split()[1])
    assert text[3] == 'map' and len(text[4:]) == rows and all(len(row) == cols for row in text[4:])
    result = coverage([x not in '@T' for row in text[4:] for x in row], rows, cols)
    result.update(map_sha256=hashlib.sha256(a.map.read_bytes()).hexdigest(), fixtures_passed=4,
                  scope='Static topology coverage only; no traffic, field coherence or policy benefit measured.')
    a.output.parent.mkdir(parents=True, exist_ok=True)
    a.output.write_text(json.dumps(result, indent=2) + '\n')
    print(json.dumps(result))


if __name__ == '__main__':
    main()
