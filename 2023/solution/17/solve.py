import collections, heapq

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    with open_input() as file:
        grid = [list(map(int, line.rstrip())) for line in file]
    return grid


# Solution to part one
@analyze
def solve_one(grid):
    shape = (len(grid), len(grid[0]))
    State = collections.namedtuple('State', ('loss', 'x', 'y', 'vx', 'vy'))
    x_start, y_start = 0, 0
    x_end, y_end = shape[0] - 1, shape[1] - 1
    max_consec = 3
    heap = [State(0, x_start, y_start, 1, 0), State(0, x_start, y_start, 0, 1)]
    heapq.heapify(heap)
    visited = set()
    while True:
        state = heapq.heappop(heap)
        if (state.x, state.y) == (x_end, y_end):
            return state.loss
        vx = state.vy
        vy = state.vx
        for direc in range(2):
            vx *= -1
            vy *= -1
            x = state.x
            y = state.y
            loss = state.loss
            for step in range(1, max_consec + 1):
                x += vx
                y += vy
                if not (0 <= x < shape[0]) or not (0 <= y < shape[1]):
                    break
                loss += grid[x][y]
                key = (x, y, vx**2, vy**2, step)
                if key in visited:
                    break
                visited.add(key)
                heapq.heappush(heap, State(loss, x, y, vx, vy))


# Solution to part two
@analyze
def solve_two(grid):
    shape = (len(grid), len(grid[0]))
    State = collections.namedtuple('State', ('loss', 'x', 'y', 'vx', 'vy'))
    x_start, y_start = 0, 0
    x_end, y_end = shape[0] - 1, shape[1] - 1
    min_consec, max_consec = 4, 10
    heap = [State(0, x_start, y_start, 1, 0), State(0, x_start, y_start, 0, 1)]
    heapq.heapify(heap)
    visited = set()
    while True:
        state = heapq.heappop(heap)
        if (state.x, state.y) == (x_end, y_end):
            return state.loss
        vx = state.vy
        vy = state.vx
        for direc in range(2):
            vx *= -1
            vy *= -1
            x = state.x
            y = state.y
            loss = state.loss
            for step in range(1, max_consec + 1):
                x += vx
                y += vy
                if not (0 <= x < shape[0]) or not (0 <= y < shape[1]):
                    break
                loss += grid[x][y]
                if step >= min_consec:
                    key = (x, y, vx**2, vy**2, step)
                    if key in visited:
                        break
                    visited.add(key)
                    heapq.heappush(heap, State(loss, x, y, vx, vy))


# Solve
print(solve_one(read()))
print(solve_two(read()))
