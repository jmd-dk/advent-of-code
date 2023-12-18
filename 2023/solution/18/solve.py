import collections, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    Instruction = collections.namedtuple('Instruction', ('direction', 'n', 'color'))
    with open_input() as file:
        plan = []
        for line in file:
            direction, n, color = re.match(r'(.) (\d+) \(#(.+)\)', line).groups()
            plan.append(Instruction(direction, int(n), color))
    return plan


# Solution to part one
@analyze
def solve_one(plan):
    def get_step(instruction):
        mapping = {'R': +1, 'D': +1j, 'L': -1, 'U': -1j}
        return mapping[instruction.direction] * instruction.n

    area_shoelace = 0
    perim = 0
    corner0 = 0
    for instruction in plan:
        step = get_step(instruction)
        perim += abs(step)
        corner1 = corner0 + step
        area_shoelace += corner0.real * corner1.imag - corner1.real * corner0.imag
        corner0 = corner1
    area = int(abs(area_shoelace) + perim) // 2 + 1
    return area


# Solution to part two
@analyze
def solve_two(plan):
    def get_step(instruction):
        mapping = {'0': +1, '1': +1j, '2': -1, '3': -1j}
        n = int(instruction.color[:5], base=16)
        direction = instruction.color[5]
        return mapping[direction] * n

    area_shoelace = 0
    perim = 0
    corner0 = 0
    for instruction in plan:
        step = get_step(instruction)
        perim += abs(step)
        corner1 = corner0 + step
        area_shoelace += corner0.real * corner1.imag - corner1.real * corner0.imag
        corner0 = corner1
    area = int(abs(area_shoelace) + perim) // 2 + 1
    return area


# Solve
print(solve_one(read()))
print(solve_two(read()))
