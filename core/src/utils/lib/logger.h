#pragma once

#include <Arduino.h>

enum LoggerTag
{
  None,
  Calibration,
};

class Logger
{
public:
  static void Begin(unsigned long baud = 115200)
  {
    Serial.begin(baud);
    delay(100);
  }

  template <typename... Args>
  static void Log(LoggerTag tag, const Args &...args)
  {
    Serial.print("[");
    Serial.print(GetTag(tag));
    Serial.print("] ");
    (Serial.print(args), ...);
    Serial.println();
  }

  template <typename... Args>
  static void Log(const Args &...args)
  {
    Log(LoggerTag::None, args...);
  }

private:
  static const char *GetTag(LoggerTag tag)
  {
    switch (tag)
    {
    case LoggerTag::None:
      return "System";
    case LoggerTag::Calibration:
      return "Calibration";
    }
    return "Unknown";
  }

  Logger() = default;
  ~Logger() = default;
};
