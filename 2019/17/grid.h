#ifndef _GRID_H
#define _GRID_H

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cassert>
#include <cstdlib>

struct Coord {
public:
  int64_t x;
  int64_t y;

  Coord();
  Coord(int64_t x, int64_t y);
  Coord operator+(Coord other) const;
  Coord operator-(Coord other) const;
  Coord operator/(int64_t scalar) const;
  Coord operator*(int64_t scalar) const;
};

template <typename T>
class Grid {
private:
  mutable Coord translation_;
  mutable Coord capacity_;

  bool has_points_;
  T default_elem_;

  mutable T *data_;

  static constexpr size_t kInitialWidth = 16;
  static constexpr size_t kInitialHeight = 16;

  Coord to_grid_native(Coord coord) const {
    return translation_ + coord;
  }

  bool out_of_bounds(Coord coord) const {
    if (coord.x < 0 || coord.x >= capacity_.x ||
        coord.y < 0 || coord.y >= capacity_.y) {
      return true;
    }
    return false;
  }

  // TODO: Move as much of this as possible out of the header.
  size_t determine_scale(int64_t capacity, int64_t requirement) const {
    int64_t directional_capacity = capacity / 2;
    int64_t difference = llabs(directional_capacity - requirement);
    size_t multiplier = 1;
    while (directional_capacity <= difference) {
      directional_capacity *= 2;
      multiplier *= 2;
    }
    return multiplier;
  }

  void grow_capacity(Coord coord) const {
    size_t x_scale = determine_scale(capacity_.x, coord.x);
    size_t y_scale = determine_scale(capacity_.x, coord.y);
    size_t scale = (x_scale > y_scale) ? x_scale : y_scale;
    assert(scale > 1);

    // printf("Growing capacity by scale %d.\n", scale);

    Coord new_capacity = capacity_ * scale;

    T *new_data = new T[new_capacity.x * new_capacity.y];

    // Set everything to unknown
    memset(new_data, 0, sizeof(T) * new_capacity.x * new_capacity.y);


    // Translation from the top left of the new buffer to top-left of the old one.
    Coord delta_translation = (new_capacity / 2) - (capacity_ / 2);

    // Overlay the existing data on top.
    for (size_t y = 0; y < capacity_.y; ++y) {
      for (size_t x = 0; x < capacity_.x; ++x) {
        size_t translated_x = x + delta_translation.x;
        size_t translated_y = y + delta_translation.y;
        new_data[translated_y * new_capacity.x + translated_x] = data_[y * capacity_.x + x];
      }
    }

    T *old_data = data_;
    data_ = new_data;
    translation_ = translation_ + delta_translation;
    capacity_ = new_capacity;

    delete [] old_data;
  }

public:

  Coord bounding_upper_left;
  Coord bounding_bottom_right;

  Grid() :
    capacity_(kInitialWidth, kInitialHeight),
    has_points_(false),
    default_elem_()
  {
    data_ = new T[capacity_.x * capacity_.y];
    memset(data_, 0, sizeof(T) * capacity_.x * capacity_.y);
  }

  ~Grid() {
    delete [] data_;
  }

  T& operator[](Coord coord) {
    // printf("grid_trace: set_point((%jd, %jd), %d)\n", coord.x, coord.y, status);
    if (!has_points_) {
      // Put the point at the center of the allocated grid.
      has_points_ = true;

      translation_ = (capacity_ / 2) - coord;
      bounding_upper_left = coord;
      bounding_bottom_right = coord;
    }

    if (coord.x < bounding_upper_left.x) { bounding_upper_left.x = coord.x; }
    if (coord.y < bounding_upper_left.y) { bounding_upper_left.y = coord.y; }
    if (coord.x > bounding_bottom_right.x) { bounding_bottom_right.x = coord.x; }
    if (coord.y > bounding_bottom_right.y) { bounding_bottom_right.y = coord.y; }

    Coord adjusted_coord = to_grid_native(coord);
    if (out_of_bounds(adjusted_coord)) {
      grow_capacity(adjusted_coord);
      adjusted_coord = to_grid_native(coord);
    }

    // printf("  stored (%jd, %jd) at (%jd, %jd)\n", coord.x, coord.y, adjusted_coord.x, adjusted_coord.y);
    return data_[adjusted_coord.y * capacity_.x + adjusted_coord.x];
  }

  const T& operator[](Coord coord) const {
    Coord adjusted_coord = to_grid_native(coord);

    // printf("  retrieving (%jd, %jd) from (%jd, %jd)\n", coord.x, coord.y, adjusted_coord.x, adjusted_coord.y);
    if (out_of_bounds(adjusted_coord)) {
      // printf("grid_trace: get_point((%jd, %jd)) = 0 (out of bounds)\n", coord.x, coord.y);
      return default_elem_;
    }

    const T& val = data_[adjusted_coord.y * capacity_.x + adjusted_coord.x];
    // printf("grid_trace: get_point((%jd, %jd)) = %d\n", coord.x, coord.y, val);
    return val;
  }

  const T& at(Coord coord) const {
    return (*this)[coord];
  }
};

#endif // _GRID_H
