#pragma once

#include <Printable.h>

class Vector3 : public Printable
{
public:
  Vector3();
  Vector3(long x, long y, long z);

  inline void x(long val) { x_ = val; }
  inline void y(long val) { y_ = val; }
  inline void z(long val) { z_ = val; }

  inline long x() const { return x_; }
  inline long y() const { return y_; }
  inline long z() const { return z_; }

  // Permite hacer Serial.print(v) y Logger::Log(..., v)
  size_t printTo(Print &out) const override;
  ~Vector3() = default;

private:
  long x_;
  long y_;
  long z_;
};
