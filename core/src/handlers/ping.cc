#include "ping.h"

#include <string>

Ping::Ping() : led_(PingLed)
{
  pingUri_ = {
      .uri = "/ping",
      .method = HTTP_GET,
      .handler = &Ping::Handle,
      .user_ctx = this};

  uris_ = {&pingUri_};
  led_.Begin();
}

const std::vector<httpd_uri_t *> &Ping::Uris() const
{
  return uris_;
}

esp_err_t Ping::Handle(httpd_req_t *req)
{
  const std::string kENDL = "\n";
  Ping *self = static_cast<Ping *>(req->user_ctx);

  self->led_.Toggle();

  std::string resp = "{" + kENDL;
  resp += "  \"status\": \"pong\"," + kENDL;
  resp += "  \"led_state\": " + std::to_string(self->led_.Get()) + kENDL;
  resp += "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());
  return ESP_OK;
}
