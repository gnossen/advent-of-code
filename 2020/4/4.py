import argparse
from typing import Any, Dict, List, Tuple
import sys

Passport = List[Tuple[str, Any]]

NO_PEEK = object()
SPACE_CHARS = (" ", "\n")
LVALUE_CHARS = (tuple(map(chr, range(97, 123))) +
                tuple(map(chr, range(65, 91))))
RVALUE_CHARS = LVALUE_CHARS + tuple(map(chr, range(48, 58))) + ("#", )
HEX_CHARS = (tuple(map(chr, range(97, 103))) + tuple(map(chr, range(48, 58))))

VALID_EYE_COLORS = ("amb", "blu", "brn", "gry", "grn", "hzl", "oth")
POSSIBLE_FIELDS = (
    "byr",
    "iyr",
    "eyr",
    "hgt",
    "hcl",
    "ecl",
    "pid",
    "cid",
)

REQUIRED_FIELDS = tuple(set(POSSIBLE_FIELDS) - {"cid"})


class Lexer:
    def __init__(self, f):
        self._f = f
        self._prev = None
        self._c = None
        self._peek = NO_PEEK

    def __del__(self):
        self._f.close()

    def peek(self) -> str:
        if self._peek is not NO_PEEK:
            return self._peek
        else:
            self._peek = self._f.read(1)
            self._peek = self._peek if self._peek else None
            return self._peek

    def get(self) -> str:
        if self._peek is not NO_PEEK:
            self._prev = self._c
            self._c = self._peek
            self._peek = NO_PEEK
        else:
            self._prev = self._c
            self._c = self._f.read(1)
        if self._c:
            return self._c
        else:
            return None

    def consume(self, c: str) -> str:
        assert self.get() == c

    def prev(self) -> str:
        return self._prev


def consume_repeated_selection(lexer, selection) -> str:
    value = ""
    c = lexer.get()
    assert c in selection, "'{}' was not in expected set {}".format(
        c, selection)
    value += c
    while (c := lexer.peek()) in selection:
        lexer.get()
        value += c
    return value


# RVALUE: [a-zA-Z0-9#]+
def consume_rvalue(lexer) -> str:
    return consume_repeated_selection(lexer, RVALUE_CHARS)


# LVALUE: [a-zA-Z]+
def consume_lvalue(lexer) -> str:
    return consume_repeated_selection(lexer, LVALUE_CHARS)


# KV_PAIR: LVALUE ":" RVALUE
def consume_kv_pair(lexer: Lexer) -> Tuple[str, str]:
    key = consume_lvalue(lexer)
    lexer.consume(":")
    value = consume_rvalue(lexer)
    return (key, value)


# SPACE: ' ' | '\n'
def consume_space(lexer: Lexer) -> bool:
    c = lexer.get()
    return c in SPACE_CHARS


# PASSPORT : KV_PAIR SPACE PASSPORT | KV_PAIR
def consume_passport(lexer: Lexer) -> Passport:
    kv_pair = consume_kv_pair(lexer)
    if not consume_space(lexer) or lexer.peek() not in LVALUE_CHARS:
        return [kv_pair]
    else:
        return [kv_pair] + consume_passport(lexer)


# PASSPORTS : PASSPORT '\n' PASSPORTS | PASSPORT EOF | EOF
def consume_passports(lexer: Lexer) -> List[Passport]:
    if lexer.peek() is None:
        return []
    else:
        passport = consume_passport(lexer)
        if lexer.peek() is None:
            return [passport]
        elif lexer.peek() == "\n":
            lexer.consume("\n")
            return [passport] + consume_passports(lexer)
        else:
            assert False, "Unexpected character '{}'".format(lexer.peek())


def get_passports(path: str) -> List[Passport]:
    lexer = Lexer(open(path, 'r'))
    return consume_passports(lexer)


def validate_4_digit_year(value: str, lower: int, upper: int) -> bool:
    if len(value) != 4:
        return False
    int_value = int(value)
    if not (lower <= int_value <= upper):
        return False
    return True


def validate_byr(value: str) -> bool:
    return validate_4_digit_year(value, 1920, 2002)


def validate_iyr(value: str) -> bool:
    return validate_4_digit_year(value, 2010, 2020)


def validate_eyr(value: str) -> bool:
    return validate_4_digit_year(value, 2020, 2030)


def validate_hgt(value: str) -> bool:
    if value.endswith("cm"):
        int_value = int(value[:-2])
        return 150 <= int_value <= 193
    elif value.endswith("in"):
        int_value = int(value[:-2])
        return 59 <= int_value <= 76
    else:
        return False


def validate_hcl(value: str) -> bool:
    if not value.startswith("#"):
        return False
    if len(value) != 7:
        return False
    for c in value[1:]:
        if c not in HEX_CHARS:
            return False
    return True


def validate_ecl(value: str) -> bool:
    return value in VALID_EYE_COLORS


def validate_pid(value: str) -> bool:
    if len(value) != 9:
        return False
    for c in value:
        if not c.isdigit():
            return False
    return True


def is_valid(passport: List[Tuple[str, str]]) -> bool:
    passport_dict = dict(passport)
    keys = set(passport_dict.keys())
    if keys - set(POSSIBLE_FIELDS):
        return False
    if set(REQUIRED_FIELDS) - keys:
        return False
    validations = {
        "byr": validate_byr,
        "iyr": validate_iyr,
        "eyr": validate_eyr,
        "hgt": validate_hgt,
        "hcl": validate_hcl,
        "ecl": validate_ecl,
        "pid": validate_pid,
    }
    for key, validation_fn in validations.items():
        if not validation_fn(passport_dict[key]):
            return False
    return True


parser = argparse.ArgumentParser(description="Solve day 4.")
parser.add_argument("passport_file", help="The passport file.")
args = parser.parse_args()

passports = get_passports(args.passport_file)

print(sum(int(is_valid(passport)) for passport in passports))
