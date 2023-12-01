import ast, collections, itertools
import numpy as np

# Part one
scanners = []
with open('input.txt') as f:
    for line in f:
        if 'scanner' in line:
            scanners.append(scanner := [])
        if ',' in line:
            scanner.append(ast.literal_eval(line))
scanners = [np.array(scanner) for scanner in scanners]
Rx = np.matrix([
    [ 1,  0,  0],
    [ 0,  0, -1],
    [ 0,  1,  0],
])
Ry = np.matrix([
    [ 0,  0,  1],
    [ 0,  1,  0],
    [-1,  0,  0],
])
Rz = np.matrix([
    [ 0, -1,  0],
    [ 1,  0,  0],
    [ 0,  0,  1],
])
rotations = []
rotations_str = set()
for rx in range(3):
    for ry in range(3):
        for rz in range(3):
            for R0, R1, R2 in itertools.permutations((Rx**rx, Ry**ry, Rz**rz)):
                R = R0 @ R1 @ R2
                if str(R) not in rotations_str:
                    rotations_str.add(str(R))
                    rotations.append(np.asarray(R))
scanner_displacements = {0: np.array((0, 0, 0))}
scanner_orientations  = {0: rotations[0]}  # trivial rotation
while len(scanner_displacements) < len(scanners):
    for i, scanner_i in enumerate(scanners):
        i_bak = i
        for j, scanner_j in enumerate(scanners[i+1:], i + 1):
            i = i_bak
            if (i in scanner_displacements) + (j in scanner_displacements) != 1:
                continue
            for R_ij in rotations:
                counter = collections.Counter()
                for beacon_j in scanner_j:
                    for beacon_i in scanner_i:
                        # Note that (R_ij @ beacon_j) can be precomputed
                        # to gain performance.
                        counter[tuple(beacon_i - R_ij @ beacon_j)] += 1
                displacement, count = counter.most_common(1)[0]
                if count < 12:
                    continue
                displacement = np.array(displacement)
                if i not in scanner_displacements:
                    # Transform from i to j
                    R_ji = np.asarray(np.linalg.inv(R_ij), dtype=int)
                    displacement = R_ji @ -displacement
                    # Relabel i <-> j so that i is the known scanner
                    # and j is the new scanner.
                    i, j = j, i
                    R_ij = R_ji
                R_i = scanner_orientations[i]
                scanner_displacements[j] = R_i @ displacement + scanner_displacements[i]
                scanner_orientations[j] = R_i @ R_ij
                break
beacons = set()
for i, scanner_i in enumerate(scanners):
    beacons |= {
        tuple(scanner_orientations[i] @ beacon + scanner_displacements[i])
        for beacon in scanner_i
    }
print('part one:', len(beacons))

# Part two
max_dist = 0
for displacement_i in scanner_displacements.values():
    for displacement_j in scanner_displacements.values():
        max_dist = max(max_dist, sum(np.abs(displacement_i - displacement_j)))
print('part two:', max_dist)

