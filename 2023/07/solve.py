import collections, contextlib, itertools, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parent.parent))
from magic import *


# Reading in data
@memoize
def read():
    Hand = collections.namedtuple('Hand', ['cards', 'bid'])
    hands = []
    with open_input_file() as file:
        for line in file:
            cards, bid = line.split()
            bid = int(bid)
            hands.append(Hand(cards, bid))
    return hands


# Solution to part one
@analyze
def solve_one(hands):
    class Hand:
        labels = dict(zip('23456789TJQKA', itertools.count(2)))

        def __init__(self, cards, bid):
            self.cards = cards
            self.bid = bid
            self.parse()

        def parse(self):
            def determine_kind():
                def check(*pairs):
                    nonlocal kind
                    if all(compare(*pair) for pair in pairs):
                        yield kind
                    kind += 1

                most_common = collections.Counter(self.card_values).most_common()
                compare = lambda m, n: most_common[m][1] == n
                kind = 0
                yield from check((0, 1))
                yield from check((0, 2), (1, 1))
                yield from check((0, 2), (1, 2))
                yield from check((0, 3), (1, 1))
                yield from check((0, 3), (1, 2))
                yield from check((0, 4))
                yield from check((0, 5))

            self.card_values = tuple(self.labels[card] for card in self.cards)
            self.kind = next(determine_kind())

        def __lt__(self, other):
            return (self.kind, self.card_values) < (other.kind, other.card_values)

    hands = sorted(Hand(*hand) for hand in hands)
    return sum(hand.bid * rank for rank, hand in enumerate(hands, 1))


# Solution to part two
@analyze
def solve_two(hands):
    class Hand:
        labels = dict(zip('J23456789TQKA', itertools.count(1)))

        def __init__(self, cards, bid):
            self.cards = cards
            self.bid = bid
            self.parse()

        def parse(self):
            def set_card_values():
                self.card_values = tuple(self.labels[card] for card in self.cards)

            def determine_kind():
                def check(*pairs):
                    nonlocal kind
                    if all(compare(*pair) for pair in pairs):
                        yield kind
                    kind += 1

                most_common = collections.Counter(self.card_values).most_common()
                compare = lambda m, n: most_common[m][1] == n
                kind = 0
                yield from check((0, 1))
                yield from check((0, 2), (1, 1))
                yield from check((0, 2), (1, 2))
                yield from check((0, 3), (1, 1))
                yield from check((0, 3), (1, 2))
                yield from check((0, 4))
                yield from check((0, 5))

            with self.replace_jokers():
                set_card_values()
            self.kind = next(determine_kind())
            set_card_values()

        @contextlib.contextmanager
        def replace_jokers(self):
            most_common = collections.Counter(self.cards).most_common(2)
            get_label = lambda replacement: {'J': replacement}.get(
                most_common[0][0], most_common[0][0]
            )
            label = get_label('A' if len(most_common) == 1 else most_common[1][0])
            self.cards = (cards := self.cards).replace('J', label)
            yield
            self.cards = cards

        def __lt__(self, other):
            return (self.kind, self.card_values) < (other.kind, other.card_values)

    hands = sorted(Hand(*hand) for hand in hands)
    return sum(hand.bid * rank for rank, hand in enumerate(hands, 1))


# Solve
print(solve_one(read()))
print(solve_two(read()))
