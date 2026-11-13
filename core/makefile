# Thin wrapper around idf.py. Requires ESP-IDF v5.5 (https://docs.espressif.com/projects/esp-idf)
SHELL := /bin/bash

IDF_PATH ?= $(HOME)/esp/esp-idf
PORT     ?= /dev/ttyUSB0

# Use idf.py from PATH if the environment is already exported, otherwise export it
IDF := $(if $(shell command -v idf.py),idf.py,. $(IDF_PATH)/export.sh > /dev/null && idf.py)

.PHONY: all flash monitor run menuconfig size clean fullclean

all:
	$(IDF) build

flash:
	$(IDF) -p $(PORT) flash

monitor:
	$(IDF) -p $(PORT) monitor

# Flash and open the monitor (Ctrl+] to exit)
run:
	$(IDF) -p $(PORT) flash monitor

menuconfig:
	$(IDF) menuconfig

size:
	$(IDF) size

clean:
	$(IDF) clean

fullclean:
	$(IDF) fullclean
