#include "motor.h"

#include <algorithm>

#include <esp_err.h>

Motor::Motor(gpio_num_t pin, ledc_channel_t channel)
{
  this->Init(pin, channel);
}

void Motor::Init(gpio_num_t pin, ledc_channel_t channel)
{
  pin_ = pin;
  channel_ = channel;

  // Every motor shares the same 50 Hz timer
  ledc_timer_config_t timer = {};
  timer.speed_mode = PwmMode;
  timer.duty_resolution = PwmResolution;
  timer.timer_num = PwmTimer;
  timer.freq_hz = PwmFrequency;
  timer.clk_cfg = LEDC_AUTO_CLK;
  ESP_ERROR_CHECK(ledc_timer_config(&timer));

  ledc_channel_config_t config = {};
  config.gpio_num = pin_;
  config.speed_mode = PwmMode;
  config.channel = channel_;
  config.timer_sel = PwmTimer;
  config.duty = 0;
  config.hpoint = 0;
  ESP_ERROR_CHECK(ledc_channel_config(&config));

  Write(Velocities::STOP);
  currentVelocity_ = Velocities::STOP;
}

bool Motor::IsValidVelocity(const int vel)
{
  return (vel >= Velocities::STOP && vel <= Velocities::MAX);
}

void Motor::SetVelocity(const int vel)
{
  Logger::Log(LoggerTag::Mot, "Velocity: ", vel);

  if (!IsValidVelocity(vel))
  {
    Logger::Log(LoggerTag::Mot, "Invalid velocity");
    return;
  }

  currentVelocity_ = vel;
  Write(vel);
}

void Motor::IncreaseVelocity(const int increase)
{
  SetVelocity(currentVelocity_ + increase);
}

void Motor::DecreaseVelocity(const int decrease)
{
  SetVelocity(currentVelocity_ - decrease);
}

void Motor::Write(const int vel)
{
  // 0-180 -> MinPulseUs-MaxPulseUs -> duty over the 20 ms period
  uint32_t angle = std::clamp(vel, 0, 180);
  uint32_t pulseUs = MinPulseUs + angle * (MaxPulseUs - MinPulseUs) / 180;
  uint32_t duty = pulseUs * ((1UL << PwmResolution) - 1) / PwmPeriodUs;
  ledc_set_duty(PwmMode, channel_, duty);
  ledc_update_duty(PwmMode, channel_);
}
