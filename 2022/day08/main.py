import sys

from dataclasses import dataclass

@dataclass
class Vec:
    y: int
    x: int

    def __add__(self, other):
        return Vec(self.y + other.y, self.x + other.x)


def _is_tree_visible_from(trees, tree_pos, start_pos, direction):
    current = start_pos
    max_height = 0
    while True:
        current_height = trees[current.y][current.x]
        if current == tree_pos:
            return current_height > max_height
        max_height = max(max_height, current_height)
        current += direction
    assert False, "Unreachable code"

def _is_tree_visible(trees, position):
    height = len(trees)
    width = len(trees[0])
    return (position.x == 0 or
            position.y == 0 or
            position.x == width - 1 or
            position.y == height - 1 or
            _is_tree_visible_from(trees, position, Vec(0, position.x), Vec(1, 0)) or
            _is_tree_visible_from(trees, position, Vec(height - 1, position.x), Vec(-1, 0)) or
            _is_tree_visible_from(trees, position, Vec(position.y, 0), Vec(0, 1)) or
            _is_tree_visible_from(trees, position, Vec(position.y, width - 1), Vec(0, -1)))

assert len(sys.argv) == 2

trees = []
with open(sys.argv[1], "r") as f:
    for line in f:
        line = line.strip()
        tree_line = []
        for char in line:
            assert char.isdigit()
            tree_line.append(int(char))
        trees.append(tree_line)

height = len(trees)
width = len(trees[0])

externally_visible = 0
for y in range(height):
    for x in range(width):
        if _is_tree_visible(trees, Vec(y, x)):
            externally_visible += 1

print(externally_visible)

