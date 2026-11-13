#pragma once

class Vector3
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

  bool operator!=(const Vector3 &v) const;
  ~Vector3() = default;

private:
  long x_;
  long y_;
  long z_;
};

class Vector2
{
public:
  Vector2();
  Vector2(long x, long y);

  inline void x(long val) { x_ = val; }
  inline void y(long val) { y_ = val; }

  inline long x() const { return x_; }
  inline long y() const { return y_; }

  bool operator!=(const Vector2 &v) const;
  ~Vector2() = default;

private:
  long x_;
  long y_;
};
