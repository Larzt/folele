#include "vector.h"

Vector3::Vector3() : x_(0), y_(0), z_(0) {}

Vector3::Vector3(long x, long y, long z) : x_(x), y_(y), z_(z) {}

bool Vector3::operator!=(const Vector3 &v) const
{
  return (x_ != v.x_ || y_ != v.y_ || z_ != v.z_);
}

Vector2::Vector2() : x_(0), y_(0) {}

Vector2::Vector2(long x, long y) : x_(x), y_(y) {}

bool Vector2::operator!=(const Vector2 &v) const
{
  return (x_ != v.x_ || y_ != v.y_);
}
