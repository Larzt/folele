#include "drone.h"

void Drone::Begin()
{
  Wire.begin();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission();

  if (!this->HasCalibration())
  {
    this->Calibrate();
    this->SaveCalibration();
  }
  else
  {
    this->LoadCalibration();
  }

  this->motorAlpha_.Init(AlphaMotorPin);
  this->motorBeta_.Init(BetaMotorPin);
  wdt_reset();
}

void Drone::Calibrate()
{
  calibration_.accelerometer = this->TryCalibrateAcc();
  calibration_.gyroscope = this->TryCalibrateGyr();
}

bool Drone::HasCalibration()
{
  return (EEPROM.read(ADDR_MAGIC) == MAGIC);
}

bool Drone::SaveCalibration()
{
  if (this->HasCalibration())
  {
    return false;
  }

  data_.ax = calibration_.accelerometer.x();
  data_.ay = calibration_.accelerometer.y();
  data_.az = calibration_.accelerometer.z();

  data_.gx = calibration_.gyroscope.x();
  data_.gy = calibration_.gyroscope.y();
  data_.gz = calibration_.gyroscope.z();

  EEPROM.put(ADDR_DATA, data_);
  EEPROM.write(ADDR_MAGIC, MAGIC);
  Logger::Log(LoggerTag::Calibration, "Calibration saved!");
  return true;
}

bool Drone::LoadCalibration()
{
  if (!this->HasCalibration())
  {
    return false;
  }

  EEPROM.get(ADDR_DATA, data_);

  calibration_.accelerometer.x(data_.ax);
  calibration_.accelerometer.y(data_.ay);
  calibration_.accelerometer.z(data_.az);

  calibration_.gyroscope.x(data_.gx);
  calibration_.gyroscope.y(data_.gy);
  calibration_.gyroscope.z(data_.gz);
  Logger::Log(LoggerTag::Calibration, "Calibration loaded!");
  return true;
}

void Drone::SetMotorsVelocity(const int dx, const int dy)
{
  Logger::Log(LoggerTag::Joy, "DX: ", dx, "DY: ", dy);
  if (dx == 0 && dy == 0)
  {
    motorAlpha_.Stop();
    motorBeta_.Stop();
    return;
  }

  if (dx != 0)
  {
    motorAlpha_.IncreaseVelocity(dx);
  }
  if (dy != 0)
  {
    motorBeta_.IncreaseVelocity(dy);
  }
}

Vector3 Drone::TryCalibrateAcc()
{
  Logger::Log(LoggerTag::Calibration, "Acceleroemeter... Do not move!");
  Vector3 offset = Average(MPU_ADDR, ACC_ADDR);
  Logger::Log(LoggerTag::Calibration, "Offset: ", offset);
  return offset;
}

Vector3 Drone::TryCalibrateGyr()
{
  Logger::Log(LoggerTag::Calibration, "Gyroscope... Do not move!");
  Vector3 offset = Average(MPU_ADDR, GYR_ADDR);
  Logger::Log(LoggerTag::Calibration, "Offset: ", offset);
  return offset;
}
