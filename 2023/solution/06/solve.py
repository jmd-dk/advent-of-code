import re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    find_ints = lambda: list(map(int, re.findall(r'\d+', file.readline())))
    with open_input_file() as file:
        times = find_ints()
        dists = find_ints()
    return times, dists


# Solution to part one
@analyze
def solve_one(times, dists):
    quadratic = lambda sign: (time + sign * (time**2 - 4 * dist) ** 0.5) / 2
    prod_nwins = 1
    for time, dist in zip(times, dists):
        win_min = int(w := quadratic(-1)) + 1
        win_max = int(w := quadratic(+1)) - w.is_integer()
        nwins = win_max - win_min + 1
        prod_nwins *= nwins
    return prod_nwins


# Solution to part two
@analyze
def solve_two(times, dists):
    concat = lambda lst: int(''.join(map(str, lst)))
    time = concat(times)
    dist = concat(dists)
    quadratic = lambda sign: (time + sign * (time**2 - 4 * dist) ** 0.5) / 2
    win_min = int(w := quadratic(-1)) + 1
    win_max = int(w := quadratic(+1)) - w.is_integer()
    nwins = win_max - win_min + 1
    return nwins


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
