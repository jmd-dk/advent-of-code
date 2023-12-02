import collections, math

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parent.parent))
from magic import *


# Reading in data
@memoize
def read():
    games = collections.defaultdict(list)
    with open_input_file() as file:
        for line in file:
            heading, _, rounds = line.partition(':')
            game_id = int(heading.partition(' ')[-1])
            for many in rounds.split(';'):
                subset = collections.Counter()
                for single in many.split(','):
                    n, gem = single.strip().split()
                    subset[gem] += int(n)
                games[game_id].append(subset)
    return games


# Initial conditions
@memoize
def get_initial(part=None):
    return (bag := {'red': 12, 'green': 13, 'blue': 14})


# Solution to part one
@analyze
def solve_one(games, bag):
    return sum(
        game_id
        for game_id, game in games.items()
        if all(n <= bag[gem] for subset in game for gem, n in subset.items())
    )


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
