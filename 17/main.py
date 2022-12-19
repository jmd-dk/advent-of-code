import itertools

# Read in data
rocks_str = """
####

 # 
###
 # 

  #
  #
###

#
#
#
#

##
##
"""
rocks = []
for rock_str in rocks_str.split('\n'*2):
    rock = []
    rocks.append(rock)
    for j, row_rock_str in enumerate(reversed(rock_str.strip('\n').split('\n'))):
        for i, c in enumerate(row_rock_str):
            if c == '#':
                rock.append((i, j))
with open('input.txt') as f:
    jetpatterns = f.read().rstrip()

# Part one
width_chamber = 7
appears = (2, 3)
def reset():
    resting.clear()
    resting.update({(x, 0) for x in range(1, width_chamber + 1)})
    contour[:] = [0] + [pixel[1] for pixel in resting] + [0]
    rocks_it = itertools.cycle(rocks)
    jetpatterns_it = itertools.cycle(jetpatterns)
    return rocks_it, jetpatterns_it
resting = set()
contour = []
def spawn_rock(rock):
    y = appears[1] + 1 + max(contour)
    x = appears[0] + 1
    rock = [[x + t[0], y + t[1]] for t in rock]
    return rock
def push(rock, jet):
    sign = 2*'<>'.index(jet) - 1
    leftmost = min(pixel[0] for pixel in rock)
    rightmost = max(pixel[0] for pixel in rock)
    if sign == -1 and leftmost == 1:
        return
    if sign == +1 and rightmost == width_chamber:
        return
    for pixel in rock:
        if (pixel[0] + sign, pixel[1]) in resting:
            return
    for pixel in rock:
        pixel[0] += sign
def fall(rock):
    for pixel in rock:
        if (pixel[0], pixel[1] - 1) in resting:
            for pixel in rock:
                contour[pixel[0]] = max(contour[pixel[0]], pixel[1])
                resting.add(tuple(pixel))
            return True
    for pixel in rock:
        pixel[1] -= 1
def simulate(n):
    rocks_it, jetpatterns_it = reset()
    for _ in range(n):
        rock = spawn_rock(next(rocks_it))
        for jet in jetpatterns_it:
            push(rock, jet)
            if fall(rock):
                break
    return max(contour)
print('part one:', simulate(2022))

# Part two
print('part two:', simulate(1_000_000_000_000))

