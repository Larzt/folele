#pragma once
#include "handler.h"
#include "network.h"

/**
 * POST /wifi  (form: ssid=...&password=...)
 * Saves the station credentials in NVS and switches to STA mode.
 * If the network cannot be reached the drone falls back to its AP again.
 */
class Wifi : public Handler
{
public:
  explicit Wifi(Network &network);

  const std::vector<httpd_uri_t *> &Uris() const override;

private:
  static esp_err_t HandlePost(httpd_req_t *req);
  static void ReconnectTask(void *arg);

  Network &network_;
  httpd_uri_t postUri_;
  std::vector<httpd_uri_t *> uris_;
};
