#include "drone.h"

Drone::Drone()
{
  Wire.begin();

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(PWR_MGMT_1);
  Wire.write(0);
  Wire.endTransmission();

  this->Calibrate();

  wdt_reset();
}

void Drone::Calibrate()
{
  calibration_.accelerometer = TryCalibrateAcc();
  calibration_.gyroscope = TryCalibrateGyr();
}

void Drone::SaveCalibration()
{
  data_.ax = calibration_.accelerometer.x;
  data_.ay = calibration_.accelerometer.y;
  data_.az = calibration_.accelerometer.z;

  data_.gx = calibration_.gyroscope.x;
  data_.gy = calibration_.gyroscope.y;
  data_.gz = calibration_.gyroscope.z;

  EEPROM.put(ADDR_DATA, data_);
  EEPROM.write(ADDR_MAGIC, MAGIC);
}

bool Drone::LoadCalibration()
{
  if (EEPROM.read(ADDR_MAGIC) != MAGIC)
  {
    return false;
  }

  EEPROM.get(ADDR_DATA, data_);

  calibration_.accelerometer.x = data_.ax;
  calibration_.accelerometer.y = data_.ay;
  calibration_.accelerometer.z = data_.az;

  calibration_.gyroscope.x = data_.gx;
  calibration_.gyroscope.y = data_.gy;
  calibration_.gyroscope.z = data_.gz;
  return true;
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
