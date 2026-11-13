#pragma once
#include "blackboard.h"
#include "handler.h"
#include "led.h"

/**
 * GET /ping -> {"status": "pong", "led_state": <0|1>}
 *
 * Template for new handlers:
 *   1. Copy ping.h / ping.cc and rename the class.
 *   2. Fill one httpd_uri_t per route in the constructor (user_ctx = this).
 *   3. Write a static esp_err_t function per route; get the instance
 *      back with static_cast<Class *>(req->user_ctx).
 *   4. Add it in Server::LoadHandlers() (src/network/server.cc).
 */
class Ping : public Handler
{
public:
  Ping();

  const std::vector<httpd_uri_t *> &Uris() const override;

private:
  static esp_err_t Handle(httpd_req_t *req);

  Led led_;
  httpd_uri_t pingUri_;
  std::vector<httpd_uri_t *> uris_;
};
