import collections

# Read in data
with open('input.txt', 'r') as f:
    signal = f.read().strip()

# Part one
def find_marker(signal, width):
    def update(c, diff):
        window[c] += diff
        return (window[c] == 1) - (window[c] == 1 + diff)
    window = collections.Counter(signal[:width-1])
    n_singles = sum(1 for val in window.values() if val == 1)
    for i in range(width, len(signal) + 1):
        n_singles += update(signal[i - 1], +1)
        if n_singles == width:
            return i
        n_singles += update(signal[i - width], -1)
print('part one:', find_marker(signal, 4))

# Part two
print('part two:', find_marker(signal, 14))

