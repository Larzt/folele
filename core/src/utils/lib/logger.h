#pragma once

#include <cstdio>
#include <string>

#include "vector.h"

enum class LoggerTag
{
  None,
  Calibration,
  Joy,
  Mot,
  Network,
  Server,
  Handler,
  Power,
};

/**
 * Prints "[Tag] arg1arg2..." lines on the console (UART0).
 */
class Logger
{
public:
  template <typename... Args>
  static void Log(LoggerTag tag, const Args &...args)
  {
    std::printf("[%s] ", GetTag(tag));
    (Write(args), ...);
    std::printf("\n");
  }

  template <typename... Args>
  static void Log(const Args &...args)
  {
    Log(LoggerTag::None, args...);
  }

private:
  static void Write(const char *value) { std::fputs(value, stdout); }
  static void Write(const std::string &value) { std::fputs(value.c_str(), stdout); }
  static void Write(int value) { std::printf("%d", value); }
  static void Write(long value) { std::printf("%ld", value); }
  static void Write(unsigned value) { std::printf("%u", value); }
  static void Write(unsigned long value) { std::printf("%lu", value); }
  static void Write(double value) { std::printf("%.3f", value); }
  static void Write(const Vector2 &v) { std::printf("(%ld,%ld)", v.x(), v.y()); }
  static void Write(const Vector3 &v) { std::printf("(%ld,%ld,%ld)", v.x(), v.y(), v.z()); }

  static const char *GetTag(LoggerTag tag)
  {
    switch (tag)
    {
    case LoggerTag::None:
      return "System";
    case LoggerTag::Calibration:
      return "Calibration";
    case LoggerTag::Joy:
      return "Joystick";
    case LoggerTag::Mot:
      return "Motor";
    case LoggerTag::Network:
      return "Network";
    case LoggerTag::Server:
      return "Server";
    case LoggerTag::Handler:
      return "Handler";
    case LoggerTag::Power:
      return "Power";
    }
    return "Unknown";
  }

  Logger() = default;
  ~Logger() = default;
};
