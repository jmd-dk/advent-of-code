import collections, itertools

# Part one
segmentation = list(map(frozenset, [
    'abcefg',   # 0
    'cf',       # 1
    'acdeg',    # 2
    'acdfg',    # 3
    'bcdf',     # 4
    'abdfg',    # 5
    'abdefg',   # 6
    'acf',      # 7
    'abcdefg',  # 8
    'abcdfg',   # 9
]))
displays = []
with open('input.txt') as f:
    for line in f:
        signals, output = line.strip().split('|')
        signals = [frozenset(signal.strip()) for signal in signals.split()]
        output = [frozenset(digit.strip()) for digit in output.split()]
        displays.append((signals, output))
n_segments = [len(segment) for segment in segmentation]
n_to_nums = collections.defaultdict(list)
for num, n in enumerate(n_segments):
    n_to_nums[n].append(num)
unique_n_segments = [n for n, nums in n_to_nums.items() if len(nums) == 1]
count = sum(
    1
    for signals, output in displays
    for digit in output
    if len(digit) in unique_n_segments
)
print('part one:', count)

# Part two
def solve(possibilities):
    """This is basically a stupid implementation of depth-first search
    """
    def backup(possibilities):
        possibilities_backup = {
            signal: segments.copy()
            for signal, segments in possibilities.items()
        }
        return possibilities_backup
    def reset(possibilities, possibilities_backup):
        for signal in possibilities.copy():
            possibilities.pop(signal)
        for signal, segments in possibilities_backup.items():
            possibilities[signal] = segments.copy()
    def prune(possibilities, basecall=True):
        if basecall:
            possibilities_backup = backup(possibilities)
        any_prunings = False
        # Find signal with single possible mapping
        for signal, segments_list in possibilities.items():
            if len(segments_list) > 1:
                continue
            segments = segments_list[0]
            # Find other signal which might be eligible for pruning
            for signal_prune, segments_list_prune in possibilities.items():
                if len(segments_list_prune) == 1:
                    continue
                if len(signal & signal_prune) < len(signal):
                    continue
                # Prune away illegal segment possibilities
                segments_list_pruned = [
                    segments_prune
                    for segments_prune in segments_list_prune
                    if len(segments & segments_prune) == len(segments)
                ]
                if len(segments_list_pruned) == 0:
                    # Illegal
                    if basecall:
                        reset(possibilities, possibilities_backup)
                    return False
                if len(segments_list_pruned) < len(segments_list_prune):
                    any_prunings = True
                    possibilities[signal_prune] = segments_list_pruned
        # Remove non-informative signals
        locked = {list(signal)[0] for signal in possibilities if len(signal) == 1}
        for signal, segments_list in possibilities.copy().items():
            if len(signal) > 1 and len(signal & locked) == len(signal):
                possibilities.pop(signal)
        if any_prunings:
            return prune(possibilities, basecall=False)
        return True
    def replace(possibilities, mapping):
        for c1, c2 in mapping.items():
            if len(c1) == 1 == len(c2):
                if possibilities.get(frozenset(c1)) not in (None, [frozenset(c2)]):
                    # Illegal
                    return False
            possibilities[frozenset(c1)] = [frozenset(c2)]
        return True
    def inconsistent(possibilities):
        possibilities_backup = backup(possibilities)
        for signal_reduce, segments_list_reduce in possibilities.copy().items():
            if len(signal_reduce) == 1:
                continue
            for signal, segments_list in possibilities.copy().items():
                if len(signal) > 1 or len(segments_list) > 1 or len(segments_list[0]) > 1:
                    continue
                c1 = list(signal)[0]
                c2 = list(segments_list[0])[0]
                if c1 not in signal_reduce:
                    segments_list_reduce = [
                        frozenset(c for c in segments if c != c2)
                        for segments in segments_list_reduce
                    ]
                    if not segments_list_reduce:
                        # Found inconsistency
                        reset(possibilities, possibilities_backup)
                        return True
                    continue
                segments_list_reduce = [
                    segments
                    for segments in segments_list_reduce
                    if c2 in segments
                ]
                if not segments_list_reduce:
                    # Found inconsistency
                    reset(possibilities, possibilities_backup)
                    return True
        reset(possibilities, possibilities_backup)
        return False
    def solved(possibilities):
        # Check that all single segments exist
        for segment_target in 'abcdefg':
            segments_list = possibilities.get(frozenset(segment_target), [])
            if len(segments_list) != 1:
                return False
            segment = segments_list[0]
            if len(segment) != 1:
                return False
        return not inconsistent(possibilities)
    def illegal_solution(possibilities):
        if not solved(possibilities):
            return False  # not even a solution
        segments = set()
        for segments_list in possibilities.values():
            if len(segments_list) != 1:
                return False
            segments.add(list(segments_list[0])[0])
        return len(segments) != n_segments_max
    def guess(possibilities, lvl=0):
        if solved(possibilities):
            return possibilities
        possibilities = backup(possibilities)
        # Find signal requiring minimal guesswork
        signal_min_len = float('inf')
        for signal, segments_list in possibilities.items():
            if 1 < len(signal) < signal_min_len:
                signal_min = signal
                signal_min_len = len(signal)
        signal = signal_min
        signal_str = ''.join(signal)
        segments_list = possibilities.pop(signal).copy()
        possibilities_backup = backup(possibilities)
        # Remove signal from possibilities and replace with guesses
        for segments in segments_list:
            possibilities_guess = backup(possibilities_backup)
            segments = ''.join(segments)
            for segments_permut in itertools.permutations(segments):
                # Try replacement
                mapping = dict(zip(signal_str, segments_permut))
                if not replace(possibilities, mapping):
                    # Illegal replacement
                    reset(possibilities, possibilities_guess)
                    continue
                if inconsistent(possibilities):
                    reset(possibilities, possibilities_guess)
                    continue
                if solved(possibilities):
                    return possibilities
                if prune(possibilities):
                    # Maybe legal replacement
                    if illegal_solution(possibilities) or inconsistent(possibilities):
                        reset(possibilities, possibilities_guess)
                        continue
                    if solved(possibilities):
                        return possibilities
                    possibilities_attempt = guess(possibilities, lvl + 1)
                    if possibilities_attempt:
                        # Maybe legal
                        reset(possibilities, possibilities_attempt)
                        if solved(possibilities):
                            return possibilities
                        # Reached end of chain
                        print('Impossible to solve?')
                    else:
                        # Illegal
                        reset(possibilities, possibilities_guess)
                        continue
                else:
                    if solved(possibilities):
                        return possibilities
                    # Illegal replacement
                    reset(possibilities, possibilities_guess)
                    continue
            if solved(possibilities):
                return possibilities
            continue
        if solved(possibilities):
            return possibilities
        return False
    # Initial pruning
    prune(possibilities)
    # Solve
    solution = guess(possibilities)
    # Clean
    solution = {
        str(list(signal)[0]): str(list(segments_list[0])[0])
        for signal, segments_list in solution.items()
    }
    return solution
n_segments_max = max(n_segments)  # "8" -> 7
sum_values = 0
for signals, output in displays:
    # Prune away "8"
    signals = [
        signal
        for signal in signals
        if len(signal) < n_segments_max
    ]
    # List all possibilities
    possibilities = {
        signal: [segmentation[num] for num in n_to_nums[len(signal)]]
        for signal in signals
    }
    # Solve by guessing and pruning
    solution = solve(possibilities)
    # Decode and lookup output
    sum_values += int(
        ''.join(
            str(
                segmentation.index(  # lookup
                    frozenset(solution[segment] for segment in num)  # decode
                )
            ) for num in output
        )
    )
print('part two:', sum_values)

