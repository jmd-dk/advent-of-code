import collections, operator

# Part one
with open('input.txt') as f:
    lines = [line.strip() for line in f]
def process(line):
    openings_stack = collections.defaultdict(list)
    for i, c in enumerate(line):
        if c in openings:
            openings_stack[c].append(i)
        if c in closings:
            opening = openings[closings.index(c)]
            try:
                j = openings_stack[opening].pop()
            except IndexError:
                # Closing without matching opening
                return c
            chunk = line[j:i+1]
            if len(chunk) == 2:
                try:
                    if openings.index(chunk[0]) == closings.index(chunk[1]):
                        continue
                    raise ValueError
                except ValueError:
                    # Illegal size-2 chunk
                    for c in chunk:
                        if c in closings:
                            return c
                    print('We should never be here!')
            else:
                counter = collections.Counter(chunk)
                for opening, closing in zip(openings, closings):
                    if counter[opening] != counter[closing]:
                        # Illegal size > 2 chunk
                        return c
    # Incomplete chunk
    return openings_stack
openings = '([{<'
closings = ')]}>'
score_table = {')': 3, ']': 57, '}': 1197, '>': 25137}
score = 0
opening_stacks = []
for line in lines:
    result = process(line)
    if isinstance(result, str):
        # Corrupted line
        score += score_table[result]
    else:
        # Incomplete line
        opening_stacks.append(result)
print('part one:', score)

# Part two
score_table |= {')': 1, ']': 2, '}': 3, '>': 4}
scores = []
for opening_stack in opening_stacks:
    completion = ''.join(
        map(
            operator.itemgetter(1),
            sorted(
                (
                    (i, closings[openings.index(opening)])
                    for opening, i_list in opening_stack.items()
                    for i in i_list
                ),
                reverse=True,
            )
        )
    )
    score = 0
    for c in completion:
        score *= 5
        score += score_table[c]
    scores.append(score)
middle_score = sorted(scores)[len(scores)//2]
print('part two:', middle_score)

