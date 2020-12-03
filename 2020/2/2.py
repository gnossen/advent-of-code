import re
import sys
from typing import Tuple

RULE_REGEX = re.compile("([0-9]+)-([0-9]+) ([a-zA-Z])")

def get_rule(rule_str: str) -> Tuple[int, int]:
    match = RULE_REGEX.match(rule_str)
    rng = (int(match.group(1)), int(match.group(2)))
    assert rng[0] <= rng[1], f"Rule '{rule_str}' invalid."
    c = match.group(3)
    return rng, c

def is_valid(line: str) -> bool:
    pieces = line.split(":")
    assert len(pieces) == 2
    rule_str = pieces[0].strip()
    password = pieces[1].strip()
    (low, high), c = get_rule(rule_str)
    valid = int(password[low - 1] == c) + int(password[high - 1] == c) == 1
    return valid

if len(sys.argv) != 2:
    sys.stderr.write(f"Usage: {sys.argv[0]} INPUT_FILE")

INPUT_PATH = sys.argv[1]

valid = 0
with open(INPUT_PATH, 'r') as f:
    for line in f:
        if is_valid(line.strip()):
            valid += 1

print(valid)
