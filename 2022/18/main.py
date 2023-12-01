# Read in data
cubes = []
with open('input.txt') as f:
    for line in f:
        cube = tuple(map(int, line.split(',')))
        cubes.append(cube)

# Part one
def get_faces(cube):
    faces = [
        (cube[0] + 0.5, cube[1] + 0.5, cube[2] + 0),
        (cube[0] + 0.5, cube[1] + 0.5, cube[2] + 1),
        (cube[0] + 0.5, cube[1] + 0, cube[2] + 0.5),
        (cube[0] + 0.5, cube[1] + 1, cube[2] + 0.5),
        (cube[0] + 0, cube[1] + 0.5, cube[2] + 0.5),
        (cube[0] + 1, cube[1] + 0.5, cube[2] + 0.5),
    ]
    return faces
faces_all = set()
for cube in cubes:
    faces = get_faces(cube)
    for face in faces:
        if face in faces_all:
            faces_all.remove(face)
        else:
            faces_all.add(face)
print('part one:', len(faces_all))

# Part two
def flow():
    """Note that using recursion is non-optimal as it gets too deep"""
    xmin = min(cube[0] for cube in cubes)
    xmax = max(cube[0] for cube in cubes)
    ymin = min(cube[1] for cube in cubes)
    ymax = max(cube[1] for cube in cubes)
    zmin = min(cube[2] for cube in cubes)
    zmax = max(cube[2] for cube in cubes)
    options = [(xmin - 1, ymin - 1, zmin - 1)]
    visited = set()
    area = 0
    while options:
        cube = options.pop()
        if cube in cubes or cube in visited:
            continue
        # New visit
        visited.add(cube)
        faces = set(get_faces(cube))
        area += len(faces & faces_all)
        # Add exploration options
        x, y, z = cube
        for dx in (-1, +1):
            if xmin - 1 <= x + dx <= xmax + 1:
                options.append((x + dx, y, z))
        for dy in (-1, +1):
            if ymin - 1 <= y + dy <= ymax + 1:
                options.append((x, y + dy, z))
        for dz in (-1, +1):
            if zmin - 1 <= z + dz <= zmax + 1:
                options.append((x, y, z + dz))
    return area
print('part two:', flow())

