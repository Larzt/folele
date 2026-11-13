#pragma once
#include <atomic>
#include <string>

#include <esp_pm.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>

#include "blackboard.h"
#include "enums.h"
#include "logger.h"

/**
 * Power modes of the drone:
 *   Idle   (landed, disarmed): automatic light sleep. The CPU sleeps whenever
 *          every task is blocked and wakes up on timers or WiFi traffic.
 *   Flying (armed): light sleep is blocked so the control loop never stops;
 *          the CPU still scales between PowerMinFreqMhz and PowerMaxFreqMhz.
 *
 * Arm()/Disarm() can be called from any task (e.g. the HTTP server).
 * The motors are only driven from the main loop.
 */
// Time spent in light sleep (from the esp_pm profiling, since boot)
struct PowerStats
{
  int64_t uptimeUs = 0;
  int64_t sleepUs = 0;
  uint32_t sleepCount = 0;
};

class PowerManager
{
public:
  // Call from app_main() before anything calls Arm()/Disarm()
  static void Begin();

  static void Arm();
  static void Disarm();

  static PowerState State() { return state_.load(); }
  static bool IsFlying() { return State() == PowerState::Flying; }
  static const char *StateName(PowerState state);

  // Blocks the caller until the drone is armed or the timeout expires.
  // While blocked the chip can enter light sleep.
  static bool WaitUntilArmed(uint32_t timeoutMs);

  // Light sleep statistics. Returns false if profiling is disabled.
  static bool GetStats(PowerStats &stats);
  // Raw esp_pm report (locks + time per mode)
  static std::string Report();
  // Logs the % of time in light sleep since the previous call
  static void LogStats();

private:
  static constexpr EventBits_t ArmedBit = BIT0;

  static inline std::atomic<PowerState> state_{PowerState::Idle};
  static inline esp_pm_lock_handle_t noSleepLock_ = nullptr;
  static inline EventGroupHandle_t events_ = nullptr;
  static inline PowerStats lastStats_;

  PowerManager() = default;
  ~PowerManager() = default;
};
