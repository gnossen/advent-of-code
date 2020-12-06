import argparse

from typing import List

QUESTION_CHARS = (tuple(map(chr, range(97, 123))))

NO_PEEK = object()


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


# INDIVIDUAL : [a-z]
def consume_individual(lexer: Lexer) -> str:
    return consume_repeated_selection(lexer, QUESTION_CHARS)


# GROUP : INDIVIDUAL '\n' GROUP | INDIVIDUAL
def consume_group(lexer: Lexer) -> List[str]:
    individual = consume_individual(lexer)
    if lexer.get() != "\n" or lexer.peek() not in QUESTION_CHARS:
        return [individual]
    else:
        return [individual] + consume_group(lexer)


# GROUPS : GROUP '\n' GROUPS | GROUP EOF | EOF
def consume_groups(lexer: Lexer) -> List[List[str]]:
    if lexer.peek() is None:
        return []
    else:
        group = consume_group(lexer)
        if lexer.peek() is None:
            return [group]
        elif lexer.peek() == "\n":
            lexer.consume("\n")
            return [group] + consume_groups(lexer)
        else:
            assert False, "Unexpected character '{}'".format(lexer.peek())


def get_answers(path: str) -> List[List[str]]:
    lexer = Lexer(open(path, "r"))
    return consume_groups(lexer)


def count_unique_in_group(group: List[str]) -> int:
    return len(set.union(*map(set, group)))


parser = argparse.ArgumentParser(description="Solve day 6.")
parser.add_argument("customs_file", help="The customs file.")
args = parser.parse_args()

answers = get_answers(args.customs_file)
print(sum(map(count_unique_in_group, answers)))
