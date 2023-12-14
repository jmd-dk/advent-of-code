import collections

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    patterns = []
    with open_input_file() as file:
        patterns.append(pattern := [])
        for line in file:
            line = line.rstrip()
            if not line:
                patterns.append(pattern := [])
                continue
            pattern.append(line)
    while not patterns[-1]:
        patterns.pop()
    return patterns


# Solution to part one
@analyze
def solve_one(patterns):
    def check_reflections(pattern, transpose=False):
        if transpose:
            pattern = list(zip(*pattern))
        for i in range(1, len(pattern)):
            block1 = pattern[2 * i - 1 : i - 1 : -1]
            block0 = pattern[i - len(block1) : i]
            if block0 == block1:
                return i
        return 0

    def summarize(pattern):
        points = {False: 100, True: 1}
        return sum(
            n * check_reflections(pattern, transpose) for transpose, n in points.items()
        )

    return sum(summarize(pattern) for pattern in patterns)


# Solution to part two
@analyze
def solve_two(patterns):
    def check_reflections(pattern, transpose=False, violations_required=1):
        if transpose:
            pattern = [''.join(row) for row in zip(*pattern)]
        for i in range(1, len(pattern)):
            block0 = ''.join(pattern[max(0, 2 * i - len(pattern)) : i])
            block1 = ''.join(pattern[2 * i - 1 : i - 1 : -1])
            violations = 0
            for c0, c1 in zip(block0, block1):
                violations += c0 != c1
                if violations > violations_required:
                    break
            if violations == violations_required:
                return i
        return 0

    def summarize(pattern):
        points = {False: 100, True: 1}
        return sum(
            n * check_reflections(pattern, transpose) for transpose, n in points.items()
        )

    return sum(summarize(pattern) for pattern in patterns)


# Solve
print(solve_one(read()))
print(solve_two(read()))
