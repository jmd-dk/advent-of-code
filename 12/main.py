import collections

# Part one
caves = set()
graph = collections.defaultdict(list)
with open('input.txt') as f:
    for line in f:
        cave_0, cave_1 = line.strip().split('-')
        caves.add(cave_0)
        caves.add(cave_1)
        graph[cave_0].append(cave_1)
        graph[cave_1].append(cave_0)
def dfs(path, reject):
    for cave in graph[path[-1]]:
        if reject(cave, path):
            continue
        new_path = path.copy()
        new_path.append(cave)
        if cave == 'end':
            yield new_path
        else:
            yield from dfs(new_path, reject)
reject_1 = lambda cave, path: cave.islower() and cave in path
n_paths = sum(1 for path in dfs(['start'], reject_1))
print('part one:', n_paths)

# Part two
paths = set()
for cave_twice in caves:
    if cave_twice.isupper() or cave_twice in {'start', 'end'}:
        continue
    def reject_2(cave, path):
        return reject_1(cave, path) and (cave != cave_twice or path.count(cave) == 2)
    paths |= set(map(tuple, dfs(['start'], reject_2)))
print('part two:', len(paths))

