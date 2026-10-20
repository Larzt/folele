#pragma once
#include <stdint.h>
#include <Wire.h>
#include <avr/wdt.h>

#include "vector.h"

/**
 * Read two bytes (high and low, in this order)
 * like and integer of 16 bits with sign
 */
inline int16_t ReadInt16()
{
  uint8_t high = Wire.read();
  uint8_t low = Wire.read();
  return (int16_t)((high << 8) | low);
}

inline Vector3 Average(int mpu, int addr, int samples = 200)
{
  long sx = 0, sy = 0, sz = 0;
  for (int i = 0; i < samples; i++)
  {
    Wire.beginTransmission(mpu);
    Wire.write(addr);
    Wire.endTransmission(false);
    Wire.requestFrom(mpu, 6);

    sx += ReadInt16();
    sy += ReadInt16();
    sz += ReadInt16();

    wdt_reset();
    delay(5);
  }

  return Vector3(sx / samples, sy / samples, sz / samples);
}
