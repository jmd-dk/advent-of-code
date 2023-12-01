import collections, heapq

# Part one
with open('input.txt') as f:
    cavern = [list(map(int, line.strip())) for line in f.readlines()]
shape = (len(cavern), len(cavern[0]))
def dijkstra(start, goal, n_tiles=1):
    """Dijkstra's algorithm, terminating at goal"""
    inf = float('inf')
    distances = collections.defaultdict(lambda: inf)
    distances[start] = 0
    cur = start
    unvisited_set = set()
    unvisited_heap = []
    while cur != goal:
        for step in [(1, 0), (-1, 0), (0, 1), (0, -1)]:
            new = (cur[0] + step[0], cur[1] + step[1])
            tile = (new[0]//shape[0], new[1]//shape[1])
            if not (0 <= tile[0] < n_tiles and 0 <= tile[1] < n_tiles):
                continue
            visited = new not in unvisited_set
            if distances[new] < inf and visited:
                continue
            # Unvisited
            risk = cavern[new[0] - tile[0]*shape[0]][new[1] - tile[1]*shape[1]]
            risk += tile[0] + tile[1]
            risk -= 9*(risk//10)
            dist = min(
                distances[new],
                distances[cur] + risk,
            )
            distances[new] = dist
            if visited:
                unvisited_set.add(new)
                heapq.heappush(unvisited_heap, (dist, new))
        # Visit
        cur = heapq.heappop(unvisited_heap)[1]
        unvisited_set.remove(cur)
    return distances[goal]
start = (0, 0)
goal = (shape[0] - 1, shape[1] - 1)
lowest_total_risk = dijkstra(start, goal)
print('part one:', lowest_total_risk)

# Part two
start = (0, 0)
n_tiles = 5
goal = (n_tiles*shape[0] - 1, n_tiles*shape[1] - 1)
lowest_total_risk = dijkstra(start, goal, n_tiles)
print('part two:', lowest_total_risk)

