#pragma once
#include <driver/ledc.h>

#include "blackboard.h"
#include "logger.h"

enum Velocities
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
  Motor(gpio_num_t pin, ledc_channel_t channel);
  void Init(gpio_num_t pin, ledc_channel_t channel);

  bool IsValidVelocity(const int vel);
  void SetVelocity(const int vel);
  void IncreaseVelocity(const int increase);
  void DecreaseVelocity(const int decrease);

  void Stop() { SetVelocity(Velocities::STOP); }
  void Mid() { SetVelocity(Velocities::MID); }
  void Max() { SetVelocity(Velocities::MAX); }
  void Min() { SetVelocity(Velocities::MIN); }

  ~Motor() = default;

private:
  void Write(const int vel);

  gpio_num_t pin_;
  ledc_channel_t channel_;
  int currentVelocity_;
};
