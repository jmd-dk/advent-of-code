import collections

# Part one
with open('input.txt') as f:
    lines = f.readlines()
lines = [
    tuple([
        tuple([
            int(n.strip())
            for n in pair.split(',')
        ])
        for pair in line.split('->')
    ])
    for line in lines
]
def get_range(x1, x2):
    if x2 > x1:
        yield from range(x1, x2 + 1)
    else:
        yield from range(x1, x2 - 1, -1)
def mark_horzvert(counter):
    for (x1, y1), (x2, y2) in lines:
        if x1 == x2:
            for y in get_range(y1, y2):
                counter[x1, y] += 1
        if y1 == y2:
            for x in get_range(x1, x2):
                counter[x, y1] += 1
def count(counter):
    return sum(1 for val in counter.values() if val >= 2)
counter = collections.Counter()
mark_horzvert(counter)
print('part one:', count(counter))

# Part two
def mark_diag(counter):
    def get_range(x1, x2):
        if x2 > x1:
            yield from range(x1, x2 + 1)
        else:
            yield from range(x1, x2 - 1, -1)
    for (x1, y1), (x2, y2) in lines:
        if abs(x2 - x1) == abs(y2 - y1):
            x, y = x1, y1
            for x, y in zip(
                get_range(x1, x2),
                get_range(y1, y2),
            ):
                counter[x, y] += 1
counter = collections.Counter()
mark_horzvert(counter)
mark_diag(counter)
print('part two:', count(counter))

