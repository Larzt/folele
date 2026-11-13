#pragma once
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "blackboard.h"
#include "logger.h"
#include "power_manager.h"

/**
 * Physical way to wake the drone up: the BOOT button (GPIO0).
 *
 * - Light sleep wake-up source: on the ESP32 only *level* GPIO triggers can
 *   wake the chip, so the pin is configured as a low-level interrupt.
 * - Interrupt: the ISR disables the pin interrupt (a level interrupt would
 *   fire continuously while the button is held) and notifies a task.
 * - The task debounces, toggles Idle <-> Flying and re-enables the interrupt
 *   once the button is released. The ISR does no work besides that.
 */
class WakeButton
{
public:
  // Call from app_main() after PowerManager::Begin()
  static void Begin();

private:
  static void Isr(void *arg);
  static void Task(void *arg);

  static inline TaskHandle_t task_ = nullptr;

  WakeButton() = default;
  ~WakeButton() = default;
};
