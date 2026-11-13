#pragma once
#include <stdint.h>
#include <string>

#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_adc/adc_oneshot.h>

#include "enums.h"
#include "observer.h"

/**
 * Blackboard: every pin and hardware setting of the drone in one place.
 * Target board: ESP32 DevKit (ESP32-WROOM-32).
 *
 * Pins to avoid on this board:
 *   GPIO1/3   UART0 (USB serial)
 *   GPIO6-11  internal flash
 *   GPIO34-39 input only (no PWM, no pull-ups)
 *   ADC2      unusable while WiFi is on
 */

// Main loop period
constexpr uint32_t LoopPeriodMs = 10;

// Watchdog: reboots the board if the main loop hangs
constexpr uint32_t WatchdogTimeoutMs = 4000;

// Joystick (ADC1, powered at 3.3 V: the ADC pins must never see 5 V)
constexpr gpio_num_t XAxisPin = GPIO_NUM_34;
constexpr gpio_num_t YAxisPin = GPIO_NUM_35;
// 10-bit readings (0-1023), full 0-3.3 V range
constexpr adc_bitwidth_t AdcResolution = ADC_BITWIDTH_10;
constexpr adc_atten_t AdcAttenuation = ADC_ATTEN_DB_12;
// ADC reading with the joystick at rest
constexpr int JoyCenter = 512;

// Motors (ESCs driven like servos: 50 Hz PWM through LEDC)
constexpr gpio_num_t AlphaMotorPin = GPIO_NUM_25;
constexpr gpio_num_t BetaMotorPin = GPIO_NUM_26;
constexpr ledc_channel_t AlphaMotorChannel = LEDC_CHANNEL_0;
constexpr ledc_channel_t BetaMotorChannel = LEDC_CHANNEL_1;
constexpr ledc_mode_t PwmMode = LEDC_LOW_SPEED_MODE;
constexpr ledc_timer_t PwmTimer = LEDC_TIMER_0;
constexpr uint32_t PwmFrequency = 50;
constexpr ledc_timer_bit_t PwmResolution = LEDC_TIMER_16_BIT;
constexpr uint32_t PwmPeriodUs = 1000000 / PwmFrequency;
// Pulse width for 0 and 180 (standard servo range)
constexpr uint32_t MinPulseUs = 544;
constexpr uint32_t MaxPulseUs = 2400;

// IMU (MPU-6050 over I2C)
constexpr gpio_num_t SdaPin = GPIO_NUM_21;
constexpr gpio_num_t SclPin = GPIO_NUM_22;
constexpr uint32_t I2cFrequencyHz = 400000;
constexpr int I2cTimeoutMs = 100;
constexpr uint16_t MPU_ADDR = 0x68;
constexpr uint8_t ACC_ADDR = 0x3B;
constexpr uint8_t GYR_ADDR = 0x43;
constexpr uint8_t PWR_MGMT_1 = 0x6B;
// Samples averaged during calibration
constexpr int CalibrationSamples = 200;

// Non-volatile storage (NVS) for the IMU calibration
constexpr const char *NVS_NAMESPACE = "drone";
constexpr const char *KEY_MAGIC = "magic";
constexpr const char *KEY_DATA = "calibration";
constexpr uint8_t MAGIC = 0xA5;

// Network (WiFi STA with fallback to Access Point)
constexpr const char *WIFI_NVS_NAMESPACE = "wifi";
constexpr int MaxNetConnections = 4;
constexpr int MaxConnectionRetries = 5;
constexpr uint8_t AccessPointChannel = 1;
// On while connected as a station
constexpr gpio_num_t NetLed = GPIO_NUM_27;

// HTTP server
constexpr uint16_t ServerPort = 80;
constexpr uint16_t HeaderLength = 1024;
constexpr uint16_t MaxHandlers = 16;

// Handlers
// Toggled on every GET /ping (GPIO2 is the on-board LED of the DevKit)
constexpr gpio_num_t PingLed = GPIO_NUM_2;

// Runtime state shared between modules
// Access Point credentials (overwritten by the ones saved in NVS)
inline std::string AccessSSID = "Folele";
inline std::string AccessPASS = "12345678";
inline Observable<std::string> WifiIpAddress{"0.0.0.0"};
inline Observable<std::string> LocalIpAddress{"0.0.0.0"};
inline Observable<NetworkMode> CurrentNetworkMode{NetworkMode::Access};
inline Observable<ServerMode> CurrentServerMode{ServerMode::Developer};

// Power management
// CPU frequency range for dynamic frequency scaling
constexpr int PowerMaxFreqMhz = 240;
constexpr int PowerMinFreqMhz = 80;
// While idle the main loop wakes up this often (must be < WatchdogTimeoutMs)
constexpr uint32_t IdleLoopPeriodMs = 1000;
