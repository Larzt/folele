#pragma once
#include <esp_adc/adc_oneshot.h>

#include "blackboard.h"
#include "vector.h"

class Joystick
{
public:
  Joystick() = default;
  void Begin();
  Vector2 Read();

  ~Joystick() = default;

private:
  adc_oneshot_unit_handle_t adc_ = nullptr;
  adc_channel_t xChannel_;
  adc_channel_t yChannel_;
};
