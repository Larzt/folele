#pragma once
#include <functional>

class Subscription
{
public:
  Subscription() = default;

  Subscription(std::function<void()> unsubscribe)
      : unsubscribe_(unsubscribe) {}

  void Unsubscribe()
  {
    if (unsubscribe_)
    {
      unsubscribe_();
      unsubscribe_ = nullptr;
    }
  }

private:
  std::function<void()> unsubscribe_;
};
