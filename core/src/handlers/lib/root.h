#pragma once
#include "handler.h"

/**
 * GET /            -> control panel (HTML, see root_page.h)
 * GET /status      -> {"power", "network", "ip", "sta_ssid", "ap_ssid",
 *                      "uptime_us", "sleep_us", "sleep_count"}
 * GET /favicon.ico -> 204 (avoids 404 noise in the log)
 */
class Root : public Handler
{
public:
  Root();

  const std::vector<httpd_uri_t *> &Uris() const override;

private:
  static esp_err_t HandlePage(httpd_req_t *req);
  static esp_err_t HandleStatus(httpd_req_t *req);
  static esp_err_t HandleFavicon(httpd_req_t *req);

  httpd_uri_t pageUri_;
  httpd_uri_t statusUri_;
  httpd_uri_t faviconUri_;
  std::vector<httpd_uri_t *> uris_;
};
