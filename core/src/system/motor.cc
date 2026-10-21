#include "motor.h"

Motor::Motor(const int pin)
{
  this->Init(pin);
}

void Motor::Init(const int pin)
{
  servo_.attach(pin);
  servo_.write(Velocoties::STOP);
  currentVelocity_ = Velocoties::STOP;
}

bool Motor::IsValidVelocity(const int vel)
{
  return (vel >= Velocoties::STOP && vel <= Velocoties::MAX);
}

void Motor::SetVelocity(const int vel)
{
  Logger::Log(LoggerTag::Mot, "Velocity: ", vel);

  if (!IsValidVelocity(vel))
  {
    Logger::Log(LoggerTag::Mot, "Invalid veloctity");
    return;
  }

  currentVelocity_ = vel;
  servo_.write(vel);
}

void Motor::IncreaseVelocity(const int increase)
{
  SetVelocity(currentVelocity_ + increase);
}

void Motor::DecreaseVelocity(const int decrease)
{
  SetVelocity(currentVelocity_ - decrease);
}
