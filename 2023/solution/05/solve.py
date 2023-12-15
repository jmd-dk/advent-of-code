import collections, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    Mapping = collections.namedtuple('Mapping', ('dst_start', 'src_start', 'length'))
    with open_input() as file:
        seeds = list(map(int, re.findall(r'\d+', file.readline())))
        maps = []
        for line in file:
            if not line.strip():
                maps.append(mappings := [])
                file.readline()
                continue
            mappings.append(Mapping(*map(int, re.findall(r'\d+', line))))
    return seeds, maps


# Solution to part one
@analyze
def solve_one(seeds, maps):
    numbers = seeds
    for mappings in maps:
        for i, number in enumerate(numbers):
            for dst_start, src_start, length in mappings:
                if src_start <= number < src_start + length:
                    numbers[i] += dst_start - src_start
                    break
    return min(numbers)


# Solution to part two
@analyze
def solve_two(seeds, maps):
    class Mapping:
        def __init__(self, start, src_start, length):
            self.start = start  # dst_start
            self.src_start = src_start
            self.stop = start + length  # dst_stop
            self.src_stop = src_start + length
            self.Δ = start - src_start

        def __lt__(self, other):
            return self.src_start < other.src_start

    def apply_mapping(mapping, index=0):
        if index == len(maps):
            return mapping.start

        def recurse(mapping, stop=None):
            nonlocal value_min
            if stop is None:
                value = apply_mapping(mapping, index + 1)
            elif (start := mapping) < stop:
                value = apply_mapping(range(start, stop), index + 1)
            else:
                return
            value_min = min(value_min, value)

        value_min = float('inf')
        identity_pointer = mapping.start
        for mapping_next in maps[index]:
            start = max(mapping_next.src_start, mapping.start)
            stop = min(mapping_next.src_stop, mapping.stop)
            if stop <= start:
                continue
            mapping_next_reduced = Mapping(start + mapping_next.Δ, start, stop - start)
            recurse(identity_pointer, start)
            identity_pointer = stop
            recurse(mapping_next_reduced)
        recurse(identity_pointer, mapping.stop)
        return value_min

    seed_ranges = [
        range(start, start + length) for start, length in zip(seeds[::2], seeds[1::2])
    ]
    maps = [sorted(Mapping(*mapping) for mapping in mappings) for mappings in maps]
    return min(apply_mapping(mapping) for mapping in seed_ranges)


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
