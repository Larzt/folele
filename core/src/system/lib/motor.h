#pragma once
#include <Arduino.h>
#include <Servo.h>

#include "logger.h"

enum Velocoties
{
  STOP = 90,
  MID = 100,
  MAX = 180,
  MIN = -180,
};

class Motor
{
public:
  Motor() = default;
  Motor(const int pin);
  void Init(const int pin);

  bool IsValidVelocity(const int vel);
  void SetVelocity(const int vel);
  void IncreaseVelocity(const int increase);
  void DecreaseVelocity(const int decrease);

  void Stop() { SetVelocity(Velocoties::STOP); }
  void Mid() { SetVelocity(Velocoties::MID); }
  void Max() { SetVelocity(Velocoties::MAX); }
  void Min() { SetVelocity(Velocoties::MIN); }

  ~Motor() = default;

private:
  Servo servo_;
  int currentVelocity_;
};
