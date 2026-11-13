#include "joystick.h"

#include <esp_err.h>

void Joystick::Begin()
{
  // Both axes must be on the same ADC unit (ADC1)
  adc_unit_t unit;
  ESP_ERROR_CHECK(adc_oneshot_io_to_channel(XAxisPin, &unit, &xChannel_));
  ESP_ERROR_CHECK(adc_oneshot_io_to_channel(YAxisPin, &unit, &yChannel_));

  adc_oneshot_unit_init_cfg_t unitConfig = {};
  unitConfig.unit_id = unit;
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&unitConfig, &adc_));

  adc_oneshot_chan_cfg_t channelConfig = {};
  channelConfig.atten = AdcAttenuation;
  channelConfig.bitwidth = AdcResolution;
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_, xChannel_, &channelConfig));
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_, yChannel_, &channelConfig));
}

Vector2 Joystick::Read()
{
  int x = JoyCenter;
  int y = JoyCenter;
  adc_oneshot_read(adc_, xChannel_, &x);
  adc_oneshot_read(adc_, yChannel_, &y);
  return Vector2(x, y);
}
