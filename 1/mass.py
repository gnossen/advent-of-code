#!/usr/bin/env python3

import sys
import math


def number_stream(path):
    with open(path, 'r') as f:
        while True:
            try:
                yield int(f.readline().strip())
            except ValueError:
                break


def mass(n):
    return math.floor(n / 3.0) - 2


def main():
    if len(sys.argv) != 2:
        sys.stderr.write(f"Usage: {sys.argv[0]} filename\n")
        sys.exit(1)
    print(sum(mass(n) for n in number_stream(sys.argv[1])))


if __name__ == "__main__":
    main()
