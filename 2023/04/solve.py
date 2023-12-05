import collections, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parent.parent))
from magic import *


# Reading in data
@memoize
def read():
    cards = {}
    with open_input_file() as file:
        for line in file:
            heading, numbers = line.split(':')
            card_id = int(re.search(r'\d+', heading)[0])
            cards[card_id] = tuple(
                set(map(int, re.findall('\d+', nums))) for nums in numbers.split('|')
            )
    return cards


# Solution to part one
@analyze
def solve_one(cards):
    return sum(
        2 ** (overlap - 1)
        for winning, mine in cards.values()
        if (overlap := len(winning & mine))
    )


# Solution to part two
@analyze
def solve_two(cards):
    ncards = collections.Counter()
    for card_id, (winning, mine) in cards.items():
        ncards[card_id] += 1
        for extra_card_id in range(len(winning & mine)):
            extra_card_id += 1 + card_id
            ncards[extra_card_id] += ncards[card_id]
    return sum(ncards.values())


# Solve
print(solve_one(read()))
print(solve_two(read()))
