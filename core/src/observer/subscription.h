#pragma once
#include <functional>
#include <vector>

class Subscription
{

public:
  Subscription() = default;
  ~Subscription(std::function<void()> unsubscribe)
      : unsubscribe_(unsubscribe) {}

  void unsubscribe()
  {
    if (unsubscribe_)
    {
      unsubscribe_();
      unsubscribe_ = nullptr;
    }
  }

private:
  std::function<void()> unsuscribe_;
};
