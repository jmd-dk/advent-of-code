import itertools, functools, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    galaxies = []
    with open_input_file() as file:
        for i, line in enumerate(file):
            for match in re.finditer(r'#', line):
                j = match.start()
                galaxies.append((i, j))
    return galaxies


# Solution to part one
@analyze
def solve_one(galaxies):
    def get_empty(row_min, row_max, dim):
        empty = []
        for row in range(row_min + 1, row_max):
            for galaxy in galaxies:
                if galaxy[dim] == row:
                    break
            else:
                empty.append(row)
        return empty

    def get_dist(galaxy0, galaxy1):
        i0, i1 = extract_ordered(galaxy0, galaxy1, 0)
        j0, j1 = extract_ordered(galaxy0, galaxy1, 1)
        dist = (i1 - i0) + (j1 - j0)
        dist += sum(i0 < row < i1 for row in rows_empty)
        dist += sum(j0 < col < j1 for col in cols_empty)
        return dist

    def extract_ordered(galaxy0, galaxy1, dim):
        i0, i1 = galaxy0[dim], galaxy1[dim]
        if i0 > i1:
            i0, i1 = i1, i0
        return i0, i1

    find_extremum = lambda ext, dim: ext(galaxy[dim] for galaxy in galaxies)
    row_min = find_extremum(min, 0)
    row_max = find_extremum(max, 0)
    col_min = find_extremum(min, 1)
    col_max = find_extremum(max, 1)
    rows_empty = get_empty(row_min, row_max, 0)
    cols_empty = get_empty(col_min, col_max, 1)
    return sum(
        get_dist(galaxy0, galaxy1)
        for index, galaxy0 in enumerate(galaxies)
        for galaxy1 in galaxies[index + 1 :]
    )


# Solution to part two
@analyze
def solve_two(galaxies):
    def get_empty(row_min, row_max, dim):
        empty = []
        for row in range(row_min + 1, row_max):
            for galaxy in galaxies:
                if galaxy[dim] == row:
                    break
            else:
                empty.append(row)
        return empty

    def get_dist(galaxy0, galaxy1, expansion_factor=1_000_000):
        i0, i1 = extract_ordered(galaxy0, galaxy1, 0)
        j0, j1 = extract_ordered(galaxy0, galaxy1, 1)
        dist = (i1 - i0) + (j1 - j0)
        expansion = 0
        expansion += sum(i0 < row < i1 for row in rows_empty)
        expansion += sum(j0 < col < j1 for col in cols_empty)
        return dist + (expansion_factor - 1) * expansion

    def extract_ordered(galaxy0, galaxy1, dim):
        i0, i1 = galaxy0[dim], galaxy1[dim]
        if i0 > i1:
            i0, i1 = i1, i0
        return i0, i1

    find_extremum = lambda ext, dim: ext(galaxy[dim] for galaxy in galaxies)
    row_min = find_extremum(min, 0)
    row_max = find_extremum(max, 0)
    col_min = find_extremum(min, 1)
    col_max = find_extremum(max, 1)
    rows_empty = get_empty(row_min, row_max, 0)
    cols_empty = get_empty(col_min, col_max, 1)
    return sum(
        get_dist(galaxy0, galaxy1)
        for index, galaxy0 in enumerate(galaxies)
        for galaxy1 in galaxies[index + 1 :]
    )


# Solve
print(solve_one(read()))
print(solve_two(read()))
