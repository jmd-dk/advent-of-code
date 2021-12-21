import collections, itertools, re

# Part one
positions = {}
with open('input.txt') as f:
    for _ in range(2):
        player, start = map(int, re.search(r'(\d+).+(\d+)', f.readline()).groups())
        positions[player] = start
players = tuple(positions.keys())
scores = collections.defaultdict(int)
die = itertools.cycle(range(1, 100 + 1))
def play():
    throws = 0
    while True:
        for player, position in positions.items():
            for throw in range(3):
                position += next(die)
            position = (position - 1)%10 + 1
            positions[player] = position
            scores[player] += position
            throws += 3
            if scores[player] >= 1000:
                return throws
throws = play()
print('part one:', min(scores.values())*throws)

