#include "power.h"

#include <string>

Power::Power()
{
  armUri_ = {
      .uri = "/arm",
      .method = HTTP_POST,
      .handler = &Power::HandleArm,
      .user_ctx = this};

  disarmUri_ = {
      .uri = "/disarm",
      .method = HTTP_POST,
      .handler = &Power::HandleDisarm,
      .user_ctx = this};

  stateUri_ = {
      .uri = "/power",
      .method = HTTP_GET,
      .handler = &Power::HandleState,
      .user_ctx = this};

  uris_ = {&armUri_, &disarmUri_, &stateUri_};
}

const std::vector<httpd_uri_t *> &Power::Uris() const
{
  return uris_;
}

esp_err_t Power::HandleArm(httpd_req_t *req)
{
  PowerManager::Arm();
  return SendState(req);
}

esp_err_t Power::HandleDisarm(httpd_req_t *req)
{
  PowerManager::Disarm();
  return SendState(req);
}

esp_err_t Power::HandleState(httpd_req_t *req)
{
  return SendState(req);
}

esp_err_t Power::SendState(httpd_req_t *req)
{
  const std::string kENDL = "\n";

  std::string resp = "{" + kENDL;
  resp += "  \"state\": \"" + std::string(PowerManager::StateName(PowerManager::State())) + "\"" + kENDL;
  resp += "}";

  httpd_resp_set_type(req, "application/json");
  httpd_resp_send(req, resp.c_str(), resp.length());
  return ESP_OK;
}
