#include "wake_button.h"

#include <esp_err.h>
#include <esp_sleep.h>

void WakeButton::Begin()
{
  // The task must exist before the ISR can notify it
  xTaskCreate(&WakeButton::Task, "WakeButton", 3072, nullptr, 5, &task_);

  gpio_config_t io = {};
  io.pin_bit_mask = 1ULL << WakeButtonPin;
  io.mode = GPIO_MODE_INPUT;
  io.pull_up_en = GPIO_PULLUP_ENABLE;
  io.intr_type = GPIO_INTR_LOW_LEVEL;
  ESP_ERROR_CHECK(gpio_config(&io));

  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  ESP_ERROR_CHECK(gpio_isr_handler_add(WakeButtonPin, &WakeButton::Isr, nullptr));

  // Wake-up source for (automatic) light sleep
  ESP_ERROR_CHECK(gpio_wakeup_enable(WakeButtonPin, GPIO_INTR_LOW_LEVEL));
  ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());

  Logger::Log(LoggerTag::Power, "Wake button ready on GPIO", (int)WakeButtonPin);
}

void WakeButton::Isr(void *arg)
{
  // Level interrupt: mask it until the task has handled the press
  gpio_intr_disable(WakeButtonPin);

  BaseType_t woken = pdFALSE;
  vTaskNotifyGiveFromISR(task_, &woken);
  portYIELD_FROM_ISR(woken);
}

void WakeButton::Task(void *arg)
{
  while (true)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    // Debounce: ignore glitches shorter than ButtonDebounceMs
    vTaskDelay(pdMS_TO_TICKS(ButtonDebounceMs));
    if (gpio_get_level(WakeButtonPin) == 0)
    {
      Logger::Log(LoggerTag::Power, "Wake button pressed");
      if (PowerManager::IsFlying())
      {
        PowerManager::Disarm();
      }
      else
      {
        PowerManager::Arm();
      }

      // Wait for the release so a long press counts once
      while (gpio_get_level(WakeButtonPin) == 0)
      {
        vTaskDelay(pdMS_TO_TICKS(20));
      }
      vTaskDelay(pdMS_TO_TICKS(ButtonDebounceMs));
    }

    gpio_intr_enable(WakeButtonPin);
  }
}
