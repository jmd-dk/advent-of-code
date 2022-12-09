import re

# Read in data
steps = []
with open('input.txt', 'r') as f:
    for line in f:
        match = re.search(r'(.) (\d+)', line)
        direction = match.group(1)
        amount = int(match.group(2))
        step = {
            'L': (-amount,       0),
            'R': (+amount,       0),
            'D': (      0, -amount),
            'U': (      0, +amount),
        }[direction]
        steps.append(step)

# Part 1
def move(steps, n_knots=2):
    knots = tuple([0, 0] for i in range(n_knots))
    head, tail = knots[0], knots[-1]
    tail_visited = {tuple(tail)}
    for step in steps:
        n = abs(step[0]) + abs(step[1])
        dim_head = int(step[1] != 0)
        sign_head = 2*(step[dim_head] > 0) - 1
        for _ in range(n):
            head[dim_head] += sign_head
            for knot0, knot1 in zip(knots, knots[1:]):
                moves = [(abs(knot0[dim] - knot1[dim]) == 2) for dim in range(2)]
                if not any(moves):
                    break
                if all(moves):
                    for dim in range(2):
                        knot1[dim] += (knot0[dim] - knot1[dim])//2
                    continue
                dim = moves.index(True)
                offset = (knot1[dim] - knot0[dim])//2
                knot1[:] = knot0
                knot1[dim] += offset
            tail_visited.add(tuple(tail))
    return len(tail_visited)
print('part one:', move(steps))

# Part two
print('part two:', move(steps, 10))

