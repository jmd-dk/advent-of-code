import functools, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    rows = []
    with open_input_file() as file:
        for line in file:
            springs, sizes = line.rstrip().split()
            rows.append((springs, tuple(map(int, sizes.split(',')))))
    return rows


# Solution to part one
@analyze
def solve_one(rows):
    @functools.cache
    def count(springs, sizes):
        if not sizes:
            return '#' not in springs
        if not springs:
            return 0

        sum_arrangements = 0
        spring = springs[0]
        if spring in '.?':
            sum_arrangements += count(springs[1:], sizes)
        if spring in '#?':
            size = sizes[0]
            if re.match(rf'^(#|\?){{{size}}}(\.|\?|$)', springs):
                sum_arrangements += count(springs[size + 1 :], sizes[1:])
        return sum_arrangements

    return sum(count(springs, sizes) for springs, sizes in rows)


# Solution to part two
@analyze
def solve_two(rows):
    @functools.cache
    def count(springs, sizes):
        if not sizes:
            return '#' not in springs
        if not springs:
            return 0

        sum_arrangements = 0
        spring = springs[0]
        if spring in '.?':
            sum_arrangements += count(springs[1:], sizes)
        if spring in '#?':
            size = sizes[0]
            if re.match(rf'^(#|\?){{{size}}}(\.|\?|$)', springs):
                sum_arrangements += count(springs[size + 1 :], sizes[1:])
        return sum_arrangements

    def unfold(springs, sizes, n=5):
        springs = '?'.join([springs] * n)
        sizes *= n
        return springs, sizes

    return sum(count(*unfold(springs, sizes)) for springs, sizes in rows)


# Solve
print(solve_one(read()))
print(solve_two(read()))
