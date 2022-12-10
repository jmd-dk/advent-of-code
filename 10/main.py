import re

# Read in data
instructions = []
with open('input.txt', 'r') as f:
    for line in f:
        opcode, n = re.search(r'(\S+) ?(-?\d+)?', line).groups()
        if n is not None:
            n = int(n)
        instructions.append((opcode, n))
def execute(instructions, readout_times):
    def check():
        nonlocal readout
        if cycle != readout:
            return
        yield x
        readout = next(readout_times)
    x = cycle = 1
    readout_times = iter(readout_times)
    readout = next(readout_times)
    for opcode, n in instructions:
        yield from check()
        if opcode == 'noop':
            cycle += 1
            continue
        cycle += 1
        yield from check()
        cycle += 1
        x += n
# Part one
def get_signal_strength(instructions, readout_times):
    return sum(
        cycle*x
        for cycle, x in zip(readout_times, execute(instructions, readout_times))
    )
readout_times = range(20, 260, 40)
print('part one:', get_signal_strength(instructions, readout_times))

# Part two
def draw(width, height):
    screen = []
    values = execute(instructions, range(1, 1 + width*height))
    for row in range(height):
        screen.append('\n')
        for col in range(width):
            x = next(values)
            screen.append('#.'[abs(col - x) > 1])
    return ''.join(screen[1:])
print('part two:', draw(40, 6), sep='\n')

