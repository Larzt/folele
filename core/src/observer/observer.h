#pragma once
#include "subscription.hpp"

#include <vector>
#include <functional>

template <typename T>
class Observable
{
public:
  using Callback = std::function<void(const T &)>;

  Observable() : _value{} {}
  explicit Observable(T initial_value) : _value(initial_value) {}

  Subscription subscribe(Callback callback)
  {
    _callbacks.push_back(callback);
    auto it = std::prev(_callbacks.end());

    return Subscription([this, it]()
                        { _callbacks.erase(it); });
  }

  void set(const T &new_value)
  {
    if (_value != new_value)
    {
      _value = new_value;
      notify();
    }
  }

  T get() const { return _value; }

  Observable &operator=(const T &new_value)
  {
    set(new_value);
    return *this;
  }

private:
  T _value;
  std::vector<Callback> _callbacks;

  void notify()
  {
    for (auto &cb : _callbacks)
    {
      cb(_value);
    }
  }
};
