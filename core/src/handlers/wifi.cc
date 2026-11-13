#include "wifi.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "http_utils.h"
#include "logger.h"

Wifi::Wifi(Network &network) : network_(network)
{
  postUri_ = {
      .uri = "/wifi",
      .method = HTTP_POST,
      .handler = &Wifi::HandlePost,
      .user_ctx = this};

  uris_ = {&postUri_};
}

const std::vector<httpd_uri_t *> &Wifi::Uris() const
{
  return uris_;
}

esp_err_t Wifi::HandlePost(httpd_req_t *req)
{
  Wifi *self = static_cast<Wifi *>(req->user_ctx);

  std::string body;
  if (!HttpUtils::ReadBody(req, body))
  {
    return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid body");
  }

  std::string ssid = HttpUtils::FormValue(body, "ssid");
  std::string password = HttpUtils::FormValue(body, "password");
  if (ssid.empty())
  {
    return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing ssid");
  }

  Network::SaveNetCredentials(ssid, password);
  Logger::Log(LoggerTag::Network, "Credentials saved for ", ssid);

  HttpUtils::SendJson(req, "{\"status\": \"saved\", \"ssid\": \"" + HttpUtils::JsonEscape(ssid) + "\"}");

  // Switching mode stops the AP the client is using: let the response
  // leave first and reconnect from another task
  xTaskCreate(&Wifi::ReconnectTask, "WifiReconnect", 4096, &self->network_, 5, nullptr);
  return ESP_OK;
}

void Wifi::ReconnectTask(void *arg)
{
  Network *network = static_cast<Network *>(arg);

  vTaskDelay(pdMS_TO_TICKS(1000));
  network->Reconnect();
  vTaskDelete(nullptr);
}
