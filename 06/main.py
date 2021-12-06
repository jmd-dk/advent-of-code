# Part one
with open('input.txt') as f:
    fishies_list = [int(i) for i in f.read().strip().split(',')]
period = 7
lag = 2
fishies = [fishies_list.count(timer) for timer in range(period + lag)]
def spawn(fishies, days):
    for day in range(1, days + 1):
        fishies_next = [0 for _ in range(period + lag)]
        n_fish = fishies[0]
        fishies_next[period - 1      ] = n_fish
        fishies_next[period - 1 + lag] = n_fish
        for timer, n_fish in enumerate(fishies[1:], 1):
            fishies_next[timer - 1] += n_fish
        fishies = fishies_next
    return sum(fishies)
print('part one:', spawn(fishies, 80))

# Part two
print('part one:', spawn(fishies, 256))

