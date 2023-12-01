import collections, re
import numpy as np

# Part one
range_inclusive = lambda bgn, end: range(bgn, end + 1)
Step = collections.namedtuple('Step', ('state', 'x', 'y', 'z'))
steps = []
with open('input.txt') as f:
    for line in f:
        match = re.search(r'(on|off) x=(-?\d+)\.\.(-?\d+),y=(-?\d+)\.\.(-?\d+),z=(-?\d+)\.\.(-?\d+)', line)
        state = (match.group(1) == 'on')
        x = range_inclusive(*map(int, match.group(2, 3)))
        y = range_inclusive(*map(int, match.group(4, 5)))
        z = range_inclusive(*map(int, match.group(6, 7)))
        steps.append(Step(state, x, y, z))
def transform(s):
    return slice(size + s.start, size + s.stop)
size = 50
grid = np.zeros([2*size + 1]*3, dtype=bool)
for step in steps:
    grid[transform(step.x), transform(step.y), transform(step.z)] = step.state
print('part one:', grid.sum())

