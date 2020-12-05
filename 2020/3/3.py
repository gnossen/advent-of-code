import argparse

from typing import List

TreeMap = List[List[bool]]


def get_map_from_path(path: str) -> TreeMap:
    tree_map = []
    with open(path, "r") as f:
        for line in f:
            row = []
            stripped = line.strip()
            for c in stripped:
                if c == "#":
                    row.append(True)
                elif c == ".":
                    row.append(False)
                else:
                    assert False, f"Encountered unknown character '{c}'"
            if tree_map:
                assert (len(row) == len(tree_map[-1]))
            tree_map.append(row)
    return tree_map


def lookup(x: int, y: int, tree_map: TreeMap) -> bool:
    width = len(tree_map[0])
    return tree_map[y][x % width]


def count_trees(tree_map: TreeMap, right_pace: int) -> int:
    tree_count = 0
    x = 0
    for y, _ in enumerate(tree_map):
        if lookup(x, y, tree_map):
            tree_count += 1
        x += right_pace
    return tree_count


parser = argparse.ArgumentParser(description="Solve day 3.")
parser.add_argument("map_file", help="The file containing the tree map.")
args = parser.parse_args()

tree_map = get_map_from_path(args.map_file)

print(count_trees(tree_map, 3))
