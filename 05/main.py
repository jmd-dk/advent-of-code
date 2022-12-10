import collections, copy, re

# Read in data
stacks = None
Rearrangement = collections.namedtuple(
    'Rearrangement', ('num', 'src', 'dst'),
)
procedure = []
with open('input.txt') as f:
    # Read in stacks of crates
    for line in f:
        if stacks is None:
            stacks = [[] for _ in range(len(line)//4)]
        if not '[' in line:
            break
        for i, content in enumerate(line[1::4]):
            if content.isalpha():
                stacks[i].append(content)
    for stack in stacks:
        stack.reverse()
    # Read in rearrangements
    f.readline()
    for line in f:
        nums = list(map(int, re.search(r'(\d+).+(\d+).+(\d+)', line).groups()))
        for i in range(1, 3):
            nums[i] -= 1
        procedure.append(Rearrangement(*nums))
stacks_ori = stacks
reset_stacks = lambda: copy.deepcopy(stacks_ori)

# Part one
stacks = reset_stacks()
for r in procedure:
    for _ in range(r.num):
        stacks[r.dst].append(stacks[r.src].pop())
get_top = lambda stacks: ''.join(stack[-1] for stack in stacks)
print('part one:', get_top(stacks))

# Part two
stacks = reset_stacks()
for r in procedure:
    stacks[r.dst] += stacks[r.src][ -r.num:]
    stacks[r.src]  = stacks[r.src][:-r.num ]
print('part two:', get_top(stacks))

