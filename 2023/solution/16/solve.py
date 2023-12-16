import collections

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    with open_input() as file:
        grid = [line.rstrip() for line in file]
    return grid


# Solution to part one
@analyze
def solve_one(grid):
    shape = (len(grid), len(grid[0]))

    def construct_space():
        space = {
            direction: [direction] for direction in [(-1, 0), (+1, 0), (0, -1), (0, +1)]
        }
        return space

    def construct_mirror(mirror):
        for key, val in mirror.copy().items():
            mirror[negate(key)] = negate(val)
        for key, val in mirror.copy().items():
            mirror[val] = key
        mirror = {key: [val] for key, val in mirror.items()}
        return mirror

    def construct_splitter(direction):
        direction_perp = (direction[1], direction[0])
        splitter = {
            direction: [direction],
            direction_perp: [direction, negate(direction)],
        }
        for direction_in, directions_out in splitter.copy().items():
            splitter[negate(direction_in)] = [
                negate(direction_out) for direction_out in directions_out
            ]
        return splitter

    def step(beams):
        beams_next = set()
        for position, direction in beams:
            position = add(position, direction)
            if not all(0 <= position[dim] < shape[dim] for dim in range(2)):
                continue
            for direction in tiles[lookup(position)][direction]:
                if direction in energized[position]:
                    continue
                energized[position].add(direction)
                beams_next.add((position, direction))
        return beams_next

    lookup = lambda position: grid[position[0]][position[1]]
    negate = lambda direction: tuple(-value for value in direction)
    add = lambda position, direction: tuple(p + d for p, d in zip(position, direction))

    tiles = {
        '.': construct_space(),
        '/': construct_mirror({(+1, 0): (0, -1)}),
        '\\': construct_mirror({(+1, 0): (0, +1)}),
        '|': construct_splitter((1, 0)),
        '-': construct_splitter((0, 1)),
    }
    energized = collections.defaultdict(set)
    beams = {((0, -1), (0, 1))}
    while beams:
        beams = step(beams)
    return len(energized)


# Solution to part two
@analyze
def solve_two(grid):
    shape = (len(grid), len(grid[0]))

    def construct_space():
        space = {
            direction: [direction] for direction in [(-1, 0), (+1, 0), (0, -1), (0, +1)]
        }
        return space

    def construct_mirror(mirror):
        for key, val in mirror.copy().items():
            mirror[negate(key)] = negate(val)
        for key, val in mirror.copy().items():
            mirror[val] = key
        mirror = {key: [val] for key, val in mirror.items()}
        return mirror

    def construct_splitter(direction):
        direction_perp = (direction[1], direction[0])
        splitter = {
            direction: [direction],
            direction_perp: [direction, negate(direction)],
        }
        for direction_in, directions_out in splitter.copy().items():
            splitter[negate(direction_in)] = [
                negate(direction_out) for direction_out in directions_out
            ]
        return splitter

    def step(beams, energized):
        beams_next = set()
        for position, direction in beams:
            position = add(position, direction)
            if not all(0 <= position[dim] < shape[dim] for dim in range(2)):
                continue
            for direction in tiles[lookup(position)][direction]:
                if direction in energized[position]:
                    continue
                energized[position].add(direction)
                beams_next.add((position, direction))
        return beams_next

    def count(position, direction):
        energized = collections.defaultdict(set)
        beams = {(position, direction)}
        while beams:
            beams = step(beams, energized)
        return len(energized)

    def test():
        for direction in [(-1, 0), (+1, 0), (0, -1), (0, +1)]:
            position = [
                {-1: shape[dim], +1: -1, 0: 0}[direction[dim]] for dim in range(2)
            ]
            index = direction.index(0)
            for x in range(shape[index]):
                position[index] = x
                yield count(tuple(position), direction)

    lookup = lambda position: grid[position[0]][position[1]]
    negate = lambda direction: tuple(-value for value in direction)
    add = lambda position, direction: tuple(p + d for p, d in zip(position, direction))

    tiles = {
        '.': construct_space(),
        '/': construct_mirror({(+1, 0): (0, -1)}),
        '\\': construct_mirror({(+1, 0): (0, +1)}),
        '|': construct_splitter((1, 0)),
        '-': construct_splitter((0, 1)),
    }

    return max(test())


# Solve
print(solve_one(read()))
print(solve_two(read()))
