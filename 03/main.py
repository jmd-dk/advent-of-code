import functools
import numpy as np

# Part one
fname = 'input.txt'
with open(fname) as f:
    row = f.readline().strip()
n = len(row)
int2 = functools.partial(int, base=2)
data = np.loadtxt(
    fname,
    dtype=int,
    converters={0: int2},
)
gamma = []
for col in range(n - 1, -1, -1):
    ones = sum(1 for i in data if i & (1 << col))
    zeros = len(data) - ones
    gamma.append(1 if ones > zeros else 0)
epsilon = [2 + ~i for i in gamma]
to_dec = lambda gamma: int2(''.join(map(str, gamma)))
gamma = to_dec(gamma)
epsilon = to_dec(epsilon)
print('part one:', gamma*epsilon)

# Part two
def get_rating(data, extreme):
    data_trimmed = data
    for col in range(n - 1, -1, -1):
        ones  = [i for i in data_trimmed if     i & (1 << col)]
        zeros = [i for i in data_trimmed if not i & (1 << col)]
        data_trimmed = extreme([(len(ones) + .5, ones), (len(zeros), zeros)])[1]
        if len(data_trimmed) == 1:
            return data_trimmed[0]
oxygen_generator_rating = get_rating(data, max)
co2_scrubber_rating     = get_rating(data, min)
print('part two:', oxygen_generator_rating*co2_scrubber_rating)

