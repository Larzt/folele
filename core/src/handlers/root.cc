#include "root.h"

#include <string>

#include "blackboard.h"
#include "http_utils.h"
#include "network.h"
#include "power_manager.h"
#include "root_page.h"

Root::Root()
{
  pageUri_ = {
      .uri = "/",
      .method = HTTP_GET,
      .handler = &Root::HandlePage,
      .user_ctx = this};

  statusUri_ = {
      .uri = "/status",
      .method = HTTP_GET,
      .handler = &Root::HandleStatus,
      .user_ctx = this};

  faviconUri_ = {
      .uri = "/favicon.ico",
      .method = HTTP_GET,
      .handler = &Root::HandleFavicon,
      .user_ctx = this};

  uris_ = {&pageUri_, &statusUri_, &faviconUri_};
}

const std::vector<httpd_uri_t *> &Root::Uris() const
{
  return uris_;
}

esp_err_t Root::HandlePage(httpd_req_t *req)
{
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  return httpd_resp_send(req, RootPage, HTTPD_RESP_USE_STRLEN);
}

esp_err_t Root::HandleStatus(httpd_req_t *req)
{
  using HttpUtils::JsonEscape;

  bool station = CurrentNetworkMode.Get() == NetworkMode::Station;
  std::string ip = station ? WifiIpAddress.Get() : LocalIpAddress.Get();

  std::string staSsid, staPass;
  Network::LoadNetCredentials(staSsid, staPass);

  PowerStats stats;
  PowerManager::GetStats(stats);

  std::string json = "{";
  json += "\"power\": \"" + std::string(PowerManager::StateName(PowerManager::State())) + "\", ";
  json += "\"network\": \"" + std::string(station ? "station" : "access") + "\", ";
  json += "\"ip\": \"" + JsonEscape(ip) + "\", ";
  json += "\"sta_ssid\": \"" + JsonEscape(staSsid) + "\", ";
  json += "\"ap_ssid\": \"" + JsonEscape(AccessSSID) + "\", ";
  json += "\"uptime_us\": " + std::to_string(stats.uptimeUs) + ", ";
  json += "\"sleep_us\": " + std::to_string(stats.sleepUs) + ", ";
  json += "\"sleep_count\": " + std::to_string(stats.sleepCount);
  json += "}";

  return HttpUtils::SendJson(req, json);
}

esp_err_t Root::HandleFavicon(httpd_req_t *req)
{
  httpd_resp_set_status(req, "204 No Content");
  return httpd_resp_send(req, nullptr, 0);
}
