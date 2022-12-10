import re

# Read in data
class Directory(dict):
    def __init__(self, parent=None):
        self.parent = parent
        self._size = None
    def __getitem__(self, key):
        if (val := self.get(key)) is None:
            val = self[key] = type(self)(self)
        return val
    def __iter__(self):
        yield self
        for val in self.values():
            if isinstance(val, type(self)):
                yield from val
    def __len__(self):
        if self._size is None:
            self._size = sum(int(d) for d in self.values())
        return self._size
    def __int__(self):
        return len(self)
root = Directory()
with open('input.txt', 'r') as f:
    for line in f:
        if (match := re.search(r'^(\d+) (.+)', line)):
            cwd[match.group(2)] = int(match.group(1))
            continue
        elif not (match := re.search(r'\$ cd (.+)', line)):
            continue
        cd_dir = match.group(1)
        if cd_dir == '/':
            cwd = root
        elif cd_dir == '..':
            cwd = cwd.parent
        else:
            cwd = cwd[cd_dir]

# Part one
size_max = 100_000
print('part one:', sum(len(d) for d in root if len(d) <= size_max))

# Part two
size_tot, size_req = 70_000_000, 30_000_000
size_del = size_req - (size_tot - len(root))
print('part two:', min(len(d) for d in root if len(d) >= size_del))

