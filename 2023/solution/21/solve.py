import fractions, math, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    n_steps = None
    grid = []
    with open_input() as file:
        for line in file:
            row = line.rstrip()
            if re.match(r'\d+', row):
                n_steps = int(row)
            elif row:
                grid.append(row)
    return grid, n_steps


# Solution to part one
@analyze
def solve_one(grid, n_steps):
    if n_steps is None:
        n_steps = 64
    shape = (len(grid), len(grid[0]))

    def find_start():
        for i, row in enumerate(grid):
            if (j := row.find('S')) != -1:
                return i, j

    def walk(n_steps):
        visit = {find_start()}
        visited = {
            (i, j)
            for i, row in enumerate(grid)
            for j, ch in enumerate(row)
            if ch == '#'
        }
        reachable = set()
        odd = n_steps & 1
        for t in range(n_steps + 1):
            visit_next = set()
            while visit:
                i, j = visit.pop()
                visited.add((i, j))
                if t & 1 == odd:
                    reachable.add((i, j))
                for di, dj in [(-1, 0), (+1, 0), (0, -1), (0, +1)]:
                    i_next, j_next = i + di, j + dj
                    if not (0 <= i_next < shape[0]) or not (0 <= j_next < shape[1]):
                        continue
                    if (i_next, j_next) in visited:
                        continue
                    visit_next.add((i_next, j_next))
            visit = visit_next
        return len(reachable)

    return walk(n_steps)


# Solution to part two
@analyze
def solve_two(grid, n_steps):
    if n_steps is None:
        n_steps = 26501365
    shape = (len(grid), len(grid[0]))

    def find_start():
        for i, row in enumerate(grid):
            if (j := row.find('S')) != -1:
                grid[i] = row.replace('S', '.')
                return i, j

    def walk(n_steps):
        start = find_start()
        special = shape[0] == shape[1]
        special &= shape[0] & 1
        special &= (n_steps - shape[0] // 2) % shape[0] == 0
        if special:
            reachable = walk_general(shape[0] // 2, start)
            def check_diamond():
                i, j = 0, shape[0]//2
                for di, dj in [(+1, +1), (+1, -1), (-1, -1), (-1, +1)]:
                    for dstep in range(shape[0]//2):
                        i += di
                        j += dj
                        if (i, j) not in reachable:
                            return
                return True
            if check_diamond():
                return walk_special(n_steps, start)
        return len(walk_general(n_steps, start))

    def walk_general(n_steps, start):
        visit = {start}
        visited = set()
        reachable = set()
        odd = n_steps & 1
        for t in range(n_steps + 1):
            visit_next = set()
            while visit:
                i, j = visit.pop()
                visited.add((i, j))
                if t & 1 == odd:
                    reachable.add((i, j))
                for di, dj in [(-1, 0), (+1, 0), (0, -1), (0, +1)]:
                    i_next, j_next = i + di, j + dj
                    if grid[i_next % shape[0]][j_next % shape[1]] == '#':
                        continue
                    if (i_next, j_next) in visited:
                        continue
                    visit_next.add((i_next, j_next))
            visit = visit_next
        return reachable

    def walk_special(n_steps, start):
        order = 2
        x = [i * shape[0] // 2 for i in range(1, 2 * (order + 1), 2)]
        y = [len(walk_general(xi, start)) for xi in x]

        def poly_lagrange(p):
            a = (
                fractions.Fraction(
                    math.prod(p - xj for xj in x if xj != xi),
                    math.prod(xi - xj for xj in x if xj != xi),
                )
                for xi in x
            )
            return sum(ai * yi for ai, yi in zip(a, y))

        return poly_lagrange(n_steps)

    return walk(n_steps)


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
