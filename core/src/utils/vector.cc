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
