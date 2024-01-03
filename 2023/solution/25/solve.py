import collections, heapq, re

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
    return graph


# Solution to part one
@analyze
def solve_one(graph):
    def get_path_furthest(component):
        visited = set()
        visit = [(0, component)]
        prev = {}
        while visit:
            dist, component = heapq.heappop(visit)
            if component in visited:
                continue
            dist += 1
            visited.add(component)
            for connection in graph[component]:
                if connection in visited:
                    continue
                heapq.heappush(visit, (dist, connection))
                if connection not in prev:
                    prev[connection] = component
        path = [component]
        while True:
            if (component := prev.get(component)) is None:
                break
            path.append(component)
        return path

    def disconnect():
        counter = collections.Counter()
        for component in graph:
            path = get_path_furthest(component)
            for connection in zip(path, path[1:]):
                counter[frozenset(connection)] += 1
        connection = tuple(counter.most_common(1)[0][0])
        graph[connection[0]].remove(connection[1])
        graph[connection[1]].remove(connection[0])

    def fill():
        visited = set()
        visit = {next(iter(graph))}
        while visit:
            visited.add(component := visit.pop())
            visit |= graph[component] - visited
        return len(visited)

    for _ in range(3):
        disconnect()

    return (n := fill()) * (len(graph) - n)


# Solve
print(solve_one(read()))
