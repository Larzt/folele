#include "power_manager.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

#include <esp_err.h>
#include <esp_timer.h>
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

std::string PowerManager::Report()
{
  // esp_pm only prints to a FILE*: write it into a memory buffer
  std::string buffer(2048, '\0');
  FILE *stream = fmemopen(buffer.data(), buffer.size(), "w");
  if (stream == nullptr)
  {
    return "";
  }
  esp_pm_dump_locks(stream);
  fclose(stream);

  buffer.resize(strlen(buffer.c_str()));
  return buffer;
}

bool PowerManager::GetStats(PowerStats &stats)
{
  std::string report = Report();

  // "SLEEP     80 M        123456      42%": time (us) is the second to last field
  size_t line = report.find("\nSLEEP");
  if (line == std::string::npos)
  {
    return false;
  }
  size_t end = report.find('\n', line + 1);
  std::string fields = report.substr(line + 1, end - line - 1);

  std::vector<std::string> tokens;
  size_t pos = 0;
  while (pos < fields.size())
  {
    size_t start = fields.find_first_not_of(' ', pos);
    if (start == std::string::npos)
      break;
    size_t stop = fields.find(' ', start);
    tokens.push_back(fields.substr(start, stop - start));
    pos = stop;
  }
  if (tokens.size() < 2)
  {
    return false;
  }

  stats.sleepUs = atoll(tokens[tokens.size() - 2].c_str());
  stats.uptimeUs = esp_timer_get_time();

  size_t counts = report.find("light_sleep_counts:");
  stats.sleepCount = counts == std::string::npos ? 0 : strtoul(report.c_str() + counts + 19, nullptr, 10);
  return true;
}

void PowerManager::LogStats()
{
  PowerStats now;
  if (!GetStats(now))
  {
    return;
  }

  int64_t window = now.uptimeUs - lastStats_.uptimeUs;
  int64_t slept = now.sleepUs - lastStats_.sleepUs;
  int percent = window > 0 ? (int)(slept * 100 / window) : 0;

  Logger::Log(LoggerTag::Power, "Light sleep: ", percent, "% of the last ",
              (int)(window / 1000000), " s (", (unsigned long)(now.sleepCount - lastStats_.sleepCount), " times)");
  lastStats_ = now;
}
