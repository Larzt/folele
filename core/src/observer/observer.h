#pragma once
#include "subscription.h"

#include <functional>
#include <iterator>
#include <vector>

template <typename T>
class Observable
{
public:
  using Callback = std::function<void(const T &)>;

  Observable() : value_{} {}
  explicit Observable(T initial_value) : value_(initial_value) {}

  Subscription Subscribe(Callback callback)
  {
    callbacks_.push_back(callback);
    auto it = std::prev(callbacks_.end());

    return Subscription([this, it]()
                        { callbacks_.erase(it); });
  }

  void Set(const T &new_value)
  {
    if (value_ != new_value)
    {
      value_ = new_value;
      Notify();
    }
  }

  T Get() const { return value_; }

  Observable &operator=(const T &new_value)
  {
    Set(new_value);
    return *this;
  }

private:
  T value_;
  std::vector<Callback> callbacks_;

  void Notify()
  {
    for (auto &cb : callbacks_)
    {
      cb(value_);
    }
  }
};
