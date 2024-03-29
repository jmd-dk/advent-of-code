import collections

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    with open_input() as file:
        grid = {
            complex(j, i): ch
            for i, line in enumerate(file)
            for j, ch in enumerate(line.rstrip())
        }
    return grid


# Solution to part one
@analyze
def solve_one(grid):
    size = int(len(grid) ** 0.5)
    beams = {(complex(-1), complex(+1))}
    energized = collections.defaultdict(set)
    while beams:
        beams_next = set()
        for pos, vel in beams:
            pos += vel
            if not (0 <= pos.imag < size) or not (0 <= pos.real < size):
                continue
            if vel in energized[pos]:
                continue
            energized[pos].add(vel)
            match (tile := grid[pos]), vel:
                case ('.', _) | ('|', -1j | 1j) | ('-', -1 | 1):
                    beams_next.add((pos, vel))
                case '/' | '\\', _:
                    vel = {'/': -1, '\\': +1}[tile] * complex(vel.imag, vel.real)
                    beams_next.add((pos, vel))
                case '|' | '-', _:
                    vel = {'|': 1j, '-': 1}[tile]
                    beams_next.add((pos, -vel))
                    beams_next.add((pos, +vel))
        beams = beams_next
    return len(energized)


# Solution to part two
@analyze
def solve_two(grid):
    size = int(len(grid) ** 0.5)

    def shine(*beam, trace_splitters=False):
        beams = {tuple(map(complex, beam))}
        energized = set()
        connected = set()
        while beams:
            beams_next = set()
            for pos, vel in beams:
                pos += vel
                if not (0 <= pos.imag < size) or not (0 <= pos.real < size):
                    continue
                if (pos, vel) in energized:
                    continue
                energized.add((pos, vel))
                match (tile := grid[pos]), vel:
                    case ('.', _) | ('|', -1j | 1j) | ('-', -1 | 1):
                        beams_next.add((pos, vel))
                    case '/' | '\\', _:
                        vel = {'/': -1, '\\': +1}[tile] * complex(vel.imag, vel.real)
                        beams_next.add((pos, vel))
                    case '|' | '-', _:
                        if not trace_splitters:
                            energized |= splitters[pos].energized
                            continue
                        if len(energized) > 1:
                            connected.add(pos)
                            continue
                        vel = get_splitter_vel(tile)
                        beams_next.add((pos, -vel))
                        beams_next.add((pos, +vel))
            beams = beams_next
        return energized, connected

    def get_splitter_vel(tile, perp=False):
        vels = {'|': 1j, '-': 1}
        if perp:
            vels['|'], vels['-'] = vels['-'], vels['|']
        return vels[tile]

    def test():
        for i in range(size):
            yield measure(*shine(complex(-1, i), +1))
            yield measure(*shine(complex(size, i), -1))
            yield measure(*shine(complex(i, -1), +1j))
            yield measure(*shine(complex(i, size), -1j))

    def measure(energized, connected):
        return len({pos for pos, vel in energized})

    class Splitter:
        def __init__(self, pos, energized, connections):
            self.pos = pos
            self.energized = energized
            self.connections = connections
            self.fully_connected = False
            self.fully_energized = False

        def connect(self, splitters):
            missing = self.connections.copy()
            while missing:
                pos = missing.pop()
                splitter = splitters[pos]
                if splitter.fully_connected:
                    self.connections |= splitter.connections
                    missing -= splitter.connections
                else:
                    missing |= splitter.connections - self.connections
                self.connections.add(pos)
            self.connections.discard(self.pos)
            self.fully_connected = True

        def energize(self):
            missing = self.connections.copy()
            while missing:
                pos = missing.pop()
                splitter = splitters[pos]
                self.energized |= splitter.energized
                if splitter.fully_energized:
                    missing -= splitter.connections
            self.fully_energized = True

    splitters = {
        pos: Splitter(
            pos,
            *shine(
                pos - (vel := get_splitter_vel(tile, perp=True)),
                vel,
                trace_splitters=True,
            ),
        )
        for pos, tile in grid.items()
        if tile in '|-'
    }
    for splitter in splitters.values():
        splitter.connect(splitters)
    for splitter in splitters.values():
        splitter.energize()
    return max(test())


# Solve
print(solve_one(read()))
print(solve_two(read()))
