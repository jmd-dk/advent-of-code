import collections, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parent.parent))
from magic import *


# Reading in data
@memoize
def read():
    Mapping = collections.namedtuple('Mapping', ('dst_start', 'src_range'))
    with open_input_file() as file:
        seeds = list(map(int, re.findall(r'\d+', file.readline())))
        maps = []
        for line in file:
            if not line.strip():
                maps.append(mappings := [])
                file.readline()
                continue
            dst_start, src_start, length = map(int, re.findall(r'\d+', line))
            src_range = range(src_start, src_start + length)
            mappings.append(Mapping(dst_start, src_range))
    return seeds, maps


# Solution to part one
@analyze
def solve_one(seeds, maps):
    numbers = seeds
    for mappings in maps:
        for i, number in enumerate(numbers):
            for mapping in mappings:
                if number in mapping.src_range:
                    numbers[i] += mapping.dst_start - mapping.src_range.start
                    break
    return min(numbers)


# Solution to part two
@analyze
def solve_two(seed_ranges, maps):
    number_min = float('inf')
    for start, length in zip(seed_ranges[::2], seed_ranges[1::2]):
        for number in range(start, start + length):
            for mappings in maps:
                for mapping in mappings:
                    if number in mapping.src_range:
                        number += mapping.dst_start - mapping.src_range.start
                        break
            number_min = min(number_min, number)
    return number_min


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
