# Read data
def compartmentalize(s):
    s = s.strip()
    return s[:len(s)//2], s[len(s)//2:]
with open('input.txt', 'r') as f:
    rucksacks = list(map(compartmentalize, f))

# Part one
def get_common_item(containers):
    items = None
    for container in containers:
        items = (set(container) if items is None else items & set(container))
    return items.pop()
def get_priority(item):
    return 1 + ord(item.lower()) - ord('a') + 26*item.isupper()
priority_sum = 0
for rucksack in rucksacks:
    priority_sum += get_priority(get_common_item(rucksack))
print('part one:', priority_sum)

# Part two
priority_sum = 0
groupsize = 3
for group in zip(*(rucksacks[i::groupsize] for i in range(groupsize))):
    priority_sum += get_priority(get_common_item(map(''.join, group)))
print('part two:', priority_sum)

