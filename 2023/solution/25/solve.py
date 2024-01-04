import collections, math, random, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    graph = collections.defaultdict(set)
    with open_input() as file:
        for line in file:
            component, *connections = re.findall(r'\w+', line)
            graph[component] |= set(connections)
            for connection in connections:
                graph[connection].add(component)
    graph = {
        component: sorted(connections)  # sort for determinism
        for component, connections in graph.items()
    }
    return graph


# Solution to part one
@analyze
def solve_one(graph):
    def karger(graph):
        def contract():
            while True:
                index = random.randrange(0, len(labels))
                j = labels[index]
                components_j = groups[j]
                if not (connections := graph[random.choice(components_j)]):
                    continue
                component_i = random.choice(connections)
                i = labellings[component_i]
                if i != j:
                    break
            labels[index] = labels[-1]
            labels.pop()
            group_i = groups[i]
            group_j = groups.pop(j)
            for component_j in components_j:
                labellings[component_j] = i
            cut(group_i, j)
            cut(group_j, i)
            group_i += components_j

        def cut(group, j):
            for component in group:
                connections = set(graph[component])
                for connection in graph[component]:
                    if labellings[connection] == j:
                        connections.remove(connection)
                graph[component] = sorted(connections)  # sort for determinism

        groups = {}
        labellings = {}
        for i, component in enumerate(graph):
            groups[i] = [component]
            labellings[component] = i
        labels = list(groups)
        while len(groups) > 2:
            contract()
        return graph, groups, labellings

    def is_mistaken(graph, groups, labellings, ncut):
        for i, group in groups.items():
            connections_ij = set()
            for component in group:
                for connection in graph[component]:
                    if labellings[connection] != i:
                        connections_ij.add(connection)
            if len(connections_ij) != ncut:
                return True

    random.seed('aoc')
    ncut = 3
    while True:
        graph_reduced, groups, labellings = karger(graph.copy())
        if not is_mistaken(graph_reduced, groups, labellings, ncut):
            break
    return math.prod(map(len, groups.values()))


# Solve
print(solve_one(read()))
