#pragma once
#include <vector>

#include <esp_http_server.h>

#include "blackboard.h"
#include "handler.h"
#include "logger.h"

/**
 * HTTP server: every route lives in a Handler (see src/handlers).
 * To add one, create it from the Ping template and add it in LoadHandlers().
 */
class Server
{
public:
  Server() = default;
  ~Server();

  httpd_handle_t Start();

  void AddHandler(Handler *handler);
  void ResetHandlers();

private:
  httpd_handle_t server_ = nullptr;
  std::vector<Handler *> handlers_;

  void LoadHandlers();
  void RegisterHandlers();
  void DeleteHandlers();
};
