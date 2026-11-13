#pragma once
#include <cstdlib>
#include <string>

#include <esp_http_server.h>

/**
 * Small helpers shared by the handlers.
 */
namespace HttpUtils
{
  // Decodes application/x-www-form-urlencoded text ("%20" and "+" -> ' ')
  inline std::string UrlDecode(const std::string &text)
  {
    std::string out;
    for (size_t i = 0; i < text.size(); i++)
    {
      if (text[i] == '+')
      {
        out += ' ';
      }
      else if (text[i] == '%' && i + 2 < text.size())
      {
        out += (char)strtol(text.substr(i + 1, 2).c_str(), nullptr, 16);
        i += 2;
      }
      else
      {
        out += text[i];
      }
    }
    return out;
  }

  // Value of `key` in a "a=1&b=2" body (decoded), empty if missing
  inline std::string FormValue(const std::string &body, const std::string &key)
  {
    size_t pos = 0;
    while (pos < body.size())
    {
      size_t end = body.find('&', pos);
      if (end == std::string::npos)
        end = body.size();

      std::string pair = body.substr(pos, end - pos);
      size_t eq = pair.find('=');
      if (eq != std::string::npos && pair.substr(0, eq) == key)
        return UrlDecode(pair.substr(eq + 1));

      pos = end + 1;
    }
    return "";
  }

  // Escapes a string to be placed inside JSON quotes
  inline std::string JsonEscape(const std::string &text)
  {
    std::string out;
    for (char c : text)
    {
      if (c == '"' || c == '\\')
        out += '\\';
      if ((unsigned char)c < 0x20)
        continue;
      out += c;
    }
    return out;
  }

  // Reads the whole request body (up to maxLen bytes)
  inline bool ReadBody(httpd_req_t *req, std::string &body, size_t maxLen = 512)
  {
    if (req->content_len > maxLen)
      return false;

    body.assign(req->content_len, '\0');
    size_t received = 0;
    while (received < req->content_len)
    {
      int ret = httpd_req_recv(req, body.data() + received, req->content_len - received);
      if (ret <= 0)
        return false;
      received += ret;
    }
    return true;
  }

  inline esp_err_t SendJson(httpd_req_t *req, const std::string &json)
  {
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json.c_str(), json.length());
  }
}
