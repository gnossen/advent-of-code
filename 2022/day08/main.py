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

def _in_bounds(pos, height, width):
    return (pos.x >= 0 and pos.x < width and pos.y >= 0 and pos.y < height)

def _scenic_score_in_direction(trees, pos, d):
    height = len(trees)
    width = len(trees[0])
    current = pos + d
    start_height = trees[pos.y][pos.x]
    score = 0
    while _in_bounds(current, height, width):
        score += 1
        current_height = trees[current.y][current.x]
        if current_height >= start_height:
            break
        current += d
    return score

def _scenic_score(trees, pos):
    dirs = [
        Vec(0, 1),
        Vec(1, 0),
        Vec(0, -1),
        Vec(-1, 0),
    ]
    score = 1
    for d in dirs:
        score *= _scenic_score_in_direction(trees, pos, d)
    return score

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

# print(_scenic_score_in_direction(trees, Vec(1, 2), Vec(1, 0)))

max_score = 0
for y in range(height):
    for x in range(width):
        max_score = max(max_score, _scenic_score(trees, Vec(y, x)))

print(max_score)

