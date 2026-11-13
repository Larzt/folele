#pragma once
#include <vector>

#include <esp_http_server.h>

/**
 * Base class of every HTTP route group. A handler owns its httpd_uri_t
 * structs and returns them in Uris(); the Server registers them.
 * Use Ping (ping.h / ping.cc) as the template for new handlers.
 */
class Handler
{
public:
  virtual ~Handler() = default;

  virtual const std::vector<httpd_uri_t *> &Uris() const = 0;

  virtual bool EnabledInApMode() const { return true; }
  virtual bool EnabledInStaMode() const { return true; }
};
