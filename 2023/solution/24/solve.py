import collections, fractions, operator, random, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    Hailstone = collections.namedtuple(
        'Hailstone', ('px', 'py', 'pz', 'vx', 'vy', 'vz')
    )
    Hailstone.p = property(lambda self: self[:3])
    Hailstone.v = property(lambda self: self[3:])
    hailstones = []
    limits = (200_000_000_000_000, 400_000_000_000_000)
    with open_input() as file:
        for line in file:
            numbers = list(map(int, re.findall(r'-?\d+', line)))
            if len(numbers) == 6:
                hailstones.append(Hailstone(*numbers))
            elif len(numbers) == 2:
                limits = tuple(numbers)
    return hailstones, limits


# Solution to part one
@analyze
def solve_one(hailstones, limits):
    count = 0
    for i, hail_i in enumerate(hailstones):
        for j, hail_j in enumerate(hailstones[i + 1 :], i + 1):
            dx = hail_i.px - hail_j.px
            dy = hail_i.py - hail_j.py
            if (denominator := hail_i.vy * hail_j.vx - hail_i.vx * hail_j.vy) == 0:
                continue
            numerator = hail_j.vy * dx - hail_j.vx * dy
            if (t_i := fractions.Fraction(numerator, denominator)) <= 0:
                continue
            numerator = hail_i.vy * dx - hail_i.vx * dy
            if (t_j := fractions.Fraction(numerator, denominator)) <= 0:
                continue
            if not (limits[0] <= (x := hail_i.px + t_i * hail_i.vx) <= limits[1]):
                continue
            if not (limits[0] <= (y := hail_i.py + t_i * hail_i.vy) <= limits[1]):
                continue
            count += 1
    return count


# Solution to part two
@analyze
def solve_two(hailstones, limits):
    """Collision between the rock and a hailstone at time t:
        P + t*V = p + t*v
    where P = (P[0], P[1], P[2]) and V = (V[0], V[1], V[2]) are the
    initial position and velocity of the rock and p = (p[0], p[1], p[2])
    and v = (v[0], v[1], v[2]) are the initial position and velocity
    for the hailstone. This gives three scalar equations for t.
    Equating the first and second equation for t gives
        P[1]*V[0] - P[0]*V[1] = p[0]*(v[1] - V[1]) - p[1]*(v[0] - V[0]) - P[0]*v[1] + P[1]*v[0]
    which is independent on the chosen hailstone as evident
    from the left-hand-side. Equating two copies of the right-hand side
    for two hailstones i and j yields
        + P[0]*(v_j[1] - v_i[1])
        - P[1]*(v_j[0] - v_i[0])
        + V[1]*(p_j[0] - p_i[0])
        - V[0]*(p_j[1] - p_i[1])
        =
        - p_i[0]*v_i[1]
        + p_i[1]*v_i[0]
        + p_j[0]*v_j[1]
        - p_j[1]*v_j[0]
    From symmetry we get corresponding equations for ([1], [2]) and ([2], [0]).
    These are 3 linear equations in 6 unknowns. Adding a third hailstone, k,
    we can extend the system of equations with three similar equations from
    e.g. (j, k). The system of 6 linear equations in 6 unknown can then be
    solved using Gaussian elimination.
    """

    def solve(*indices):
        if len(set(indices)) != len(indices):
            return

        def construct_system(hails):
            matrix = []
            b = []
            for i in range(2):
                j = i + 1
                hail_i, hail_j = hails[i], hails[j]
                b += list(
                    map(
                        operator.sub,
                        cross(hail_i.p, hail_i.v),
                        cross(hail_j.p, hail_j.v),
                    )
                )[::-1]
                row_v = [+(hail_i.v[1] - hail_j.v[1]), -(hail_i.v[0] - hail_j.v[0]), 0]
                row_p = [-(hail_i.p[1] - hail_j.p[1]), +(hail_i.p[0] - hail_j.p[0]), 0]
                matrix.append(row_v + row_p)
                row_v = [-(hail_i.v[2] - hail_j.v[2]), 0, +(hail_i.v[0] - hail_j.v[0])]
                row_p = [+(hail_i.p[2] - hail_j.p[2]), 0, -(hail_i.p[0] - hail_j.p[0])]
                matrix.append(row_v + row_p)
                row_v = [0, +(hail_i.v[2] - hail_j.v[2]), -(hail_i.v[1] - hail_j.v[1])]
                row_p = [0, -(hail_i.p[2] - hail_j.p[2]), +(hail_i.p[1] - hail_j.p[1])]
                matrix.append(row_v + row_p)
            augment(matrix, b)
            return matrix

        def cross(v_i, v_j):
            return [
                v_i[1] * v_j[2] - v_i[2] * v_j[1],
                v_i[2] * v_j[0] - v_i[0] * v_j[2],
                v_i[0] * v_j[1] - v_i[1] * v_j[0],
            ]

        def augment(matrix, b):
            for row, el in zip(matrix, b):
                row.append(el)

        def gauss(matrix):
            n = len(matrix)
            for i in range(n):
                i_pivot = i
                pivot = matrix[i_pivot][i]
                for j in range(i + 1, n):
                    if abs(matrix[j][i]) > pivot:
                        pivot = matrix[j][i]
                        i_pivot = j
                if matrix[i][i_pivot] == 0:
                    return True
                for j in range(n + 1):
                    matrix[i][j], matrix[i_pivot][j] = matrix[i_pivot][j], matrix[i][j]
                for j in range(i + 1, n):
                    ratio = matrix[j][i] / matrix[i][i]
                    for k in range(i + 1, n + 1):
                        matrix[j][k] -= matrix[i][k] * ratio
                    matrix[j][i] = 0

        def back_substitute(matrix):
            n = len(matrix)
            x = [None for _ in range(n)]
            for i in range(n - 1, -1, -1):
                x[i] = (
                    matrix[i][n] - sum(matrix[i][j] * x[j] for j in range(i + 1, n))
                ) / matrix[i][i]
            return x

        hails = [hailstones[index] for index in indices]
        matrix = construct_system(hails)
        for row in matrix:
            for i, el in enumerate(row):
                row[i] = fractions.Fraction(el)
        if gauss(matrix):
            return
        return type(hails[0])(*back_substitute((matrix)))

    n_hail = 3
    random.seed('aoc')
    select = lambda: (random.randrange(len(hailstones)) for _ in range(n_hail))
    while (solution := solve(*select())) is None:
        pass
    return sum(solution.p)


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
