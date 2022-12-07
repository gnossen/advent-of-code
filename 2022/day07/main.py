import sys
from typing import List

assert len(sys.argv) == 2

class File:
    def __init__(self, name, size):
        self._name = name
        self._size = size
        self._parent = None

    def name(self):
        return self._name

    def size(self):
        return self._size

    def set_parent(self, parent):
        self._parent = parent

    def get_parent(self):
        return self._parent

    def has_child(self, name):
        return False

    def get_child(self, name):
        assert False, "Files don't have children."

    def at_most_size(self, size):
        # Only directories count.
        return 0

class Directory:
    def __init__(self, name):
        self._name = name
        self._children = {}
        self._parent = None

    def name(self):
        return self._name

    def add_child(self, child):
        child.set_parent(self)
        self._children[child.name()] = child

    def size(self):
        total = 0
        for _, child in self._children.items():
            total += child.size()
        return total

    def set_parent(self, parent):
        self._parent = parent

    def get_parent(self):
        return self._parent

    def has_child(self, name):
        return name in self._children

    def get_child(self, name):
        return self._children[name]

    def at_most_size(self, size):
        total = 0
        for _, child in self._children.items():
            total += child.at_most_size(size)
        if self.size() <= size:
            total += self.size()
        return total

def parse_cmd(line: str) -> List[str]:
    parts = line.strip().split(" ")
    assert parts[0] == "$"
    return parts[1:]

INITIAL_MODE = 0
READ_CMD_OUTPUT = 1

with open(sys.argv[1], 'r') as f:
    mode = INITIAL_MODE
    root_directory = Directory("/")
    current_directory = None
    for line in f:
        line = line.strip()
        if mode == INITIAL_MODE:
            assert line.startswith("$")
            cmd = parse_cmd(line)
            assert cmd == ["cd",  "/"], cmd
            mode = READ_CMD_OUTPUT
            current_directory = root_directory
        elif mode == READ_CMD_OUTPUT and line.startswith("$"):
            # A new command has begun
            cmd = parse_cmd(line)
            if cmd[0] == "cd":
                assert len(cmd) == 2
                if cmd[1] == "..":
                    current_directory = current_directory.get_parent()
                else:
                    # assert not current_directory.has_child(cmd[1])
                    next_dir = None
                    if not current_directory.has_child(cmd[1]):
                        next_dir = Directory(cmd[1])
                    else:
                        next_dir = current_directory.get_child(cmd[1])
                        current_directory.add_child(next_dir)
                    current_directory = next_dir
            elif cmd[0] == "ls":
                assert len(cmd) == 1
            else:
                raise Exception("Unknown command {}".format(line))
            mode = READ_CMD_OUTPUT
        elif mode == READ_CMD_OUTPUT:
            parts = line.split(" ")
            assert len(parts) == 2
            if parts[0] == "dir":
                if not current_directory.has_child(parts[1]):
                    new_dir = Directory(parts[1])
                    current_directory.add_child(new_dir)
            elif parts[0].isdigit():
                if not current_directory.has_child(parts[1]):
                    new_file = File(parts[1], int(parts[0]))
                    current_directory.add_child(new_file)
            else:
                raise Exception("Unknown command output: {}".format(line))
    print(root_directory.at_most_size(100000))

