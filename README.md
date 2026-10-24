# Folele

Firmware de un dron de dos motores pilotado con un joystick analógico, escrito en C++ para Arduino Nano/Uno (ATmega328P).

Forma parte de un proyecto más amplio para construir un dron completo. En esta fase el Arduino:

- Lee el joystick y lo convierte en la velocidad de cada motor.
- Calibra la IMU (acelerómetro y giroscopio) y guarda la calibración en la EEPROM.
- Usa un watchdog de 4 s que reinicia la placa si el programa se bloquea.

## Hardware

| Componente | Conexión |
| --- | --- |
| Arduino Nano / Uno | USB, 115200 baudios |
| Joystick analógico | X → A0, Y → A1 |
| Motor Alpha (ESC) | Pin 3 |
| Motor Beta (ESC) | Pin 9 |
| IMU | I²C, dirección 0x68 |

## Estructura

```text
core/
├── main.cc          arranque y bucle principal
├── makefile         compilación y carga sin el IDE de Arduino
└── src/
    ├── system/      Drone, Motor, Joystick
    ├── utils/       Logger, Vector2/Vector3, lectura I²C
    └── observer/    patrón observador (en desarrollo)
docs/
└── scheme.drawio    diagrama de bloques del sistema
```

## Compilación

Requiere [arduino-cli](https://arduino.github.io/arduino-cli/) con el núcleo AVR y la librería Servo:

```bash
arduino-cli core install arduino:avr
arduino-cli lib install Servo
```

Desde la carpeta `core/`:

```bash
make            # compila y muestra el uso de memoria
make upload     # carga en la placa (PORT=/dev/ttyUSB0 para cambiar el puerto)
make monitor    # monitor serie a 115200 baudios
make clean      # borra la carpeta build
```

Un Nano con el bootloader antiguo necesita `make upload BAUD=57600`.
