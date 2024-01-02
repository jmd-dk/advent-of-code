import collections, itertools

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    grid = []
    with open_input() as file:
        for line in file:
            grid.append(line.rstrip())
    return grid


# Solution to part one
@analyze
def solve_one(grid):
    shape = (len(grid), len(grid[0]))
    start = (0, grid[0].index('.'))
    stop = (shape[0] - 1, grid[-1].index('.'))

    def build_graph():
        graph = {}
        for i, j in itertools.product(*map(range, shape)):
            match grid[i][j]:
                case '#':
                    continue
                case '.':
                    possibilities = {
                        (i, j)
                        for i, j, slope in [
                            (i - 1, j, '^'),
                            (i + 1, j, 'v'),
                            (i, j - 1, '<'),
                            (i, j + 1, '>'),
                        ]
                        if 0 <= i < shape[0]
                        and 0 <= j < shape[1]
                        and grid[i][j] in ['.', slope]
                    }
                case '^':
                    possibilities = {(i - 1, j)}
                case 'v':
                    possibilities = {(i + 1, j)}
                case '<':
                    possibilities = {(i, j - 1)}
                case '>':
                    possibilities = {(i, j + 1)}
            graph[i, j] = possibilities
        return graph

    def reduce_graph(graph, pos_ori, graph_reduced=None):
        if graph_reduced is None:
            graph_reduced = {}
        pos_prev = pos = pos_ori
        for n in itertools.count(1):
            possibilities = graph[pos] - {pos_prev}
            if len(possibilities) == 0:
                if pos == stop:
                    graph_reduced[pos_ori] = ([stop], n - 1)
                return graph_reduced
            if len(possibilities) == 1:
                pos_prev = pos
                pos = next(iter(possibilities))
                continue
            graph_reduced[pos_ori] = (possibilities, n)
            for pos in possibilities:
                reduce_graph(graph, pos, graph_reduced)
            return graph_reduced

    def hike(pos):
        possibilities, n = graph_reduced[pos]
        if len(possibilities) == 1:
            return n
        return n + max(hike(pos) for pos in possibilities)

    graph_reduced = reduce_graph(build_graph(), start)
    return hike(start)


# Solution to part two
@analyze
def solve_two(grid):
    shape = (len(grid), len(grid[0]))
    start = (0, grid[0].index('.'))
    stop = (shape[0] - 1, grid[-1].index('.'))

    def build_graph():
        graph = {}
        for i, j in itertools.product(*map(range, shape)):
            if grid[i][j] == '#':
                continue
            possibilities = {
                (i, j)
                for i, j in [(i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1)]
                if 0 <= i < shape[0] and 0 <= j < shape[1] and grid[i][j] != '#'
            }
            graph[i, j] = possibilities
        return graph

    def reduce_graph(graph):
        def reduce(pos_prev, pos):
            for n in itertools.count(1):
                if pos in forks:
                    return pos, n
                possibilities = graph[pos] - {pos_prev}
                pos_prev = pos
                pos = next(iter(possibilities))

        forks = [stop, start] + [
            pos for pos, possibilities in graph.items() if len(possibilities) > 2
        ]
        graph_reduced = collections.defaultdict(dict)
        for pos_ori in forks:
            for pos in graph[pos_ori]:
                pos, n = reduce(pos_ori, pos)
                if pos is None:
                    continue
                graph_reduced[pos_ori][pos] = n
                graph_reduced[pos][pos_ori] = n
        graph_reduced[start] = graph_reduced.pop(start)

        def remove_backtrack_boundary(pos_prev, pos):
            graph_reduced[pos].pop(pos_prev)
            if stop in graph_reduced[pos]:
                return
            pos_prev = pos
            for pos in graph_reduced[pos_prev]:
                if len(graph_reduced[pos]) == 3:
                    return remove_backtrack_boundary(pos_prev, pos)

        pos_prev = next(iter(graph_reduced[start]))
        graph_reduced[pos_prev].pop(start)
        for pos in set(graph_reduced[pos_prev]) - {start}:
            remove_backtrack_boundary(pos_prev, pos)
        return graph_reduced

    def build_array_reduced(graph_reduced):
        enumeration = {pos: i for i, pos in enumerate(graph_reduced)}
        array_reduced = [
            [
                (enumeration[pos1], graph_reduced[pos0][pos1])
                for pos1 in graph_reduced[pos0]
            ]
            for pos0 in graph_reduced
        ]
        return array_reduced

    def hike(i):
        result = -1
        visited[i] = True
        for j, n in array_reduced[i]:
            if visited[j]:
                continue
            elif j == 0:
                result = n
            elif (tmp := n + hike(j)) > result:
                result = tmp
        visited[i] = False
        return result

    array_reduced = build_array_reduced(reduce_graph(build_graph()))
    visited = [False for _ in array_reduced]
    i_start = len(array_reduced) - 1
    return hike(i_start)


# Solve
print(solve_one(read()))
print(solve_two(read()))
