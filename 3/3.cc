#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Vector {
  int64_t x;
  int64_t y;

  Vector() : x(0), y(0) {}
  Vector(int64_t x, int64_t y) : x(x), y(y) {}

  static Vector from_direction(char direction, int64_t length) {
    if (direction == 'U') {
      return Vector(0, length);
    } else if (direction == 'D') {
      return Vector(0, -1 * length);
    } else if (direction == 'R') {
      return Vector(length, 0);
    } else if (direction == 'L') {
      return Vector(-1 * length, 0);
    } else {
      throw std::runtime_error("Unknown direction encountered.");
    }
  }

  Vector operator+(const Vector &other) {
    return Vector(x + other.x, y + other.y);
  }

  Vector &operator+=(const Vector &other) {
    x += other.x;
    y += other.y;
    return *this;
  }

  friend std::ostream &operator<<(std::ostream &os, const Vector &v);
};

std::ostream &operator<<(std::ostream &os, const Vector &v) {
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

struct LineSegment {
  Vector a;
  Vector b;

  LineSegment(Vector a, Vector b) : a(a), b(b) {}

  friend std::ostream &operator<<(std::ostream &os, const LineSegment &s);

  bool vertical() const { return a.x == b.x; }

  bool horizontal() const { return a.y == b.y; }

  int64_t left() const { return std::min(a.x, b.x); }

  int64_t right() const { return std::max(a.x, b.x); }

  int64_t bottom() const { return std::min(a.y, b.y); }

  int64_t top() const { return std::max(a.y, b.y); }
};

std::ostream &operator<<(std::ostream &os, const LineSegment &s) {
  os << "(" << s.a << ", " << s.b << ")";
  return os;
}

bool intersects(const LineSegment &a, const LineSegment &b,
                std::vector<Vector> &intersections) {
  if (a.vertical() && b.vertical()) {
    if (a.a.x != b.a.x) {
      return false;
    }
    int64_t top = std::min(a.top(), b.top());
    int64_t bottom = std::max(a.bottom(), b.bottom());
    if (top >= bottom) {
      for (int64_t i = bottom; i <= top; ++i) {
        intersections.emplace_back(a.a.x, i);
      }
      return true;
    }
  } else if (a.horizontal() && b.vertical()) {
    if (b.a.x < a.left() || b.a.x > a.right()) {
      return false;
    }
    if (a.a.y < b.bottom() || a.a.y > b.top()) {
      return false;
    }
    intersections.emplace_back(b.a.x, a.a.y);
    return true;
  } else if (a.vertical() && b.horizontal()) {
    return intersects(b, a, intersections);
  } else {
    if (a.a.y != b.a.y) {
      return false;
    }
    int64_t right = std::min(a.right(), b.right());
    int64_t left = std::max(a.left(), b.left());
    if (right >= left) {
      for (int64_t i = left; i <= right; ++i) {
        intersections.emplace_back(i, a.a.y);
      }
    }
    return true;
  }
  return false;
}

template <typename Container>
bool get_segment(std::istream &is, Container &container) {
  if (is.eof()) {
    return false;
  }
  char direction;
  int64_t length;
  is >> direction;
  is >> length;
  if (is.peek() == ',') {
    is.get();
  }
  container.push_back(Vector::from_direction(direction, length));
  return true;
}

template <typename Container>
void get_points(Container &a, Container &b, std::istream &is) {
  while (is.peek() != '\n') {
    get_segment(is, a);
  }
  is.get();
  while (get_segment(is, b))
    ;
}

template <typename VectorContainer, typename LineSegmentContainer>
void relative_to_absolute(const VectorContainer &relative,
                          LineSegmentContainer &absolute) {
  Vector current;
  for (const auto &v : relative) {
    Vector next = current + v;
    absolute.emplace_back(current, next);
    current = next;
  }
}

template <typename Container>
void get_wires_as_line_segments(Container &a, Container &b, std::istream &is) {
  std::vector<Vector> points_a;
  std::vector<Vector> points_b;
  get_points(points_a, points_b, is);
  relative_to_absolute(points_a, a);
  relative_to_absolute(points_b, b);
}

int64_t manhattan_distance(const Vector &a, const Vector &b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
}

int64_t manhattan_distance(const Vector &a) {
  return manhattan_distance(a, Vector(0, 0));
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " filename" << std::endl;
    return 1;
  }
  std::fstream fs;
  fs.open(argv[1], std::fstream::in);
  std::vector<LineSegment> wire_a;
  std::vector<LineSegment> wire_b;
  get_wires_as_line_segments(wire_a, wire_b, fs);
  std::vector<Vector> intersections;
  for (const auto &a : wire_a) {
    for (const auto &b : wire_b) {
      intersects(a, b, intersections);
    }
  }
  std::sort(intersections.begin(), intersections.end(),
            [](const Vector &a, const Vector &b) {
              return manhattan_distance(a) < manhattan_distance(b);
            });
  if (intersections.size() <= 1) {
    std::cerr << "No intersections found!" << std::endl;
    return 1;
  }
  std::cout << manhattan_distance(intersections[1]) << std::endl;
  return 0;
}
