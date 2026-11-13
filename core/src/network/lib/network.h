#pragma once
#include <string>

#include <esp_event.h>
#include <esp_netif.h>
#include <esp_wifi.h>

#include "blackboard.h"
#include "led.h"
#include "logger.h"

/**
 * WiFi: connects as a station (STA) with the credentials saved in NVS.
 * If there are none, or the connection fails MaxConnectionRetries times,
 * it falls back to Access Point (AP) mode.
 */
class Network
{
public:
  Network() : led_(NetLed) {}
  ~Network() = default;

  void Connect();
  void Reconnect();
  void StartAccessPoint();
  static bool LoadNetCredentials(std::string &ssid, std::string &pass);
  static void SaveNetCredentials(const std::string &ssid, const std::string &pass);

  static bool LoadApCredentials(std::string &ssid, std::string &pass);
  static void SaveApCredentials(const std::string &ssid, const std::string &pass);
  void ApplyApCredentials();

private:
  Led led_;
  static inline int currentStationRetries_ = 0;

  esp_netif_t *apNetif_ = nullptr;
  static void EventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId, void *eventData);
};
