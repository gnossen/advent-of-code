import argparse

from typing import List


def get_binary_coord(bsp_code: str, maximum: int, down_char: str,
                     up_char: str) -> int:
    low = 0
    high = maximum - 1
    for c in bsp_code:
        if c == down_char:
            high = (high + low) // 2
        elif c == up_char:
            low = -(-(high + low) // 2)
        else:
            assert False, "Encountered unknown character '{}' in '{}'".format(
                c, bsp_code)
    assert low == high, "{}, {}".format(low, high)
    return low


def get_y(bsp_code: str) -> int:
    return get_binary_coord(bsp_code, 128, "F", "B")


def get_x(bsp_code: str) -> int:
    return get_binary_coord(bsp_code, 8, "L", "R")


def get_seat_id(bsp_code: str) -> int:
    y = get_y(bsp_code[:7])
    x = get_x(bsp_code[-3:])
    return y * 8 + x


def get_seat_ids(path: str) -> List[int]:
    seat_ids = []
    with open(path, "r") as f:
        for line in f:
            seat_ids.append(get_seat_id(line.strip()))
    return seat_ids


parser = argparse.ArgumentParser(description="Solve day 5.")
parser.add_argument("boarding_pass_file", help="The boarding pass file.")
args = parser.parse_args()

seat_ids = get_seat_ids(args.boarding_pass_file)
possible_seat_ids = range(127 * 8 + 7)

candidate_seats = set(possible_seat_ids) - set(seat_ids)
my_seat = [
    s for s in candidate_seats if (s - 1) in seat_ids and (s + 1) in seat_ids
][0]
print(my_seat)
