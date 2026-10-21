#include <Arduino.h>

#include "drone.h"
#include "joystick.h"
#include "logger.h"
#include "vector.h"

Drone drone;
Joystick joystick;
Vector2 JoyInput;

int main()
{
  // Initialize the arduino system
  init();
  wdt_enable(WDTO_4S);
  Logger::Begin();
  drone.Begin();
  joystick.Begin();

  Logger::Log(LoggerTag::None, "Sistema listo");

  while (true)
  {
    loop();
  }
}

void loop()
{

  Vector2 raw = joystick.Read();
  int dx = (raw.x() - JoyCenter) * 0.01;
  int dy = (raw.y() - JoyCenter) * 0.01;

  Vector2 input(dx, dy);
  if (JoyInput != input)
  {
    JoyInput = input;
    Logger::Log(LoggerTag::Joy, "Raw: ", raw, " Input: ", JoyInput);
    drone.SetMotorsVelocity(dx, dy);
  }

  wdt_reset();
}
