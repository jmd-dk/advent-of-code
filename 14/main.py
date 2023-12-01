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
rocks_ori = rocks

# Part one
def reset():
    rocks.clear()
    rocks.update(rocks_ori)
def pour(sand, bottomless):
    if bottomless and not (xmin <= sand[0] <= xmax):
        return True
    for dx, dy in [(0, 1), (-1, 1), (1, 1)]:
        sand_new = (sand[0] + dx, sand[1] + dy)
        if sand_new not in rocks and pour(sand_new, bottomless):
            return True
    rocks.add(sand)
def measure_sand(bottomless):
    n = len(rocks)
    pour(spawn, bottomless)
    return len(rocks) - n
rocks = set()
spawn = (500, 0)
reset()
print('part one:', measure_sand(bottomless=True))

# Part two
reset()
y_floor = 2 + ymax
height = y_floor - spawn[1]
rocks |= {
    (x_floor, y_floor)
    for x_floor in range(spawn[0] - height, spawn[0] + height + 1)
}
print('part two:', measure_sand(bottomless=False))

