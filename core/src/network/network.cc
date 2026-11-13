#include "network.h"

#include <cstring>
#include <string>

#include <nvs.h>
#include <nvs_flash.h>

void Network::Connect()
{
  std::string ssid, password;

  led_.Begin();

  esp_err_t res = nvs_flash_init();
  if (res == ESP_ERR_NVS_NO_FREE_PAGES || res == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    res = nvs_flash_init();
  }
  ESP_ERROR_CHECK(res);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &EventHandler, this, NULL));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, &EventHandler, this, NULL));

  bool hasSta = LoadNetCredentials(ssid, password);

  wifi_config_t staConfig = {};

  if (hasSta)
  {
    strlcpy((char *)staConfig.sta.ssid, ssid.c_str(), sizeof(staConfig.sta.ssid));
    strlcpy((char *)staConfig.sta.password, password.c_str(), sizeof(staConfig.sta.password));

    esp_netif_create_default_wifi_sta();

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &staConfig));
  }
  else
  {
    Logger::Log(LoggerTag::Network, "Fallback a Access Point");
    esp_wifi_disconnect();
    esp_wifi_stop();
    // StartAccessPoint() creates the AP interface (only once)
    this->StartAccessPoint();
  }

  ESP_ERROR_CHECK(esp_wifi_start());
}

void Network::StartAccessPoint()
{
  if (apNetif_ == nullptr)
  {
    apNetif_ = esp_netif_create_default_wifi_ap();
  }

  wifi_config_t apConfig = {};

  std::string ssid, pass;
  if (LoadApCredentials(ssid, pass))
  {
    AccessSSID = ssid;
    AccessPASS = pass;
  }
  else
  {
    ssid = AccessSSID;
    pass = AccessPASS;
  }

  strlcpy((char *)apConfig.ap.ssid, ssid.c_str(), sizeof(apConfig.ap.ssid));
  strlcpy((char *)apConfig.ap.password, pass.c_str(), sizeof(apConfig.ap.password));

  apConfig.ap.authmode = pass.length() >= 8 ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;
  apConfig.ap.max_connection = MaxNetConnections;
  apConfig.ap.channel = AccessPointChannel;

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &apConfig));
  ESP_ERROR_CHECK(esp_wifi_start());

  esp_netif_ip_info_t ipInfo;
  if (esp_netif_get_ip_info(apNetif_, &ipInfo) == ESP_OK)
  {
    char ipStr[16];
    snprintf(ipStr, sizeof(ipStr), IPSTR, IP2STR(&ipInfo.ip));
    LocalIpAddress = ipStr;
  }

  Logger::Log(LoggerTag::Network, "Access Point '", ssid, "' IP: ", LocalIpAddress.Get());
  CurrentNetworkMode = NetworkMode::Access;
}

bool Network::LoadNetCredentials(std::string &ssid, std::string &password)
{
  nvs_handle_t handle;
  if (nvs_open(WIFI_NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK)
  {
    CurrentServerMode.Set(ServerMode::Developer);
    return false;
  }

  uint8_t modeVal = 0;
  if (nvs_get_u8(handle, "srv_mode", &modeVal) == ESP_OK)
  {
    CurrentServerMode.Set(static_cast<ServerMode>(modeVal));
  }
  else
  {
    CurrentServerMode.Set(ServerMode::Developer);
  }

  size_t ssidLen = 0, passLen = 0;

  if (nvs_get_str(handle, "ssid", NULL, &ssidLen) != ESP_OK ||
      nvs_get_str(handle, "password", NULL, &passLen) != ESP_OK)
  {
    nvs_close(handle);
    return false;
  }

  std::string ssidBuf(ssidLen, '\0');
  std::string passBuf(passLen, '\0');

  nvs_get_str(handle, "ssid", ssidBuf.data(), &ssidLen);
  nvs_get_str(handle, "password", passBuf.data(), &passLen);

  ssid = ssidBuf.c_str();
  password = passBuf.c_str();

  nvs_close(handle);
  return true;
}

void Network::Reconnect()
{
  std::string ssid, pass;
  if (!LoadNetCredentials(ssid, pass))
  {
    Logger::Log(LoggerTag::Network, "No STA credentials saved, cannot reconnect");
    return;
  }

  Logger::Log(LoggerTag::Network, "Manual reconnect to ", ssid);
  currentStationRetries_ = 0;

  WifiIpAddress = "0.0.0.0";
  CurrentNetworkMode = NetworkMode::Access;

  wifi_config_t staConfig = {};
  strlcpy((char *)staConfig.sta.ssid, ssid.c_str(), sizeof(staConfig.sta.ssid));
  strlcpy((char *)staConfig.sta.password, pass.c_str(), sizeof(staConfig.sta.password));

  esp_wifi_stop();
  esp_wifi_set_mode(WIFI_MODE_STA);
  esp_wifi_set_config(WIFI_IF_STA, &staConfig);
  esp_wifi_start();
}

bool Network::LoadApCredentials(std::string &ssid, std::string &password)
{
  nvs_handle_t handle;
  if (nvs_open(WIFI_NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK)
    return false;

  size_t ssidLen = 0, passLen = 0;

  if (nvs_get_str(handle, "ap_ssid", NULL, &ssidLen) != ESP_OK ||
      nvs_get_str(handle, "ap_pass", NULL, &passLen) != ESP_OK)
  {
    nvs_close(handle);
    return false;
  }

  std::string ssidBuf(ssidLen, '\0');
  std::string passBuf(passLen, '\0');

  nvs_get_str(handle, "ap_ssid", ssidBuf.data(), &ssidLen);
  nvs_get_str(handle, "ap_pass", passBuf.data(), &passLen);

  ssid = ssidBuf.c_str();
  password = passBuf.c_str();

  nvs_close(handle);
  return true;
}

void Network::SaveApCredentials(const std::string &ssid, const std::string &password)
{
  nvs_handle_t handle;
  ESP_ERROR_CHECK(nvs_open(WIFI_NVS_NAMESPACE, NVS_READWRITE, &handle));

  ESP_ERROR_CHECK(nvs_set_str(handle, "ap_ssid", ssid.c_str()));
  ESP_ERROR_CHECK(nvs_set_str(handle, "ap_pass", password.c_str()));

  ESP_ERROR_CHECK(nvs_commit(handle));
  nvs_close(handle);

  AccessSSID = ssid;
  AccessPASS = password;
}

void Network::ApplyApCredentials()
{
  if (CurrentNetworkMode.Get() != NetworkMode::Access)
    return;

  wifi_config_t apConfig = {};
  std::string ssid = AccessSSID;
  std::string pass = AccessPASS;

  strlcpy((char *)apConfig.ap.ssid, ssid.c_str(), sizeof(apConfig.ap.ssid));
  strlcpy((char *)apConfig.ap.password, pass.c_str(), sizeof(apConfig.ap.password));

  apConfig.ap.authmode = pass.length() >= 8 ? WIFI_AUTH_WPA2_PSK : WIFI_AUTH_OPEN;
  apConfig.ap.max_connection = MaxNetConnections;
  apConfig.ap.channel = AccessPointChannel;

  esp_err_t err = esp_wifi_set_config(WIFI_IF_AP, &apConfig);
  if (err != ESP_OK)
  {
    Logger::Log(LoggerTag::Network, "Could not apply AP credentials: ", esp_err_to_name(err));
  }
}

void Network::SaveNetCredentials(const std::string &ssid, const std::string &password)
{
  nvs_handle_t handle;
  ESP_ERROR_CHECK(nvs_open(WIFI_NVS_NAMESPACE, NVS_READWRITE, &handle));

  ESP_ERROR_CHECK(nvs_set_str(handle, "ssid", ssid.c_str()));
  ESP_ERROR_CHECK(nvs_set_str(handle, "password", password.c_str()));

  uint8_t mode = static_cast<uint8_t>(ServerMode::Production);
  ESP_ERROR_CHECK(nvs_set_u8(handle, "srv_mode", mode));

  ESP_ERROR_CHECK(nvs_commit(handle));
  nvs_close(handle);

  CurrentServerMode.Set(ServerMode::Production);
}

void Network::EventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId, void *eventData)
{
  Network *self = static_cast<Network *>(arg);

  if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_START)
  {
    esp_wifi_connect();
  }

  else if (eventBase == WIFI_EVENT && eventId == WIFI_EVENT_STA_DISCONNECTED)
  {
    self->led_.Off();
    CurrentNetworkMode = NetworkMode::Access;

    if (currentStationRetries_ < MaxConnectionRetries)
    {
      Logger::Log(LoggerTag::Network, "Reintentando conexión STA...");
      esp_wifi_connect();
      currentStationRetries_++;
    }
    else
    {
      Logger::Log(LoggerTag::Network, "Fallback a Access Point");
      self->StartAccessPoint();
    }
  }

  else if (eventBase == IP_EVENT && eventId == IP_EVENT_STA_GOT_IP)
  {
    ip_event_got_ip_t *event = (ip_event_got_ip_t *)eventData;

    char ipStr[16];
    snprintf(ipStr, sizeof(ipStr), IPSTR, IP2STR(&event->ip_info.ip));

    WifiIpAddress = ipStr;

    self->led_.On();

    Logger::Log(LoggerTag::Network, "WiFi IP: ", WifiIpAddress.Get());

    CurrentNetworkMode = NetworkMode::Station;

    currentStationRetries_ = 0;
  }
}
