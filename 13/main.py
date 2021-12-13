import re
import numpy as np

# Part one
dots = []
folds = []
with open('input.txt') as f:
    for line in f:
        if (match := re.search(r'(\d+),(\d+)', line)):
            dots.append(tuple(map(int, match.groups())))
        elif (match := re.search(r'fold along (x|y)=(\d+)', line)):
            dim = 'xy'.index(match.group(1))
            coord = int(match.group(2))
            folds.append((dim, coord))
shape = [max(dot[dim] + 1 for dot in dots) for dim in range(2)]
paper = np.zeros(shape, dtype=bool)
for dot in dots:
    paper[dot] = True
def fold(paper, dim, coord):
    # As we always fold along the middle, the coord is in fact redundant
    assert coord == paper.shape[dim]//2
    slices_upper = (slice(coord),           slice(None))[::1-2*dim]
    slices_lower = (slice(None, coord, -1), slice(None))[::1-2*dim]
    return paper[slices_upper] | paper[slices_lower]
folds_iter = iter(folds)
for dim, coord in folds_iter:
    paper = fold(paper, dim, coord)
    break
print('part one:', paper.sum())

# Part two
def display(paper):
    for j in range(paper.shape[1]):
        for i in range(paper.shape[0]):
            c = '#' if paper[i, j] else '.'
            print(c, end='')
        print()
for dim, coord in folds_iter:
    paper = fold(paper, dim, coord)
display(paper)

