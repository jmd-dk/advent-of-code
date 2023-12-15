import collections, math, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    Number = collections.namedtuple('Number', ('value', 'i_span', 'j'))
    numbers = set()
    symbols = {}
    with open_input() as file:
        for j, line in enumerate(file):
            for match in re.finditer(r'\d+|[^.\s]', line):
                value = match[0]
                if value.isdecimal():
                    numbers.add(Number(int(value), match.span(), j))
                else:
                    symbols[match.start(), j] = value
    return numbers, symbols


# Solution to part one
@analyze
def solve_one(numbers, symbols):
    def check(number):
        j = number.j
        for i in range(*number.i_span):
            for di in range(-1, 2):
                for dj in range(-1, 2):
                    position = (i + di, j + dj)
                    if position in symbols:
                        return True

    return sum(number.value for number in numbers if check(number))


# Solution to part two
@analyze
def solve_two(numbers, symbols):
    symbol_gear = '*'
    nadjacent = 2
    gears = collections.defaultdict(set)

    def check(number):
        j = number.j
        for i in range(*number.i_span):
            for di in range(-1, 2):
                for dj in range(-1, 2):
                    position = (i + di, j + dj)
                    if symbols.get(position) == symbol_gear:
                        gears[position].add(number.value)

    for number in numbers:
        check(number)
    return sum(math.prod(gear) for gear in gears.values() if len(gear) == nadjacent)


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
