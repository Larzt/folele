#pragma once
#include <driver/i2c_master.h>
#include <nvs.h>

#include "blackboard.h"
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

class Drone
{
public:
  Drone() = default;
  // Call from app_main(): I2C, NVS and PWM are not ready before
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
  void StopMotors();

private:
  void InitStorage();
  void InitImu();

  Motor motorAlpha_;
  Motor motorBeta_;

  Vector3 TryCalibrateAcc();
  Vector3 TryCalibrateGyr();

  CalibrationData data_;
  IMU calibration_;
  nvs_handle_t storage_ = 0;
  i2c_master_bus_handle_t i2cBus_ = nullptr;
  i2c_master_dev_handle_t imu_ = nullptr;
};
