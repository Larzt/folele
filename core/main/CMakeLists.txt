# The firmware lives in src/: register it as part of the main component.
# SRC_DIRS only picks up .c/.cpp/.S, so .cc files are listed through a glob
# (run `idf.py reconfigure` after adding or removing files).
file(GLOB_RECURSE SOURCES "*.cc" "../src/*.cc")

idf_component_register(
  SRCS ${SOURCES}
  INCLUDE_DIRS
    "../src/system" "../src/system/lib"
    "../src/utils/lib"
    "../src/observer"
    "../src/network/lib"
    "../src/handlers/lib"
  REQUIRES esp_driver_gpio esp_driver_i2c esp_driver_ledc esp_adc nvs_flash
           esp_wifi esp_netif esp_event esp_http_server esp_pm
)
