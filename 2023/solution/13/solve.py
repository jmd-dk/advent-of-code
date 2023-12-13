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
    def summarize(pattern):
        def check_reflection(pattern, transpose=False):
            shape = (len(pattern), len(pattern[0]))
            if transpose:
                shape = (shape[1], shape[0])
            possibilities = set(range(shape[1] - 1))
            for i in range(shape[0]):
                for possibility in possibilities.copy():
                    j_bgn = max(0, 2 - shape[1] + 2 * possibility)
                    j_end = possibility + 1
                    for j in range(j_bgn, j_end):
                        i_left, j_left = i, j
                        i_right, j_right = i_left, -j_left + 2 * possibility + 1
                        if transpose:
                            i_left, j_left = j_left, i_left
                            i_right, j_right = j_right, i_right
                        if pattern[i_left][j_left] != pattern[i_right][j_right]:
                            possibilities.remove(possibility)
                            break
            if len(possibilities) == 1:
                return 1 + next(iter(possibilities))
            return 0

        points = {False: 1, True: 100}
        return sum(
            n * check_reflection(pattern, transpose) for transpose, n in points.items()
        )

    return sum(summarize(pattern) for pattern in patterns)


# Solution to part two
@analyze
def solve_two(patterns):
    def summarize(pattern):
        def check_reflection(pattern, transpose=False):
            shape = (len(pattern), len(pattern[0]))
            if transpose:
                shape = (shape[1], shape[0])
            possibilities = set(range(shape[1] - 1))
            violations = collections.Counter()
            for i in range(shape[0]):
                for possibility in possibilities.copy():
                    j_bgn = max(0, 2 - shape[1] + 2 * possibility)
                    j_end = possibility + 1
                    for j in range(j_bgn, j_end):
                        i_left, j_left = i, j
                        i_right, j_right = i_left, -j_left + 2 * possibility + 1
                        if transpose:
                            i_left, j_left = j_left, i_left
                            i_right, j_right = j_right, i_right
                        if pattern[i_left][j_left] != pattern[i_right][j_right]:
                            violations[possibility] -= 1
                            if violations[possibility] == -2:
                                possibilities.remove(possibility)
                                break
            possibility, violation = violations.most_common(1)[0]
            if violation == -1:
                return 1 + possibility
            return 0

        points = {False: 1, True: 100}
        return sum(
            n * check_reflection(pattern, transpose) for transpose, n in points.items()
        )

    return sum(summarize(pattern) for pattern in patterns)


# Solve
print(solve_one(read()))
print(solve_two(read()))
