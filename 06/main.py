import collections

# Read in data
with open('input.txt', 'r') as f:
    signal = f.read().strip()

# Part 1
def find_marker(signal, width):
    window = collections.Counter(signal[:width-1])
    for i in range(width, len(signal) + 1):
        window[signal[i - 1]] += 1
        if window.most_common(1)[0][1] == 1:
            return i
        window[signal[i - width]] -= 1
print('part one:', find_marker(signal, 4))

# Part 2
print('part two:', find_marker(signal, 14))

