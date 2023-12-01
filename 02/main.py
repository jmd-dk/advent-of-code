# Read data
with open('input.txt', 'r') as f:
    strategy = [tuple(line.strip().split()) for line in f]

# Part one
base = {'X': 1, 'Y': 2, 'Z': 3}
wins = {
    ('A', 'Y'),
    ('B', 'Z'),
    ('C', 'X'),
}
draws = {
    ('A', 'X'),
    ('B', 'Y'),
    ('C', 'Z'),
}
score_tot = sum(
    + base[game[1]]      # hand
    + 6*(game in wins)   # win
    + 3*(game in draws)  # draw
    for game in strategy
)
print('part one:', score_tot)

# Part two
base = {'A': 1, 'B': 2, 'C': 3}
outcomes = {
    'X': 0,  # loose
    'Y': 3,  # draw
    'Z': 6,  # win
}
strategies = {
    'X': {  # loose
        'A': 'C',
        'B': 'A',
        'C': 'B',
    },
    'Y': {  # draw
        'A': 'A',
        'B': 'B',
        'C': 'C',
    },
    'Z': {  # win
        'A': 'B',
        'B': 'C',
        'C': 'A',
    },
}
score_tot = sum(
    outcomes[goal] + base[strategies[goal][opponent]]
    for opponent, goal in strategy
)
print('part two:', score_tot)
