import collections, itertools, re

# Part one
positions = {}
with open('input.txt') as f:
    for _ in range(2):
        player, start = map(int, re.search(r'(\d+).+(\d+)', f.readline()).groups())
        positions[player] = start
def play_deterministic(positions):
    positions = positions.copy()
    die = itertools.cycle(range(1, 100 + 1))
    scores = collections.defaultdict(int)
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
                return scores, throws
scores, throws = play_deterministic(positions)
print('part one:', min(scores.values())*throws)

# Part two
def play_dirac(position_1, position_2, score_1=0, score_2=0, freq=1, player=1, wins=None):
    positions = [position_1, position_2]
    scores    = [score_1,    score_2]
    if wins is None:
        wins = [0, 0]
    position = positions[player - 1]
    score    = scores   [player - 1]
    player_other = player%2 + 1
    for n, freq_new in die_dirac.items():
        position_new = (position + n - 1)%10 + 1
        score_new = score + position_new
        if score_new >= 21:
            wins[player - 1] += freq*freq_new
        else:
            positions[player - 1] = position_new
            scores   [player - 1] = score_new
            play_dirac(*positions, *scores, freq*freq_new, player_other, wins)
    return wins
die_dirac = collections.Counter(
    map(sum, itertools.product(range(1, 3 + 1), repeat=3))
)
wins = play_dirac(positions[1], positions[2])
print('part two:', max(wins))

