import re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parent.parent))
from magic import *


# Reading in data
@memoize
def read():
    with open_input_file() as file:
        lines = [line.strip() for line in file]
    return lines


# Solution to part one
@analyze
def solve_one(lines):
    sum_calibration = 0
    for line in lines:
        m = re.search(r'\d', line)[0]
        n = re.search(r'\d', line[::-1])[0]
        calibration = int(m + n)
        sum_calibration += calibration
    return sum_calibration


# Solution to part two
@analyze
def solve_two(lines):
    numbers = ['one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine']
    values = {number: str(index) for index, number in enumerate(numbers, 1)}
    values |= {number[::-1]: str(index) for index, number in enumerate(numbers, 1)}
    pattern_digit = r'\d'
    pattern_spelling = '|'.join(numbers)
    pattern_forward = '{}|{}'.format(pattern_digit, pattern_spelling)
    pattern_backward = '{}|{}'.format(pattern_digit, pattern_spelling[::-1])
    regex_forward = re.compile(pattern_forward)
    regex_backward = re.compile(pattern_backward)
    sum_calibration = 0
    for line in lines:
        m = values.get(m := re.search(regex_forward, line)[0], m)
        n = values.get(n := re.search(regex_backward, line[::-1])[0], n)
        calibration = int(m + n)
        sum_calibration += calibration
    return sum_calibration


# Solve
print(solve_one(read()))
print(solve_two(read()))
