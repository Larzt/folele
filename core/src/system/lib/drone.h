#pragma once
#include <Wire.h>
#include <EEPROM.h>

#include "motor.h"
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

// Motors
const int AlphaMotorPin = 3;
const int BetaMotorPin = 9;

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
  Drone() = default;
  // Llamar desde main() después de init(): no se puede usar Wire/Servo antes
  void Begin();
  ~Drone() = default;

  // Calibration
  void Calibrate();
  bool HasCalibration();
  bool SaveCalibration();
  bool LoadCalibration();
  void ClearCalibration();

  // Motors
  void SetMotorsVelocity(const int dx, const int dy);

private:
  Motor motorAlpha_;
  Motor motorBeta_;

  Vector3 TryCalibrateAcc();
  Vector3 TryCalibrateGyr();

  CalibrationData data_;
  IMU calibration_;
};
