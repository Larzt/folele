#pragma once
#include "handler.h"
#include "power_manager.h"

/**
 * POST /arm    -> Flying (normal mode, motors follow the joystick)
 * POST /disarm -> Idle   (motors stopped, automatic light sleep)
 * GET  /power  -> current state
 * GET  /power/stats -> raw esp_pm report (time in each power mode)
 *
 * /arm, /disarm and /power answer {"state": "idle" | "flying"}.
 */
class Power : public Handler
{
public:
  Power();

  const std::vector<httpd_uri_t *> &Uris() const override;

private:
  static esp_err_t HandleArm(httpd_req_t *req);
  static esp_err_t HandleDisarm(httpd_req_t *req);
  static esp_err_t HandleState(httpd_req_t *req);
  static esp_err_t HandleStats(httpd_req_t *req);
  static esp_err_t SendState(httpd_req_t *req);

  httpd_uri_t armUri_;
  httpd_uri_t disarmUri_;
  httpd_uri_t stateUri_;
  httpd_uri_t statsUri_;
  std::vector<httpd_uri_t *> uris_;
};
