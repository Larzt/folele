# Busca recursivamente archivos que coincidan con un patrón (sin depender de `find`)
rwildcard = $(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

# Placa: Arduino Nano / Uno (ATmega328P a 16 MHz)
MCU         := atmega328p
F_CPU       := 16000000L
VARIANT     ?= standard
PORT        ?= COM3
# Nano con bootloader antiguo: make upload BAUD=57600
BAUD        ?= 115200
SERIAL_BAUD ?= 115200
# Librerías del core AVR que se usan (las de hardware/avr/*/libraries)
LIBS        ?= Wire EEPROM

TARGET   := main
BUILD    := build

ifneq ($(wildcard /proc/sys/fs/binfmt_misc/WSLInterop),)
# En WSL el toolchain instalado es el de Windows (y solo Windows ve los COMx),
# así que se reenvía todo al make de Windows: `make upload PORT=COM5` funciona igual
GOALS := $(or $(MAKECMDGOALS),all)

.PHONY: $(GOALS)
$(firstword $(GOALS)):
	@cmd.exe /c "cd /d $(shell wslpath -w .) && make $(GOALS) $(MAKEOVERRIDES)"

$(wordlist 2,$(words $(GOALS)),$(GOALS)):
	@:

else

# Toolchain y core AVR instalados con `arduino-cli core install arduino:avr`
# (ruta: `arduino-cli config get directories.data`)
ifeq ($(OS),Windows_NT)
  SHELL  := cmd.exe
  native  = $(subst /,\,$1)
  MKDIR   = if not exist "$(subst /,\,$1)" mkdir "$(subst /,\,$1)"
  RMDIR   = if exist "$1" rmdir /s /q "$1"
  EXE    := .exe
  ARDUINO_DATA ?= C:/dev/arduino/data
else
  native  = $1
  MKDIR   = mkdir -p $1
  RMDIR   = rm -rf $1
  ARDUINO_DATA ?= $(HOME)/.arduino15
endif

PKG          := $(ARDUINO_DATA)/packages/arduino
CORE_ROOT    := $(lastword $(sort $(wildcard $(PKG)/hardware/avr/*)))
AVR_BIN      := $(lastword $(sort $(wildcard $(PKG)/tools/avr-gcc/*)))/bin
AVRDUDE_DIR  := $(lastword $(sort $(wildcard $(PKG)/tools/avrdude/*)))

ifeq ($(CORE_ROOT),)
  $(error No se encuentra el core AVR en $(ARDUINO_DATA). Instalalo con: arduino-cli core install arduino:avr)
endif

tool    = $(call native,$1$(EXE))
CC      := $(call tool,$(AVR_BIN)/avr-gcc)
CXX     := $(call tool,$(AVR_BIN)/avr-g++)
AR      := $(call tool,$(AVR_BIN)/avr-gcc-ar)
OBJCOPY := $(call tool,$(AVR_BIN)/avr-objcopy)
SIZE    := $(call tool,$(AVR_BIN)/avr-size)
AVRDUDE := $(call tool,$(AVRDUDE_DIR)/bin/avrdude)

CORE_DIR    := $(CORE_ROOT)/cores/arduino
VARIANT_DIR := $(CORE_ROOT)/variants/$(VARIANT)
LIBS_DIR    := $(CORE_ROOT)/libraries
LIB_SRC_DIRS := $(addprefix $(LIBS_DIR)/,$(addsuffix /src,$(LIBS)))

MCUFLAGS := -mmcu=$(MCU) -DF_CPU=$(F_CPU) -DARDUINO=10607 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR
OPTFLAGS := -Os -g -flto -ffunction-sections -fdata-sections

CXXFLAGS      := -std=gnu++17 -Wall -Wextra -fno-exceptions -fno-threadsafe-statics $(OPTFLAGS) $(MCUFLAGS)
LIB_CFLAGS    := -std=gnu11 -w -fno-fat-lto-objects $(OPTFLAGS) $(MCUFLAGS)
LIB_CXXFLAGS  := -std=gnu++11 -w -fpermissive -fno-exceptions -fno-threadsafe-statics -fno-fat-lto-objects $(OPTFLAGS) $(MCUFLAGS)
LIB_ASFLAGS   := -x assembler-with-cpp -flto $(MCUFLAGS)
LDFLAGS       := -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections -mmcu=$(MCU)
LIB_INCLUDES  := -I$(CORE_DIR) -I$(VARIANT_DIR) $(addprefix -I,$(LIB_SRC_DIRS))

# Detección automática de fuentes y cabeceras propias
SRCS     := $(wildcard *.cc) $(call rwildcard,src,*.cc)
HDRS     := $(wildcard *.h) $(call rwildcard,src,*.h)
INC_DIRS := $(sort $(dir $(HDRS)))
INCLUDES := $(addprefix -I,$(INC_DIRS)) $(LIB_INCLUDES)

OBJS     := $(SRCS:%.cc=$(BUILD)/%.o)
DEPS     := $(OBJS:.o=.d)

# Core de Arduino (sin su main.cpp: el main() es el de main.cc) + librerías de LIBS
CORE_SRCS := $(filter-out $(CORE_DIR)/main.cpp,$(wildcard $(CORE_DIR)/*.c $(CORE_DIR)/*.cpp $(CORE_DIR)/*.S))
CORE_OBJS := $(patsubst $(CORE_DIR)/%,$(BUILD)/core/%.o,$(CORE_SRCS))
LIBS_SRCS := $(foreach d,$(LIB_SRC_DIRS),$(call rwildcard,$d,*.c) $(call rwildcard,$d,*.cpp))
LIBS_OBJS := $(patsubst $(LIBS_DIR)/%,$(BUILD)/libs/%.o,$(LIBS_SRCS))
CORE_LIB  := $(BUILD)/core.a

ELF := $(BUILD)/$(TARGET).elf
HEX := $(BUILD)/$(TARGET).hex

.PHONY: all upload monitor size clean info

all: $(HEX) size

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex -R .eeprom $< $@

$(ELF): $(OBJS) $(CORE_LIB)
	$(CC) $(LDFLAGS) $(OBJS) $(CORE_LIB) -lm -o $@

$(CORE_LIB): $(CORE_OBJS) $(LIBS_OBJS)
	$(AR) rcs $@ $^

# -MMD -MP genera los .d para recompilar cuando cambie cualquier .h incluido
$(BUILD)/%.o: %.cc
	@$(call MKDIR,$(dir $@))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(BUILD)/core/%.c.o: $(CORE_DIR)/%.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(LIB_CFLAGS) $(LIB_INCLUDES) -c $< -o $@

$(BUILD)/core/%.cpp.o: $(CORE_DIR)/%.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(LIB_CXXFLAGS) $(LIB_INCLUDES) -c $< -o $@

$(BUILD)/core/%.S.o: $(CORE_DIR)/%.S
	@$(call MKDIR,$(dir $@))
	$(CC) $(LIB_ASFLAGS) $(LIB_INCLUDES) -c $< -o $@

$(BUILD)/libs/%.c.o: $(LIBS_DIR)/%.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(LIB_CFLAGS) $(LIB_INCLUDES) -c $< -o $@

$(BUILD)/libs/%.cpp.o: $(LIBS_DIR)/%.cpp
	@$(call MKDIR,$(dir $@))
	$(CXX) $(LIB_CXXFLAGS) $(LIB_INCLUDES) -c $< -o $@

size: $(ELF)
	$(SIZE) -C --mcu=$(MCU) $<

upload: $(HEX)
	$(AVRDUDE) -C $(AVRDUDE_DIR)/etc/avrdude.conf -p $(MCU) -c arduino -P $(PORT) -b $(BAUD) -D -U flash:w:$(HEX):i

monitor:
	arduino-cli monitor -p $(PORT) -c baudrate=$(SERIAL_BAUD)

clean:
	@$(call RMDIR,$(BUILD))

info:
	@echo Fuentes:   $(SRCS)
	@echo Cabeceras: $(HDRS)
	@echo Core:      $(CORE_ROOT)
	@echo Toolchain: $(AVR_BIN)

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

endif
