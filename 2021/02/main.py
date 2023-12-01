import collections

# Part one
counter = collections.Counter()
# Read and process data simultaneously
with open('input.txt') as f:
    for line in f:
        direction, amount = line.split()
        amount = int(amount)
        counter[direction] += amount
horizontal = counter['forward'] - counter['backward']
depth = counter['down'] - counter['up']
print('part one:', horizontal*depth)

# Part two
horizontal = 0
depth = 0
aim = 0
sign = {'up': -1, 'down': +1}
with open('input.txt') as f:
    for line in f:
        direction, amount = line.split()
        amount = int(amount)
        if direction == 'forward':
            horizontal += amount
            depth += aim*amount
        else:
            aim += sign[direction]*amount
print('part two:', horizontal*depth)

