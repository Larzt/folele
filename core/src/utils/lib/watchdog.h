#pragma once

#include <esp_task_wdt.h>

#include "blackboard.h"

/**
 * Task watchdog for the main task: if Reset() is not called within the
 * timeout, the ESP32 panics and reboots.
 */
class Watchdog
{
public:
  // Call from app_main(): subscribes the current (main) task
  static void Begin(uint32_t timeoutMs = WatchdogTimeoutMs)
  {
    esp_task_wdt_config_t config = {
        .timeout_ms = timeoutMs,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    esp_task_wdt_reconfigure(&config);
    esp_task_wdt_add(nullptr);
  }

  static void Reset() { esp_task_wdt_reset(); }

private:
  Watchdog() = default;
  ~Watchdog() = default;
};
