import itertools

# Read in data
rocks = set()
xmin = +float('inf')
xmax = -float('inf')
ymax = -float('inf')
with open('input.txt') as f:
    for line in f:
        corners = [tuple(map(int, corner.split(','))) for corner in line.rstrip().split(' -> ')]
        for corner0, corner1 in zip(corners, corners[1:]):
            x0, x1 = sorted((corner0[0], corner1[0]))
            y0, y1 = sorted((corner0[1], corner1[1]))
            rocks |= {
                (i, j)
                for i in range(x0, x1 + 1)
                for j in range(y0, y1 + 1)
            }
            xmin = min(x0, xmin)
            xmax = max(x1, xmax)
            ymax = max(y1, ymax)
def reset_rocks():
    rocks.clear()
    rocks.update(rocks_ori)
rocks_ori = rocks.copy()

# Part one
def pour(bottomless=True):
    sand = spawn
    while True:
        if bottomless and not (xmin <= sand[0] <= xmax):
            return
        for dx, dy in [(0, 1), (-1, 1), (1, 1)]:
            sand_new = (sand[0] + dx, sand[1] + dy)
            if sand_new not in rocks:
                sand = sand_new
                break
        else:
            break
    rocks.add(sand)
    if not bottomless and sand == spawn:
        return
    return True
spawn = (500, 0)
reset_rocks()
for n in itertools.count():
    if not pour():
        break
print('part one:', n)

# Part two
reset_rocks()
y_floor = 2 + ymax
height = y_floor - spawn[1]
rocks |= {
    (x_floor, y_floor)
    for x_floor in range(spawn[0] - height, spawn[0] + height + 1)
}
for n in itertools.count(1):
    if not pour(False):
        break
print('part two:', n)

