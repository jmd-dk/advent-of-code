import pathlib, sys
sys.path.append(str(pathlib.Path(__file__).parent.parent))
from helper import get_input_file

import re


# Part one
def solve(spelling=False):
    numbers = ['one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine']
    values = {number: str(index) for index, number in enumerate(numbers, 1)}
    values |= {number[::-1]: str(index) for index, number in enumerate(numbers, 1)}
    pattern_digit = r'\d'
    if spelling:
        pattern_spelling = '|'.join(numbers)
        pattern_forward = '{}|{}'.format(pattern_digit, pattern_spelling)
        pattern_backward = '{}|{}'.format(pattern_digit, pattern_spelling[::-1])
    else:
        pattern_forward = pattern_backward = pattern_digit
    regex_forward = re.compile(pattern_forward)
    regex_backward = re.compile(pattern_backward)
    s = 0
    with open(get_input_file()) as file:
        for line in file:
            m = values.get(m := re.search(regex_forward, line)[0], m)
            n = values.get(n := re.search(regex_backward, line[::-1])[0], n)
            s += int(m + n)
    return s
print('part one:', solve())

# Part two
print('part two:', solve(spelling=True))

