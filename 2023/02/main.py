import pathlib, sys
sys.path.append(str(pathlib.Path(__file__).parent.parent))
from helper import get_input_file

import collections, math, re


# Read data
def read():
    games = collections.defaultdict(list)
    with open(get_input_file()) as file:
        for line in file:
            game_id = int(re.search(r'\d+', line)[0])
            for many in line.partition(':')[-1].split(';'):
                subset = collections.Counter()
                for single in many.split(','):
                    n, gem = single.strip().split()
                    n = int(n)
                    subset[gem] += n
                games[game_id].append(subset)
    return games
games = read()


# Part one
def solve1(games, bag):
    sum_id = 0
    for game_id, game in games.items():
        for subset in game:
            if any(n > bag[gem] for gem, n in subset.items()):
                break
        else:
            sum_id += game_id
    return sum_id
bag = {'red': 12, 'green': 13, 'blue': 14}
print('part one:', solve1(games, bag))


# Part two
def solve2(games, bag):
    sum_power = 0
    for game_id, game in games.items():
        minimum = collections.Counter()
        for subset in game:
            for gem, n in subset.items():
                minimum[gem] = max(minimum[gem], n)
        sum_power += math.prod(minimum.values())
    return sum_power
print('part two:', solve2(games, bag))

