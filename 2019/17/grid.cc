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

std::ostream& operator<<(std::ostream& os, const Coord& c) {
  os << "(" << c.x << ", " << c.y << ")";
  return os;
}
