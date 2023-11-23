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

    def __str__(self):
        return f"- {self._name} (file, size={self._size})"

class Directory:
    def __init__(self, name):
        self._name = name
        self._children = {}
        self._parent = None
        self._size = None

    def name(self):
        return self._name

    def add_child(self, child):
        child.set_parent(self)
        self._children[child.name()] = child
        self._size = None

    def size(self):
        total = 0
        for _, child in self._children.items():
            total += child.size()
        self._size  = total
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

    # Returns None if None is found, otherwise a Directory.
    def smallest_greater_than(self, size):
        smallest = None
        for child in self._children.values():
            if not isinstance(child, Directory):
                continue
            child_res = child.smallest_greater_than(size)
            if not child_res:
                continue
            if child_res.size() < size:
                continue
            if not smallest:
                smallest = child_res
            elif child_res.size() < smallest.size():
                smallest = child_res
        if not smallest and self.size() >= size:
            smallest = self
        return smallest

    def __repr__(self):
        return f"Directory({self._name}, size={self.size()})"

    __str__ = __repr__

    def full_tree(self):
        out = f"- {self._name} (dir, size={self.size()})\n"
        child_names = list(self._children.keys())
        child_names.sort(key=lambda c: self._children[c].size())
        for c in child_names:
            child = self._children[c]
            sub_out = str(child)
            for line in sub_out.split("\n"):
                out += ("  " + line + "\n")
        return out

    def get_directories(self):
        dirs = []
        for c in self._children.values():
            if not isinstance(c, Directory):
                continue
            dirs += c.get_directories()
        dirs += [self]
        dirs.sort(key=lambda d: d.size())
        return dirs

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

    disk_size = 70000000
    update_size = 30000000
    required_size = update_size - (disk_size - root_directory.size())
    print(root_directory.smallest_greater_than(required_size).size())

