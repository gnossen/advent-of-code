import sys

from dataclasses import dataclass

@dataclass(frozen=True)
class Vec:
    y: int
    x: int

    def __add__(self, other):
        return Vec(self.y + other.y, self.x + other.x)

    def __mul__(self, other):
        assert isinstance(other, int)
        return Vec(other * self.y, other * self.x)

    __rmul__ = __mul__

    def __sub__(self, other):
        return Vec(self.y - other.y, self.x - other.x)

def knot_grid(head, tail):
    max_y = max(head.y, tail.y)
    max_x = max(head.x, tail.x)

    s = ""
    for y in range(max_y + 1):
        for x in range(max_x + 1):
            if y == head.y and x == head.x:
                s += "H"
            elif y == tail.y and x == tail.x:
                s += "T"
            else:
                s += "."
        s += "\n"
    return s

DIR_LOOKUP = {
    "R": Vec(0, 1),
    "U": Vec(1, 0),
    "L": Vec(0, -1),
    "D": Vec(-1, 0),
}

def interpret_delta(direction, magnitude):
    assert direction in DIR_LOOKUP
    return DIR_LOOKUP[direction] * magnitude

def are_touching(head, tail):
    delta = head - tail
    return abs(delta.x) in (0, 1) and abs(delta.y) in (0, 1)

def update_tail(head, tail):
    delta = head - tail
    incremental_y = int(delta.y / abs(delta.y)) if delta.y != 0 else 0
    incremental_x = int(delta.x / abs(delta.x)) if delta.x != 0 else 0
    return tail + Vec(incremental_y, incremental_x)

assert len(sys.argv) == 2

head = Vec(0, 0)
tail = Vec(0, 0)

tail_spots = {tail}
with open(sys.argv[1], "r") as f:
    for line in f:
        components = line.strip().split(" ")
        assert len(components) == 2
        assert components[1].isdigit(), components[1]
        movement = interpret_delta(components[0], int(components[1]))
        head += movement
        while not are_touching(head, tail):
            tail = update_tail(head, tail)
            tail_spots.add(tail)

print(len(tail_spots))
