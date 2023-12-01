import re

# Read in data
sensors = {}
num_re = r'(-?\d+)'
with open('input.txt') as f:
    for line in f:
        xs, ys, xb, yb = map(
            int,
            re.search(rf'.+x={num_re}.+y={num_re}'*2, line).groups()
        )
        sensors[xs, ys] = xb, yb

# Part one
dist = lambda pos0, pos1: sum(
    abs(coord0 - coord1)
    for coord0, coord1 in zip(pos0, pos1)
)
def construct_ranges(row):
    def add_range(sensor, beacon):
        d = dist(sensor, beacon) - abs(sensor[1] - row)
        if d <= 0:
            return
        xmin = sensor[0] - d
        xmax = sensor[0] + d + 1
        for i, r in enumerate(ranges):
            if not r:
                continue
            elif r.start <= xmin and r.stop >= xmax:
                return
            elif xmin <= r.start and xmax >= r.stop:
                ranges[i] = None
            elif r.start < xmax <= r.stop:
                ranges[i] = range(xmax, r.stop)
            elif r.start <= xmin < r.stop:
                ranges[i] = range(r.start, xmin)
        ranges.append(range(xmin, xmax))
    def combine_ranges(ranges):
        if not ranges:
            return ranges
        ranges = sorted(ranges, key=(lambda r: r.start))
        r = ranges[0]
        start, stop = r.start, r.stop
        ranges_combined = []
        for r in ranges[1:]:
            if stop == r.start:
                stop = r.stop
                continue
            ranges_combined.append(range(start, stop))
            start, stop = r.start, r.stop
        ranges_combined.append(range(start, stop))
        return ranges_combined
    ranges = []
    for sensor, beacon in sensors.items():
        add_range(sensor, beacon)
    ranges = [r for r in ranges if r]
    ranges = combine_ranges(ranges)
    return ranges
def count_ruled_out(row):
    ranges = construct_ranges(row)
    count = sum(r.stop - r.start for r in ranges)
    count -= sum(sensor[1] == row for sensor in sensors)
    count -= len({beacon for beacon in sensors.values() if beacon[1] == row})
    return count
print('part one:', count_ruled_out(2_000_000))

# Part two
def get_rows_cross(lim):
    lines = []
    rows_cross = set()
    for sensor, beacon in sensors.items():
        d = dist(sensor, beacon)
        y = sensor[1]
        for a in range(-1, 2, 2):
            for sign in range(-1, 2, 2):
                x0 = sensor[0]
                x1 = x0 + sign*d
                b = y - a*x1
                if sign == -1:
                    x0, x1 = x1, x0
                if a == -1:
                    lines.append((a, b, x0, x1))
                    continue
                for (a_other, b_other, x0_other, x1_other) in lines:
                    db = b - b_other
                    da = a - a_other
                    x_cross = -db//da
                    if x_cross*da != -db:
                        continue
                    if not (x0 <= x_cross <= x1) or not (x0_other <= x_cross <= x1_other):
                        continue
                    y_cross = a*x_cross + b
                    if 0 <= y_cross <= lim:
                        rows_cross.add(y_cross)
    return rows_cross
def find_tuning_frequency(lim):
    for row in get_rows_cross(lim):
        ranges = construct_ranges(row)
        if len(ranges) > 1:
            return ranges[0].stop*lim + row
print('part two:', find_tuning_frequency(4_000_000))

