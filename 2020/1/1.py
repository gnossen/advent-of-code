import itertools
import functools
import operator
import sys

COUNT = 3
TARGET = 2020
FILENAME = "in.txt"

numbers = []

if len(sys.argv) != 4:
   sys.stderr.write(f"USAGE: {sys.argv[0]} FILE COUNT TARGET\n")
   sys.exit(1)

FILENAME = sys.argv[1]
COUNT = int(sys.argv[2])
TARGET = int(sys.argv[3])

with open(FILENAME, "r") as f:
    for line in f:
        numbers.append(int(line))

print(next(functools.reduce(operator.mul, res) for res in itertools.combinations(numbers, COUNT) if sum(res) == TARGET))
