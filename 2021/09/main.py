import numpy as np

# Part one
big = 10
with open('input.txt') as f:
    heightmap = np.pad([list(map(int, line.strip())) for line in f], 1, constant_values=big)
minmask = np.ones(heightmap.shape, dtype=bool)
for dim in range(2):
    for dir in (-1, +1):
        minmask &= heightmap < np.roll(heightmap, dir, axis=dim)
minima = heightmap[minmask]
risk_levels = 1 + minima
print('part one:', sum(risk_levels))

# Part two
boundary = 9
basins = -np.ones(minmask.shape, dtype=int)
for n, (i, j) in enumerate(zip(*np.nonzero(minmask))):
    basins[i, j] = n
def permut(*coords, dim=None):
    # General
    # return np.roll(coords, dim)
    # Fast (2D only)
    return coords[::2*dim-1]
def grow():
    grown = False
    for i in range(basins.shape[0]):
        for j in range(basins.shape[1]):
            n = basins[i, j]
            if n == -1:
                continue
            for dim in range(2):
                i2, j2 = permut(i, j, dim=dim)
                for interval in (
                    range(i2 + 1, basins.shape[dim]),
                    range(i2 - 1, -1, -1),
                ):
                    for i3 in interval:
                        i3, j3 = permut(i3, j2, dim=dim)
                        if heightmap[i3, j3] >= boundary:
                            break
                        if basins[i3, j3] == n:
                            continue
                        grown = True
                        basins[i3, j3] = n
    return grown
while grow():
    pass
basins_sizes = [np.sum(basins == n) for n in range(minima.size)]
print('part two:', np.prod(sorted(basins_sizes)[-3:]))

