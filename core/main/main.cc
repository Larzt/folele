#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

#include "blackboard.h"
#include "drone.h"
#include "joystick.h"
#include "logger.h"
#include "network.h"
#include "power_manager.h"
#include "server.h"
#include "vector.h"
#include "wake_button.h"
#include "watchdog.h"

Drone drone;
Joystick joystick;
Vector2 JoyInput;
Network network;
Server server(network);

extern "C" void app_main()
{
  Watchdog::Begin();
  PowerManager::Begin();
  WakeButton::Begin();
  drone.Begin();
  joystick.Begin();

  network.Connect();
  server.Start();
  Watchdog::Reset();

  Logger::Log(LoggerTag::None, "System ready");

  PowerState lastState = PowerState::Idle;
  int64_t lastStatsUs = esp_timer_get_time();

  while (true)
  {
    // Idle: motors stopped, block so the chip can light sleep until /arm
    if (!PowerManager::IsFlying())
    {
      if (lastState == PowerState::Flying)
      {
        drone.StopMotors();
        lastState = PowerState::Idle;
      }

      PowerManager::WaitUntilArmed(IdleLoopPeriodMs);
      Watchdog::Reset();

      if (esp_timer_get_time() - lastStatsUs >= PowerStatsPeriodMs * 1000LL)
      {
        PowerManager::LogStats();
        lastStatsUs = esp_timer_get_time();
      }
      continue;
    }

    // Flying: normal mode, the motors follow the joystick
    if (lastState == PowerState::Idle)
    {
      JoyInput = Vector2();
      lastState = PowerState::Flying;
    }

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

    Watchdog::Reset();
    vTaskDelay(pdMS_TO_TICKS(LoopPeriodMs));
  }
}
