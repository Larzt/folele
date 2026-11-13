#pragma once
#include <stdint.h>

#include <driver/i2c_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "blackboard.h"
#include "vector.h"
#include "watchdog.h"

/**
 * Read two bytes (high and low, in this order)
 * as a signed 16-bit integer
 */
inline int16_t ToInt16(uint8_t high, uint8_t low)
{
  return (int16_t)((high << 8) | low);
}

/**
 * Average of `samples` readings of the three 16-bit registers
 * starting at `reg` (X, Y, Z). Failed readings are skipped.
 */
inline Vector3 Average(i2c_master_dev_handle_t device, uint8_t reg, int samples = CalibrationSamples)
{
  long sx = 0, sy = 0, sz = 0;
  int valid = 0;
  for (int i = 0; i < samples; i++)
  {
    uint8_t data[6];
    if (i2c_master_transmit_receive(device, &reg, 1, data, sizeof(data), I2cTimeoutMs) == ESP_OK)
    {
      sx += ToInt16(data[0], data[1]);
      sy += ToInt16(data[2], data[3]);
      sz += ToInt16(data[4], data[5]);
      valid++;
    }

    Watchdog::Reset();
    vTaskDelay(pdMS_TO_TICKS(5));
  }

  if (valid == 0)
  {
    return Vector3();
  }
  return Vector3(sx / valid, sy / valid, sz / valid);
}
