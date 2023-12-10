import itertools, functools, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parent.parent))
from magic import *


# Reading in data
@memoize
def read():
    with open_input_file() as file:
        sketch = [line.rstrip() for line in file]
    return sketch


# Solution to part one
@analyze
def solve_one(sketch):
    shape = (len(sketch), len(sketch[0]))

    @functools.cache
    def get_start():
        regex = re.compile(r'S')
        for i, row in enumerate(sketch):
            if match := re.search(regex, row):
                return i, match.start()

    @functools.cache
    def setup_pipes():
        pipes = {
            '|': ((+1, 0), (-1, 0)),
            '-': ((0, +1), (0, -1)),
            'L': ((-1, 0), (0, +1)),
            'J': ((-1, 0), (0, -1)),
            '7': ((+1, 0), (0, -1)),
            'F': ((+1, 0), (0, +1)),
        }
        start = get_start()
        directions_start = []
        for direction in [(-1, 0), (+1, 0), (0, -1), (0, +1)]:
            position = add(start, direction)
            if not all(0 <= position[dim] < shape[dim] for dim in range(2)):
                continue
            pipe = lookup(position)
            if (directions := pipes.get(pipe)) is None:
                continue
            if (backwards := negate(direction)) not in directions:
                continue
            directions_start.append(direction)
        pipes['S'] = tuple(directions_start)
        return pipes

    def trace_loop():
        pipes = setup_pipes()
        position = get_start()
        direction = negate(pipes['S'][0])
        for count in itertools.count():
            pipe = lookup(position)
            if pipe == 'S' and count:
                return count
            backwards = negate(direction)
            direction = next(
                direction for direction in pipes[pipe] if direction != backwards
            )
            position = add(position, direction)

    lookup = lambda position: sketch[position[0]][position[1]]
    negate = lambda direction: tuple(-value for value in direction)
    add = lambda position, direction: tuple(p + d for p, d in zip(position, direction))

    return trace_loop() // 2


# Solution to part two
@analyze
def solve_two(sketch):
    shape = (len(sketch), len(sketch[0]))

    @functools.cache
    def get_start():
        regex = re.compile(r'S')
        for i, row in enumerate(sketch):
            if match := re.search(regex, row):
                return i, match.start()

    @functools.cache
    def setup_pipes():
        pipes = {
            '|': ((+1, 0), (-1, 0)),
            '-': ((0, +1), (0, -1)),
            'L': ((-1, 0), (0, +1)),
            'J': ((-1, 0), (0, -1)),
            '7': ((+1, 0), (0, -1)),
            'F': ((+1, 0), (0, +1)),
        }
        start = get_start()
        directions_start = []
        for direction in [(-1, 0), (+1, 0), (0, -1), (0, +1)]:
            position = add(start, direction)
            if not all(0 <= position[dim] < shape[dim] for dim in range(2)):
                continue
            pipe = lookup(position)
            if (directions := pipes.get(pipe)) is None:
                continue
            if (backwards := negate(direction)) not in directions:
                continue
            directions_start.append(direction)
        pipes['S'] = tuple(directions_start)
        return pipes

    def trace_polygon():
        pipes = setup_pipes()
        position = get_start()
        direction = negate(pipes['S'][0])
        vertices = []
        for count in itertools.count():
            pipe = lookup(position)
            if pipe not in '|-':
                vertices.append(position)
            if pipe == 'S' and count:
                return count, vertices
            backwards = negate(direction)
            direction = next(
                direction for direction in pipes[pipe] if direction != backwards
            )
            position = add(position, direction)

    lookup = lambda position: sketch[position[0]][position[1]]
    negate = lambda direction: tuple(-value for value in direction)
    add = lambda position, direction: tuple(p + d for p, d in zip(position, direction))

    count, vertices = trace_polygon()
    area_shoelace = abs(
        sum(i0 * j1 - i1 * j0 for (i0, j0), (i1, j1) in zip(vertices, vertices[1:]))
    )
    return (area_shoelace - count) // 2 + 1


# Solve
print(solve_one(read()))
print(solve_two(read()))
