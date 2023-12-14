# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    with open_input_file() as file:
        grid = [list(line.rstrip()) for line in file]
    while not grid[-1]:
        grid.pop()
    return grid


# Solution to part one
@analyze
def solve_one(grid):
    def tilt():
        for i, row in enumerate(grid):
            for j, stone in enumerate(row):
                if stone != 'O':
                    continue
                k = i
                for k in range(i - 1, -1, -1):
                    if grid[k][j] != '.':
                        k += 1
                        break
                grid[i][j] = '.'
                grid[k][j] = 'O'

    tilt()
    return sum(
        len(grid) - i for i, row in enumerate(grid) for stone in row if stone == 'O'
    )


# Solution to part two
@analyze
def solve_two(grid):
    def tilt():
        for i, row in enumerate(grid):
            for j, stone in enumerate(row):
                if stone != 'O':
                    continue
                k = i
                for k in range(i - 1, -1, -1):
                    if grid[k][j] != '.':
                        k += 1
                        break
                grid[i][j] = '.'
                grid[k][j] = 'O'

    def rotate():
        grid[:] = (list(row[::-1]) for row in zip(*grid))

    def cycle():
        for _ in range(4):
            tilt()
            rotate()

    def record():
        stones = []
        for i, row in enumerate(grid):
            for j, stone in enumerate(row):
                if stone == 'O':
                    stones.append((i, j))
        return tuple(stones)

    def run(n=1_000_000_000):
        records = {0: record()}
        for i in range(1, n + 1):
            cycle()
            if (i_prev := records.get(stones := record())) is None:
                records[stones] = i
                continue
            for _ in range((n - i) % (i - i_prev)):
                cycle()
            return

    run()
    return sum(
        len(grid) - i for i, row in enumerate(grid) for stone in row if stone == 'O'
    )


# Solve
print(solve_one(read()))
print(solve_two(read()))
