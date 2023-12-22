import collections, itertools, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    range_inclusive = lambda start, stop: range(start, stop + 1)
    bricks = []
    with open_input() as file:
        for line in file:
            coords = tuple(map(int, re.findall(r'\d+', line)))
            bricks.append(tuple(range_inclusive(*coords[dim::3]) for dim in range(3)))
    return bricks


# Solution to part one
@analyze
def solve_one(bricks):
    def construct_tower():
        bricks.sort(key=(lambda brick: brick[2].start))
        tower = {
            xyz: i
            for i, brick in enumerate(bricks)
            for xyz in itertools.product(*brick)
        }
        return tower

    def fall():
        for i, brick in enumerate(bricks):
            if (z := fall_single(brick)) == brick[2].start:
                continue
            disintegrate(brick)
            brick = bricks[i] = (*brick[:2], range(z, z + len(brick[2])))
            for xyz in itertools.product(*brick):
                tower[xyz] = i

    def fall_single(brick):
        for z in range(brick[2].start - 1, -1, -1):
            for x, y in itertools.product(*brick[:2]):
                if (x, y, z) in tower:
                    return z + 1
        return z + 1

    def disintegrate(brick):
        for xyz in itertools.product(*brick):
            tower.pop(xyz)

    def get_supports(brick, z):
        return {
            i
            for x, y in itertools.product(*brick[:2])
            if (i := tower.get((x, y, z))) is not None
        }

    def check(i):
        return all(len(supporters[j]) != 1 for j in supports[i])

    tower = construct_tower()
    fall()
    supports = [get_supports(brick, brick[2].stop) for brick in bricks]
    supporters = [get_supports(brick, brick[2].start - 1) for brick in bricks]
    return sum(check(i) for i in range(len(bricks)))


# Solution to part two
@analyze
def solve_two(bricks):
    def construct_tower():
        bricks.sort(key=(lambda brick: brick[2].start))
        tower = {
            xyz: i
            for i, brick in enumerate(bricks)
            for xyz in itertools.product(*brick)
        }
        return tower

    def fall(tower):
        for i, brick in enumerate(bricks):
            if (z := fall_single(brick, tower)) == brick[2].start:
                continue
            disintegrate(brick, tower)
            brick = bricks[i] = (*brick[:2], range(z, z + len(brick[2])))
            for xyz in itertools.product(*brick):
                tower[xyz] = i

    def fall_single(brick, tower):
        for z in range(brick[2].start - 1, -1, -1):
            for x, y in itertools.product(*brick[:2]):
                if (x, y, z) in tower:
                    return z + 1
        return z + 1

    def disintegrate(brick, tower):
        for xyz in itertools.product(*brick):
            tower.pop(xyz)

    def fall_single_check(brick, tower):
        z = brick[2].start - 1
        for x, y in itertools.product(*brick[:2]):
            if (x, y, z) in tower:
                return False
        return z != 0

    def check(i, tower):
        disintegrate(bricks[i], tower := tower.copy())
        n = 0
        for j, brick in enumerate(bricks[i + 1 :], i + 1):
            if not fall_single_check(brick, tower):
                continue
            n += 1
            disintegrate(brick, tower)
        return n

    tower = construct_tower()
    fall(tower)
    return sum(check(i, tower) for i in range(len(bricks)))


# Solve
print(solve_one(read()))
print(solve_two(read()))
