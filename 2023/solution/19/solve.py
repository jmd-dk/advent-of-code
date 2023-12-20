import math, operator, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    val_min, val_max = 1, 4000

    def parse_rule(rule):
        if match := re.match(r'(\w)(<|>)(\d+):(\w+)', rule):
            xmas, cmp, val, dest = match.groups()
            cmp_func = getattr(operator, {'<': 'lt', '>': 'gt'}[cmp])
            val = int(val)
            range_xmas = {
                '<': range(val_min, val),
                '>': range(val + 1, val_max + 1),
            }[cmp]
            rule = lambda part, *, xmas=xmas, cmp_func=cmp_func, val=val, dest=dest: (
                cmp_func(part[xmas], val) * dest
            )
        else:
            dest = rule
            xmas = 'x'
            range_xmas = range(val_min, val_max + 1)
            rule = lambda part: dest
        rule.xmas = xmas
        rule.range = range_xmas
        rule.dest = dest
        return rule

    workflows, parts = {}, []
    with open_input() as file:
        for line in file:
            if match := re.match(r'(\w+)\{(.+)\}', line):
                name, rules = match.groups()
                rules = list(map(parse_rule, rules.split(',')))
                workflows[name] = rules
            elif part := {
                xmas: int(rating)
                for xmas, rating in re.findall(r'([xmas])=(\d+)', line)
            }:
                parts.append(part)
    return workflows, parts


# Solution to part one
@analyze
def solve_one(workflows, parts):
    def sort(part):
        name = 'in'
        while True:
            for rule in workflows[name]:
                if (name := rule(part)) == 'A':
                    return part
                elif name == 'R':
                    return {}
                elif name:
                    break

    return sum(sum(sort(part).values()) for part in parts)


# Solution to part two
@analyze
def solve_two(workflows, parts):
    val_min, val_max = (
        getattr(next(iter(workflows.values()))[-1].range, attr)
        for attr in ('start', 'stop')
    )
    val_max -= 1

    def merge_ranges(range0, range1):
        range_overlap = range(
            max(range0.start, range1.start), min(range0.stop, range1.stop)
        )
        range_exclude = range(range0.start, min(range0.stop, range1.start))
        if not range_exclude:
            range_exclude = range(max(range0.start, range1.stop), range0.stop)
        return range_overlap, range_exclude

    def check(ranges, name='in'):
        if name == 'A':
            yield ranges
            return
        elif name == 'R':
            return
        for rule in workflows[name]:
            range_overlap, range_exclude = merge_ranges(ranges[rule.xmas], rule.range)
            if range_overlap:
                yield from check(ranges | {rule.xmas: range_overlap}, rule.dest)
            if range_exclude:
                ranges[rule.xmas] = range_exclude

    ranges = {ch: range(val_min, val_max + 1) for ch in 'xmas'}
    return sum(math.prod(len(r) for r in ranges.values()) for ranges in check(ranges))


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
