import collections, heapq, itertools

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    with open_input() as file:
        grid = {
            complex(j, i): int(ch)
            for i, line in enumerate(file)
            for j, ch in enumerate(line.rstrip())
        }
    return grid


# Solution to part one
@analyze
def solve_one(grid):
    end = complex(*max((pos.real, pos.imag) for pos in grid.keys()))
    max_consec = 3
    State = collections.namedtuple('State', ('loss', 'id', 'pos', 'vel'))
    new_state = lambda loss, pos, vel, *, id_gen=itertools.count(): State(
        loss,
        next(id_gen),
        pos,
        vel,
    )
    start, dest = 0, end
    heap = [new_state(0, start, 1), new_state(0, start, 1j)]
    visited = set()
    while True:
        state = heapq.heappop(heap)
        if state.pos == dest:
            return state.loss
        vel_perp = complex(state.vel.imag, state.vel.real)
        for vel in [vel_perp, -vel_perp]:
            pos = state.pos
            loss = state.loss
            for step in range(max_consec):
                pos += vel
                if (l := grid.get(pos)) is None:
                    break
                loss += l
                key = (pos, abs(vel.real), abs(vel.imag), step)
                if key in visited:
                    break
                visited.add(key)
                state_new = new_state(loss, pos, vel)
                heapq.heappush(heap, state_new)


# Solution to part two
@analyze
def solve_two(grid):
    end = complex(*max((pos.real, pos.imag) for pos in grid.keys()))
    min_consec, max_consec = 4, 10
    State = collections.namedtuple('State', ('loss', 'id', 'pos', 'vel'))
    new_state = lambda loss, pos, vel, *, id_gen=itertools.count(): State(
        loss,
        next(id_gen),
        pos,
        vel,
    )
    start, dest = 0, end
    heap = [new_state(0, start, 1), new_state(0, start, 1j)]
    visited = set()
    while True:
        state = heapq.heappop(heap)
        if state.pos == dest:
            return state.loss
        vel_perp = complex(state.vel.imag, state.vel.real)
        for vel in [vel_perp, -vel_perp]:
            pos = state.pos
            loss = state.loss
            for step in range(1, max_consec + 1):
                pos += vel
                if (l := grid.get(pos)) is None:
                    break
                loss += l
                if step >= min_consec:
                    key = (pos, abs(vel.real), abs(vel.imag), step)
                    if key in visited:
                        break
                    visited.add(key)
                    state_new = new_state(loss, pos, vel)
                    heapq.heappush(heap, state_new)


# Solve
print(solve_one(read()))
print(solve_two(read()))
