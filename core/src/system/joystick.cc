#include "joystick.h"

void Joystick::Begin()
{
  pinMode(XAxisPin, INPUT);
  pinMode(YAxisPin, INPUT);
}

Vector2 Joystick::Read()
{
  return Vector2(analogRead(XAxisPin), analogRead(YAxisPin));
}
