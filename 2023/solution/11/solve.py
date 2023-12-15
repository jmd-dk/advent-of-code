import functools, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    factor = None
    galaxies = []
    with open_input() as file:
        for i, line in enumerate(file):
            for match in re.finditer(r'#', line):
                j = match.start()
                galaxies.append((i, j))
            if match := re.match(r'\d+', line):
                factor = int(match[0])
    return galaxies, factor


# Solution to part one
@analyze
def solve_one(galaxies, factor):
    @functools.cache
    def get_empty(dim):
        find_extremum = lambda ext, dim: ext(galaxy[dim] for galaxy in galaxies)
        row_min = find_extremum(min, dim)
        row_max = find_extremum(max, dim)
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
        rows_empty = get_empty(0)
        cols_empty = get_empty(1)
        dist += sum(i0 < row < i1 for row in rows_empty)
        dist += sum(j0 < col < j1 for col in cols_empty)
        return dist

    def extract_ordered(galaxy0, galaxy1, dim):
        i0, i1 = galaxy0[dim], galaxy1[dim]
        if i0 > i1:
            i0, i1 = i1, i0
        return i0, i1

    return sum(
        get_dist(galaxy0, galaxy1)
        for index, galaxy0 in enumerate(galaxies)
        for galaxy1 in galaxies[index + 1 :]
    )


# Solution to part two
@analyze
def solve_two(galaxies, factor):
    if factor is None:
        factor = 1_000_000

    @functools.cache
    def get_empty(dim):
        find_extremum = lambda ext, dim: ext(galaxy[dim] for galaxy in galaxies)
        row_min = find_extremum(min, dim)
        row_max = find_extremum(max, dim)
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
        rows_empty = get_empty(0)
        cols_empty = get_empty(1)
        expansion = 0
        expansion += sum(i0 < row < i1 for row in rows_empty)
        expansion += sum(j0 < col < j1 for col in cols_empty)
        return dist + (factor - 1) * expansion

    def extract_ordered(galaxy0, galaxy1, dim):
        i0, i1 = galaxy0[dim], galaxy1[dim]
        if i0 > i1:
            i0, i1 = i1, i0
        return i0, i1

    return sum(
        get_dist(galaxy0, galaxy1)
        for index, galaxy0 in enumerate(galaxies)
        for galaxy1 in galaxies[index + 1 :]
    )


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
