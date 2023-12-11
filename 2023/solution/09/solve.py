# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    with open_input_file() as file:
        histories = [list(map(int, line.split())) for line in file]
    return histories


# Solution to part one
@analyze
def solve_one(histories):
    def extrapolate(numbers):
        def diff(numbers):
            return [n - m for m, n in zip(numbers, numbers[1:])]

        sum_right = 0
        while any(numbers):
            sum_right += numbers[-1]
            numbers = diff(numbers)
        return sum_right

    return sum(extrapolate(history) for history in histories)


# Solution to part two
@analyze
def solve_two(histories):
    def extrapolate_left(numbers):
        def diff(numbers):
            return [n - m for m, n in zip(numbers, numbers[1:])]

        sum_left = 0
        sign = -1
        while any(numbers):
            sum_left += (sign := -sign) * numbers[0]
            numbers = diff(numbers)
        return sum_left

    return sum(extrapolate_left(history) for history in histories)


# Solve
print(solve_one(read()))
print(solve_two(read()))
