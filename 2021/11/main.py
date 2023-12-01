import itertools
import numpy as np

# Part one
with open('input.txt') as f:
    octopi_ini = np.array([[int(octo) for octo in line.strip()] for line in f])
def flash(octopi, n_flashes_prev=0):
    flashes = (octopi > 9)
    n_flashes = flashes.sum()
    if not n_flashes:
        octopi[octopi < 0] = 0
        return n_flashes_prev
    for i, slice_i in enumerate(slices):
        slice_i_rev = slices[2*i%3]
        for j, slice_j in enumerate(slices):
            if i == 0 == j:
                continue
            slice_j_rev = slices[2*j%3]
            octopi[slice_i, slice_j] += flashes[slice_i_rev, slice_j_rev]
    octopi[flashes] = -8  # any number below -7 is safe
    return flash(octopi, n_flashes_prev + n_flashes)
slices = [
    slice(None),     # all
    slice(1, None),  # right
    slice(0, -1),    # left
]
def take_step(octopi):
    octopi += 1
    return flash(octopi)
octopi = octopi_ini.copy()
n_steps = 100
n_flashes = 0
for step in range(1, n_steps + 1):
    n_flashes += take_step(octopi)
print('part one:', n_flashes)

# Part two
octopi = octopi_ini.copy()
for step in itertools.count(1):
    if take_step(octopi) == octopi.size:
        break
print('part two:', step)

