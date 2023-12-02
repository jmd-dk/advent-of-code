import collections, math, re

# Summon magic
(
    lambda: __import__('sys').path.append(
        str(__import__('pathlib').Path(__file__).parent.parent)
    )
)()
from magic import *


# Reading in data
@memoize
def read():
    games = collections.defaultdict(list)
    with open_input_file() as file:
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


# Initial conditions
@memoize
def get_initial(part=None):
    return (bag := {'red': 12, 'green': 13, 'blue': 14})


# Solution to part one
@analyze
def solve_one(games, bag):
    sum_id = 0
    for game_id, game in games.items():
        for subset in game:
            if any(n > bag[gem] for gem, n in subset.items()):
                break
        else:
            sum_id += game_id
    return sum_id


# Solution to part two
@analyze
def solve_two(games, bag):
    sum_power = 0
    for game_id, game in games.items():
        minimum = collections.Counter()
        for subset in game:
            for gem, n in subset.items():
                minimum[gem] = max(minimum[gem], n)
        sum_power += math.prod(minimum.values())
    return sum_power


# Solve
print(solve_one(read(), get_initial()))
print(solve_two(read(), get_initial()))
