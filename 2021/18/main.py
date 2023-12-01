import ast, copy, itertools

# Part one
class SnailFishNumber:
    def __init__(self, left, rght=None):
        vlue = None
        if rght is None:
            if isinstance(left, str):
                left, rght = type(self)(ast.literal_eval(left))
            elif isinstance(left, int):
                left, vlue = None, left
            elif isinstance(left, type(self)):
                left, rght, vlue = left.left, left.rght, left.vlue
            else:
                left, rght = left
                left = type(self)(left)
                rght = type(self)(rght)
        else:
            left = type(self)(left)
            rght = type(self)(rght)
        self.left = copy.deepcopy(left)
        self.rght = copy.deepcopy(rght)
        self.vlue = vlue
    def __iter__(self):
        yield self.left
        yield self.rght
    def __add__(self, other):
        if other == 0:
            return self.copy()
        if self.vlue is not None:
            return type(self)(self.vlue + type(self)(other).vlue)
        return type(self)(self, other).reduce()
    def __radd__(self, other):
        if other == 0:
            return self.copy()
        if self.vlue is not None:
            return type(self)(type(self)(other).vlue + self.vlue)
        return type(self)(other, self)
    def __iadd__(self, other):
        return self.update(self + other)
    def __repr__(self):
        if self.vlue is not None:
            return repr(self.vlue)
        return f'[{self.left}, {self.rght}]'
    def update(self, other):
        self.left = other.left
        self.rght = other.rght
        self.vlue = other.vlue
        return self
    def copy(self):
        return copy.deepcopy(self)
    def walk(self, parent=None, depth=0):
        if self.vlue is not None:
            yield self, parent, depth
        else:
            yield from self.left.walk(self, depth + 1)
            yield from self.rght.walk(self, depth + 1)
    def nullify(self):
        self.left = None
        self.rght = None
        self.vlue = 0
    def explode(self):
        sfn_prev = sfn_explode = sfn_rght = None
        walk = self.walk()
        for sfn, parent, depth in walk:
            if depth == 5:  # depth of 0 is a bare number
                sfn_left = sfn_prev
                sfn_explode = parent
                try:
                    next(walk)  # skip
                    sfn_rght, _, _ = next(walk)
                except StopIteration:
                    pass
                break
            sfn_prev = sfn
        if not sfn_explode:
            return
        if sfn_left:
            sfn_left += sfn_explode.left
        if sfn_rght:
            sfn_rght += sfn_explode.rght
        sfn_explode.nullify()
        return True
    def split(self):
        for sfn, parent, depth in self.walk():
            if sfn.vlue is not None and sfn.vlue >= 10:
                sfn.left = type(self)((sfn.vlue + 0)//2)
                sfn.rght = type(self)((sfn.vlue + 1)//2)
                sfn.vlue = None
                return True
    def reduce(self):
        while True:
            if self.explode():
                continue
            if self.split():
                continue
            return self
    @property
    def magnitude(self):
        if self.vlue is not None:
            return self.vlue
        return 3*self.left.magnitude + 2*self.rght.magnitude
with open('input.txt') as f:
    sfns = [SnailFishNumber(line) for line in f]
print('part one:', sum(sfns).magnitude)

# Part two
mag_max = max(
    (sfn0 + sfn1).magnitude
    for sfn0, sfn1 in itertools.permutations(sfns, 2)
)
print('part two:', mag_max)

