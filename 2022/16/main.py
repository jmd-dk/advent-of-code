import collections, itertools, re

# Read in data
class Valve:
    valves = {}
    def __init__(self, name, rate, connections):
        self.name = name
        self.rate = rate
        self.connections = connections
        self.distances = {}
        self.valves[name] = self
    def __repr__(self):
        return self.name
    @classmethod
    def connect(cls):
        for valve in cls.valves.values():
            valve.connections = [
                cls.valves[connection]
                for connection in valve.connections
            ]
    @classmethod
    def complete_network(cls, valve_start):
        def bfs(valve_bfs):
            def append_visit(valves, length=0):
                for valve in valves:
                    if valve not in visited:
                        visit.append((valve, length + 1))
            visited = set()
            visit = collections.deque()
            append_visit(valve_bfs.connections)
            while visit:
                valve_target, length = visit.popleft()
                if valve_target in visited:
                    continue
                visited.add(valve_target)
                if valve_target.rate > 0:
                    valve_bfs.distances[valve_target] = length
                append_visit(valve_target.connections, length)
        if isinstance(valve_start, str):
            valve_start = cls.valves[valve_start]
        for valve in cls.valves.values():
            if valve.rate > 0 or valve is valve_start:
                bfs(valve)
with open('input.txt') as f:
    for line in f:
        match = re.search(r'Valve (..).+rate=(\d+).+valves? (.+)', line)
        name = match.group(1)
        rate = int(match.group(2))
        connections = match.group(3).split(', ')
        Valve(name, rate, connections)

# Part one
Valve.connect()
valve_start = Valve.valves['AA']
Valve.complete_network(valve_start)
def solve(valve_start, time_tot, workers=1):
    def _solve(valve, opened, time, workers):
        if time <= 0:
            if workers > 1:
                return _solve(valve_start, opened, time_tot, workers - 1)
            return 0
        key = (valve, opened, time, workers)
        loot = cache.get(key)
        if loot is not None:
            return loot
        pressure = 0
        # Deliberately quit working early
        if workers > 1:
            pressure = _solve(valve, opened, 0, workers)
        # Go to next valve
        for valve_next, distance in valve.distances.items():
            if valve_next not in opened:
                pressure = max(
                    pressure,
                    _solve(
                        valve_next, opened | {valve_next}, time - distance - 1, workers,
                    ),
                )
        pressure += valve.rate*time
        cache[key] = pressure
        return pressure
    cache = {}
    return _solve(valve_start, frozenset(), time_tot, workers)
print('part one:', solve(valve_start, 30))

# Part two
print('part two:', solve(valve_start, 26, 2))

