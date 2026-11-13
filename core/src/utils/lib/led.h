#pragma once
#include <driver/gpio.h>

class Led
{
public:
  explicit Led(gpio_num_t pin, bool state = false, bool activeLow = false)
      : pin_(pin), state_(state), activeLow_(activeLow) {}

  // Configures the pin: call from app_main(), not from a global constructor
  void Begin();

  void On();
  void Off();
  void Toggle();
  void Set(bool state);
  bool Get() const { return state_; }

private:
  void Apply();

  gpio_num_t pin_;
  bool state_;
  bool activeLow_;
};
