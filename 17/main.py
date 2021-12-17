import itertools, re
import numpy as np

# Part one
with open('input.txt') as f:
    num = r'(-?\d+)'
    x_min, x_max, y_min, y_max = map(
        int,
        re.search(
            rf'x={num}\.\.{num}.*y={num}\.\.{num}',
            f.read(),
        ).groups(),
    )
x_target = np.array((x_min, x_max))
y_target = np.array((y_min, y_max))
def in_target(pos):
    return in_target_x(pos) and in_target_y(pos)
def in_target_x(pos):
    return x_target[0] <= pos[0] <= x_target[1]
def in_target_y(pos):
    return y_target[0] <= pos[1] <= y_target[1]
def above_target(pos):
    return in_target_x(pos) and pos[1] > y_target[0]
def shoot(pos, vel):
    step = 0
    path = [tuple(pos)]
    while True:
        step += 1
        pos += vel                 # kinematics
        vel[0] -= np.sign(vel[0])  # drag
        vel[1] -= 1                # gravity
        path.append(tuple(pos))
        if in_target(pos):
            return True, path, 'hit'
        if pos[0] > x_target[1] and (pos[1] > y_target[1] or step == 1):
            return False, path, 'too fast'
        if vel[0] == 0 and not above_target(pos):
            return False, path, 'too slow'
def shootalot(pos_ini, vy_gen):
    """This solution assumes that the target is
    to the right and below the initial position.
    """
    fac = 2  # somewhat arbitrary
    n_consec_fails_max = fac*(x_target[0] - pos_ini[0])
    n_consec_fails = 0
    paths = {}
    pos = np.empty(2, dtype=int)
    vel = np.empty(2, dtype=int)
    any_hits = False
    for vy in vy_gen:
        for vx in itertools.count(1):
            # Initial state
            vel[0] = vx
            vel[1] = vy
            pos[:] = pos_ini
            # Fire
            hit, path, reason = shoot(pos, vel)
            any_hits |= hit
            if hit:
                paths[vx, vy] = path
                n_consec_fails = 0
                continue
            if reason == 'too fast':
                n_consec_fails += 1
                break
        else:
            n_consec_fails += 1
        if any_hits and n_consec_fails == n_consec_fails_max:
            break
    return paths
pos_ini = np.array((0, 0))
paths  = shootalot(pos_ini, itertools.count())
paths |= shootalot(pos_ini, itertools.count(-1, -1))
print('part one:', max(pos[1] for path in paths.values() for pos in path))

# Part two
print('part two:', len(paths))

