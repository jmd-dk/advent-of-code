# Load data
with open('input.txt', 'r') as f:
    data = f.read()

# Part one
calories = [sum(map(int, elf.strip().split('\n'))) for elf in data.split('\n'*2)]
print('part one:', max(calories))

# Part two
import heapq
print('part two:', sum(heapq.nlargest(3, calories)))

