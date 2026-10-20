#pragma once
#include <Wire.h>
#include <EEPROM.h>

#include "logger.h"
#include "vector.h"
#include "logic.h"

struct IMU
{
  Vector3 accelerometer;
  Vector3 gyroscope;
};

struct CalibrationData
{
  long ax, ay, az, gx, gy, gz;
};

// IMU
const int MPU_ADDR = 0x68;
const int ACC_ADDR = 0x3B;
const int GYR_ADDR = 0x43;
const int PWR_MGMT_1 = 0x6B;

// EEPROM
const byte MAGIC = 0xA5;
const int ADDR_MAGIC = 0;
const int ADDR_DATA = 1;

class Drone
{
public:
  Drone();
  ~Drone() = default;

  void Calibrate();
  void SaveCalibration();
  void LoadCalibration();
  void ClearCalibration();

private:
  Vector3 TryCalibrateAcc();
  Vector3 TryCalibrateGyr();

  CalibrationData data_;
  IMU calibration_;
};
