import heapq, inspect, math, re

# Read in data
class Monkey:
    def __init__(self, levels, operations, throw):
        self._levels = levels.copy()
        self.operation = operation
        self.throw = throw
        self.reset()
    def __int__(self):
        return self.inspections
    def __mul__(self, other):
        return int(self)*int(other)
    __rmul__ = __mul__
    def reset(self):
        self.levels = self._levels.copy()
        self.inspections = 0
monkeys = []
with open('input.txt', 'r') as f:
    while f.readline():
        levels = list(map(int, re.findall(r'\d+', f.readline())))
        operation = eval('lambda old:' + re.search('new *=(.+)', f.readline()).group(1))
        div, a, b = (int(re.search(r'(\d+)', f.readline()).group(1)) for _ in range(3))
        throw = lambda level, *, div=div, a=a, b=b: b if level%div else a
        monkeys.append(Monkey(levels, operation, throw))
        f.readline()

# Part one
def play(monkeys, n_rounds, relief):
    lcm = math.lcm(*(  # hacky, but fun
        inspect.signature(monkey.throw).parameters['div'].default
        for monkey in monkeys
    ))
    for r in range(n_rounds):
        for monkey in monkeys:
            for level in monkey.levels:
                monkey.inspections += 1
                level = monkey.operation(level)//relief
                level %= lcm
                monkeys[monkey.throw(level)].levels.append(level)
            monkey.levels.clear()
def get_monkey_business(monkeys, n_rounds, relief=1, n_most_active=2):
    for monkey in monkeys:
        monkey.reset()
    play(monkeys, n_rounds, relief)
    return math.prod(heapq.nlargest(n_most_active, monkeys, key=int))
print('part one:', get_monkey_business(monkeys, 20, 3))

# Part two
print('part two:', get_monkey_business(monkeys, 10_000))

