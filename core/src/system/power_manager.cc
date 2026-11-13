#include "power_manager.h"

#include <esp_err.h>
#include <esp_wifi.h>

void PowerManager::Begin()
{
  events_ = xEventGroupCreate();

  esp_pm_config_t config = {};
  config.max_freq_mhz = PowerMaxFreqMhz;
  config.min_freq_mhz = PowerMinFreqMhz;
  config.light_sleep_enable = true;
  ESP_ERROR_CHECK(esp_pm_configure(&config));

  // Held while flying: forbids light sleep
  ESP_ERROR_CHECK(esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, "flying", &noSleepLock_));

  Logger::Log(LoggerTag::Power, "Idle: automatic light sleep enabled");
}

void PowerManager::Arm()
{
  PowerState expected = PowerState::Idle;
  if (!state_.compare_exchange_strong(expected, PowerState::Flying))
  {
    return;
  }

  ESP_ERROR_CHECK(esp_pm_lock_acquire(noSleepLock_));
  // Lowest latency for the commands while flying (only applies in STA mode)
  esp_wifi_set_ps(WIFI_PS_NONE);
  xEventGroupSetBits(events_, ArmedBit);

  Logger::Log(LoggerTag::Power, "Flying: light sleep disabled");
}

void PowerManager::Disarm()
{
  PowerState expected = PowerState::Flying;
  if (!state_.compare_exchange_strong(expected, PowerState::Idle))
  {
    return;
  }

  xEventGroupClearBits(events_, ArmedBit);
  // Radio sleeps between beacons again (only applies in STA mode)
  esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
  ESP_ERROR_CHECK(esp_pm_lock_release(noSleepLock_));

  Logger::Log(LoggerTag::Power, "Idle: automatic light sleep enabled");
}

const char *PowerManager::StateName(PowerState state)
{
  switch (state)
  {
  case PowerState::Idle:
    return "idle";
  case PowerState::Flying:
    return "flying";
  }
  return "unknown";
}

bool PowerManager::WaitUntilArmed(uint32_t timeoutMs)
{
  EventBits_t bits = xEventGroupWaitBits(events_, ArmedBit, pdFALSE, pdTRUE, pdMS_TO_TICKS(timeoutMs));
  return (bits & ArmedBit) != 0;
}
