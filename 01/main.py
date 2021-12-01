import numpy as np

# Load data
data = np.loadtxt('input.txt', dtype=int)

# Part one
def measure_increases(data):
    return np.sum(np.diff(data) > 0)
print('part one:', measure_increases(data))

# Part two
length_window = 3
data_processed = np.convolve(data, np.ones(length_window), mode='valid')
print('part two:', measure_increases(data_processed))

