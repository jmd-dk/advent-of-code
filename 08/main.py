# Read in data
grid = []
with open('input.txt', 'r') as f:
    for line in f:
        grid.append(list(map(int, line.rstrip())))
shape = len(grid), len(grid[0])

# Part 1
visible = set()
identity = lambda x: x
for transpose in range(2):
    for transform in (identity, reversed):
        for i in range(shape[transpose]):
            highest = -1
            for j in transform(range(shape[transpose])):
                x, y = i, j
                if transpose:
                    x, y = y, x
                height = grid[x][y]
                if height > highest:
                    visible.add((x, y))
                    highest = height
print('part one:', len(visible))

# Part 2
def get_score(i, j):
    score = 1
    height = grid[i][j]
    for x in range(i + 1, shape[0]):
        if grid[x][j] >= height:
            break
    score *= x - i
    for x in range(i - 1, -1, -1):
        if grid[x][j] >= height:
            break
    score *= i - x
    for y in range(j + 1, shape[1]):
        if grid[i][y] >= height:
            break
    score *= y - j
    for y in range(j - 1, -1, -1):
        if grid[i][y] >= height:
            break
    score *= j - y
    return score
highest_score = max(
    get_score(i, j)
    for i in range(1, shape[0] - 1)
    for j in range(1, shape[1] - 1)
)
print('part two:', highest_score)


