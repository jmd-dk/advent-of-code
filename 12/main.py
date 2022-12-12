import collections

# Read in data
grid = []
c2n = lambda c: ord({'S': 'a', 'E': 'z'}.get(c, c)) - ord('a')
start = end = None
with open('input.txt', 'r') as f:
    for i, line in enumerate(f):
        grid.append(list(map(c2n, line.rstrip())))
        if start is None and 'S' in line:
            start = (i, line.index('S'))
        if end is None and 'E' in line:
            end = (i, line.index('E'))
height = len(grid)
width = len(grid[0])

# Part one
def lookaround(x, y, z, step, upwards):
    sign = 1 - 2*upwards
    for i, j in ((-1, 0), (+1, 0), (0, -1), (0, +1)):
        if not (0 <= x + i < height) or not (0 <= y + j < width):
            continue
        neighbour = (x +  i, y + j)
        if neighbour in visited:
            continue
        if sign*grid[neighbour[0]][neighbour[1]] < sign*z - 1:
            continue
        neighbours.append((neighbour, step))
        visited.add(neighbour)
visited = set()
neighbours = collections.deque()
def init(start, upwards):
    visited.clear()
    neighbours.clear()
    visited.add(start)
    z = grid[start[0]][start[1]]
    lookaround(*start, z, 1, upwards)
    return start, 0
def bfs(start, end):
    z_end = None
    if isinstance(end, int):
        z_end, end = end, None
    z = grid[start[0]][start[1]]
    if start == end or z == z_end:
        return 0
    if z_end is None:
        upwards = (grid[start[0]][start[1]] < grid[end[0]][end[1]])
    else:
        upwards = (z < z_end)
    neighbour, step = init(start, upwards)
    while neighbours:
        neighbour, step = neighbours.popleft()
        z_neighbour = grid[neighbour[0]][neighbour[1]]
        if neighbour == end or z_neighbour == z_end:
            return step
        lookaround(*neighbour, z_neighbour, step + 1, upwards)
    return -1
print('part one:', bfs(start, end))

# Part two
print('part two:', bfs(end, 0))

