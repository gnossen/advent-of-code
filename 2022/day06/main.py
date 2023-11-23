import sys

def get_char_count(s):
    chars = set()
    for c in s:
        chars |= {c}
    return len(chars)


def get_packet_index(s, packet_size):
    for i, k in enumerate(s):
        if i < 4:
            continue
        packet = s[i-packet_size:i]
        if get_char_count(packet) == packet_size:
            return i
    assert False


assert len(sys.argv) == 2

with open(sys.argv[1], "r") as f:
    s = f.read().strip()
    print(get_packet_index(s, 14))
