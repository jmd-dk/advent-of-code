# Read data
with open('input.txt', 'r') as f:
    assignments = [
        sorted(
            (
                range(*map(int, section.split('-')))
                for section in line.split(',')
            ),
            key=(lambda r: (r.start, -r.stop))
        )
        for line in f
    ]

# Part one
fully_contain = sum(
    r0.stop >= r1.stop
    for r0, r1 in assignments
)
print('part one:', fully_contain)

# Part two
overlap = sum(
    r0.stop >= r1.start
    for r0, r1 in assignments
)
print('part two:', overlap)

