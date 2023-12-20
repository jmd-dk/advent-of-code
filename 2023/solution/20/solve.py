import collections, itertools, math, re

# Summon magic
__import__('sys').path.append(str(__import__('pathlib').Path(__file__).parents[2]))
from magic import *


# Reading in data
@memoize
def read():
    class Module:
        def __init__(self, name, kind, dests):
            self.name = name
            self.kind = kind
            self.dests = dests
            self.flipflop = False
            self.conjunction = {}

    modules = {}
    with open_input() as file:
        for line in file:
            module, dests = line.split('->')
            kind, name = re.match(r'(%|&)?(\w+)', module).groups()
            dests = re.findall(r'\w+', dests)
            modules[name] = Module(name, kind, dests)
    for module in modules.values():
        module.dests = [modules.get(dest, dest) for dest in module.dests]
    return modules


# Solution to part one
@analyze
def solve_one(modules):
    def reset_conjunctions():
        for module in modules.values():
            if module.kind != '&':
                continue
            for module_in in modules.values():
                if module in module_in.dests:
                    module.conjunction[module_in] = False

    def press_button():
        signals = [Signal('button', modules['broadcaster'], False)]
        while signals:
            counts[True] += (count_high := sum(signal.pulse for signal in signals))
            counts[False] += len(signals) - count_high
            signals = handle_signals(signals)
        return counts

    def handle_signals(signals_in):
        signals_out = []
        for signal in signals_in:
            if isinstance(recver := signal.recver, str):
                continue
            match recver.kind, signal.pulse:
                case None, _:
                    pulse = signal.pulse
                case '%', False:
                    pulse = recver.flipflop = not recver.flipflop
                case '&', _:
                    recver.conjunction[signal.sender] = signal.pulse
                    pulse = not all(recver.conjunction.values())
                case _:
                    continue
            for dest in recver.dests:
                signals_out.append(Signal(recver, dest, pulse))
        return signals_out

    Signal = collections.namedtuple('Signal', ('sender', 'recver', 'pulse'))
    reset_conjunctions()
    counts = collections.Counter()
    for _ in range(1000):
        press_button()
    return math.prod(counts.values())


# Solution to part two
@analyze
def solve_two(modules):
    def reset_conjunctions():
        for module in modules.values():
            if module.kind != '&':
                continue
            for module_in in modules.values():
                if module in module_in.dests:
                    module.conjunction[module_in] = False

    def press_button():
        signals = [Signal('button', modules['broadcaster'], False)]
        while signals:
            signals = handle_signals(signals)

    def handle_signals(signals_in):
        signals_out = []
        for signal in signals_in:
            if isinstance(recver := signal.recver, str):
                continue
            match recver.kind, signal.pulse:
                case None, _:
                    pulse = signal.pulse
                case '%', False:
                    pulse = recver.flipflop = not recver.flipflop
                case '&', _:
                    recver.conjunction[signal.sender] = signal.pulse
                    pulse = not all(recver.conjunction.values())
                    if pulse and len(cycle_times[recver]) < 2:
                        cycle_times[recver].append(iteration)
                case _:
                    continue
            for dest in recver.dests:
                signals_out.append(Signal(recver, dest, pulse))
        return signals_out

    def assume(assumption, valid):
        if valid:
            return
        match assumption:
            case 0:
                message = 'only a single penultimate module exists'
            case 1:
                messge = 'the penultimate module is a conjunction'
            case 2:
                messge = 'all antepenultimate modules are conjunctions'
            case 3:
                message = (
                    'a high pulse is emitted for the second time by all '
                    'antepenultimate modules after exactly twice the number '
                    'of iterations it took for the first time'
                )
        raise RuntimeError(f'The assumption that {message} is violated')

    Signal = collections.namedtuple('Signal', ('sender', 'recver', 'pulse'))
    ultimate = 'rx'
    penultimates = [module for module in modules.values() if ultimate in module.dests]
    assume(0, len(penultimates) == 1)
    penultimate = penultimates.pop()
    assume(1, penultimate.kind == '&')
    antepenultimates = [
        module for module in modules.values() if penultimate in module.dests
    ]
    assume(2, all(module.kind == '&' for module in antepenultimates))
    reset_conjunctions()
    cycle_times = collections.defaultdict(list)
    for iteration in itertools.count(1):
        press_button()
        if all(len(cycle_times[module]) >= 2 for module in antepenultimates):
            break
    assume(
        3,
        all(
            cycle_times[module][1] == 2 * cycle_times[module][0]
            for module in antepenultimates
        ),
    )
    return math.lcm(*(cycle_times[module][0] for module in antepenultimates))


# Solve
print(solve_one(read()))
print(solve_two(read()))
