#include "vector.h"

#include <Print.h>

Vector3::Vector3()
{
  this->x_ = 0;
  this->y_ = 0;
  this->z_ = 0;
}

Vector3::Vector3(long x, long y, long z)
{
  this->x_ = x;
  this->y_ = y;
  this->z_ = z;
}

bool Vector3::operator!=(const Vector3 v)
{
return (this->x_ != v.x_ || this->y_ != v.y_ || this->z_ != v.z_);
}

size_t Vector3::printTo(Print &out) const
{
  size_t n = 0;
  n += out.print("(");
  n += out.print(x_);
  n += out.print(",");
  n += out.print(y_);
  n += out.print(",");
  n += out.print(z_);
  n += out.print(")");
  return n;
}

Vector2::Vector2()
{
  this->x_ = 0;
  this->y_ = 0;
}

Vector2::Vector2(long x, long y)
{
  this->x_ = x;
  this->y_ = y;
}

bool Vector2::operator!=(const Vector2 v)
{
  return (this->x_ != v.x_ || this->y_ != v.y_);
}

size_t Vector2::printTo(Print &out) const
{
  size_t n = 0;
  n += out.print("(");
  n += out.print(x_);
  n += out.print(",");
  n += out.print(y_);
  n += out.print(")");
  return n;
}
