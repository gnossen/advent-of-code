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

def knot_grid(knots):
    max_y = max(k.y for k in knots)
    max_x = max(k.x for k in knots)

    s = ""
    for y in range(max_y + 1):
        for x in range(max_x + 1):
            for i, _ in enumerate(knots):
                if y == knots[i].y and x == knots[i].x:
                    if i == 0:
                        s += "H"
                    else:
                        s += str(i)
                    break
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

KNOT_COUNT = 10

knots = [Vec(0, 0) for _ in range(KNOT_COUNT)]

tail_spots = {knots[-1]}
with open(sys.argv[1], "r") as f:
    for line in f:
        components = line.strip().split(" ")
        assert len(components) == 2
        assert components[1].isdigit(), components[1]
        magnitude = int(components[1])
        head_dir = DIR_LOOKUP[components[0]]
        for _ in range(magnitude):
            knots[0] += head_dir
            for i in range(1, KNOT_COUNT):
                if not are_touching(knots[i-1], knots[i]):
                    knots[i] = update_tail(knots[i-1], knots[i])
            tail_spots.add(knots[-1])
            # print(knot_grid(knots))

print(len(tail_spots))
