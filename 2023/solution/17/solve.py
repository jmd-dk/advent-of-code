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
    size = int(len(grid) ** 0.5)
    State = collections.namedtuple('State', ('loss', 'id', 'pos', 'vel'))
    new_state = lambda loss, pos, vel, *, id_gen=itertools.count(): State(
        loss,
        next(id_gen),
        pos,
        vel,
    )
    get_direction = lambda vel: vel / abs(vel) if vel != 0 else 0
    get_key = lambda pos, vel: (pos, abs(vel.real), abs(vel.imag))
    start, dest = 0, (size - 1) * (1 + 1j)
    queue = [new_state(0, start, 0)]
    visited = set()
    while True:
        state = heapq.heappop(queue)
        key = get_key(state.pos, state.vel)
        if key in visited:
            continue
        visited.add(key)
        if state.pos == dest:
            break
        for pos in [state.pos - 1, state.pos + 1, state.pos - 1j, state.pos + 1j]:
            if (loss := grid.get(pos)) is None:
                continue
            direc_state = get_direction(state.vel)
            vel = pos - state.pos
            direc = get_direction(vel)
            if direc == -direc_state:
                continue
            elif direc == direc_state:
                if abs(state.vel) == 3:
                    continue
                vel += state.vel
            if get_key(pos, vel) in visited:
                continue
            state_new = new_state(state.loss + loss, pos, vel)
            heapq.heappush(queue, state_new)
    return state.loss


# Solution to part two
@analyze
def solve_two(grid):
    size = int(len(grid) ** 0.5)
    State = collections.namedtuple('State', ('loss', 'id', 'pos', 'vel'))
    new_state = lambda loss, pos, vel, *, id_gen=itertools.count(): State(
        loss,
        next(id_gen),
        pos,
        vel,
    )
    get_direction = lambda vel: vel / abs(vel) if vel != 0 else 0
    get_key = lambda pos, vel: (pos, abs(vel.real), abs(vel.imag))
    start, dest = 0, (size - 1) * (1 + 1j)
    queue = [new_state(0, start, 0)]
    visited = set()
    while True:
        state = heapq.heappop(queue)
        key = get_key(state.pos, state.vel)
        if key in visited:
            continue
        visited.add(key)
        if state.pos == dest:
            break
        for pos in [state.pos - 1, state.pos + 1, state.pos - 1j, state.pos + 1j]:
            if (loss := grid.get(pos)) is None:
                continue
            direc_state = get_direction(state.vel)
            vel = pos - state.pos
            direc = get_direction(vel)
            if direc == -direc_state:
                continue
            elif direc == direc_state:
                if abs(state.vel) == 10:
                    continue
                vel += state.vel
            elif 0 < abs(state.vel) < 4:
                continue
            if get_key(pos, vel) in visited:
                continue
            state_new = new_state(state.loss + loss, pos, vel)
            heapq.heappush(queue, state_new)
    return state.loss


# Solve
print(solve_one(read()))
print(solve_two(read()))
