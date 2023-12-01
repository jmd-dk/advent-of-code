import collections, copy

# Part one
class Image:
    def __init__(self, enhancement_algorithm, fill=0):
        if isinstance(enhancement_algorithm, str):
            enhancement_algorithm = list(map('.#'.index, enhancement_algorithm))
        self.enhancement_algorithm = enhancement_algorithm
        self.fill = fill
        self.points = collections.defaultdict(lambda: self.fill)
        inf = float('inf')
        self.extreme = {
            'x_min': +inf,
            'x_max': -inf,
            'y_min': +inf,
            'y_max': -inf,
        }
    def __getitem__(self, key):
        return self.points[key]
    def __setitem__(self, key, value):
        if isinstance(value, str):
            value = '.#'.index(value)
        self.points[key] = value
        if not value:
            return
        for dim, k in zip('xy', key):
            self.extreme[f'{dim}_min'] = min(self.extreme[f'{dim}_min'], k)
            self.extreme[f'{dim}_max'] = max(self.extreme[f'{dim}_max'], k)
    def __repr__(self):
        s = []
        x_min, x_max = self.extreme['x_min'], self.extreme['x_max']
        y_min, y_max = self.extreme['y_min'], self.extreme['y_max']
        for     y in range(y_min, y_max + 1):
            for x in range(x_min, x_max + 1):
                s.append('.#'[self[x, y]])
            s.append('\n')
        return ''.join(s).strip()
    def copy(self):
        return copy.deepcopy(self)
    def enhance(self, n=1):
        if n == 0:
            return
        # Build new, enhanced image
        win_size = 3
        x_min, x_max = self.extreme['x_min'], self.extreme['x_max']
        y_min, y_max = self.extreme['y_min'], self.extreme['y_max']
        new_image = type(self)(self.enhancement_algorithm, self.fill)
        for y     in range(y_min - win_size//2, y_max + 1 + win_size//2):
            for x in range(x_min - win_size//2, x_max + 1 + win_size//2):
                win = []
                for     y_win in range(y - win_size//2, y + 1 + win_size//2):
                    for x_win in range(x - win_size//2, x + 1 + win_size//2):
                        win.append(self[x_win, y_win])
                num = int(''.join(map(str, win)), base=2)
                new_image[x, y] = self.enhancement_algorithm[num]
        # Flip the infinite sea if applicable
        if new_image.fill == 0 and new_image.enhancement_algorithm[0] == 1:
            new_image.fill = 1
        elif new_image.fill == 1 and new_image.enhancement_algorithm[-1] == 0:
            new_image.fill = 0
        # Assign enhanced attributes to original instance
        for attr, value in vars(new_image).items():
            setattr(self, attr, value)
        # Call recursively
        self.enhance(n - 1)
    @property
    def size(self):
        if self.fill == 1:
            return float('inf')
        return sum(1 for value in self.points.values() if value == 1)
with open('input.txt') as f:
    image_ini = Image(f.readline().strip())
    f.readline()  # blank
    for y, line in enumerate(f):  # y grows downwards
        for x, c in enumerate(line.strip()):
            image_ini[x, y] = c
image = image_ini.copy()
image.enhance(2)
print('part one:', image.size)

# Part two
image = image_ini.copy()
image.enhance(50)
print('part two:', image.size)

