#include "grid.h"

Coord::Coord() : x(0), y(0) {}
Coord::Coord(int64_t x, int64_t y) : x(x), y(y) {}

Coord Coord::operator+(Coord other) const {
  return Coord(x + other.x, y + other.y);
}

Coord Coord::operator-(Coord other) const {
  return Coord(x - other.x, y - other.y);
}

Coord Coord::operator/(int64_t scalar) const {
  return Coord(x / scalar, y / scalar);
}

Coord Coord::operator*(int64_t scalar) const {
  return Coord(x * scalar, y * scalar);
}

bool Coord::operator==(const Coord& other) const {
  return x == other.x && y == other.y;
}

bool Coord::operator!=(const Coord& other) const {
  return !(*this == other);
}

bool Coord::operator<(const Coord& other) const {
  if (x < other.x) {
    return true;
  }
  if (x > other.x) {
    return false;
  }
  return y < other.y;
}

std::ostream& operator<<(std::ostream& os, const Coord& c) {
  os << "(" << c.x << ", " << c.y << ")";
  return os;
}

