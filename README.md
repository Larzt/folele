# Folele

Firmware for a two-motor drone piloted with an analog joystick, written in C++ for the ESP32 with [ESP-IDF](https://docs.espressif.com/projects/esp-idf) v5.5.

It is part of a larger project to build a complete drone. At this stage the ESP32:

- Reads the joystick and turns it into the speed of each motor.
- Calibrates the IMU (accelerometer and gyroscope) and stores the calibration in flash (NVS).
- Runs a 4 s task watchdog that reboots the board if the program hangs.
- Connects to WiFi as a station (credentials in NVS) or falls back to Access Point mode, and serves an HTTP API.

## Hardware

| Component | Connection |
| --- | --- |
| ESP32 DevKit (ESP32-WROOM-32) | USB, 115200 baud |
| Analog joystick (powered at 3.3 V) | X → GPIO34, Y → GPIO35 |
| Alpha motor (ESC) | GPIO25 |
| Beta motor (ESC) | GPIO26 |
| IMU | I²C (SDA GPIO21, SCL GPIO22), address 0x68 |
| Network LED (on while connected as STA) | GPIO27 |
| Ping LED (on-board LED) | GPIO2 |

All pins and hardware settings (I²C addresses, PWM, ADC, watchdog, baud rate) live in [`core/src/system/blackboard.h`](core/src/system/blackboard.h).

The ESP32 is a 3.3 V board: its ADC pins must never see 5 V. The ESCs are driven with 50 Hz PWM (LEDC), the standard servo signal.

## Structure

```text
core/
├── CMakeLists.txt     ESP-IDF project
├── sdkconfig.defaults default configuration (target, tick rate, console)
├── makefile           shortcuts for idf.py
├── main/
│   ├── CMakeLists.txt main component (compiles main/ and src/)
│   └── main.cc        app_main: setup and main loop
└── src/
    ├── system/        Drone, Motor, Joystick and the blackboard (pins and settings)
    ├── network/       WiFi (STA / AP fallback) and HTTP server
    ├── handlers/      HTTP routes: Handler base class and Ping (template)
    ├── utils/         Logger, Watchdog, Led, Vector2/Vector3, I²C reading
    └── observer/      Observable values (runtime state in the blackboard)
docs/
└── scheme.drawio      system block diagram
```

## Network

On boot the drone tries to join the WiFi network saved in NVS (namespace `wifi`). If there is none, or it fails 5 times, it starts its own Access Point (`Folele` / `12345678`, IP `192.168.4.1`). The HTTP server listens on port 80:

| Route | Response |
| --- | --- |
| `GET /ping` | `{"status": "pong", "led_state": 0\|1}` (toggles the ping LED) |
| `POST /arm` | Switches to **Flying**: `{"state": "flying"}` |
| `POST /disarm` | Switches to **Idle**: `{"state": "idle"}` |
| `GET /power` | Current power state |

To add a route, copy `src/handlers/ping.cc` / `lib/ping.h`, rename the class and add it in `Server::LoadHandlers()` (`src/network/server.cc`).

## Power modes

| State | Motors | ESP32 |
| --- | --- | --- |
| **Idle** (boot, after `/disarm`) | stopped, joystick ignored | automatic light sleep + CPU at 80-240 MHz; the radio sleeps between beacons in STA mode |
| **Flying** (after `/arm`) | follow the joystick | light sleep blocked so the control loop never stops; WiFi power save off |

Light sleep only happens in STA mode: while the drone runs its own Access Point the WiFi driver keeps the chip awake, so Idle only lowers the CPU frequency.

## Build

Requires [ESP-IDF v5.5](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32/get-started/index.html) installed in `~/esp/esp-idf` (or set `IDF_PATH`). The makefile exports the environment by itself if `idf.py` is not on the `PATH`. From the `core/` folder:

```bash
make            # compiles (idf.py build)
make flash      # flashes the board (PORT=/dev/ttyUSB0 by default)
make monitor    # serial monitor at 115200 baud (Ctrl+] to exit)
make run        # flash + monitor
make menuconfig # ESP-IDF configuration
make size       # memory usage
make clean      # removes the build output (fullclean removes everything)
```

Or directly with `idf.py build`, `idf.py -p /dev/ttyUSB0 flash monitor`, etc.

On WSL the board has to be attached to Linux first with [usbipd-win](https://learn.microsoft.com/windows/wsl/connect-usb).
