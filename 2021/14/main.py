import collections, re

# Part one
rules = {}
with open('input.txt') as f:
    template = f.readline().strip()
    f.readline()  # blank
    for line in f:
        pair, product = re.search(r'(..) -> (.)', line).groups()
        rules[pair] = product
pairs_ini = collections.Counter(
    [el0 + el1 for el0, el1 in zip(template, template[1:])]
)
def progress(pairs, n_steps):
    for step in range(1, 1 + n_steps):
        for pair, count in pairs.copy().items():
            pairs[pair] -= count
            element = rules[pair]
            pairs[pair[0] + element] += count
            pairs[element + pair[1]] += count
def count_elements(pairs):
    elements = collections.Counter([template[0], template[-1]])
    for pair, count in pairs.items():
        elements[pair[0]] += count
        elements[pair[1]] += count
    for element in elements:
        elements[element] //= 2
    return elements
def compute(n_steps):
    pairs = pairs_ini.copy()
    progress(pairs, n_steps)
    elements = count_elements(pairs)
    # Note: elements.most_common(1) will be slower as it builds
    # a heap, which is only beneficial when finding the top n items,
    # n > 1.
    return max(elements.values()) - min(elements.values())
print('part one:', compute(10))

# Part two
print('part two:', compute(40))

