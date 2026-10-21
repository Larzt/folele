#pragma once
#include <Arduino.h>

#include "vector.h"

const uint8_t XAxisPin = A0;
const uint8_t YAxisPin = A1;
// Lectura del ADC (0-1023) con el joystick en reposo
const int JoyCenter = 512;

class Joystick
{
public:
  Joystick() = default;
  void Begin();
  Vector2 Read();

  ~Joystick() = default;

private:
};
