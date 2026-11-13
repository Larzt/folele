#include "server.h"

#include "ping.h"
#include "power.h"

Server::~Server()
{
  if (server_)
  {
    httpd_stop(server_);
  }
  DeleteHandlers();
}

httpd_handle_t Server::Start()
{
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port = ServerPort;
  config.max_req_hdr_len = HeaderLength;
  config.max_uri_handlers = MaxHandlers;
  config.lru_purge_enable = true;

  if (httpd_start(&server_, &config) != ESP_OK)
  {
    Logger::Log(LoggerTag::Server, "Could not wake up the server");
    return nullptr;
  }

  LoadHandlers();
  RegisterHandlers();

  Logger::Log(LoggerTag::Server, "Server listening in port: ", ServerPort);
  return server_;
}

void Server::AddHandler(Handler *handler)
{
  handlers_.push_back(handler);
}

void Server::ResetHandlers()
{
  Logger::Log(LoggerTag::Server, "Clearing server routes");

  for (auto *handler : handlers_)
  {
    for (auto *uri : handler->Uris())
    {
      httpd_unregister_uri_handler(server_, uri->uri, uri->method);
    }
  }

  DeleteHandlers();

  LoadHandlers();
  RegisterHandlers();
}

void Server::LoadHandlers()
{
  this->AddHandler(new Ping());
  this->AddHandler(new Power());
  if (CurrentServerMode.Get() == ServerMode::Developer)
  {
    // Handlers only available in developer mode
  }
}

void Server::RegisterHandlers()
{
  for (auto *handler : handlers_)
  {
    for (auto *uri : handler->Uris())
    {
      if (httpd_register_uri_handler(server_, uri) != ESP_OK)
      {
        Logger::Log(LoggerTag::Server, "Error registering URI ", uri->uri);
      }
    }
  }
}

void Server::DeleteHandlers()
{
  for (auto *handler : handlers_)
  {
    delete handler;
  }

  handlers_.clear();
}
