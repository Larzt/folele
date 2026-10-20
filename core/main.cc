#include <Arduino.h>

#include "drone.h"
#include "logger.h"
#include "vector.h"

int main()
{
  // Initialize the arduino system
  init();
  wdt_enable(WDTO_4S);
  Logger::Begin();

  Drone drone;

  while (true)
  {
    loop();
  }
}

void loop() {

}
