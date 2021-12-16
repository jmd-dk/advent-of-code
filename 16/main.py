import collections, functools, itertools
from math import prod

# Part one
with open('input.txt') as f:
    packet_hex = f.read().strip()
int2 = functools.partial(int, base=2)
def hex_to_bin(packet_hex):
    packet_int = int(packet_hex, base=16)
    packet = bin(packet_int)[2:]
    packet = '0'*(4*len(packet_hex) - len(packet)) + packet
    return packet
transmission = hex_to_bin(packet_hex)
def read(it, size=None):
    return ''.join(itertools.islice(it, size))
def read_literal(it):
    return int2(''.join(read_literal_groups(it)))
def read_literal_groups(it):
    size = 4
    while True:
        last = (read(it, 1) == '0')
        yield read(it, size)
        if last:
            return
def parse(it):
    if isinstance(it, str):
        it = iter(it)
    version = int2(read(it, 3))
    type_ID = int2(read(it, 3))
    if type_ID == 4:
        # Literal value
        value = read_literal(it)
        packet = Packet(version, type_ID, value=value)
    else:
        # Operator packet
        length_type_ID = read(it, 1)
        children = []
        if length_type_ID == '0':
            # Next 15 bits represent total length of sub-packets
            length = int2(read(it, 15))
            ini = it.__length_hint__()
            while ini - it.__length_hint__() != length:
                children.append(parse(it))
        else:
            # Next 11 bits represents number of sub-packets
            num_subpackets = int2(read(it, 11))
            for i in range(num_subpackets):
                children.append(parse(it))
        packet = Packet(version, type_ID, children=children)
    return packet
class Packet:
    def __init__(self, version, type_ID, *, value=None, children=None):
        self.version = version
        self.type_ID = type_ID
        self.value = value
        if children is None:
            children = []
        self.children = children
    def __repr__(self, lvl=1):
        """AST"""
        s = f'{self.__class__.__name__}(v{self.version}, {operations[self.type_ID].symbol}) → {self.value}'
        if self.children:
            s += '\n' + '\n'.join('    '*lvl + child.__repr__(lvl + 1) for child in self.children)
        return s
base_packet = parse(transmission)
def sumup_versions(packet, version_sum=0):
    version_sum += packet.version
    for child in packet.children:
        version_sum += sumup_versions(child)
    return version_sum
print('part one:', sumup_versions(base_packet))

# Part two
def assign_value(packet):
    if packet.value is not None:
        return
    for child in packet.children:
        assign_value(child)
    packet.value = operations[packet.type_ID].func(packet)
Operation = collections.namedtuple('Operation', ('ID', 'symbol', 'func'))
operations = [
    Operation(0, '+',   lambda packet: sum (child.value for child in packet.children)),
    Operation(1, '×',   lambda packet: prod(child.value for child in packet.children)),
    Operation(2, 'min', lambda packet: min (child.value for child in packet.children)),
    Operation(3, 'max', lambda packet: max (child.value for child in packet.children)),
    Operation(4, 'num', lambda packet: packet.value),
    Operation(5, '>',   lambda packet: int(packet.children[0].value >  packet.children[1].value)),
    Operation(6, '<',   lambda packet: int(packet.children[0].value <  packet.children[1].value)),
    Operation(7, '==',  lambda packet: int(packet.children[0].value == packet.children[1].value)),
]
assign_value(base_packet)
print('part two:', base_packet.value)

