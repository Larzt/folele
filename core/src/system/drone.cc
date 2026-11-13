#include "drone.h"

#include <esp_err.h>
#include <nvs_flash.h>

void Drone::Begin()
{
  this->InitStorage();
  this->InitImu();

  if (!this->HasCalibration())
  {
    this->Calibrate();
    this->SaveCalibration();
  }
  else
  {
    this->LoadCalibration();
  }

  this->motorAlpha_.Init(AlphaMotorPin, AlphaMotorChannel);
  this->motorBeta_.Init(BetaMotorPin, BetaMotorChannel);
  Watchdog::Reset();
}

void Drone::InitStorage()
{
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &storage_));
}

void Drone::InitImu()
{
  i2c_master_bus_config_t busConfig = {};
  busConfig.i2c_port = I2C_NUM_0;
  busConfig.sda_io_num = SdaPin;
  busConfig.scl_io_num = SclPin;
  busConfig.clk_source = I2C_CLK_SRC_DEFAULT;
  busConfig.glitch_ignore_cnt = 7;
  busConfig.flags.enable_internal_pullup = true;
  ESP_ERROR_CHECK(i2c_new_master_bus(&busConfig, &i2cBus_));

  i2c_device_config_t deviceConfig = {};
  deviceConfig.dev_addr_length = I2C_ADDR_BIT_LEN_7;
  deviceConfig.device_address = MPU_ADDR;
  deviceConfig.scl_speed_hz = I2cFrequencyHz;
  ESP_ERROR_CHECK(i2c_master_bus_add_device(i2cBus_, &deviceConfig, &imu_));

  // Wake the MPU-6050 up (it boots in sleep mode)
  const uint8_t wake[] = {PWR_MGMT_1, 0};
  if (i2c_master_transmit(imu_, wake, sizeof(wake), I2cTimeoutMs) != ESP_OK)
  {
    Logger::Log(LoggerTag::Calibration, "IMU not responding");
  }
}

void Drone::Calibrate()
{
  calibration_.accelerometer = this->TryCalibrateAcc();
  calibration_.gyroscope = this->TryCalibrateGyr();
}

bool Drone::HasCalibration()
{
  uint8_t magic = 0;
  return (nvs_get_u8(storage_, KEY_MAGIC, &magic) == ESP_OK && magic == MAGIC);
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

  ESP_ERROR_CHECK(nvs_set_blob(storage_, KEY_DATA, &data_, sizeof(data_)));
  ESP_ERROR_CHECK(nvs_set_u8(storage_, KEY_MAGIC, MAGIC));
  ESP_ERROR_CHECK(nvs_commit(storage_));
  Logger::Log(LoggerTag::Calibration, "Calibration saved!");
  return true;
}

bool Drone::LoadCalibration()
{
  if (!this->HasCalibration())
  {
    return false;
  }

  size_t size = sizeof(data_);
  if (nvs_get_blob(storage_, KEY_DATA, &data_, &size) != ESP_OK || size != sizeof(data_))
  {
    return false;
  }

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
    motorAlpha_.Mid();
    motorBeta_.Mid();
    return;
  }

  if (dx != 0)
  {
    if (dx < 0)
    {
      motorAlpha_.DecreaseVelocity(dx);
    }
    else
    {
      motorAlpha_.IncreaseVelocity(dx);
    }
  }

  if (dy != 0)
  {
    if (dy < 0)
    {

      motorBeta_.DecreaseVelocity(dy);
    }
    else
    {
      motorBeta_.IncreaseVelocity(dy);
    }
  }
}

void Drone::StopMotors()
{
  motorAlpha_.Stop();
  motorBeta_.Stop();
}

Vector3 Drone::TryCalibrateAcc()
{
  Logger::Log(LoggerTag::Calibration, "Accelerometer... Do not move!");
  Vector3 offset = Average(imu_, ACC_ADDR);
  Logger::Log(LoggerTag::Calibration, "Offset: ", offset);
  return offset;
}

Vector3 Drone::TryCalibrateGyr()
{
  Logger::Log(LoggerTag::Calibration, "Gyroscope... Do not move!");
  Vector3 offset = Average(imu_, GYR_ADDR);
  Logger::Log(LoggerTag::Calibration, "Offset: ", offset);
  return offset;
}
