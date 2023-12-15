import collections, itertools, math, re, sys

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    nodes = {}
    with open_input() as file:
        turns = ['LR'.index(choice) for choice in file.readline().strip()]
        file.readline()
        for line in file:
            node, left, right = re.findall(r'\w+', line)
            nodes[node] = (left, right)
    return turns, nodes


# Solution to part one
@analyze
def solve_one(turns, nodes):
    turn = lambda gen=itertools.cycle(turns): next(gen)
    node_start, node_stop = 'AAA', 'ZZZ'
    node = node_start
    for step in itertools.count(1):
        if (node := nodes[node][turn()]) == node_stop:
            break
    return step


# Solution to part two
@analyze
def solve_two(turns, nodes):
    def assumption_violated(assumption, violated):
        if not violated:
            return
        if assumption == 0:
            message = 'a given ghost only ever reaches the same stop'
        elif assumption == 1:
            message = (
                'the stop is reached for the second time '
                'after exactly twice the number of steps '
                'it took the first time'
            )
        raise RuntimeError(f'The assumption that {message} is violated')

    def find_cycle_times(node):
        turn = lambda gen=itertools.cycle(turns): next(gen)
        stops = collections.defaultdict(list)
        for step in itertools.count(1):
            if not at_stop(node := nodes[node][turn()]):
                continue
            stops[node].append(step)
            if len(stops[node]) < 2:
                continue
            assumption_violated(0, len(stops) != 1)
            cycle = next(iter(stops.values()))
            assumption_violated(1, cycle[1] != 2 * cycle[0])
            return cycle[0]

    generate_check = lambda c: lambda node: node.endswith(c)
    at_start = generate_check('A')
    at_stop = generate_check('Z')
    nodes_ghosts = [node for node in nodes if at_start(node)]
    times = [find_cycle_times(node) for node in nodes_ghosts]
    return math.lcm(*times)


# Solve
print(solve_one(*read()))
print(solve_two(*read()))
