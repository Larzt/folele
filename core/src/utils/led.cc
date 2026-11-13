#include "led.h"

void Led::Begin()
{
  gpio_reset_pin(pin_);
  gpio_set_direction(pin_, GPIO_MODE_OUTPUT);

  Apply();
}

void Led::On()
{
  state_ = true;
  Apply();
}

void Led::Off()
{
  state_ = false;
  Apply();
}

void Led::Toggle()
{
  state_ = !state_;
  Apply();
}

void Led::Set(bool state)
{
  state_ = state;
  Apply();
}

void Led::Apply()
{
  int level = activeLow_ ? !state_ : state_;
  gpio_set_level(pin_, level);
}
