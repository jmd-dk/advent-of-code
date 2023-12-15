import collections, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    with open_input() as file:
        sequence = file.read().rstrip().split(',')
    return sequence


# Solution to part one
@analyze
def solve_one(sequence):
    def compute_hash(string):
        value = 0
        for c in string:
            value += ord(c)
            value *= 17
            value %= 256
        return value

    return sum(compute_hash(string) for string in sequence)


# Solution to part two
@analyze
def solve_two(sequence):
    def compute_hash(string):
        value = 0
        for c in string:
            value += ord(c)
            value *= 17
            value %= 256
        return value

    def organize(sequence):
        boxes = collections.defaultdict(dict)
        for string in sequence:
            match = re.match(r'(\w+)(=|-)(\d)?', string)
            label = match[1]
            operation = match[2]
            i = compute_hash(label)
            if operation == '-':
                boxes[i].pop(label, None)
            elif operation == '=':
                focal = int(match[3])
                boxes[i][label] = focal
        return boxes

    return sum(
        (1 + i) * j * focal
        for i, box in organize(sequence).items()
        for j, focal in enumerate(box.values(), 1)
    )


# Solve
print(solve_one(read()))
print(solve_two(read()))
