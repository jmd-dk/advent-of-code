# Part one
with open('input.txt') as f:
    crabs = [int(crab) for crab in f.read().split(',')]
def measure_1(target):
    return sum(abs(crab - target) for crab in crabs)
def minimize(measure):
    target_min = min(crabs)
    target_max = max(crabs)
    fuel_min = measure(target_min)
    fuel_max = measure(target_max)
    while True:
        target = (target_min + target_max)//2
        fuel      = measure(target)
        fuel_left = measure(target - 1)
        fuel_rght = measure(target + 1)
        grad_left = fuel - fuel_left
        grad_rght = fuel_rght - fuel
        grad_left = fuel - fuel_left
        if grad_left < 0 < grad_rght:
            return target, fuel
        if 0 < grad_left:
            target_max = target
        else:
            target_min = target
target, fuel = minimize(measure_1)
print('part one:', fuel)

# Part two
def measure_2(target):
    return sum((dist + 1)*dist//2 for crab in crabs if (dist := abs(crab - target)))
target, fuel = minimize(measure_2)
print('part two:', fuel)

